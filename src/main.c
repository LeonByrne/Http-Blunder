#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
	int server_fd, client_fd;
	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = INADDR_ANY;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd == 0)
	{
		printf("Failed to bind socket.\n");
		return -1;
	}

	if(bind(server_fd, (struct sockaddr *) &address, sizeof(address)) != 0)
	{
		printf("Failed to bind socket.\n");
		close(server_fd);
		return -1;
	}

	if(listen(server_fd, 10) != 0)
	{
		printf("Listen failed.\n");
		close(server_fd);
		return -1;
	}

	client_fd = accept(server_fd, NULL, NULL);
	if(client_fd < 0)
	{
		printf("Failed to accept a connection.\n");
		close(server_fd);
		return -1;
	}

	char buffer[256] = {0};
	recv(client_fd, buffer, 256, 0);
	printf("Request: %s\n", buffer);

	return 0;
}