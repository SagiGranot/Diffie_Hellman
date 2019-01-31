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

int main(void)
{
	//
	//connect to server
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  int nrecv;
	struct sockaddr_in s = {0};
	s.sin_family = AF_INET;
	s.sin_port = htons(PORT);
	s.sin_addr.s_addr = htonl(IP_ADDR);
	if (connect(sock, (struct sockaddr*)&s, sizeof(s)) < 0)
	{
		perror("connect");
		return 1;
	}
	printf("Successfully connected.\n");
	//create puzzels
	uint32_t Xi;
	uint64_t Pi = 0;
	uint64_t Ki;
	key_set *key_sets = (key_set*)malloc(17*sizeof(key_set));
	srand(time(NULL));
	char puzzel[0xFFFF][16];
	uint64_t array[0xFFFF] = {0};
	printf("\nCreating puzzles.");
	for (int i=0; i<0xFFFF; i++){
		Pi = 0;
		Pi += gen_random_32();
		while ((Xi = gen_random_32() > 0xFFFF) && (array[Xi] != 0));
		Ki = gen_random_64();
		generate_sub_keys((unsigned char*)&Pi, key_sets);
		memcpy(&puzzel[i][0],"PID ",4);
		memcpy(&puzzel[i][4], &Xi, sizeof(Xi));
		memcpy(&puzzel[i][8], &Ki, sizeof(Ki));
		array[Xi] = Ki;
		process_message(puzzel[i],puzzel[i],key_sets,ENCRYPTION_MODE);
		process_message(puzzel[i]+8,puzzel[i]+8,key_sets,ENCRYPTION_MODE);
	}
  //send all puzzles
	if (send(sock, puzzel, sizeof(puzzel), 0) < 0)
	{
		perror("send");
		return 1;
	}
	printf("\nSuccessfully sent");
  //get solved puzzles
  char psave[9];
  if (recv(sock, &psave , sizeof(psave), 0) < 0)
	{
		perror("send");
		return 1;
	}
  printf("\nShared key: %s\n", psave);
  close(sock);

  return 0;
}
