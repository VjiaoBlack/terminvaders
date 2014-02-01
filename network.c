#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "network.h"

/* Make a connection to a server. Return the FD or -1 if there was an error. */
int make_connection(char* name) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("error: cannot create socket");
        return -1;
    }
    if ((server = gethostbyname(name)) == NULL) {
        perror("error: cannot gethostbyname()");
        return -1;
    }

    /* Create server address. */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);

    /* Connect to the server. */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        return -1;
    return sockfd;
}

/* Send a client->server or server->client message via sockfd. */
int transmit(int sockfd, int command, char* data) {
    int datalen = (data ? strlen(data) : 0) + 1, retval;
    char* encoded = malloc(sizeof(char) * (datalen + 16));

    if (datalen == 1)
        snprintf(encoded, 16, "%d|%d|", command, 1);
    else
        snprintf(encoded, datalen + 16, "%d|%d|%s", command, datalen, data);
    retval = write(sockfd, encoded, strlen(encoded) + 1);
    free(encoded);
    return retval;
}

/* Receive a client->server or server->client message via sockfd. */
int receive(int sockfd, int* command, char** data) {
    char buffer[1024], *tempbuf;
    int nread, datalen, offset, remainder;

    nread = read(sockfd, buffer, 1024);
    if (nread < 0)
        return -1;
    if (sscanf(buffer, "%d|%d|", command, &datalen) < 2)
        return -1;
    *data = malloc(sizeof(char) * datalen);
    offset = strcspn(buffer + strcspn(buffer, "|") + 1, "|") + 1;
    strncpy(*data, buffer + offset, datalen);
    if (nread < offset + datalen) {  /* We need to read more. */
        remainder = offset + datalen - nread;
        tempbuf = malloc(sizeof(char) * remainder);
        if (read(sockfd, tempbuf, remainder) < 0)
            return -1;
        strncpy(*data + nread - offset, tempbuf, remainder);
        free(tempbuf);
    }
    return 0;
}
