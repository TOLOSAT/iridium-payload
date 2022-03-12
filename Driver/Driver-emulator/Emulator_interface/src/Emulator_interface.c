/*
 ============================================================================
 Name        : Emulator_interface.c
 Author      : Erwan
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define IP "127.0.0.1"
#define SA struct sockaddr

int create_client(int port, char* ip){
	//Returns the id to the client

	struct sockaddr_in servaddr;

	// socket create and verification
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(port);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	return sockfd;
}

void close_client(int sockfd){
	close(sockfd);
}

void send_to_server(int sockfd, char * msg){
	write(sockfd, msg, sizeof(msg));
}

const char * read_from_server(int sockfd){
	char buff[MAX];
	read(sockfd, buff, sizeof(buff));

	return buff;
}

void func(int sockfd)
{
	//Example function. Loops between sending messages and receiving messages.

	char buff[MAX];
	int n;
	for (;;) {
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		} else if ((strcmp(buff, "")) != 0){
			printf("From Server : %s\n", buff);
		}
	}

