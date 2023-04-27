//Includes
#include "comms.h"
#include "process_packet.h"

#define V12_CONVERSION 0.00322344322 //13.2/(4095) as 12V = 3V in (4*3.3) = 13.2
#define V3_3_CONVERSION 0.00080566406 //3.3/(4095)

typedef union data_t {
   byte b;
   float f;
   byte s[40];
} data_t;

void process_payload(module_info_t *module, packet_t *packet, HardwareSerial *port);
void response_packet(module_info_t *module, PacketType type, HardwareSerial *port); 
float pico_temperature();
void packet_send(packet_t *packet, HardwareSerial *port); 

data_t data; 
uint8_t ctrl_mode = 0x00;
uint8_t empty_payload_flag = 1;

uint8_t process_packet(module_info_t *module, packet_t *packet, HardwareSerial *port) 
{

    //Packet Port Check
    if (port == &UART1)
    {
        packet_send(packet, &UART0);
        packet_send(packet, &UART_USB);
        return SENT;
    }

    //Asign ID WORKS
    if (module->ID == UNASSIGNED)
    {
        //Packet Type Check
        if (packet->type != ID || packet->payload[0] != 0x00) 
        {
            packet_send(packet, &UART1);
            return NEXT; 
        }

        module->ID = packet->ID;

        response_packet(module, ID, port);
        return RESPONSE;
    }

    //----------------------------------

    //Packet ID check WORKS
    if (module->ID != packet->ID) 
    {
        packet_send(packet, &UART1);
        return NEXT; 
    }

    //Valid packet check WORKS
    if (!((packet->type >= ID && packet->type <= TEST_PROBE) || 
            (packet->type >= SUPPLY_VOLTAGE && packet->type <= IP3) || 
            (packet->type >= ENABLE && packet->type <= VGA_ATTENUATION) || 
            (packet->type >= LOWER_CUTOFF && packet->type <= UPPER_CUTOFF) || 
            (packet->type >= LO_LEVEL && packet->type <= UPPER_OUTPUT)))
    {
        return DROPPED;
    }

    //Packet type ID WORKS
    if (packet->type != ID) 
    {
        process_payload(module, packet, port);
        return RESPONSE;
    }

    //Empty payload WORKS
    if (packet->length == PACKET_BYTES_USED) 
    {
        //Clear ID
        module->ID = UNASSIGNED;
        response_packet(module, ID, port);
        return RESPONSE;
    }

    return DROPPED;
}

void process_payload(module_info_t *module, packet_t *packet, HardwareSerial *port)
{
    //Update module freq
    if (packet->type == FREQUENCY)
    {   
        if (packet->length == PACKET_BYTES_USED)
        {
            data.f = module->frequency;
            empty_payload_flag = 1;
        } else 
        {
            module->frequency = *((float*)packet->payload);
        }
    }
    
    //Enable/Disable
    if (packet->type == ENABLE)
    {
        if (packet->length == PACKET_BYTES_USED)
        {
            data.b = ctrl_mode;
            empty_payload_flag = 1;
        } else
        {
            if (packet->payload[0] == 0)
            {   
                ctrl_mode = 0x00;
            } else if (packet->payload[0] == 0xFF)
            {
                ctrl_mode = 0xFF;
            }
            digitalWrite(CTRL_3V3, ctrl_mode/0xFF);
        }
        UART1.println(ctrl_mode, HEX);
    }   

    //Vga Attenuation
    if (packet->type == VGA_ATTENUATION)
    {
        if (packet->length == PACKET_BYTES_USED)
        {
            data.b = module->vga_attenuation;
            empty_payload_flag = 1;
        } else
        {
            module->vga_attenuation = packet->payload[0];
        }
    }

    response_packet(module, static_cast<PacketType>(packet->type), port);
}

void response_packet(module_info_t *module, PacketType type, HardwareSerial *port) 
{
    packet_t packet;
    packet.length = 0;
    packet.ID = module->ID;
    packet.type = type;

    if (((type >= SUPPLY_VOLTAGE && type < ENABLE) || (type >= LOWER_CUTOFF && type < FAIL)) && type != FREQUENCY ) 
    {
        packet.length = 4;
    }

    switch (type)
    {
    case ID:
        if (packet.ID != UNASSIGNED)
        {
            packet.length = 1;
            data.b = 0xFF;
        }
        break;
    case PING:
        packet.type = PONG;
        break;
    case PONG:
        packet.type = PING;
        break;
    case PROTOCOL_VERSION:
        packet.length = module->protocol_version.length();
        module->protocol_version.getBytes(data.s, module->protocol_version.length() + 1);
        break;
    case FIRMWARE_VERSION:
        packet.length = module->firmware_version.length();
        module->firmware_version.getBytes(data.s, module->firmware_version.length() + 1);
        break;
    case CATEGORY:
        packet.length = 1;
        data.b = module->category;
        break;
    case PART_NUMBER:
        packet.length = module->part_number.length();
        module->part_number.getBytes(data.s, module->description.length() + 1);
        break;
    case DESCRIPTION:
        packet.length = module->description.length();
        module->description.getBytes(data.s, module->description.length() + 1);
        break;
    case TEST_PROBE:
        packet.length += 1;
        data.b = digitalRead(21);
        break;
     case SUPPLY_VOLTAGE:
        data.f = analogRead(A2) * V12_CONVERSION;
        break;
    case USB_VOLTAGE:
        data.f = analogRead(A3) * 5/(2^12);
        break;
    case TEMPERATURE:
        data.f = analogReadTemp();
        break;
    case FREQUENCY:
        //Check 
        if (empty_payload_flag)
        {
            packet.length = 4;
            empty_payload_flag = 0;
        }
        break;
    case LOWER_FREQUENCY:
        data.f = 10;
        break;
    case UPPER_FREQUENCY:
        data.f = 10;
        break;
    case GAIN:
        data.f = 10;
        break;
     case MAX_INPUT_POWER:
        data.f = 10;
        break;
    case NOISE_FIGURE:
        data.f = 10;
        break;
    case OP1DB:
        data.f = 10;
        break;
    case IP3:
        data.f = 10;
        break;
    case ENABLE:
    case VGA_ATTENUATION:
        //Check
        if (empty_payload_flag)
        {
            packet.length = 1;
            empty_payload_flag = 0;
        }
        break;    
    case LOWER_CUTOFF:
        data.f = 10;
        break;
     case UPPER_CUTOFF:
        data.f = 10;
        break;
    case LO_LEVEL:
        data.f = 10;
        break;
    case LO_FREQUENCY:
        data.f = 10;
        break;
    case LOWER_LO:
        data.f = 10;
        break;
    case UPPER_LO:
        data.f = 10;
        break;
    case LOWER_OUTPUT:
        data.f = 10;
        break;
    case UPPER_OUTPUT:
        data.f = 10;
        break;
    default:
    
        String F = "FAIL";
        packet.length = F.length();
        F.getBytes(data.s, F.length());
        break;
    }

    uint8_t data_sum = 0;

    packet.payload = new byte[packet.length];

    for (uint8_t index = 0; index < packet.length; index++)
    {
        data_sum += packet.payload[index] = data.s[index];
    }

    packet.length += PACKET_BYTES_USED;

    packet.checksum = ~(packet.length + packet.ID + packet.type + data_sum - 1);

    packet_send(&packet, port);

    delete [] packet.payload;
}


void packet_send(packet_t *packet, HardwareSerial *port) 
{ 
    // port->write((byte*)&packet, sizeof(packet));
    port->write(packet->length);
    port->write(packet->ID);
    port->write(packet->type);
    port->write(packet->payload, packet->length - PACKET_BYTES_USED);
    port->write(packet->checksum);
    //UART1.println("COMPLEAT SEND");
}