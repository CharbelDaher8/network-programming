int readlines(char *filename){
	FILE *fileptr = fopen(filename, "r");

	char line[1024];
	int i;
	for(i=0; fgets(line, sizeof(line), fileptr) != NULL; i++){
		;
	}
	fclose(fileptr);
	return i;
}

char **makeTable(char *filename, int lineCount){
	char line[1024];
	FILE *fileptr = fopen(filename, "r");
	if(fileptr == NULL){
		printf("Fileptr in makeTable failed");
		return 0;
	}
	char **result = calloc(lineCount, sizeof(char*));
	for(int i=0; i<lineCount && fgets(line, sizeof(line), fileptr) != NULL; ++i){
		result[i] = strdup(line);
	}
	fclose(fileptr);
	return result;

}


int authenticate(int clientSock){
	char login[1024] = "s for sign up, l for login";
	char bytes[1024] = {0};
	send(clientSock, login, sizeof(login), 0);
	recv(clientSock, bytes, sizeof(bytes), 0);
	char username[1024] = {0};
	char password[1024] = {0};
	int lineCount = 0;
	int currentLine = 0;
	FILE *ptr = fopen("password.txt", "a+");
	fflush(stdin);
	fflush(stdout);
	if(ptr == NULL){
		printf("FILE FAILED");
	}
	char enter[1024] = "enter your username\n";
	char enter2[1024] = "enter your password\n";
	if(bytes[0] == 's'){
		send(clientSock, enter, sizeof(enter), 0);
		int bytesRec2 =  recv(clientSock, username, sizeof(username), 0);
		if(bytesRec2 == -1){
			printf("Username failed");
		}
		printf("Username:%s", username);
		send(clientSock, enter2, sizeof(enter2), 0);
		int bytesRec3 = recv(clientSock, password, sizeof(password), 0);
		if(bytesRec3 == -1){
			printf("password failed");
		}
		printf("Password:%s", password);
		fprintf(ptr, "%s", username);
		fprintf(ptr, "%s", password);
		fclose(ptr);
	
	}else if(bytes[0] == 'l'){
		char line[1024] ={0}; 
		send(clientSock, enter, sizeof(enter), 0);
		int bytesRec2 =  recv(clientSock, username, sizeof(username), 0);
		if(bytesRec2 == -1){
			printf("Username failed");
		}
		printf("Username:%s\n", username);
		send(clientSock, enter2, sizeof(enter2), 0);
		
		int bytesRec3 = recv(clientSock, password, sizeof(password), 0);
		if(bytesRec3 == -1){
			printf("password failed");
		}
		printf("Password:%s\n", password);
		lineCount = readlines("password.txt");
		char **result = makeTable("password.txt", lineCount);	

		for(int i=0; i<=lineCount; ++i){
			if(strstr(username, result[i]) != NULL){
				printf("found match at line:%d\n", i);
				printf("checking for password\n");
				if(strstr(password, result[i+1]) != NULL){
					printf("password authenticated:%s", password);
					printf("Logged in with username:%sand Password:%s\n", username, password);
					return 1;
				}else{
					printf("Wrong password dawg");
				}

			}

		}

		for(int i=0; i<=lineCount; ++i){
			free(result[i]);
		}
		free(result);



	}
}

