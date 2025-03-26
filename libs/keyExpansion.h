#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "GF_functions.h"

uint8_t ** keyExpansion(char * key);
void subWord(uint8_t word[4]);
void rotWord(uint8_t word[4]);


uint8_t ** keyExpansion(char key[16]){
	// Nk je duzina word-a round key-a. Fiksno je 4 za AES-128. Duzina kljuca za AES-128 je 16
	int i, j, Nk = 4; 
	
	// Dodela memorije pokazivacu koji ce biti vracen
	uint8_t ** w;
	w = malloc(44 * sizeof(uint8_t *));
	for(i=0;i<44;i++)
		w[i] = malloc(Nk * sizeof(uint8_t));
	
	// Dodela vrednosti prva 4 word-a (kopija kljuca)
	i=0;
	while(i < Nk)
	{
		w[i][0] = (uint8_t)key[i*4];
		w[i][1] = (uint8_t)key[i*4 + 1];
		w[i][2] = (uint8_t)key[i*4 + 2];
		w[i][3] = (uint8_t)key[i*4 + 3];
		i++;
	}
	
	// Round constants - 10 konstanti vezane za generaciju kljuceva, fiksno odredjene u dokumentaciji
	uint8_t Rcon[] = {0, 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};
	uint8_t temp[4];
	
	// Sledeci blok je prilagodjen da bude optimalan i pregledan za AES-128, ali ne radi za ostale verzije
	while(i<=43)
	{
		// Kopiranje w[i-1]
		memcpy(temp, w[i-1], 4);
		
		if(i%Nk == 0)
		{
			rotWord(temp);
			subWord(temp);
			temp[0] ^= Rcon[i/Nk];
		}
		
		// XOR-ovanje w[i] sa temp[i] bajt po bajt
		for(j=0;j<4;j++)
			w[i][j] = temp[j] ^ w[i-Nk][j];
		i++;
	}
	return w;
}

// Podfunkcije
void subWord(uint8_t word[4])
{
	// Operacija je potpuno ista kao u subBytes, samo se vrsi nad jednim redom, umesto nad matricom
	uint8_t b_inv, ans=0, temp;
	int i1, i, j;
	for(i1=0; i1<4; i1++)
	{
		b_inv = mult_inv(word[i1]);
		ans = 0;
		// Za svaki bit jednog bajta
		for(i=0;i<8;i++)
		{
			temp = b_inv;
			// Originalni bajt se rotira za [i] mesta kako bi se lakse iskalkulisala nova vrednost [i]-tog bita
			for(j=0;j<i;j++)
				temp = (temp>>1) ^ ((temp&1)<<7);
			// Kalkulisanje nove vrednosti b_inv[i]
			temp = (temp&1) ^ ((temp&(1<<4))>0) ^ ((temp&(1<<5))>0) ^ ((temp&(1<<6))>0) ^ ((temp&(1<<7))>0);
			// Dodavanje izracunatog bita u konacnu vrednost
			ans ^= (temp<<i);
		}
		// XOR-ovanje konacne vrednosti sa konstantom b_0110_0011
		ans ^= 99;
		word[i1] = ans;
	}
}

void rotWord(uint8_t word[4])
{
	// Rotiranje reci za jedno mesto ulevo
	int i;
	uint8_t temp;
	for(i=0;i<3;i++)
	{
		temp = word[i];
		word[i] = word[i+1];
		word[i+1] = temp;
	}
}
