/*
 * Developed by Andrew Gomenyuk <platinium9889@gmail.com>
 * https://github.com/renair
 * Uncomment test fileds to see raw data dumps.
 */

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

int get_server_response(const char* master, unsigned char* buff, int* data_len);

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
	printf("Blacklist response: %s\n", configuration._blacklistresponse[0] == 0 ? "noaddr" : configuration._blacklistresponse);
	printf("Blacklist:\n");
	print_tree(configuration._blacklist);
	//creating server socket
	struct sockaddr_in bind_addr;
	struct sockaddr_in client_addr;
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock == -1)
	{
		printf("Socket opening error!\n");
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
	unsigned char buff[BUFFLEN];
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
		if(addr_len >= 50)
		{
			printf("Address to long too long.\n\n");
			continue;
		}
		if(is_exist(configuration._blacklist, addr))
		{
			printf("%s is blocked!\n",addr);
			if(configuration._blacklistresponse[0] == '\0')
			{
				buff[2] |= 1<<7; //set answer bit
				buff[3] |= 3; //set address not found
				int sent_len = sendto(sock, buff, data_len, 0, (struct sockaddr*)&client_addr, client_len);
				printf("Sent bytes as noaddr: %d\n\n",sent_len);
			}
			else
			{
				int fake_len = 12 + addr_len + 6 + 10;
				unsigned char fake_response[fake_len];
				memset(fake_response, 0, fake_len);
				memcpy(fake_response, buff, 12 + addr_len + 6); //copy income packet
				memcpy(fake_response+fake_len-4,configuration._blacklistresponse,4); //copy ip to last 4 bytes
				fake_response[2] = 129; //set flags
				fake_response[3] = 128; //set flags
				fake_response[5] = 0;	//set amount of questions to 0
				fake_response[7] = 1;   //set amount of answers to 1
				fake_response[9] = 0;	//set amount of authority records to 0
				fake_response[11] = 0;	//set amount of additional records to 0
				fake_response[fake_len-5] = 4; //set length of response data
//test field
				for(int i = 0; i < fake_len;++i)
				{
					printf("%02x ",fake_response[i]);
				}
				printf("\n");
//end test field
				int sent_len = sendto(sock, fake_response, fake_len, 0, (struct sockaddr*)&client_addr, client_len);
				printf("Sent bytes for fake addr: %d\n\n",sent_len);
			}
			continue;
		}
		else
		{
			printf("%s is allowed\n",addr);
		}
		if(!get_server_response(configuration._masterdns, buff, &data_len))
		{
			continue;
		}
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

int get_server_response(const char* master, unsigned char* buff, int* data_len)
{
	//connect to server and send client's query
	int google = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(google == -1)
	{
		printf("Can't create connection to connect to master DNS.\n");
		return 0;
	}
	struct sockaddr_in google_addr;
	google_addr.sin_family = AF_INET;
	google_addr.sin_port = htons(PORT);
	if(inet_aton(master, &google_addr.sin_addr) == 0)
	{
		printf("Cant transfer addres to long.\n");
		close(google);
		return 0;
	}
//test field
//	printf("Data from client:\n");
//	for(int i = 0; i < *data_len;++i)
//	{
//		if(buff[i] > 32 && buff[i] < 127)
//		{
//			printf("%4c ",buff[i]);
//			continue;
//		}
//		printf("%4d ", buff[i]);
//	}
//	printf("\n");
//end test field
	sendto(google, buff, *data_len, 0, (struct sockaddr*)&google_addr, sizeof(google_addr));
	printf("Data sent to master DNS.\n");
	*data_len = recv(google, buff, BUFFLEN, 0);
	printf("Master DNS response length: %d\n", *data_len);
//test field
//	printf("Data from server:\n");
//	for(int i = 0; i < *data_len;++i)
//	{
//		if(buff[i] > 32 && buff[i] < 127)
//		{
//			printf("%4c ",buff[i]);
//			continue;
//		}
//		printf("%4d ", buff[i]);
//	}
//end test field
	close(google);
	return 1;
}
