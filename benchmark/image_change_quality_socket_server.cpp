#include "socket.cpp"
#include "libjpeg_utils.cpp"

void process_read_jpeg(int client_fd) {
    // Receive input metadata
    uint32_t input_size;
    socket_recv(client_fd, (unsigned char*)&input_size, sizeof(input_size));

    // Receive input data
    unsigned char* input_buffer = (unsigned char*)malloc(input_size);
    ssize_t received = socket_recv(client_fd, input_buffer, input_size);
    printf("[Server] Recv: %zd bytes\n", received);

    // Process
    struct jpeg_parsed_data in_jpeg_data = read_jpeg(input_buffer, input_size);
    free(input_buffer);

    // Send response metadata
    uint32_t metadata[3];
    metadata[0] = in_jpeg_data.image_buffer_size;
    metadata[1] = in_jpeg_data.image_height;
    metadata[2] = in_jpeg_data.image_width;
    socket_send(client_fd, (unsigned char*)metadata, sizeof(metadata));

    // Send response
    ssize_t sent = socket_send(client_fd, in_jpeg_data.image_buffer, in_jpeg_data.image_buffer_size);
    printf("[Server] Sent: %zd bytes\n", sent);
}

void process_write_jpeg(int client_fd) {
    // Receive input metadata
    int quality;
    struct jpeg_parsed_data in_jpeg_data = {0};

    uint32_t input_metadata[4];
    socket_recv(client_fd, (unsigned char*)input_metadata, sizeof(input_metadata));
    quality = input_metadata[0];
    in_jpeg_data.image_buffer_size = input_metadata[1];
    in_jpeg_data.image_height = input_metadata[2];
    in_jpeg_data.image_width = input_metadata[3];

    // Receive input data
    in_jpeg_data.image_buffer = (JSAMPLE*)malloc(in_jpeg_data.image_buffer_size);
    ssize_t received = socket_recv(client_fd, in_jpeg_data.image_buffer, in_jpeg_data.image_buffer_size);
    printf("[Server] Recv: %zd bytes\n", received);

    // Process
    struct jpeg_parsed_data out_jpeg_data = write_jpeg(quality, in_jpeg_data);

    // Send response metadata
    uint32_t output_metadata[3];
    output_metadata[0] = out_jpeg_data.image_buffer_size;
    output_metadata[1] = out_jpeg_data.image_height;
    output_metadata[2] = out_jpeg_data.image_width;
    socket_send(client_fd, (unsigned char*)output_metadata, sizeof(output_metadata));

    // Send response
    ssize_t sent = socket_send(client_fd, out_jpeg_data.image_buffer, out_jpeg_data.image_buffer_size);
    printf("[Server] Sent: %zd bytes\n", sent);
}

int main() {
    int server_fd = socket_setup_server();
    int client_fd = accept(server_fd, NULL, NULL);  // Accept single connection
    if (client_fd < 0) {perror("accept");}

    process_read_jpeg(client_fd);

    process_write_jpeg(client_fd);
    
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    
    return 0;
}
