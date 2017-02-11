#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#include "configurator.h"

#define BUFFLEN 512
#define PORT 53



int main(int argc, char** argv)
{
	//loading configuration
	struct config configuration;
	load_config("dns.conf", &configuration);
	if(configuration._status != 0)
	{
		printf("Configuration error. Code: %d\n", configuration._status);
		exit(1);
	}
	printf("Master dns: %s\n",configuration._masterdns);
	printf("Blacklist response: %s\n", configuration._blacklistresponse);
	printf("Blacklist:\n");
	print_tree(configuration._blacklist);
	//creating sockets
	struct sockaddr_in bind_addr;
	struct sockaddr_in client_addr;
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock == -1)
	{
		printf("(%d)Socket opening error!\n",sock);
		exit(1);
	}
	memset((void*)&bind_addr, 0, sizeof(bind_addr));
	memset((void*)&client_addr, 0, sizeof(client_addr));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(PORT);
	bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sock, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) == -1)
	{
		printf("Can't bind socket to %d port.\n",PORT);
		exit(1);
	}
	char buff[BUFFLEN];
	memset(buff, 0, BUFFLEN);
	while(1)
	{
		int data_len = 0;
		int client_len = sizeof(struct sockaddr);
		if(!(data_len = recvfrom(sock, buff, BUFFLEN, 0, (struct sockaddr*)&client_addr, &client_len)))
		{
			printf("Error in recvfrom.\n");
		}
		printf("Clint's data length: %d\n", data_len);
		char addr[50];
		int addr_len = 0;
		//reading address
		for(int i = 13; i < data_len && addr_len < 50;++i)
		{
			if(buff[i] == 0)
			{
				addr[addr_len] = 0;
				break;
			}
			if(buff[i] < 15)
			{
				addr[addr_len++] = '.';
				continue;
			}
			addr[addr_len++] = buff[i];
		}
		if(is_exist(configuration._blacklist, addr))
		{
			printf("%s is blocked!\n\n",addr);
			continue;
		}
		else
		{
			printf("%s is allowed\n",addr);
		}
		//connect to server and send client's query
		int google = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(google == -1)
		{
			printf("Can't create connection to connect to master DNS.\n");
			continue;
		}
		struct sockaddr_in google_addr;
		google_addr.sin_family = AF_INET;
		google_addr.sin_port = htons(PORT);
		if(inet_aton(configuration._masterdns, &google_addr.sin_addr) == 0)
		{
			printf("Cant transfer addres to long.\n");
			close(google);
			continue;
		}
		sendto(google, buff, data_len, 0, (struct sockaddr*)&google_addr, sizeof(google_addr));
		printf("Data sent to master DNS.\n");
		data_len = recv(google, buff, BUFFLEN, 0);
		printf("Master DNS response length: %d\n", data_len);
//test field




//end test field
		close(google);
		//send response to client
		int sent_bytes = sendto(sock, buff, data_len, 0, (struct sockaddr*)&client_addr, client_len);
		if(data_len == sent_bytes)
		{
			printf("DONE\n\n");
		}
		else
		{
			printf("Error %d. Sent %d/%d bytes.\n", errno, sent_bytes, data_len);
			switch(errno)
			{
				case EBADF:
					printf("The socket argument is not a valid file descriptor.\n");
					break;
				case ECONNRESET:
					printf("A connection was forcibly closed by a peer.\n");
					break;
				case EINVAL:
					printf("The arguments is not valid.");
					break;
				case EAFNOSUPPORT:
					printf("Address family not supported by protocol.\n");
					break;
			}
			printf("\n");
		}
	}
	close(sock);
	return 0;
}
