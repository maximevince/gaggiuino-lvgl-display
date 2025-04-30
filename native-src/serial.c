#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include "nextion_parser.h"

static int serial_fd = -1;
static nextion_parser_t parser;
static nextion_message_callback_t user_callback = NULL;

void serial_message_callback(const uint8_t* message, uint16_t length) {
    // TODO: Handle the parsed message
    printf("Received message of length %d\n", length);

    // print ascii message
    printf("ASCII: ");
    for (uint16_t i = 0; i < length; i++) {
        // check if printable
        if (message[i] >= 32 && message[i] <= 126) {
            printf("%c", message[i]);
        } else {
            printf(".");
        }
    }
    printf("\n");

    // print hex message
    printf("HEX: ");
    for (uint16_t i = 0; i < length; i++) {
        printf("%02X ", message[i]);
    }
    printf("\n");
    printf("\n");
}

int serial_init(const char* port, int baudrate, nextion_message_callback_t callback) {
    // Store the user callback if provided
    user_callback = callback;

    // Open the serial port in raw mode
    serial_fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serial_fd < 0) {
        perror("Error opening serial port");
        return -1;
    }

    // Get current port settings
    struct termios tty;
    if (tcgetattr(serial_fd, &tty) != 0) {
        perror("Error getting port attributes");
        close(serial_fd);
        return -1;
    }

    // Set baudrate
    speed_t speed;
    switch (baudrate) {
        case 9600:   speed = B9600;   break;
        case 19200:  speed = B19200;  break;
        case 38400:  speed = B38400;  break;
        case 57600:  speed = B57600;  break;
        case 115200: speed = B115200; break;
        default:
            fprintf(stderr, "Unsupported baudrate: %d\n", baudrate);
            close(serial_fd);
            return -1;
    }

    // Set baudrate
    if (cfsetispeed(&tty, speed) != 0 || cfsetospeed(&tty, speed) != 0) {
        perror("Error setting baudrate");
        close(serial_fd);
        return -1;
    }

    // Set raw mode
    tty.c_cflag |= (CLOCAL | CREAD);    // Ignore modem controls
    tty.c_cflag &= ~PARENB;             // No parity
    tty.c_cflag &= ~CSTOPB;             // 1 stop bit
    tty.c_cflag &= ~CSIZE;              // Clear size bits
    tty.c_cflag |= CS8;                 // 8 bits
    tty.c_cflag &= ~CRTSCTS;            // No hardware flow control

    // Raw input
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~OPOST;              // Raw output

    // Set read timeout
    tty.c_cc[VMIN] = 0;                 // Non-blocking read
    tty.c_cc[VTIME] = 0;                // No timeout

    // Apply settings
    if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
        perror("Error setting port attributes");
        close(serial_fd);
        return -1;
    }

    // Initialize the parser
    nextion_parser_init(&parser);

    return 0;
}

void serial_task(void) {
    if (serial_fd < 0) {
        return;
    }

    uint8_t buf[256];
    ssize_t n = read(serial_fd, buf, sizeof(buf));

    if (n > 0) {
        // Process each byte through the parser
        for (ssize_t i = 0; i < n; i++) {
            nextion_parser_process_byte(&parser, buf[i], user_callback ? user_callback : serial_message_callback);
        }
    } else if (n < 0 && errno != EAGAIN) {
        perror("Error reading from serial port");
    }
}

ssize_t serial_write(const uint8_t* data, size_t length) {
    if (serial_fd < 0) {
        return -1;
    }

    ssize_t bytes_written = write(serial_fd, data, length);
    if (bytes_written < 0) {
        perror("Error writing to serial port");
        return -1;
    }

    return bytes_written;
}

void serial_close(void) {
    if (serial_fd >= 0) {
        close(serial_fd);
        serial_fd = -1;
    }
} 