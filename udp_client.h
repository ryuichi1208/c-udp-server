#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

// Default client settings
#define DEFAULT_CLIENT_TIMEOUT 5  // seconds
#define DEFAULT_CLIENT_BUFFER_SIZE 1024

/**
 * UDP Client configuration structure
 */
typedef struct {
    char server_ip[INET_ADDRSTRLEN];
    int server_port;
    int timeout_seconds;
    int buffer_size;
} ClientConfig;

/**
 * Initialize UDP client with default configuration
 *
 * @return ClientConfig structure with default settings
 */
ClientConfig init_client_config(void);

/**
 * Set server address for client
 *
 * @param config Client configuration
 * @param ip Server IP address
 * @param port Server port
 * @return 0 on success, -1 on failure
 */
int set_server_address(ClientConfig *config, const char *ip, int port);

/**
 * Create a UDP socket for client
 *
 * @return Socket file descriptor or -1 on error
 */
int create_client_socket(void);

/**
 * Set socket timeout for receiving responses
 *
 * @param sockfd Socket file descriptor
 * @param seconds Timeout in seconds
 * @return 0 on success, -1 on error
 */
int set_socket_timeout(int sockfd, int seconds);

/**
 * Send message to server and get response
 *
 * @param sockfd Socket file descriptor
 * @param config Client configuration
 * @param message Message to send
 * @param response Buffer to store response
 * @param response_size Size of response buffer
 * @return Number of bytes received or -1 on error
 */
ssize_t send_and_receive(int sockfd, const ClientConfig *config,
                        const char *message, char *response, size_t response_size);

#endif /* UDP_CLIENT_H */
