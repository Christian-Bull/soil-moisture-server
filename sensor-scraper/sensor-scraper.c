#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<syslog.h>
#include <string.h>
#include <time.h>

#define I2C_DEV_PATH "/dev/i2c-1"
#define ADS1115_ADDR 0x48     // Default I2C address
#define ADS1115_CONVERSION_REG 0x00
#define ADS1115_CONFIG_REG     0x01

// reads the current raw and voltage value from a capacative soil moisture sensor
// and outputs the reading to their respective files as outlined below
// overwrites the current data in the file

int main(void)
{
    const char *fileNameRaw = "/var/soil/raw_current";
    const char *fileNameVolt = "/var/soil/voltage_current";
    ssize_t nr;

    int fd = open(I2C_DEV_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return EXIT_FAILURE;
    }

    if (ioctl(fd, I2C_SLAVE, ADS1115_ADDR) < 0) {
        perror("Failed to select I2C device");
        close(fd);
        return EXIT_FAILURE;
    }

    // Configure ADS1115 for single-ended input A0, ±4.096V range, 128 SPS
    uint16_t config = 0xC383;  // OS=1 (start single conv), MUX=100 (AIN0/GND), PGA=001 (+/-4.096V), MODE=1, DR=100 (128 SPS)
    uint8_t config_bytes[3];
    config_bytes[0] = ADS1115_CONFIG_REG;
    config_bytes[1] = (config >> 8) & 0xFF;
    config_bytes[2] = config & 0xFF;

    if (write(fd, config_bytes, 3) != 3) {
        perror("Failed to write config");
        close(fd);
        return EXIT_FAILURE;
    }

    // Wait for conversion (~8 ms @128 SPS)
    usleep(10000);

    // Read conversion result (2 bytes)
    uint8_t reg = ADS1115_CONVERSION_REG;
    if (write(fd, &reg, 1) != 1) {
        perror("Failed to set pointer register");
        close(fd);
        return EXIT_FAILURE;
    }

    uint8_t data[2];
    if (read(fd, data, 2) != 2) {
        perror("Failed to read conversion");
        close(fd);
        return EXIT_FAILURE;
    }

    int16_t raw = (data[0] << 8) | data[1];
    double voltage = raw * 4.096 / 32768.0;  // Convert to volts (PGA ±4.096 V)

    close(fd);

    // end of getting data from sensor

    // update raw file
    
    // format write to file
    char buf[128];
    snprintf(buf, sizeof(buf), "%d\n", raw);

    const char *text = buf;

    fd = open (fileNameRaw, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
            perror ("open");
            return -1;
    }

    nr = write(fd, text, strlen(text));
    if (nr == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    if (close (fd) == -1) {
        perror ("close");
    }

    // update voltage file
    snprintf(buf, sizeof(buf), "%f\n", voltage);

    fd = open (fileNameVolt, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
            perror ("open");
            return -1;
    }

    nr = write(fd, text, strlen(text));
    if (nr == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    if (close (fd) == -1) {
        perror ("close");
    }
 

    return EXIT_SUCCESS;
}
