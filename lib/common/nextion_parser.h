#ifndef NEXTION_PARSER_H
#define NEXTION_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdbool.h>

// Maximum size of the message buffer
#define NEXTION_MAX_MESSAGE_SIZE 256

// Message types
typedef enum {
    NEXTION_MSG_TYPE_TOUCH_EVENT,
    NEXTION_MSG_TYPE_STRING,
    NEXTION_MSG_TYPE_NUMERIC,
    NEXTION_MSG_TYPE_BINARY,
    NEXTION_MSG_TYPE_UNKNOWN
} nextion_msg_type_t;

// Command types
typedef enum {
    NEXTION_CMD_UNKNOWN,
    NEXTION_CMD_PAGE,
    NEXTION_CMD_REF,
    NEXTION_CMD_CLICK,
    NEXTION_CMD_REF_STOP,
    NEXTION_CMD_REF_STAR,
    NEXTION_CMD_GET,
    NEXTION_CMD_SENDME,
    NEXTION_CMD_COVX,
    NEXTION_CMD_TEXT_ASSIGN,
    NEXTION_CMD_VALUE_ASSIGN,
    NEXTION_CMD_VAR_ASSIGN,
} nextion_cmd_type_t;

// Message structure
typedef struct {
    nextion_msg_type_t type;
    uint8_t* data;
    uint16_t length;
} nextion_msg_t;


// Parser state structure
typedef struct {
    uint8_t buffer[NEXTION_MAX_MESSAGE_SIZE];
    uint16_t buffer_index;
    uint8_t ff_count;  // Count of consecutive 0xFF bytes
} nextion_parser_t;

// Command structure (union)
typedef struct {
    nextion_cmd_type_t type;
    union {
        struct { char path[64]; } get;
        struct { char page[32]; } page;
        struct { char object[64]; char value[128]; } text_assign;
        struct { char object[64]; int value; } value_assign;
        struct { char name[64]; int value; } var_assign;
        char buf[128];
    } data;
} nextion_cmd_t;

// Initialize the message handler
void nextion_msg_handler_init(void);

// Process a complete message from the Nextion parser
void nextion_msg_handler_process(const uint8_t* message, uint16_t length);

// Parse a string command into a command structure
nextion_cmd_t nextion_parse_command(const char* cmd_str);

// Callback function type for completed messages
typedef void (*nextion_message_callback_t)(const uint8_t* message, uint16_t length);

// Initialize the parser
void nextion_parser_init(nextion_parser_t* parser);

// Process a single byte of input
// Returns true if a complete message was found and callback was called
bool nextion_parser_process_byte(nextion_parser_t* parser, uint8_t byte, nextion_message_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif // NEXTION_PARSER_H
