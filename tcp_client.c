#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>	//for kbhit()

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 4096
char address[200] = "0.0.0.0";
char port[10] = "0";
int delay;
#define NUM_TEXTS_MAX 100
char* texts[NUM_TEXTS_MAX] = {0};
int num_texts = 0;


int load_config(void)
{
	#define CONFIG_FILE "config.txt"
	char input_sor[4096 + 1];
	//load .\config.txt
	FILE *input;
	if ((input = fopen(CONFIG_FILE, "r")) == NULL)
	{
		printf("Cannot open config file: %s\n", CONFIG_FILE);
		return -1;
	}

	while (!feof(input)){
		if (fgets(input_sor, sizeof(input_sor) - 1, input))
		{
			char* data;
			int len = strlen(input_sor);
			if (input_sor[len-1] == '\n'){
				input_sor[--len] = '\0';	//remove line end
			}
			if ((data = strchr(input_sor, '=')) != NULL){
				data++;
				if (strstr(input_sor, "address") == input_sor){
					strcpy(address, data);
				}
				if (strstr(input_sor, "port") == input_sor){
					strcpy(port, data);
				}
				if (strstr(input_sor, "delay_millisec") == input_sor){
					delay = atoi(data);
				}
				if (strstr(input_sor, "text") == input_sor && (num_texts < NUM_TEXTS_MAX)){
					char* tmp_txt = malloc(strlen(data) + 1);
					if (tmp_txt){
						strcpy(tmp_txt, data);
						texts[num_texts++] = tmp_txt;
					}
				}
			}
		}
	}
	fclose(input);
	return 0;
}

void free_texts(void)
{
	int i;
	for(i = 0; i < num_texts; i++){
		free(texts[i]);
	}

}

int __cdecl main(int argc, char **argv) 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char sendbuf[DEFAULT_BUFLEN];
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;


	//load config
	if (load_config()){
		printf("*** Load config file failed! Exiting!\n");
		return 1;
	}

	//test printing datas
	if (1){
		printf("address: %s:%s\n", address, port);
		int i;
		if (0){
			for(i = 0; i < num_texts; i++){
				printf("%01d:%s\n", i, texts[i]);
			}
		}
	}

	if (!num_texts){
		printf("*** No text defined in config file! Exiting!\n");
		return 1;
	}

	int curr_text = 0;
	do{
		if (curr_text >= num_texts){
			curr_text = 0;
		}
		strcpy(sendbuf, texts[curr_text++]);
		
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed with error: %d\n", iResult);
			return 1;
		}

		ZeroMemory( &hints, sizeof(hints) );
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(address, port, &hints, &result);
		if ( iResult != 0 ) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			return 1;
		}


		// Attempt to connect to an address until one succeeds
		for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

			// Create a SOCKET for connecting to server
			ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
				ptr->ai_protocol);
			if (ConnectSocket == INVALID_SOCKET) {
				printf("socket failed with error: %ld\n", WSAGetLastError());
				WSACleanup();
				return 1;
			}

			// Connect to server.
			iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET) {
			printf("Unable to connect to server!\n");
			WSACleanup();
			return 1;
		}

		// Send an initial buffer
		iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		printf("sendbuf(%ld): %s(%d)\n", iResult, sendbuf, strlen(sendbuf));
		//printf("Bytes Sent: %ld\n", iResult);

		// shutdown the connection since no more data will be sent
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		// Receive until the peer closes the connection
		printf("recvbuf: ");
		do {
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if ( iResult > 0 ){
				printf("%.*s", iResult, recvbuf); //5 here refers to # of characters
				//printf("Bytes received: %d\n", iResult);
			}
			else if ( iResult == 0 ){
				printf("\n");
				printf("Connection closed\n");
			}
			else
				printf("recv failed with error: %d\n", WSAGetLastError());

		} while( iResult > 0 );

		// cleanup
		closesocket(ConnectSocket);

		printf("sleep %d millisecs\n", delay);
		Sleep(delay);


	    WSACleanup();
	}while(!kbhit());

    return 0;
}