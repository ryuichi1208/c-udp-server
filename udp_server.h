#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <yaml.h>
#include <jansson.h>
#include <time.h>
#include <errno.h>
#include <netinet/ip.h>

// Default configuration
#define DEFAULT_PORT 8888
#define DEFAULT_BUFFER_SIZE 1024
#define DEFAULT_RESPONSE "Message received"
#define DEFAULT_LOG_FILE "udp_server.log"

// Default socket options
#define DEFAULT_REUSE_ADDR 1
#define DEFAULT_RCVBUF_SIZE 8192
#define DEFAULT_SNDBUF_SIZE 8192
#define DEFAULT_BROADCAST 0
#define DEFAULT_TTL 64
#define DEFAULT_RCVTIMEO 0

// Server configuration structure
typedef struct {
    int port;
    int buffer_size;
    char response_message[256];
    char log_file[256];
    int logging_enabled;

    // Socket options
    int reuse_addr;
    int receive_buffer;
    int send_buffer;
    int broadcast;
    int ttl;
    int receive_timeout;
} ServerConfig;

// Function declarations
ServerConfig load_config(const char *config_file);
int apply_socket_options(int sockfd, const ServerConfig *config, FILE *log_fp);
void write_json_log(FILE *log_fp, const char *event_type, const char *message,
                   const char *client_ip, int client_port);

#endif /* UDP_SERVER_H */
