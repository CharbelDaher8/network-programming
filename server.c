#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "server2.h"
int main(int argc, char *argv[]){
	
	if (argc<3){
		printf("Not enough arguments, enter the address u want to start the server on with the port");
		return 1;
	}


	struct addrinfo temp;
	memset(&temp, 0, sizeof(temp));
	temp.ai_socktype = SOCK_STREAM;
	temp.ai_family = AF_INET;
	temp.ai_flags = AI_PASSIVE;
	struct addrinfo *bindaddr;
	if (getaddrinfo(argv[1], argv[2], &temp, &bindaddr)){
		fprintf(stderr, "getaddrinfo() failed. %d\n", errno);
		return 1;
	}

	int sock = socket(bindaddr->ai_family, bindaddr->ai_socktype, bindaddr->ai_protocol);
	if(sock <=0){
		printf("socket has failed with errno:%d", errno);
		return 1;
	}
	printf("Sock successfully initialized...\n");
	int bindSock = bind(sock, bindaddr->ai_addr, bindaddr->ai_addrlen);
	if(bindSock){
		printf("Binding has failed:%d", errno);
		return 1;
	}
	printf("Bind worked...\n");
	printf("Server running on address:%s at port:%s\n", argv[1], argv[2]);

	freeaddrinfo(bindaddr);

	if(listen(sock, 10) < 0){
		printf("Listen failed: %d\n", errno);
		return 1;
	}
	printf("Listening...\n");
	fd_set set;
	FD_ZERO(&set);
	FD_SET(sock, &set);
	int max = sock;

	while(1){
		fd_set sub;
		sub = set;
		char login[] = "login please";

		if(select(max+1, &sub, 0,0,0) < 0){
			printf("Select failed dawg");
			return 1;
		}
		for(int i=1; i<=max; ++i){
			if(FD_ISSET(i, &sub)){	
				if(i == sock){
					struct sockaddr_storage client;
					socklen_t clientlen = sizeof(client);
					int clientSock = accept(sock,(struct sockaddr*) &client, &clientlen);
					if(clientSock <=0){
						printf("Accept has failed");
						return 1;
					}
					FD_SET(clientSock, &set);
					if(clientSock > max){
						max = clientSock;
					}
					char addr[100];
					getnameinfo((struct sockaddr*)&client, clientlen, addr, sizeof(addr), 0,0,NI_NUMERICHOST);
					printf("[+]Incoming connection from:%s\n", addr);
					if(authenticate(clientSock)) continue;
					else{
						printf("Authentication failed");
						close(clientSock);
					}

				}else{
					char buff[1024];
					int bytesReceived = recv(i, buff, 1024, 0);
					if(bytesReceived <1){
						FD_CLR(i, &set);
						close(i);
						continue;
					}
					for(int z = 1; z<= max; ++z){
						if(FD_ISSET(z, &set)){
							if(z == sock || z == i){
								continue;

						}
							else{ 
								send(z, buff, bytesReceived, 0);
							}
					}
				}
			}
		}
	}

    }
	close(sock);
	printf("Finished");
	return 0;
}
