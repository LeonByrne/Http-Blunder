#ifndef REQUEST_H
#define REQUEST_H

typedef struct  \
{               \
	int client_fd;\
                \
	char *method; \
	char *path;   \
	char *version;\
	char *headers;\
	char *body;   \
} Request;


#endif