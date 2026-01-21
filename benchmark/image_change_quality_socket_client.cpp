#include <chrono>

#include "test_bytes.h"
#include "socket.cpp"
#include "libjpeg_utils.cpp"

#define INPUT_SIZE (sizeof(inputData) - 1)
#define OUTPUT_SIZE (sizeof(outputData) - 1)

using namespace std::chrono;

void ipc_read_jpeg(int server_fd, struct jpeg_parsed_data& in_jpeg_data) {
    // Send input metadata
    uint32_t input_size = INPUT_SIZE;
    socket_send(server_fd, (unsigned char*)&input_size, sizeof(input_size));
    
    // Send input data
    ssize_t sent = socket_send(server_fd, inputData, INPUT_SIZE);
    // printf("[Client] Sent: %zd bytes\n", sent);

    // Receive response metadata
    uint32_t metadata[3];
    socket_recv(server_fd, (unsigned char*)metadata, sizeof(metadata));
    in_jpeg_data.image_buffer_size = metadata[0];
    in_jpeg_data.image_height = metadata[1];
    in_jpeg_data.image_width = metadata[2];

    // Receive response
    in_jpeg_data.image_buffer = (JSAMPLE*)malloc(in_jpeg_data.image_buffer_size);
    ssize_t received = socket_recv(server_fd, in_jpeg_data.image_buffer, in_jpeg_data.image_buffer_size);
    // printf("[Client] Recv: %zd bytes\n", received);
}

void ipc_write_jpeg(int server_fd, int quality, struct jpeg_parsed_data& in_jpeg_data, struct jpeg_parsed_data& out_jpeg_data) {
    // Send input metadata
    uint32_t input_metadata[4];
    input_metadata[0] = quality;
    input_metadata[1] = in_jpeg_data.image_buffer_size;
    input_metadata[2] = in_jpeg_data.image_height;
    input_metadata[3] = in_jpeg_data.image_width;
    socket_send(server_fd, (unsigned char*)input_metadata, sizeof(input_metadata));

    // Send input data
    ssize_t sent = socket_send(server_fd, in_jpeg_data.image_buffer, in_jpeg_data.image_buffer_size);
    // printf("[Client] Sent: %zd bytes\n", sent);

    // Receive response metadata
    uint32_t output_metadata[3];
    socket_recv(server_fd, (unsigned char*)output_metadata, sizeof(output_metadata));
    out_jpeg_data.image_buffer_size = output_metadata[0];
    out_jpeg_data.image_height = output_metadata[1];
    out_jpeg_data.image_width = output_metadata[2];

    // Receive response
    out_jpeg_data.image_buffer = (JSAMPLE*)malloc(out_jpeg_data.image_buffer_size);
    ssize_t received = socket_recv(server_fd, out_jpeg_data.image_buffer, out_jpeg_data.image_buffer_size);
    // printf("[Client] Recv: %zd bytes\n", received);
}

int main() {
    int server_fd = socket_setup_client();    

    auto enter_time = high_resolution_clock::now();

    ////////// libjpeg calls //////////

    struct jpeg_parsed_data in_jpeg_data = {0};
    struct jpeg_parsed_data out_jpeg_data = {0};

    for (int i = 0; i < TEST_ITERATIONS; i++) {
        if (in_jpeg_data.image_buffer) {
            free(in_jpeg_data.image_buffer);
        }
        if (out_jpeg_data.image_buffer) {
            free(out_jpeg_data.image_buffer);
        }
        ipc_read_jpeg(server_fd, in_jpeg_data);
        ipc_write_jpeg(server_fd, 30, in_jpeg_data, out_jpeg_data);
    }

    ///////////////////////////////////

    auto exit_time = high_resolution_clock::now();
    
    // Validation
    RELEASE_ASSERT(OUTPUT_SIZE == out_jpeg_data.image_buffer_size, "Size mismatch");

    for(unsigned long i = 0; i < OUTPUT_SIZE; i++) {
        if (out_jpeg_data.image_buffer[i] != outputData[i]) {
            printf("Output data doesn't match at index: %lu!\n", i);
            exit(1);
        }
    }

    int64_t ns = duration_cast<nanoseconds>(exit_time - enter_time).count();
    printf("JPEG recoding time: %lld\n", (long long) (ns / TEST_ITERATIONS));

    if (in_jpeg_data.image_buffer) {
        free(in_jpeg_data.image_buffer);
    }
    if (out_jpeg_data.image_buffer) {
        free(out_jpeg_data.image_buffer);
    }
    close(server_fd);
    
    return 0;
}
