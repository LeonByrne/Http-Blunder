#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <pthread.h>

#include "wt/Server.h"

static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s = PTHREAD_COND_INITIALIZER;

void shutdown(HttpRequest *request)
{
	HttpResponse *response = create_response(request->client_fd, 200);
	WT_send_msg(response, "Shutting down");

	delete_response(response);
	pthread_cond_signal(&s);
}

void status_test(HttpRequest *request)
{
	HttpResponse *response = create_response(request->client_fd, 200);

	WT_send_status(response);

	delete_response(response);
}

void msg_test(HttpRequest *request)
{
	HttpResponse *response = create_response(request->client_fd, 200);

	WT_send_msg(response, "Hello");

	delete_response(response);
}

int main()
{
	if(WT_init(8080) != 0)
	{
		printf("Failed to initialize server, shutting down.\n");
		return -1;
	}

	WT_add_mapping("POST", "/shutdown", shutdown);
	WT_add_mapping("GET", "/status", status_test);
	WT_add_mapping("GET", "/msg", msg_test);

	// Adding pages
	WT_add_webpage("/test", "src/html/test.html");
	WT_add_webpage("/home", "src/html/home.html");
	WT_add_webpage("/WebTangle", "src/html/WebTangle.html");

	// Adding styles
	WT_add_file("/styles/home", "src/css/home.css");

	// Adding scripts
	WT_add_file("/test/date", "src/js/date.js");

	WT_add_file("/plaintext", "resources/text.txt");
	WT_add_file("/favicon.ico", "resources/bird.png");

	pthread_mutex_lock(&m);
	pthread_cond_wait(&s, &m);

	WT_shutdown();

	return 0;
}