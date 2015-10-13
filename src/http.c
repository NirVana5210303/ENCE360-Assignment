/*
Author: Gregory Kampjes

TODO: add timeout for unreliable connections when rxing
*/

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

// Clear the contents allocated to buffer, then clear buffer itself
void clear_buffer(Buffer *buff)
{
  free(buff->data);
  free(buff);
}

Buffer* http_query(char *host, char *page, int port)
{
  char addr_port[20];
  int sockfd;
  int status;
  struct addrinfo hints, *servinfo;
  int file_size = 0; // The size of the entire http packet recieved
  Buffer *http_file = new_buffer(BUF_SIZE); // buffer for the http file
  char *request = NULL;
  int request_length;

  void exit_error(void) // function to clear memory in case of error
  {
    free(request);
    clear_buffer(http_file);
    freeaddrinfo(servinfo);
    free(&hints);
    exit(1);
  }

  sprintf(addr_port, "%d", port);
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(host, addr_port, &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit_error();
  }
  sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  int rc = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
  if (rc == -1)
  {
    perror("connect");
    exit_error();
  }

  // "GET /sqlrest/CUSTOMER/3/ HTTP/1.0\r\nHost: www.thomas-bayer.com\r\nUser-Agent: downloader/1.0\r\n\r\n";
  if (page[0] == '/')
  {
    request_length = REQ_LEN + strlen(host) + strlen (page);
    request = (char*)calloc(request_length, sizeof(char));
    sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: downloader/1.0\r\n\r\n", page, host);
  }
  else // Due to the way that the strings are split in memory, it can lose a slash
  {
    request_length = REQ_LEN + strlen(host) + strlen (page) + 1;
    request = (char*)calloc(request_length, sizeof(char));
    sprintf(request, "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: downloader/1.0\r\n\r\n", page, host);
  }

  int sent = 0;
  do // Just in case the request gets fragmented when being sent
  {
  	sent = send(sockfd, request + sent, request_length - sent, 0);
  	if (sent == -1)
    {
      close(sockfd);
  	  perror("send");
  	  exit_error();
  	}
  	sent += sent;
  } while (sent < request_length);

  int recv_bits;
  do
  {
    recv_bits = recv(sockfd, http_file->data + file_size, BUF_SIZE, 0);
    file_size += recv_bits;

    if (file_size + BUF_SIZE > http_file->length)
    {
      // Someone smarter than me said that you should double allocated size when
      // resizing. I believe them.
      http_file->length = http_file->length * 2;
      http_file->data = realloc(http_file->data, http_file->length);
    }
  } while(recv_bits > 0); // When there is no data left, it will recv 0 bits

  if (recv_bits == -1) // Something went wrong
  {
    perror("recv");
    exit_error();
  }

  // Cleanup
  close(sockfd);
  free(request);
  freeaddrinfo(servinfo);
  /*
  Setting the length of the length to the size of the file, so that the
  downloader doesn't break.
  */
  http_file->length = file_size;
  return http_file;
}

// split http content from the response string
char* http_get_content(Buffer *response)
{
  char* header_end = strstr(response->data, "\r\n\r\n");

  if(header_end)
  {
    return header_end + 4;
  }
  else
  {
   return response->data;
  }
}


Buffer *http_url(const char *url)
{
  char host[BUF_SIZE];
  strncpy(host, url, BUF_SIZE);

  char *page = strstr(host, "/");
  if(page)
  {
    page[0] = '\0';
    ++page;
    return http_query(host, page, 80);
  }
  else
  {
    fprintf(stderr, "could not split url into host/page %s\n", url);
    return NULL;
  }
}
