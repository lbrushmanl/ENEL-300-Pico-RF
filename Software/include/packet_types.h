#include <cstdint>

enum Category : uint8_t {
    // Amplifiers
    LNA = 0x10,
    PA,
    VGA,

    // Filters
    LPF = 0x20,
    HPF,
    BPF,
    BSF,

    // Mixers
    MIXER_IF_RF = 0x30,
    MIXER_RF_IF,
};

enum PacketType : uint8_t {
    // Networking
    ID = 0x00,
    PING,
    PONG,

    // General Info
    PROTOCOL_VERSION,
    FIRMWARE_VERSION,
    CATEGORY,
    PART_NUMBER,
    DESCRIPTION,
    TEST_PROBE,

    // General Data
    SUPPLY_VOLTAGE = 0x10,
    USB_VOLTAGE,
    TEMPERATURE,
    FREQUENCY,
    LOWER_FREQUENCY,
    UPPER_FREQUENCY,
    GAIN,

    // Amplifiers & Mixers
    MAX_INPUT_POWER,
    NOISE_FIGURE,
    OP1DB,
    IP3,

    // Amplifiers
    ENABLE = 0x20,
    VGA_ATTENUATION, // VGA Only

    // Filters
    LOWER_CUTOFF = 0x30,
    UPPER_CUTOFF,

    // Mixers
    LO_LEVEL = 0x40,
    LO_FREQUENCY,
    LOWER_LO,
    UPPER_LO,
    LOWER_OUTPUT,
    UPPER_OUTPUT,

    //Errors
    FAIL = 0x50,
};