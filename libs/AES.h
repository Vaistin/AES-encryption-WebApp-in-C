#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#include "keyExpansion.h"
#include "AES_subfunctions.h"
#include "encoding.h"
#include "padding.h"
//#include "debug_functions.h"

void AES128_enc(char * in, char key[16]);
void AES128_dec(char * in, char key[16]);
void cipher(char * in, int Nr, uint8_t ** w);
void invCipher(char * in, int Nr, uint8_t ** w);

void AES128_enc(char * in, char key[16]){
	paddPKCS5(in);
	int i, inputLen = strlen(in);
	for(i=0; i<inputLen; i+=16)
		cipher(in + i, 10, keyExpansion(key));
	charsToHex(in, inputLen);
}

void AES128_dec(char * in, char key[16]){
	int i, inputLen = strlen(in) / 2;
	hexToChars(in);
	for(i=0; i<inputLen; i+=16)
		invCipher(in + i, 10, keyExpansion(key));
	removePadding(in, inputLen);
}


void cipher(char * in, int Nr, uint8_t ** w){
	// in -> input od 16 bajtova
	// Nr -> broj rundi (10, 12 ili 14 za AES-128, AES-192 i AES-256)
	// w -> niz "round key"-eva, koji su word-ovi od po 4 bajta
	
	// Kreiranje state-a
	uint8_t s[4][4];
	// row, column (indeksi)
	int r, c;
	for(r=0; r<4; r++)
		for(c=0; c<4; c++)
			s[r][c] = (uint8_t)in[r + 4 * c];
	
	// Menjanje state-a podfunkcijama
	addRoundKey(s, w, 0);
	for(r=1; r<Nr; r++)
	{
		subBytes(s);
		shiftRows(s);
		mixColumns(s);
		addRoundKey(s, w, r*4);
	}
	subBytes(s);
	shiftRows(s);
	addRoundKey(s, w, Nr*4);

	// Postavljanje novih vrednosti u input string
	for(r=0; r<4; r++)
		for(c=0; c<4; c++)
			in[r + 4*c] = (char)s[r][c];
	free(w);
}

void invCipher(char * in, int Nr, uint8_t ** w){
	// Kreiranje state-a
	uint8_t s[4][4];
	// row, column (indeksi)
	int r, c;
	for(r=0; r<4; r++)
		for(c=0; c<4; c++)
			s[r][c] = (uint8_t)in[r + 4*c];

	addRoundKey(s, w, Nr*4);
	for(r=Nr-1; r>0; r--)
	{
		invShiftRows(s);
		invSubBytes(s);
		addRoundKey(s, w, 4*r);
		invMixColumns(s);		
	}
	invShiftRows(s);
	invSubBytes(s);
	addRoundKey(s, w, 0);

	// Postavljanje novih vrednosti u input string
	for(r=0; r<4; r++)
		for(c=0; c<4; c++)
			in[r + 4*c] = (char)s[r][c];
			
	free(w);
}
