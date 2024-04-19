//v1
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

#define I2C_DEVICE "/dev/i2c-5"
#define TSC2007_ADDR 0x48

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} TS_Point;

int main() {
    int i2c_fd = open(I2C_DEVICE, O_RDWR);
    if (i2c_fd < 0) {
        perror("Failed to open the I2C bus");
        return EXIT_FAILURE;
    }

    if (ioctl(i2c_fd, I2C_SLAVE, TSC2007_ADDR) < 0) {
        perror("Failed to set I2C address");
        return EXIT_FAILURE;
    }

    while (1) {
        uint8_t buf[2];
        if (read(i2c_fd, buf, sizeof(buf)) != sizeof(buf)) {
            perror("Error reading       from I2C bus");
            return EXIT_FAILURE;
        }

        int16_t x = (buf[0] << 8) | buf[1];

        if (x != 0) { // Check if touch coordinates have changed
            printf("X: %d\n", x);
        }

        usleep(10000); // Short delay before next read
    }

    close(i2c_fd);
    return EXIT_SUCCESS;
}

