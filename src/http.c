//TODO: add timeout for unreliable connections when reveiving

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "http.h"

#define BUF_SIZE 1024
#define REQ_LEN 53 // base length of request without url and page

Buffer *new_buffer(size_t size)
{
  Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));
  buffer->data = (char *)calloc(size, sizeof(char));
  buffer->length = size;
  return buffer;
}

void free_buffer(Buffer *buffer)
{
  return;
}

void append_buffer(Buffer *buffer, char *data, size_t length)
{
  return;
}

Buffer* http_query(char *host, char *page, int port)
{
  printf("Host: %s\n", host);
  printf("Page: %s\n", page);
  printf("Port: %i\n", port);

  char addr_port[20];
  int sockfd;
  int status;
  struct addrinfo hints, *servinfo;
  int file_size = 0; // The size of the entire http packet recieved
  Buffer *http_file = new_buffer(BUF_SIZE); // buffer for the http file
  char *request = NULL;
  int request_length;


  sprintf(addr_port, "%d", port);
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(host, addr_port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }

  sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  int rc = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
  if (rc == -1) {
    perror("connect");
    exit(1);
  }

  //"GET /sqlrest/CUSTOMER/3/ HTTP/1.0\r\nHost: www.thomas-bayer.com\r\nUser-Agent: downloader/1.0\r\n\r\n";
  if (page[0] == '/')
  {
    request_length = REQ_LEN + strlen(host) + strlen (page);
    request = (char*)calloc(request_length, sizeof(char));
    sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: downloader/1.0\r\n\r\n", page, host);
  }
  else
  {
    request_length = REQ_LEN + strlen(host) + strlen (page) + 1;
    request = (char*)calloc(request_length, sizeof(char));
    sprintf(request, "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: downloader/1.0\r\n\r\n", page, host);
  }
  printf("\n\n%s\n\n", request);

  int sent = 0;
  do {
  	rc = send(sockfd, request + sent, request_length - sent, 0);
  	if (rc == -1) {
  	  perror("send");
  	  exit(1);
  	}
  	sent += rc;
  } while (sent < request_length);



  //char buffer[BUF_SIZE];
  int rec_bits;
  do {
    rec_bits = recv(sockfd, http_file->data + file_size, BUF_SIZE, 0);
    file_size += rec_bits;

    if (file_size + BUF_SIZE > http_file->length)
    {
        http_file->length = http_file->length * 2;
        http_file->data = realloc(http_file->data, http_file->length);
    }
    printf("%d bytes recv\n", rec_bits);
  } while(rec_bits > 0);



  printf("RC: %i\n", rc);
  if (rc == -1) {
    perror("recv");
    exit(1);
  }

  //printf("Data: %s\n", http_file->data);

  freeaddrinfo(servinfo);
  close(sockfd);
  http_file->length = file_size;
  return http_file;
}

// split http content from the response string
char* http_get_content(Buffer *response) {

  char* header_end = strstr(response->data, "\r\n\r\n");

  if(header_end) {
    return header_end + 4;
  } else {
   return response->data;
  }
}


Buffer *http_url(const char *url) {
  char host[BUF_SIZE];
  strncpy(host, url, BUF_SIZE);

  char *page = strstr(host, "/");
  if(page) {
    page[0] = '\0';

    ++page;
    return http_query(host, page, 80);
  } else {

    fprintf(stderr, "could not split url into host/page %s\n", url);
    return NULL;
  }
}
