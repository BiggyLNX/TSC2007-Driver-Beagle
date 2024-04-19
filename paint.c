//v2
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
    int16_t z1;
    int16_t z2;
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
        uint8_t buf[8];
        if (read(i2c_fd, buf, sizeof(buf)) != sizeof(buf)) {
            perror("Error reading from I2C bus");
            return EXIT_FAILURE;
        }

        int16_t x = (buf[0] << 8) | buf[1];
        int16_t y = (buf[2] << 8) | buf[3];
        int16_t z1 = (buf[4] << 8) | buf[5];
        int16_t z2 = (buf[6] << 8) | buf[7];

        if (z1 > 0 && z2 > 0) { // Check if touch detected
            printf("Touch point: (X: %d, Y: %d, Z1: %d, Z2: %d)\n", x, y, z1, z2);
        }

        usleep(10000); // Short delay before next read
    }

    close(i2c_fd);
    return EXIT_SUCCESS;
}
