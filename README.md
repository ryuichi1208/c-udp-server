# UDP Server in C

A simple UDP server implementation that loads configuration from YAML files and outputs logs in JSON format.

## Required Libraries

- libyaml
- jansson

## Installation

### On macOS

```
brew install libyaml jansson
```

### On Ubuntu

```
sudo apt-get install libyaml-dev libjansson-dev
```

## Build

```
make
```

## Run

```
./udp_server
```

## Docker Support

### Build Docker Image

```
docker build -t udp-server .
```

### Run with Docker

```
docker run -p 8888:8888/udp -v $(pwd)/config.yaml:/app/config.yaml -v $(pwd)/logs:/app/logs udp-server
```

### Run with Docker Compose

```
docker-compose up -d
```

## Configuration File

The following settings are available in the `config.yaml` file:

```yaml
server:
  port: 8888 # Port number to listen on
  buffer_size: 1024 # Buffer size for messages
  response_message: "Message received" # Response message to clients

socket_options:
  reuse_addr: true # Enable SO_REUSEADDR option
  receive_buffer: 8192 # Socket receive buffer size (SO_RCVBUF)
  send_buffer: 8192 # Socket send buffer size (SO_SNDBUF)
  broadcast: false # Enable broadcast (SO_BROADCAST)
  ttl: 64 # Time-to-live for packets (IP_TTL)
  receive_timeout: 0 # Receive timeout in seconds (0 = no timeout)

logging:
  file: "udp_server.log" # Log file name
  enable: true # Enable/disable logging
```
