#include "udp_client.h"

ClientConfig init_client_config(void) {
    ClientConfig config;
    strcpy(config.server_ip, "127.0.0.1");  // Default to localhost
    config.server_port = 8888;              // Default port
    config.timeout_seconds = DEFAULT_CLIENT_TIMEOUT;
    config.buffer_size = DEFAULT_CLIENT_BUFFER_SIZE;
    return config;
}

int set_server_address(ClientConfig *config, const char *ip, int port) {
    if (!config || !ip || port <= 0 || port > 65535) {
        return -1;
    }

    strncpy(config->server_ip, ip, INET_ADDRSTRLEN);
    config->server_ip[INET_ADDRSTRLEN - 1] = '\0';  // Ensure null termination
    config->server_port = port;
    return 0;
}

int create_client_socket(void) {
    return socket(AF_INET, SOCK_DGRAM, 0);
}

int set_socket_timeout(int sockfd, int seconds) {
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

ssize_t send_and_receive(int sockfd, const ClientConfig *config,
                        const char *message, char *response, size_t response_size) {
    if (!config || !message || !response || response_size == 0) {
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->server_port);

    if (inet_pton(AF_INET, config->server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    // Send message to server
    ssize_t sent = sendto(sockfd, message, strlen(message), 0,
                         (const struct sockaddr *)&server_addr,
                         sizeof(server_addr));
    if (sent < 0) {
        perror("Send failed");
        return -1;
    }

    // Receive response
    socklen_t server_len = sizeof(server_addr);
    ssize_t received = recvfrom(sockfd, response, response_size - 1, 0,
                               (struct sockaddr *)&server_addr, &server_len);

    if (received >= 0) {
        response[received] = '\0';  // Null terminate the response
    }

    return received;
}

int main(int argc, char *argv[]) {
    // Parse command line arguments
    const char *server_ip = "127.0.0.1";  // Default to localhost
    int server_port = 8888;               // Default port
    const char *message = "Hello, UDP Server!"; // Default message

    if (argc > 1) {
        server_ip = argv[1];
    }

    if (argc > 2) {
        server_port = atoi(argv[2]);
        if (server_port <= 0 || server_port > 65535) {
            fprintf(stderr, "Invalid port number. Using default port 8888.\n");
            server_port = 8888;
        }
    }

    if (argc > 3) {
        message = argv[3];
    }

    // Initialize client configuration
    ClientConfig config = init_client_config();
    set_server_address(&config, server_ip, server_port);

    // Create socket
    int sockfd = create_client_socket();
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set socket timeout
    if (set_socket_timeout(sockfd, config.timeout_seconds) < 0) {
        perror("Socket timeout setting failed");
        // Continue anyway
    }

    // Allocate response buffer
    char *response = malloc(config.buffer_size);
    if (!response) {
        perror("Memory allocation failed");
        close(sockfd);
        return 1;
    }

    printf("Sending message to %s:%d: \"%s\"\n",
           config.server_ip, config.server_port, message);

    // Send message and get response
    ssize_t received = send_and_receive(sockfd, &config, message,
                                      response, config.buffer_size);

    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            fprintf(stderr, "Timeout waiting for response\n");
        } else {
            perror("Failed to receive response");
        }
    } else {
        printf("Response from server: \"%s\"\n", response);
    }

    // Clean up
    free(response);
    close(sockfd);

    return (received >= 0) ? 0 : 1;
}
