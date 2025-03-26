#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

void pulli(uint64_t x);
void printHex(uint8_t * str);
void printState(uint8_t s[4][4]);
void printRoundKey(uint8_t ** w, int r);

// Print Unsigned Long Long Integer
void pulli(uint64_t x){
	uint64_t power = 0, temp=x;
	printf("%d\n", x);
	return;
	while(temp > 0)
	{
		printf("1");
		power++;
		temp>>1;
	}
	printf("dec x = %d\nbin ");
	while(power>=0)
	{
		if(x / pow(2, power--) == 1)
			printf("1");
		else
			printf("0");
	}
	putchar('\n');
}

void printHex(uint8_t * str)
{
	int i;
	for(i=0;i<4;i++)
	{
		if(str[i] < 16)
			putchar('0');
		printf("%x", str[i]);
	}
		
	printf("\n");
}

void printState(uint8_t s[4][4])
{
	int i, j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(s[i][j]<16)
				printf("0");
			printf("%x ", s[i][j]);
		}
		putchar('\n');
	}
	putchar('\n');
}

void printRoundKey(uint8_t ** w, int r)
{
	int i, j;
	for(j=0;j<4;j++)
	{
		for(i=0;i<4;i++)
		{
			if(w[r+i][j]<16)
				printf("0");
			printf("%x ", w[r+i][j]);
		}
		putchar('\n');
	}
	putchar('\n');
}
