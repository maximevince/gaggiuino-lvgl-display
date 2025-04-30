#include "nextion_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../native-src/serial.h"
#include "lv_gaggiuino_ui.h"

// Initialize the message handler
void nextion_msg_handler_init(void) {
    // No initialization needed for now
}

void nextion_parser_init(nextion_parser_t* parser) {
    memset(parser, 0, sizeof(nextion_parser_t));
}

bool nextion_parser_process_byte(nextion_parser_t* parser, uint8_t byte, nextion_message_callback_t callback) {
    // Check if we have space in the buffer
    if (parser->buffer_index >= NEXTION_MAX_MESSAGE_SIZE) {
        // Buffer overflow, reset parser
        nextion_parser_init(parser);
        return false;
    }

    // Add byte to buffer
    parser->buffer[parser->buffer_index++] = byte;

    // Check for termination sequence
    if (byte == 0xFF) {
        parser->ff_count++;
        if (parser->ff_count == 3) {
            // Found complete termination sequence (0xFF, 0xFF, 0xFF)
            // Call callback with the message (excluding the termination sequence)
            if (callback != NULL) {
                // null terminate the buffer
                parser->buffer[parser->buffer_index - 3] = '\0';
                callback(parser->buffer, parser->buffer_index - 4);
            }
            // Reset parser
            nextion_parser_init(parser);
            return true;
        }
    } else {
        // Reset FF count if we get any other byte
        parser->ff_count = 0;
    }

    return false;
}


// Parse a string command into a command structure
nextion_cmd_t nextion_parse_command(const char* cmd_str) {
    nextion_cmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.type = NEXTION_CMD_UNKNOWN;

    if (cmd_str == NULL) return cmd;

    // Match "get <path>"
    if (strncmp(cmd_str, "get ", 4) == 0) {
        strncpy(cmd.data.get.path, cmd_str + 4, sizeof(cmd.data.get.path) - 1);
        cmd.type = NEXTION_CMD_GET;
        return cmd;
    }

    // Match "page <name>"
    if (strncmp(cmd_str, "page ", 5) == 0) {
        strncpy(cmd.data.page.page, cmd_str + 5, sizeof(cmd.data.page.page) - 1);
        cmd.type = NEXTION_CMD_PAGE;
        return cmd;
    }

    // Match "<obj>.txt=\"value\""
    const char* txt_ptr = strstr(cmd_str, ".txt=\"");
    if (txt_ptr && cmd_str[strlen(cmd_str) - 1] == '"') {
        size_t obj_len = txt_ptr - cmd_str;
        if (obj_len < sizeof(cmd.data.text_assign.object)) {
            strncpy(cmd.data.text_assign.object, cmd_str, obj_len);
            cmd.data.text_assign.object[obj_len] = '\0';
            strncpy(cmd.data.text_assign.value, txt_ptr + 6, strlen(txt_ptr + 6) - 1);
            cmd.data.text_assign.value[strlen(txt_ptr + 6) - 1] = '\0';
            cmd.type = NEXTION_CMD_TEXT_ASSIGN;
            return cmd;
        }
    }

    // Match "<obj>.val=<number>"
    const char* val_ptr = strstr(cmd_str, ".val=");
    if (val_ptr) {
        size_t obj_len = val_ptr - cmd_str;
        if (obj_len < sizeof(cmd.data.value_assign.object)) {
            strncpy(cmd.data.value_assign.object, cmd_str, obj_len);
            cmd.data.value_assign.object[obj_len] = '\0';
            cmd.data.value_assign.value = atoi(val_ptr + 5);
            cmd.type = NEXTION_CMD_VALUE_ASSIGN;
            return cmd;
        }
    }

    // Match simple "name=value" (without a dot)
    const char* eq_ptr = strchr(cmd_str, '=');
    if (eq_ptr && strchr(cmd_str, '.') == NULL) {
        size_t name_len = eq_ptr - cmd_str;
        if (name_len < sizeof(cmd.data.var_assign.name)) {
            strncpy(cmd.data.var_assign.name, cmd_str, name_len);
            cmd.data.var_assign.name[name_len] = '\0';
            cmd.data.var_assign.value = atoi(eq_ptr + 1);
            cmd.type = NEXTION_CMD_VAR_ASSIGN;
            return cmd;
        }
    }

    return cmd;
}


// Process a complete message from the Nextion parser
void nextion_msg_handler_process(const uint8_t* message, uint16_t length) {
    if (message == NULL || length == 0) {
        return;
    }

    // Treat the message as a string command
    char* cmd_str = (char*)message;
    nextion_cmd_t cmd = nextion_parse_command(cmd_str);
    
    // Handle the command based on its type
    switch (cmd.type) {
        case NEXTION_CMD_PAGE:
            // Handle page command
            printf("Page command: %s\n", cmd.data.page.page);
            lv_gaggiuino_show_page(cmd.data.page.page);
            break;
            
        case NEXTION_CMD_REF:
            // Handle ref command
            printf("Ref command: %s\n", cmd.data.buf);
            break;
            
        case NEXTION_CMD_CLICK:
            // Handle click command
            printf("Click command: %s\n", cmd.data.buf);
            break;
            
        case NEXTION_CMD_REF_STOP:
            // Handle ref_stop command
            printf("Ref stop command: %s\n", cmd.data.buf);
            break;
            
        case NEXTION_CMD_REF_STAR:
            // Handle ref_star command
            printf("Ref star command: %s\n", cmd.data.buf);
            break;
            
        case NEXTION_CMD_GET:
            // Handle get command
            printf("Get command: %s\n", cmd.data.get.path);
            // if the command is "modeSelect", send 0x88, 0xFF, 0xFF, 0xFF
            if (strncmp(cmd.data.get.path, "modeSelect", strlen("modeSelect")) == 0) {
                uint8_t get_cmd[] = {0x88, 0xFF, 0xFF, 0xFF};
                serial_write(get_cmd, sizeof(get_cmd));
            }
            break;
            
        case NEXTION_CMD_SENDME:
            // Handle sendme command
            printf("Sendme command: %s\n", cmd.data.buf);
            break;
            
        case NEXTION_CMD_COVX:
            // Handle covx command
            printf("Covx command: %s\n", cmd.data.buf);
            break;

        case NEXTION_CMD_TEXT_ASSIGN:
            // Handle text assign command
            printf("Text assign command: %s = %s\n", cmd.data.text_assign.object, cmd.data.text_assign.value);
            // update the text of the object
            lv_gaggiuino_update_text(cmd.data.text_assign.object, cmd.data.text_assign.value);
            break;

        case NEXTION_CMD_VALUE_ASSIGN:
            // Handle value assign command
            printf("Value assign command: %s = %d\n", cmd.data.value_assign.object, cmd.data.value_assign.value);
            break;

        case NEXTION_CMD_VAR_ASSIGN:
            // Handle var assign command
            printf("Var assign command: %s = %d\n", cmd.data.var_assign.name, cmd.data.var_assign.value);
            break;

        default:
        case NEXTION_CMD_UNKNOWN:
            printf("Unknown command: %s\n", cmd_str);
            // check if this a known UI element, e.g. popupMSG
            if (strncmp(cmd_str, "popupMSG", strlen("popupMSG")) == 0) {
                printf("PopupMSG command: %s\n", cmd.data.buf);
                lv_gaggiuino_show_popup();
            }
            break;
    }
}