#ifndef COMMS_H
#define COMMS_H

//Includes
#include <Arduino.h>
#include "packet_types.h"

//Defines
#define UART_USB Serial
#define UART0 Serial1
#define UART1 Serial2
#define CTRL_3V3 22
#define PACKET_BYTES_USED 3
#define UNASSIGNED 0xFF

//Structs
typedef struct module_info_t  {
  byte ID;
  String protocol_version;
  String firmware_version;
  byte category;
  String part_number;
  String description;
  float frequency;
  byte vga_attenuation;
} module_info_t;

typedef struct packet_t  {
  byte length;
  byte ID;
  byte type;
  byte *payload;
  byte checksum;
} packet_t;

//Enums
enum return_types : uint8_t {
    SENT = 0,
    NEXT,
    RESPONSE,
    DROPPED,
};

//Methods


#endif //COMMS_H