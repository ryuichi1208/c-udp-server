#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include "udp_server.h"

/**
 * Create and initialize a UDP socket
 *
 * @return Socket file descriptor or -1 on error
 */
int create_udp_socket(void);

/**
 * Apply socket options based on configuration
 *
 * @param sockfd Socket file descriptor
 * @param config Server configuration containing socket options
 * @param log_fp Log file pointer (can be NULL)
 * @return 0 on success, -1 on error
 */
int apply_socket_options(int sockfd, const ServerConfig *config, FILE *log_fp);

/**
 * Bind socket to address and port
 *
 * @param sockfd Socket file descriptor
 * @param port Port number to bind to
 * @param log_fp Log file pointer (can be NULL)
 * @return 0 on success, -1 on error
 */
int bind_socket(int sockfd, int port, FILE *log_fp);

/**
 * Get client information as string
 *
 * @param addr Client address structure
 * @param ip_str Buffer to store IP address string
 * @param ip_str_size Size of ip_str buffer
 * @param port Pointer to store port number
 * @return 0 on success, -1 on error
 */
int get_client_info(const struct sockaddr_in *addr, char *ip_str,
                   size_t ip_str_size, int *port);

/**
 * Receive message from client
 *
 * @param sockfd Socket file descriptor
 * @param buffer Buffer to store received message
 * @param buffer_size Size of buffer
 * @param client_addr Pointer to store client address
 * @param client_len Pointer to store client address length
 * @return Number of bytes received or -1 on error
 */
ssize_t receive_message(int sockfd, char *buffer, size_t buffer_size,
                       struct sockaddr_in *client_addr, socklen_t *client_len);

/**
 * Send message to client
 *
 * @param sockfd Socket file descriptor
 * @param message Message to send
 * @param client_addr Client address
 * @param client_len Client address length
 * @return Number of bytes sent or -1 on error
 */
ssize_t send_message(int sockfd, const char *message,
                    const struct sockaddr_in *client_addr, socklen_t client_len);

#endif /* SOCKET_UTILS_H */
