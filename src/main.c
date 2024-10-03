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
	WT_send_msg(request->client_fd, 200, "Shutting down");

	pthread_cond_signal(&s);
}


int main()
{
	if(WT_init(8080) != 0)
	{
		printf("Failed to initialize server, shutting down.");
		return -1;
	}

	WT_add_mapping("POST", "/shutdown", shutdown);

	WT_add_webpage("/test", "pages/test.html");
	WT_add_webpage("/home", "pages/home.html");
	WT_add_webpage("/WebTangle", "pages/WebTangle.html");

	WT_add_file("/plaintext", "resources/text.txt");
	WT_add_file("/favicon.ico", "resources/bird.png");

	pthread_mutex_lock(&m);
	pthread_cond_wait(&s, &m);

	WT_shutdown();

	return 0;
}