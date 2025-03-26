#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libs/response_builders.h"

// Linkovanje sa Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 5120
#define DEFAULT_PORT "27015"

int __cdecl main(void) 
{
	// Kreiranje WSADATA strukture za koriscenje Winsock-a
    WSADATA wsaData;
    int iResult;
    
    // Poziva verziju 2.2 winsock-a, funkcionalno konstruktor, vraca 0 na uspesnu inicijalizaciju
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // Struktura koja ce sadrzati specifikacije za rezoluciju adresa preko getaddrinfo
    struct addrinfo hints;
	// Inicijalizuje [hints] na nule
    ZeroMemory(&hints, sizeof(hints));
    // Setovanje specifikacija
    hints.ai_family = AF_INET; // Koristice se samo IpV4
    hints.ai_socktype = SOCK_STREAM; // Stream socket koji je koriscen od strane TCP protokola
    hints.ai_protocol = IPPROTO_TCP; // Koristi TCP protokol
    hints.ai_flags = AI_PASSIVE; // Dozvoljava socketu da se veze na bilo koju adresu

	// Pokazivac na ulancanu listu struktura koje predstavljaju adrese
    struct addrinfo *result = NULL;
    // Vezuje [result] za prvi element ulancane liste adresa
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    
    // Kreira socket za slusanje
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Vezivanje socketa za adresu
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }	
	// Oslobadja memoriju dodeljen za pracenje adresa--, jer je socket sada vec vezan za jednu od njih
    freeaddrinfo(result);

	// Oznacava da [ListenSocket] osluskuje za konekcije
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("Listening on port %s...\n", DEFAULT_PORT);
    
    // Kreiranje klijentskog socketa kojim ce se razmenjivati podaci
    SOCKET ClientSocket = INVALID_SOCKET;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    
    // Socket za sluasnje vise nije potreban
    closesocket(ListenSocket);
    
    // Promenljive za razmenu podataka
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
	char respbuf[DEFAULT_BUFLEN];
    int respbuflen = DEFAULT_BUFLEN;
    
    // Prima zahteve sa datog soccet-a dok korisnik ne zatvori konekciju
    do {
		printf("Listening...\n");
		// Potrebno je ocistiti [recvbuf] da ne bi dolazilo do konflikta kada se prima vise zahteva
		ZeroMemory(recvbuf, recvbuflen);
		// Prima zahtev
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
			printf("Client says: %s\n", recvbuf);
			
			// Proverava jel prima index GET request
			if(strncmp("GET / HTTP/1.1", recvbuf, 14) == 0)
			{
				returnIndex(respbuf);
			}
			// Proverava jel prima POST request
			else if(strncmp("POST", recvbuf, 4) == 0)
			{	
				returnPost(respbuf, recvbuf);
			}
			// Ako ne prima date vrste zahteva, treba da vrati 404 Not Found
			else
			{
				returnNotFound(respbuf);
			}
				
            // Salje odgovor
			iSendResult = send(ClientSocket, respbuf, strlen(respbuf), 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Server says: %s\n\n", respbuf);
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);

    // Zatvarnje konekcije
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
	
    // Ciscenje
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}
