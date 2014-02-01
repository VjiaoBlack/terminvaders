/* Defines */

#define PORT 1300
#define PROTOCOL_VER 1
#define NAME_LEN 32

#define CMD_QUIT 0
#define CMD_CONNECT 1
#define CMD_GETPID 2

#define ERR_FULL "The server is full."
#define ERR_SERVER_STOPPING "The server is shutting down."
#define ERR_UNAUTHORIZED "Only a local client can issue this command."
#define ERR_INVALID "Invalid command."
#define ERR_BAD_NAME "The name you entered is invalid."
#define ERR_KICKED "You have been kicked from the server."

/* Functions */

int make_connection(char*);
int transmit(int, int, char*);
int receive(int, int*, char**);
