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


int main(int argc, char *argv[]){
	
	if(argc<3){
		printf("Please enter the address and port to bind to\n"); 
		return 1;
	}

	struct addrinfo temp;
	memset(&temp, 0, sizeof(temp));
	temp.ai_socktype = SOCK_STREAM;
	struct addrinfo *bindaddr;
	if(getaddrinfo(argv[1], argv[2], &temp, &bindaddr)){
		printf("Getaddrinfo() failed\n");
		return 1;
	}

	int sock = socket(bindaddr->ai_family, bindaddr->ai_socktype, bindaddr->ai_protocol);
	if(sock <= 0){
		printf("Socket has failed with errno");
		return 1;
	}
	printf("Socket worked\n");

    	if (connect(sock,bindaddr->ai_addr, bindaddr->ai_addrlen)) {
		printf("Connect has failed\n");
		return 1;
	}


	freeaddrinfo(bindaddr);

	printf("Connected to address:%s at port:%s, say hello!\n", argv[1], argv[2]);
	 
	while(1) {
		fd_set set;
		FD_ZERO(&set);
		FD_SET(sock, &set);
		FD_SET(0,&set);
		 

		struct timeval to;
		to.tv_sec = 0;
		to.tv_usec = 1000;
		if(select(sock+1, &set, 0,0, &to) < 0){
            		fprintf(stderr, "select() failed. (%d)\n", errno);
			return 1;

		}
		if(FD_ISSET(sock,&set)){
			char buff[1024];
			int bytes_received = recv(sock, buff, 1024, 0);
			if (bytes_received < 1) {
				printf("Connection closed\n");
				break;
			}
			printf("Received :%.*s\n",bytes_received, buff);
        	}
		if(FD_ISSET(0, &set)){
			char buff[1024];
			fgets(buff, 1024, stdin);
			int bytes_sent = send(sock, buff, 1024, 0);
			printf("<You>: %s", buff);
		}

	}

}
