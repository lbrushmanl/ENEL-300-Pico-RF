//Includes
#include <Arduino.h>
#include "comms.h"
#include "process_packet.h"

//Definitons
#define PORT_NUM 3

//Ports Array
HardwareSerial *serial_port[] = {
  &UART_USB, 
  &UART0,
  &UART1
};

module_info_t module;

uint8_t p = 0;

//Methods
//void port_check();
void packet_read(HardwareSerial *port);
void module_init();

void setup() 
{
  UART_USB.begin(115200);
  UART0.begin(115200);
  UART1.begin(115200);
  pinMode(CTRL_3V3, OUTPUT);
  pinMode(25, OUTPUT);
  digitalWrite(CTRL_3V3, LOW);
  analogReadResolution(12);
  module_init();
}

void loop() 
{
  if (serial_port[p]->available() >= 4) 
  {
    packet_read(serial_port[p]);
  }
  p++;
  if (p >= PORT_NUM)
  {
    p = 0;
  }
  digitalWrite(25, HIGH);
  delay(100);
  digitalWrite(25, LOW);
  delay(100);
}

//Initilises the packet with -1 for single byte types and Null for the payload
void packet_init(packet_t* packet)
{
  packet->length = packet->ID = packet->checksum = UNASSIGNED;
  packet->payload = NULL;
}

void module_init() 
{
    module.ID = UNASSIGNED;
    module.protocol_version = "0.1.0";
    module.firmware_version = "0.1.0";
    module.category = LNA;
    module.part_number = "PMA2-33LN+";
    module.description = "This device is a low noise amplifire";
    module.frequency = UNASSIGNED;
    module.vga_attenuation = UNASSIGNED;
}

// //Check ports for packets
// void port_check()
// {
//   for (uint8_t port = 0; port < PORT_NUM; port++)
//   {

//   }
// }


//Reads bytes from UART buffer an asigneds them to the corisponding struct varible
void packet_read(HardwareSerial *port) 
{
  packet_t packet;
  packet_init(&packet);

  uint32_t data_sum = 0;

  packet.length = port->read();

  packet.ID = port->read();
  packet.type = port->read();


  uint8_t payload_length = packet.length - PACKET_BYTES_USED;

  packet.payload = new byte[payload_length];

  for (uint8_t data_index = 0; data_index < payload_length; data_index++) {
    data_sum += packet.payload[data_index] = port->read();
  }

  packet.checksum = port->read();

  byte checksum = ~(packet.length + packet.ID + packet.type + data_sum - 1);

  if (checksum == packet.checksum)
  {
    process_packet(&module, &packet, port); 
    delete [] packet.payload;
  }
}