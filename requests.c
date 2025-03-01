#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count,char *token, int check)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    if (query_params != NULL && !check) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else if(!check) {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    if (query_params != NULL && check) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else if(check) {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }
    compute_message(message, line);
    sprintf(line,"Host: %s",host);
    compute_message(message, line);
    if (cookies != NULL) {
        for (int i = 0; i < cookies_count; i++) {
            strcat(message, "Cookie: ");
            strcat(message, cookies[i]);
            strcat(message, "\r\n");
        }
    }
    if(token!=NULL) {
        strcat(message,"Authorization: Bearer ");
        strcat(message,token);
        strcat(message,"\r\n");
    }
    compute_message(message, "");
    return message;
}
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count,char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    int message_length = 0;

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    message_length += strlen(line);

    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    message_length += strlen(line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
                in order to write Content-Length you must first compute the message size
    */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    message_length += strlen(line);

    int body_len = 0;
    for (int i = 0; i < body_data_fields_count; i++) {
        body_len += strlen(body_data[i]);
    }

    sprintf(line, "Content-Length: %d", body_len);
    compute_message(message, line);
    message_length += strlen(line);

    // Step 4 (optional): add cookies
    if (cookies != NULL) {
        for (int i = 0; i < cookies_count; i++) {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
            message_length += strlen(line);
        }
    }
    if(token!=NULL) {
        strcat(message,"Authorization: Bearer ");
        strcat(message,token);
        strcat(message,"\r\n");
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    for (int i = 0; i < body_data_fields_count; i++) {
        compute_message(message, body_data[i]);
        message_length += strlen(body_data[i]);
    }

    free(line);
    return message;
}

