FROM debian:bullseye-slim

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    libyaml-dev \
    libjansson-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy source files
COPY . .

# Build the application
RUN make

# Expose UDP port
EXPOSE 8888/udp

# Run the UDP server
CMD ["./udp_server"]
