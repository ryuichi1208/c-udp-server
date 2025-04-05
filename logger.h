#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <jansson.h>
#include <time.h>

/**
 * Initialize logging system
 *
 * @param log_file Path to the log file
 * @return FILE pointer to the opened log file, or NULL on failure
 */
FILE* init_logger(const char *log_file);

/**
 * Write a JSON formatted log entry
 *
 * @param log_fp File pointer to the log file
 * @param event_type Type of event being logged
 * @param message The message to log
 * @param client_ip Client IP address (can be NULL)
 * @param client_port Client port number (ignored if client_ip is NULL)
 */
void write_json_log(FILE *log_fp, const char *event_type, const char *message,
                   const char *client_ip, int client_port);

/**
 * Close the logger
 *
 * @param log_fp File pointer to the log file
 */
void close_logger(FILE *log_fp);

/**
 * Log server start event
 *
 * @param log_fp File pointer to the log file
 * @param port The port server is listening on
 */
void log_server_start(FILE *log_fp, int port);

/**
 * Log server stop event
 *
 * @param log_fp File pointer to the log file
 */
void log_server_stop(FILE *log_fp);

/**
 * Log received message event
 *
 * @param log_fp File pointer to the log file
 * @param message The received message
 * @param client_ip Client IP address
 * @param client_port Client port number
 */
void log_message_received(FILE *log_fp, const char *message,
                         const char *client_ip, int client_port);

/**
 * Log sent message event
 *
 * @param log_fp File pointer to the log file
 * @param message The sent message
 * @param client_ip Client IP address
 * @param client_port Client port number
 */
void log_message_sent(FILE *log_fp, const char *message,
                     const char *client_ip, int client_port);

/**
 * Log error event
 *
 * @param log_fp File pointer to the log file
 * @param error_message The error message
 * @param client_ip Client IP address (can be NULL)
 * @param client_port Client port number (ignored if client_ip is NULL)
 */
void log_error(FILE *log_fp, const char *error_message,
              const char *client_ip, int client_port);

#endif /* LOGGER_H */
