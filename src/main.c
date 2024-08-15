#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <fcntl.h>

void send_page(int, const char *);

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

	send_page(client_fd, "resources/test.html");

	return 0;
}

void send_page(int client_fd, const char *page)
{
	int page_fd = open(page, O_RDONLY);
	struct stat page_stat;

	fstat(page_fd, &page_stat);

	char response[256];
	snprintf(response, sizeof(response),
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: %ld\r\n"
		"\r\n",
		page_stat.st_size
	);

	send(client_fd, response, strlen(response), 0);
	sendfile(client_fd, page_fd, NULL, page_stat.st_size);

	close(page_fd);
}