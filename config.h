#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <yaml.h>
#include "udp_server.h"

/**
 * Load server configuration from YAML file
 *
 * @param config_file Path to the configuration file
 * @return ServerConfig structure with loaded configuration
 */
ServerConfig load_config(const char *config_file);

/**
 * Print current configuration to stdout
 *
 * @param config The server configuration
 */
void print_config(const ServerConfig *config);

/**
 * Validate configuration values and set defaults if necessary
 *
 * @param config The server configuration to validate
 * @return 0 on success, -1 on validation error
 */
int validate_config(ServerConfig *config);

#endif /* CONFIG_H */
