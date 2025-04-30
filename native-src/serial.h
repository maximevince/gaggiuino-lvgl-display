#ifndef SERIAL_TASK_H
#define SERIAL_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nextion_parser.h"
#include <sys/types.h>

/**
 * Initialize the serial port with the given port name and baudrate
 * @param port The serial port to open (e.g. "/dev/ttyUSB0")
 * @param baudrate The baudrate to use
 * @return 0 on success, -1 on error
 */
int serial_init(const char* port, int baudrate, nextion_message_callback_t callback);

/**
 * Write data to the serial port
 * @param data The data to write
 * @param length The length of the data to write
 * @return The number of bytes written, or -1 on error
 */
ssize_t serial_write(const uint8_t* data, size_t length);

/**
 * Task function to be called in the main loop to handle serial communication
 */
void serial_task(void);

/**
 * Close the serial port
 */
void serial_close(void);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_TASK_H 