#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void paddPKCS5(char * input);
void removePadding(char * input, int inputLen);

void paddPKCS5(char * input)
{
	if(strlen(input) % 16 != 0)
	{
		int padVal = 16 - strlen(input) % 16;
		int i;
		for(i=strlen(input); (i % 16) != 0; i++)
			input[i] = padVal;
		input[i] = 0;
	}
	return;
}

void removePadding(char * input, int inputLen)
{
	int i, padVal = input[inputLen - 1];
	// Ako padding potencijalno postoji, validira ga
	if(1 <= padVal && padVal <= 15)
	{
		// Proverava da li je svako mesto koje bi trebalo da se padduje, pravilno paddovano
		for(i=inputLen-padVal; i<inputLen; i++)
			if(input[i] != padVal)
				return;
		
		// Posto je padding validan, brise ga
		for(i=inputLen-padVal; i<inputLen; i++)
			input[i] = 0;	
	}
}
