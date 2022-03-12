/*
 ============================================================================
 Name        : 1.c
 Author      : Erwan
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

int create_server(int port){
	int sockfd;
	struct sockaddr_in servaddr;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	return sockfd;
}

int listen_to_client(int sockfd){

	int connfd, len;
	struct sockaddr_in cli;

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");

	return connfd;
}

void close_server(int sockfd){
	close(sockfd);
}

void read_from_client(int connfd, char * cmd){

	// read the message from client and copy it in buffer
	read(connfd, cmd, sizeof(cmd));
}

void send_to_client(int connfd, char * msg){

	write(connfd, msg, sizeof(msg));
}

typedef enum{
	FALSE = 0,
	TRUE = 1
}bool;

bool cmp_part_str(char * str1, unsigned int start, unsigned int nb_char, char * str2){
	int length = sizeof(char)*nb_char;

	bool result = FALSE;

	if(memcmp(&str1[start], str2, length) == 0){
		result = TRUE;
	}

	return result;
}

const char * read_command(char * cmd){
	/*
	 * Reads a string command (cmd) and returns a string response emulating the Iridium modem response to the command
	 */

	unsigned int cmd_sz = strlen(cmd);

	if( cmd_sz < 2 ){
		printf("%s\n", "Error: command must be at least 2 characters long");
	}

	printf("Command is of size %d\n", cmd_sz);
	printf("Command is %s\n", cmd);

	if(cmp_part_str(cmd, 0, 2, "AT")){
		if( cmd_sz < 4 ){
			return "OK";
		} else if( cmd_sz == 4 ){
			//AT + 2 characters
			if(cmp_part_str(cmd, 2, 2, "En")){
				return "En";
			} else if(cmp_part_str(cmd, 2, 2, "In")){
				return "In";
			} else if(cmp_part_str(cmd, 2, 2, "Qn")){
				return "Qn";
			} else if(cmp_part_str(cmd, 2, 2, "Vn")){
				return "Vn";
			} else if(cmp_part_str(cmd, 2, 2, "Zn")){
				return "Zn";
			} else if(cmp_part_str(cmd, 2, 2, "&V")){
				return "&V";
			} else if(cmp_part_str(cmd, 2, 2, "%R")){
				return "%R";
			} else if(cmp_part_str(cmd, 2, 2, "*F")){
				return "*F";
			} else{
				return "Unrecognized command";
			}


		}else if( cmd_sz == 5 ){
			//AT + 3 characters
			return "3char";
		}

	}

	return "No return";
}



int main(void) {

	int serv_id = create_server(PORT);
	int client_id = listen_to_client(serv_id);

	bool end_loop = FALSE;

	char cmd[256];

	while(!end_loop){
		read_from_client(client_id, cmd);

		if (strcmp(cmd, "close") == 0)
		{
			end_loop = TRUE;

			printf("%s\n", "Closing emulator");
		} else if (strcmp(cmd, "") != 0)
		{
			printf("Sending command %s\n", cmd);

			char * answer = read_command(cmd);

			printf("Modem: %s\n", answer);

			send_to_client(client_id, answer);
		}

	}



	close_server(serv_id);

    return EXIT_SUCCESS;
}
