#ifndef PROCRSS_PACKET_H
#define PROCRSS_PACKET_H

//Includes
#include <Arduino.h>

//Methods
uint8_t process_packet(module_info_t *module, packet_t *packet, HardwareSerial *port); 
void packet_send(packet_t *packet, HardwareSerial *port); 


#endif //PROCRSS_PACKET_H