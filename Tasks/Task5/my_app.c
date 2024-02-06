#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/mydevice"

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        perror("Failed to open the device");
        return -1;
    }

    // Reading from the kernel module
    char read_buffer[256];
    ssize_t bytes_read = read(fd, read_buffer, sizeof(read_buffer));
    if (bytes_read == -1) {
        perror("Failed to read from the device");
        close(fd);
        return -1;
    }

    printf("Read from the device: %s\n", read_buffer);

    // Writing to the kernel module
    const char* write_data = "New Message";
    ssize_t bytes_written = write(fd, write_data, strlen(write_data));
    if (bytes_written == -1) {
        perror("Failed to write to the device");
        close(fd);
        return -1;
    }

    printf("Write to the device successful\n");

    char read_buffer[256];
    ssize_t bytes_read = read(fd, read_buffer, sizeof(read_buffer));
    if (bytes_read == -1) {
        perror("Failed to read from the device");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}