#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// Define I2C address of the TSC2007-Q1 device
#define TSC2007_ADDRESS 0x48

// Function to read X and Y coordinates from the TSC2007-Q1
void readCoordinates(int file, uint8_t command, int *value) {
    uint8_t data[2];

    // Write command byte to specify the operation
    if (write(file, &command, 1) != 1) {
        perror("Write failed");
        exit(1);
    }

    // Read 2 bytes of data
    if (read(file, data, 2) != 2) {
        perror("Read failed");
        exit(1);
    }

    // Form 12-bit value
    *value = (data[0] << 8) | data[1];
}

int main() {
    char *device = "/dev/i2c-5";
    int file;
    int x, y;

    // Open the I2C bus
    if ((file = open(device, O_RDWR)) < 0) {
        perror("Failed to open the bus.");
        exit(1);
    }

    // Set the I2C slave address
    if (ioctl(file, I2C_SLAVE, TSC2007_ADDRESS) < 0) {
        perror("Failed to acquire bus access and/or talk to slave.");
        exit(1);
    }

    while(1) {
        // Read X coordinate
        readCoordinates(file, 0b11001000, &x); // Address for reading X coordinate

        // Read Y coordinate
        readCoordinates(file, 0b11001001, &y); // Address for reading Y coordinate

       
        int x_percent = (x * 100) / 4096; 
        int y_percent = (y * 100) / 4096; 

        // Print the coordinates without the % symbol
        printf("Touch position: X=%d, Y=%d\n", x_percent, y_percent);


        usleep(100000); 
    }

    close(file);

    return 0;
}
