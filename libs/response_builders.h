#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "AES.h"
#include "encoding.h"

void returnIndex(char * respbuf);
void returnNotFound(char * respbuf);
void returnPost(char * respbuf, char * recvbuf);

void returnIndex(char * respbuf)
{
	// Cita HTML
	char document[4096] = {0};
	FILE * f = fopen("index.html", "r");
	char ch;
	int docLen = 0;
	while ((ch = fgetc(f)) != EOF)
		document[docLen++] = ch;
	fclose(f);
	
	// Gradi response
	sprintf(respbuf,
	"\nHTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: %d"
	"\r\n\r\n"
	"%s", docLen, document);	
}

void returnNotFound(char * respbuf)
{
	// Gradi header response
	strcpy(respbuf, 
	"HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 13\r\n"
    "Connection: keep-alive\r\n\r\n"
    "404 Not Found");
}

void returnPost(char * respbuf, char * recvbuf)
{
	// Uzima parametre, NOTE: funkcionise samo ako su parametri poslati redosledom sa index.html stranice
	char encrypt;
	char inputBox[769] = {0};
	char key[49] = {0};
	char outputBox[301] = {0};
	int i;
	// Trazi poslednji newline '\n' karakter, jer je posle njega Payload post zahteva
	for(i=strlen(recvbuf)-1; i>=0; i--)
		if(recvbuf[i] == '\n')
		{
			char * payload = recvbuf + i + 1;
			
			// [encrypt] ce uvek biti jedan karakter odmah posle "enc="
			encrypt = payload[4] - '0';
			
			// Cita [inputBox]
			payload = payload + 15;
			for(i=0; payload[i] != '&'; i++)
				inputBox[i] = payload[i];
			decodeParam(inputBox);
			
			// Cita [key]
			payload = payload + i + 8;
			for(i=0; payload[i] != 0; i++)
				key[i] = payload[i];
			decodeParam(key);
			
			int outputLen;
			if(encrypt)
			{
				AES128_enc(inputBox, key);
				outputLen = strlen(inputBox);
			}
			else
			{
				outputLen = strlen(inputBox) / 2;
				AES128_dec(inputBox, key);
				// Proverava da li je output citljiv, tako sto zdesna ulevo trazi prvi ne-nula karakter
				for(i=outputLen-1; i>=0; i--)
					if(inputBox[i] != 0)
					{
						// Svaki karakter posle prvog ne-nula karaktera mora biti citljiv
						while(i >= 0)
						{
							if(!isprint(inputBox[i]))
							{
								strcpy(inputBox, "Dati sifrat i kljuc ne proizvode citljiv tekst.");
								outputLen = strlen(inputBox);
								break;
							}
							i--;
						}
						break;
					}
			}
			// Upisuje rezultat u outputBox
			memcpy(outputBox, inputBox, outputLen);
			break;
		}
	
	// Cita dokument
	char document[4096] = {0};
	int docLen = 0;
	FILE * f = fopen("index.html", "r");
	char c;
	int checkForOutput = 1;
	while((c = fgetc(f)) != EOF)
	{
		document[docLen++] = c;
		// Proverava da li je naisao na outputBox
		if(checkForOutput)
			if(c == '>' && docLen > 14)
				if(strncmp(document + docLen - 15, "id='outputBox'", 14) == 0)
				{
					// Dodaje rezultat enkripcije/dekripcije u outputBox
					strcat(document, outputBox);
					docLen += strlen(outputBox);
					checkForOutput = 0;
				}
	}
	fclose(f);
	// Kreira response
	sprintf(respbuf,
	"\nHTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Connection: keep-alive\r\n"
	"Content-Length: %d\r\n"
	"\r\n"
	"%s", docLen, document);
}
