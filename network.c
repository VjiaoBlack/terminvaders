#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "network.h"

#define HEADER_SIZE 8
#define HEADER_FMT "%02d%06d"

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
    int datalen = (data ? strlen(data) : 0) + 1, retval = 0, remainder;
    char *encoded = malloc(sizeof(char) * (HEADER_SIZE + datalen)), *unsent;

    /* Encode data into a message for sending. */
    snprintf(encoded, HEADER_SIZE + 1, HEADER_FMT, command, datalen);
    if (datalen > 1)
        snprintf(encoded + HEADER_SIZE, datalen, "%s", data);

    /* write() remaining encoded data until all is sent, or we get an error. */
    remainder = HEADER_SIZE + datalen;
    unsent = encoded;
    while (remainder) {
        retval = write(sockfd, unsent, remainder);
        if (retval < 0)
            goto cleanup;
        remainder -= retval;
        unsent += retval;
    }

    cleanup:
    free(encoded);
    return retval;
}

/* Receive a client->server or server->client message via sockfd. */
int receive(int sockfd, int* command, char** data) {
    char header[HEADER_SIZE];
    int nread, datalen, remainder;

    /* read() until we get the whole header, or an error. */
    remainder = HEADER_SIZE;
    while (remainder) {
        nread = read(sockfd, header + HEADER_SIZE - remainder, remainder);
        if (nread < 1)
            return -1;
        remainder -= nread;
    }

    /* Process the header and malloc() enough to hold the data segment. */
    if (sscanf(header, HEADER_FMT, command, &datalen) < 2)
        return -1;
    *data = malloc(sizeof(char) * datalen);

    /* Handle the no-data case for certain commands (CMD_STATUS, ...). */
    if (datalen == 1) {
        *data[0] = '\0';
        return 0;
    }

    /* read() until we get all data, or an error. */
    remainder = datalen;
    while (remainder) {
        nread = read(sockfd, *data + datalen - remainder, remainder);
        if (nread < 1)
            return -1;
        remainder -= nread;
    }
    return 0;
}
