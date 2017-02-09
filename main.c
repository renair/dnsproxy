#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUFFLEN 512
#define PORT 53

int main(int argc, char** argv)
{
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
	}
	char buff[BUFFLEN];
	memset(buff, 0, BUFFLEN);
	while(1)
	{
		int data_len = 0;
		int client_len = 16;
		if(!(data_len = recvfrom(sock, buff, BUFFLEN, 0, (struct sockaddr*)&client_addr, &client_len)))
		{
			printf("Error in recvfrom.\n");
		}
		printf("Data length: %d\n", data_len);
		for(int i = 12; i < data_len;++i)
		{
			if(buff[i] == 0)
			{
				break;
			}
			if(buff[i] < 15)
			{
				printf(".");
				continue;
			}
			printf("%c",buff[i]);
		}
		printf("\n");
		//connect to google and send query there
		int google = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(google == -1)
		{
			printf("Can't create connection to connect to master DNS.\n");
			continue;
		}
		struct sockaddr_in google_addr;
		google_addr.sin_family = AF_INET;
		google_addr.sin_port = htons(PORT);
		if(inet_aton("8.8.8.8", &google_addr.sin_addr) == 0)
		{
			printf("Cant transfer addres to long.\n");
			close(google);
			continue;
		}
		sendto(google, buff, data_len, 0, (struct sockaddr*)&google_addr, sizeof(google_addr));
		printf("Data sent to google.\n");
		data_len = recv(google, buff, BUFFLEN, 0);
		printf("Google response: %d\n", data_len);
		close(google);
		//send response to client
		//TODO client_len == 16 and it's not right, what to do
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
