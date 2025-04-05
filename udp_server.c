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

// Function to write JSON logs
void write_json_log(FILE *log_fp, const char *event_type, const char *message,
                   const char *client_ip, int client_port) {
    json_t *log_entry = json_object();

    // Get timestamp
    time_t now = time(NULL);
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // Build JSON object
    json_object_set_new(log_entry, "timestamp", json_string(timestamp));
    json_object_set_new(log_entry, "event", json_string(event_type));
    json_object_set_new(log_entry, "message", json_string(message));

    if (client_ip != NULL) {
        json_t *client = json_object();
        json_object_set_new(client, "ip", json_string(client_ip));
        json_object_set_new(client, "port", json_integer(client_port));
        json_object_set_new(log_entry, "client", client);
    }

    // Dump JSON
    char *json_str = json_dumps(log_entry, JSON_INDENT(2));
    fprintf(log_fp, "%s\n", json_str);
    fflush(log_fp);

    // Cleanup
    free(json_str);
    json_decref(log_entry);
}

// Function to apply socket options
int apply_socket_options(int sockfd, const ServerConfig *config, FILE *log_fp) {
    // Set socket option SO_REUSEADDR
    int result;
    if (config->reuse_addr) {
        int optval = 1;
        result = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        if (result < 0) {
            perror("Failed to set SO_REUSEADDR");
            if (log_fp) {
                write_json_log(log_fp, "socket_error", "Failed to set SO_REUSEADDR", NULL, 0);
            }
            return -1;
        }
        printf("Set SO_REUSEADDR: enabled\n");
    }

    // Set receive buffer size (SO_RCVBUF)
    if (config->receive_buffer > 0) {
        result = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &config->receive_buffer, sizeof(config->receive_buffer));
        if (result < 0) {
            perror("Failed to set SO_RCVBUF");
            if (log_fp) {
                write_json_log(log_fp, "socket_error", "Failed to set receive buffer size", NULL, 0);
            }
            return -1;
        }
        printf("Set SO_RCVBUF: %d bytes\n", config->receive_buffer);
    }

    // Set send buffer size (SO_SNDBUF)
    if (config->send_buffer > 0) {
        result = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &config->send_buffer, sizeof(config->send_buffer));
        if (result < 0) {
            perror("Failed to set SO_SNDBUF");
            if (log_fp) {
                write_json_log(log_fp, "socket_error", "Failed to set send buffer size", NULL, 0);
            }
            return -1;
        }
        printf("Set SO_SNDBUF: %d bytes\n", config->send_buffer);
    }

    // Set broadcast option (SO_BROADCAST)
    if (config->broadcast) {
        int optval = 1;
        result = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
        if (result < 0) {
            perror("Failed to set SO_BROADCAST");
            if (log_fp) {
                write_json_log(log_fp, "socket_error", "Failed to enable broadcast", NULL, 0);
            }
            return -1;
        }
        printf("Set SO_BROADCAST: enabled\n");
    }

    // Set IP Time-To-Live (IP_TTL)
    if (config->ttl > 0) {
        result = setsockopt(sockfd, IPPROTO_IP, IP_TTL, &config->ttl, sizeof(config->ttl));
        if (result < 0) {
            perror("Failed to set IP_TTL");
            if (log_fp) {
                write_json_log(log_fp, "socket_error", "Failed to set TTL", NULL, 0);
            }
            return -1;
        }
        printf("Set IP_TTL: %d\n", config->ttl);
    }

    // Set receive timeout (SO_RCVTIMEO)
    if (config->receive_timeout > 0) {
        struct timeval tv;
        tv.tv_sec = config->receive_timeout;
        tv.tv_usec = 0;

        result = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        if (result < 0) {
            perror("Failed to set SO_RCVTIMEO");
            if (log_fp) {
                write_json_log(log_fp, "socket_error", "Failed to set receive timeout", NULL, 0);
            }
            return -1;
        }
        printf("Set SO_RCVTIMEO: %d seconds\n", config->receive_timeout);
    }

    return 0;
}

// Function to load configuration from YAML file
ServerConfig load_config(const char *config_file) {
    ServerConfig config;
    // Set default values
    config.port = DEFAULT_PORT;
    config.buffer_size = DEFAULT_BUFFER_SIZE;
    strcpy(config.response_message, DEFAULT_RESPONSE);
    strcpy(config.log_file, DEFAULT_LOG_FILE);
    config.logging_enabled = 1;

    // Set default socket options
    config.reuse_addr = DEFAULT_REUSE_ADDR;
    config.receive_buffer = DEFAULT_RCVBUF_SIZE;
    config.send_buffer = DEFAULT_SNDBUF_SIZE;
    config.broadcast = DEFAULT_BROADCAST;
    config.ttl = DEFAULT_TTL;
    config.receive_timeout = DEFAULT_RCVTIMEO;

    FILE *fh = fopen(config_file, "r");
    if (!fh) {
        fprintf(stderr, "Cannot open configuration file %s. Using default settings.\n", config_file);
        return config;
    }

    yaml_parser_t parser;
    yaml_event_t event;

    // Initialize parser
    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Failed to initialize YAML parser.\n");
        fclose(fh);
        return config;
    }

    yaml_parser_set_input_file(&parser, fh);

    char key[256] = {0};
    int in_server_section = 0;
    int in_logging_section = 0;
    int in_socket_options_section = 0;
    int done = 0;

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            fprintf(stderr, "Parse error\n");
            break;
        }

        switch(event.type) {
            case YAML_SCALAR_EVENT:
                if (strcmp((char *)event.data.scalar.value, "server") == 0) {
                    in_server_section = 1;
                    in_logging_section = 0;
                    in_socket_options_section = 0;
                } else if (strcmp((char *)event.data.scalar.value, "logging") == 0) {
                    in_server_section = 0;
                    in_logging_section = 1;
                    in_socket_options_section = 0;
                } else if (strcmp((char *)event.data.scalar.value, "socket_options") == 0) {
                    in_server_section = 0;
                    in_logging_section = 0;
                    in_socket_options_section = 1;
                } else if (in_server_section && strcmp(key, "") == 0) {
                    strcpy(key, (char *)event.data.scalar.value);
                } else if (in_server_section && strcmp(key, "port") == 0) {
                    config.port = atoi((char *)event.data.scalar.value);
                    key[0] = '\0';
                } else if (in_server_section && strcmp(key, "buffer_size") == 0) {
                    config.buffer_size = atoi((char *)event.data.scalar.value);
                    key[0] = '\0';
                } else if (in_server_section && strcmp(key, "response_message") == 0) {
                    strcpy(config.response_message, (char *)event.data.scalar.value);
                    key[0] = '\0';
                } else if (in_logging_section && strcmp(key, "") == 0) {
                    strcpy(key, (char *)event.data.scalar.value);
                } else if (in_logging_section && strcmp(key, "file") == 0) {
                    strcpy(config.log_file, (char *)event.data.scalar.value);
                    key[0] = '\0';
                } else if (in_logging_section && strcmp(key, "enable") == 0) {
                    config.logging_enabled = strcmp((char *)event.data.scalar.value, "true") == 0 ? 1 : 0;
                    key[0] = '\0';
                } else if (in_socket_options_section && strcmp(key, "") == 0) {
                    strcpy(key, (char *)event.data.scalar.value);
                } else if (in_socket_options_section && strcmp(key, "reuse_addr") == 0) {
                    config.reuse_addr = strcmp((char *)event.data.scalar.value, "true") == 0 ? 1 : 0;
                    key[0] = '\0';
                } else if (in_socket_options_section && strcmp(key, "receive_buffer") == 0) {
                    config.receive_buffer = atoi((char *)event.data.scalar.value);
                    key[0] = '\0';
                } else if (in_socket_options_section && strcmp(key, "send_buffer") == 0) {
                    config.send_buffer = atoi((char *)event.data.scalar.value);
                    key[0] = '\0';
                } else if (in_socket_options_section && strcmp(key, "broadcast") == 0) {
                    config.broadcast = strcmp((char *)event.data.scalar.value, "true") == 0 ? 1 : 0;
                    key[0] = '\0';
                } else if (in_socket_options_section && strcmp(key, "ttl") == 0) {
                    config.ttl = atoi((char *)event.data.scalar.value);
                    key[0] = '\0';
                } else if (in_socket_options_section && strcmp(key, "receive_timeout") == 0) {
                    config.receive_timeout = atoi((char *)event.data.scalar.value);
                    key[0] = '\0';
                }
                break;
            case YAML_MAPPING_END_EVENT:
                if (in_server_section) {
                    in_server_section = 0;
                } else if (in_logging_section) {
                    in_logging_section = 0;
                } else if (in_socket_options_section) {
                    in_socket_options_section = 0;
                }
                break;
            case YAML_STREAM_END_EVENT:
                done = 1;
                break;
            default:
                break;
        }

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(fh);

    printf("Configuration loaded: Port=%d, Buffer size=%d, Response message=%s\n",
           config.port, config.buffer_size, config.response_message);
    printf("Log settings: File=%s, Enabled=%s\n",
           config.log_file, config.logging_enabled ? "yes" : "no");
    printf("Socket options: REUSEADDR=%s, RCVBUF=%d, SNDBUF=%d, BROADCAST=%s, TTL=%d, RCVTIMEO=%d\n",
           config.reuse_addr ? "yes" : "no",
           config.receive_buffer,
           config.send_buffer,
           config.broadcast ? "yes" : "no",
           config.ttl,
           config.receive_timeout);

    return config;
}

int main() {
    // Load configuration from file
    ServerConfig config = load_config("config.yaml");

    // Open log file
    FILE *log_fp = NULL;
    if (config.logging_enabled) {
        log_fp = fopen(config.log_file, "a");
        if (!log_fp) {
            fprintf(stderr, "Cannot open log file %s. Logging is disabled.\n", config.log_file);
        } else {
            write_json_log(log_fp, "server_start", "Server started", NULL, 0);
        }
    }

    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    char *buffer = malloc(config.buffer_size);
    if (!buffer) {
        perror("Memory allocation failed");
        if (log_fp) {
            write_json_log(log_fp, "error", "Memory allocation failed", NULL, 0);
            fclose(log_fp);
        }
        exit(EXIT_FAILURE);
    }

    socklen_t client_len = sizeof(client_addr);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        free(buffer);
        if (log_fp) {
            write_json_log(log_fp, "error", "Socket creation failed", NULL, 0);
            fclose(log_fp);
        }
        exit(EXIT_FAILURE);
    }

    // Apply socket options
    if (apply_socket_options(server_fd, &config, log_fp) < 0) {
        fprintf(stderr, "Failed to apply some socket options. Continuing with defaults.\n");
        if (log_fp) {
            write_json_log(log_fp, "warning", "Failed to apply some socket options", NULL, 0);
        }
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config.port);

    // Bind socket
    if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        free(buffer);
        if (log_fp) {
            write_json_log(log_fp, "error", "Socket bind failed", NULL, 0);
            fclose(log_fp);
        }
        exit(EXIT_FAILURE);
    }

    printf("UDP server started. Listening on port %d...\n", config.port);

    while (1) {
        // Receive message from client
        int len = recvfrom(server_fd, buffer, config.buffer_size, 0,
                         (struct sockaddr *)&client_addr, &client_len);

        if (len < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // This is a timeout case - we can handle it if needed
                printf("Receive timeout occurred\n");
                if (log_fp) {
                    write_json_log(log_fp, "timeout", "Receive timeout occurred", NULL, 0);
                }
                continue;
            }

            perror("Receive error");
            if (log_fp) {
                write_json_log(log_fp, "error", "Failed to receive message", NULL, 0);
            }
            continue;
        }

        buffer[len] = '\0';
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);

        printf("Message from client %s:%d: %s\n",
               client_ip, client_port, buffer);

        if (log_fp) {
            write_json_log(log_fp, "message_received", buffer, client_ip, client_port);
        }

        // Send response to client
        sendto(server_fd, config.response_message, strlen(config.response_message), 0,
               (const struct sockaddr *)&client_addr, client_len);

        if (log_fp) {
            write_json_log(log_fp, "message_sent", config.response_message, client_ip, client_port);
        }
    }

    free(buffer);
    close(server_fd);
    if (log_fp) {
        write_json_log(log_fp, "server_stop", "Server stopped", NULL, 0);
        fclose(log_fp);
    }
    return 0;
}
