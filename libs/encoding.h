#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ENCODING_H
#define ENCODING_H

void decodeParam(char * encoded);
void hexToChars(char * hexText);
void charsToHex(char * charText, int charLen);

void decodeParam(char * encoded)
{
	int i, j=0;
	for(i=0; encoded[i] != 0; i++)
		// Konvertuje hex u char po potrebi
		if(encoded[i] == '%')
		{
			char current;
			if(encoded[i + 1] <= '9')
				current = encoded[i + 1] - '0';
			else
				current = encoded[i + 1] - 'A' + 10;
			current = current << 4;
			
			if(encoded[i + 2] <= '9')
				current ^= (encoded[i + 2] - '0'); 
			else
				current ^= (encoded[i + 2] - 'A' + 10); 
			encoded[j++] = current;
			i += 2;
		}
		else if(encoded[i] == '+')
			encoded[j++] = ' ';
		else
			encoded[j++] = encoded[i];
		
	// Cisti ostatak stringa
	while(encoded[j] != 0)
		encoded[j++] = 0;
	return;
}

// Pretvara sifrat (iz inputBox-a) iz heksadecimalnog oblika u karaktere koji ce moci da budu desifrovani
void hexToChars(char * hexText)
{
	int i, hexTextLen = strlen(hexText);
	// Konvertuje svaki karakter u hex vrednost koju predstavlja
	for(i=0; i<hexTextLen; i++)
		if(hexText[i] <= '9')
			hexText[i] -= '0';
		else if(hexText[i] <= 'F')
			hexText[i] = hexText[i] - 'A' + 10;
		else
			hexText[i] = hexText[i] - 'a' + 10;
	
	// Spaja parove hex vrednosti u po jednu vrednsot
	for(i=0; i<hexTextLen; i+=2)
	{
		hexText[i / 2] = hexText[i] << 4;
		hexText[i / 2] ^= hexText[i + 1];
	}
	
	// Brise ostatak
	for(i=hexTextLen/2; i<hexTextLen; i++)
		hexText[i] = 0;
}

// Konvertuje necitljiv sifrat u citljiv hex oblik
void charsToHex(char * charText, int charLen)
{
	char hexText[charLen * 2];
	int i;
	for(i=0; i<charLen; i++)
		if((uint8_t)charText[i] < 16)
		{
			hexText[i * 2] = '0';
			sprintf(hexText + (i * 2 + 1), "%x", (uint8_t)charText[i]);
		}
		else
			sprintf(hexText + (i * 2), "%x", (uint8_t)charText[i]);
			
	for(i=0; i<charLen * 2; i++)
		hexText[i] = toupper(hexText[i]);
	
	memcpy(charText, hexText, charLen * 2);
}

#endif
