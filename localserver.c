#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>
#include "crypt.h"

#define PORT 0x0da2
#define IP_ADDR 0x7f000001
#define QUEUE_LEN 20

int main(void)
{
	//start server
	int listenS = socket(AF_INET, SOCK_STREAM, 0);
	if (listenS < 0)
	{
		perror("socket");
		return 1;
	}
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);
	if (bind(listenS, (struct sockaddr*)&s, sizeof(s)) < 0)
	{
		perror("bind");
		return 1;
	}
	if (listen(listenS, QUEUE_LEN) < 0)
	{
		perror("listen");
		return 1;
	}
	struct sockaddr_in clientIn;
	int clientInSize = sizeof clientIn;
	int newfd = accept(listenS, (struct sockaddr*)&clientIn, (socklen_t*)&clientInSize);
	int nrecv;
	if (newfd < 0)
	{
		perror("accept");
		return 1;
	}
	//get puzzles from client
	char puzzel[0xFFFF][16];
	if((nrecv = recv(newfd, &puzzel, sizeof(puzzel), 0)) < 0)
	{
		perror("recv");
		return 1;
	}
	//solve one puzzle
	int i = (rand() % 0xFFFF);
	printf("\nSuccessfully received %d bytes. Message Received %s\n", nrecv, puzzel[i]);
	key_set* skeys = malloc(17*sizeof(key_set));
	srand(time(NULL));
	process_message(puzzel[i], puzzel[i], skeys, 0);
	printf("\nMessage: ");
	for(int j=0; j<16; j++)
	 printf("%c", puzzel[i][j]);
	//send solved
	if (send(newfd, puzzel[i], sizeof(puzzel[i]), 0) < 0)
	{
		perror("send");
		return 1;
	}

	printf("\nDone sending data to client %d. \nClosing socket.\n", newfd);
	close(newfd);
	close(listenS);
	return 0;
}
