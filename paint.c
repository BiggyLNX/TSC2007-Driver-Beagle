#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// Define I2C address of the TSC2007-Q1 device
#define TSC2007_ADDRESS 0x48

// Define screen dimensions
#define SCREEN_WIDTH 1000  // Width of the screen
#define SCREEN_HEIGHT 1000 // Height of the screen

// Read X and Y coordinates from the TSC2007-Q1
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

    // Create the 12-bit value
    *value = (data[0] << 8) | data[1];
}

void mapCoordinates(int x_plus, int y_plus, int *mapped_x, int *mapped_y) {
    // Calculate mapped X coordinate
    *mapped_x = (x_plus * SCREEN_WIDTH) / 4095; // Adjusted denominator to 4095

    // Calculate mapped Y coordinate
    *mapped_y = SCREEN_HEIGHT - ((y_plus * SCREEN_HEIGHT) / 4095); // Adjusted denominator to 4095 and inverted Y axis
}


int main() {
    char *device = "/dev/i2c-5";
    int file;
    int x_plus, y_plus;

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
        // Read X+ coordinate
        readCoordinates(file, 0b11001000, &x_plus); // Address for reading X+ coordinate

        // Read Y+ coordinate
        readCoordinates(file, 0b11001001, &y_plus); // Address for reading Y+ coordinate

        // Map touch coordinates to Cartesian plane
        int mapped_x, mapped_y;
        mapCoordinates(x_plus, y_plus, &mapped_x, &mapped_y);


        mapped_y = SCREEN_HEIGHT - mapped_y;

        // Print the mapped coordinates
        printf("Mapped Touch position: X=%d, Y=%d\n", mapped_x, mapped_y);


        usleep(100000); 
    }

    close(file);

    return 0;
}
