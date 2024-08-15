#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <fcntl.h>

#include "Request.h"

void worker_thread();
void handle_request(const Request);

void send_status(const int, const int);
void send_msg(const int, const int, const char *, const char *);
void send_page(const int, const char *);

int server_fd;
int main()
{
	int client_fd;
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

	while(true)
	{
		worker_thread();
	}
	return 0;
}

void worker_thread()
{
	// Get the clients request
	int client_fd = accept(server_fd, NULL, NULL);
	if(client_fd < 0)
	{
		printf("Failed to accept a connection.\n");

		// TODO a bunch of logging
		// close(server_fd);
		// return -1;
	}

	char buffer[4096] = {0};
	recv(client_fd, buffer, 4096, 0);

	// TODO dispatch or wake another worker thread

	// Separate request into type, url, header and body
	Request request;
	request.client_fd = client_fd;
	char *savePtr;
	request.method = __strtok_r(buffer, " ", &savePtr);
	request.path = __strtok_r(NULL, " ", &savePtr);
	request.version = __strtok_r(NULL, " ", &savePtr);
	request.headers = __strtok_r(NULL, "\r\n\r\n", &savePtr);
	request.body = savePtr;

	handle_request(request);
}

void handle_request(Request request)
{
	if(strcmp(request.path, "/") == 0)
		send_page(request.client_fd, "resources/test.html");
	else
		send_status(request.client_fd, 404);
}

void send_status(const int client_fd, const int status)
{
	// TODO have a 

	char *statusMsg = NULL;
	if(status == 404)
	{
		statusMsg = "Not Found";
	} else
	{
		statusMsg = "OK";
	}

	char response[256];
		snprintf(response, sizeof(response),
			"HTTP/1.1 %d %s\r\n"
			"Content-Length: 0\r\n"
			"\r\n",
			status,
			statusMsg
	);

	send(client_fd, response, strlen(response), 0);
}

void send_page(const int client_fd, const char *page)
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