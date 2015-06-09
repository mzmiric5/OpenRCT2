#ifndef _HTTP_H_
#define _HTTP_H_

#include <jansson/jansson.h>
#include "../common.h"

typedef struct {
	int status_code;
	json_t *root;
} http_json_response;

void http_init();
void http_dispose();
http_json_response *http_request_json(const char *url);
void http_request_json_async(const char *url, void (*callback)(http_json_response*));
http_json_response *http_post_json(const char *url, const char *data);
void http_post_json_async(const char *url, const char *data, void(*callback)(http_json_response*));
void http_request_json_dispose(http_json_response *response);

#endif
