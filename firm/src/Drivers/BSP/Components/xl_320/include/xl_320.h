#ifndef _XL_320_H
#define _XL_320_H

#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
// XL-320 Types definition
// -----------------------------------------------------------------------------

// Maximum number of parameters in a packet
// (either instruction or status)
#define XL_320_MAX_PARAM    32
#define XL_320_MAX_PACKET   XL_320_MAX_PARAM+10

// RX/TX States for the switch handler
#define XL_320_TX 1
#define XL_320_RX 0
#define XL_320_RECEIVED 1

// XL_320 Communication mode
typedef enum {
	XL_320_TX_ONLY 		 =  1,
	XL_320_TXRX_ONE_PIN  = 2,
	XL_320_TXRX_TWO_PINS = 4
} XL_320_Com_Mode;

// XL-320 config
// Define hardware handlers (function pointers)
typedef struct {

	// Communication mode
	XL_320_Com_Mode mode;

    // Switch to TX/RX the half-duplex link
    void (* hw_switch)(uint8_t);

    // Send a byte and return error if not successful
    void (* hw_send_byte)(uint8_t, uint8_t);

    // Receive a byte and return error if not successful
    uint8_t (* hw_receive_byte) (uint8_t*);

    // Flush the receiver (used to make sure its clean before
    // starting to receive actual datas)
    void (* hw_flush) (void);
    
} xl_320_cfg_t;

// XL-320 packets
// Double-byte header is not represented,
// Instruction and status packets only depends on their contents:
//  - An instruction for the instruction packets (write, read, action etc.)
//  - Servo's status for the status packets
typedef struct {
    uint8_t id;
    uint8_t packet_length;
    uint8_t instruction;   // This is either an instruction or a status
    uint8_t parameters[XL_320_MAX_PARAM];
    uint8_t parameter_nb;
    uint16_t checksum;
} xl_320_packet_t;

// -----------------------------------------------------------------------------
// Errors definitions
// -----------------------------------------------------------------------------

// XL-320 error frame (see datasheet)
#define XL_320_RESULT_FAIL          0x01
#define XL_320_INST_ERROR           0x02
#define XL_320_CRC_ERROR            0x03
#define XL_320_DATA_RANGE_ERROR     0x04
#define XL_320_DATA_LENGTH_ERROR    0x05
#define XL_320_DATA_LIMIT_ERROR     0x06
#define XL_320_ACCESS_ERROR         0x07

// UART status error flags
#define XL_320_ERROR_UART_TIMEOUT  0x08  // Status packet timed-out
#define XL_320_ERROR_UART_HEADER   0x09  // Incorrect status packet header

// Application status errors masks definition
#define XL_320_ERROR_APP_ID        0x0A  // Status packet ID does not match
#define XL_320_ERROR_APP_LENGTH    0x0B  // Status packet length doesnt match
#define XL_320_ERROR_APP_CHECKSUM  0x0C  // Status packet checksum is wrong

// XL-320 error flags (see datasheet)
#define XL_320_ERROR_INPUT_VOLTAGE  0x04    // Voltage is out of operational voltage range
#define XL_320_ERROR_OVER_HEATING   0x02    // Temperature is out of operational temperature range
#define XL_320_ERROR_OVERLOAD       0x01    // Motor cannot output max load due to load being applied continouosly

// -----------------------------------------------------------------------------
// Instructions definitions
// -----------------------------------------------------------------------------

// Instructions definition
#define XL_320_HEADER            0xFF
#define XL_320_HEADER_2          0xFD
#define XL_320_INS_PING          0x01
#define XL_320_INS_READ          0x02
#define XL_320_INS_WRITE         0x03
#define XL_320_INS_REG_WRITE     0x04
#define XL_320_INS_ACTION        0x05
#define XL_320_INS_RESET         0x06
#define XL_320_REBOOT            0x08
#define XL_320_INS_SYNC_READ     0x82
#define XL_320_INS_SYNC_WRITE    0x83
#define XL_320_INS_BULK_READ     0x92
#define XL_320_INS_BULK_WRITE    0x93
#define XL_320_STATUS            0x55

// ID definitions
#define XL_320_ID_BROADCAST      0xFE
#define XL_320_ID_IMPOSSIBLE     0xFF

// Status answer level definition
#define XL_320_STATUS_NO_ANSWER   0x00
#define XL_320_STATUS_READ_ONLY   0x01
#define XL_320_STATUS_EVERYTHING  0x02

// -----------------------------------------------------------------------------
// Adress definition
// -----------------------------------------------------------------------------
//EEPROM
#define Xl_320_ADR_MODEL            0   
#define Xl_320_ADR_FIRMWARE         2
#define Xl_320_ADR_ID               3
#define Xl_320_ADR_BAUDRATE         4
#define Xl_320_ADR_RET_DELAY        5
#define Xl_320_ADR_CW_LIMIT         6
#define Xl_320_ADR_CCW_LIMIT        8
#define Xl_320_ADR_CONTROL_MODE     11
#define Xl_320_ADR_TEMP_LIMIT       12
#define Xl_320_ADR_LOW_VOLTAGE      13
#define Xl_320_ADR_HIGH_VOLTAGE     14
#define Xl_320_ADR_MAX_TORQUE       15
#define Xl_320_ADR_RET_LEVEL        17
#define Xl_320_ADR_ALARM_SHUTDOWN   18

//RAM
#define Xl_320_ADR_TORQUE_EN            24
#define XL_320_ADR_LED                  25
#define XL_320_ADR_D_GAIN               27
#define XL_320_ADR_I_GAIN               28
#define XL_320_ADR_P_GAIN               29
#define XL_320_ADR_GOAL_POSITION        30
#define XL_320_ADR_GOAL_VELOCITY        32
#define XL_320_ADR_GOAL_TORQUE          35
#define XL_320_ADR_PRESENT_POSITION     37
#define XL_320_ADR_PRESENT_VELOCITY     39
#define XL_320_ADR_PRESENT_LOAD         41
#define XL_320_ADR_PRESENT_VOLTAGE      45
#define XL_320_ADR_PRESENT_TEMPERATURE  46
#define XL_320_ADR_REGISTERED_INST      47
#define XL_320_ADR_MOVING               49
#define XL_320_ADR_ERROR                50
#define XL_320_ADR_PUNCH                51
// -----------------------------------------------------------------------------
// Maximum values allowed
// -----------------------------------------------------------------------------

#define XL_320_MAX_POSITION          0x03FF  // 300 °
#define XL_320_MAX_SPEED             0x03FF  // 62 RPM
#define XL_320_MAX_ACCELERATION      0x03FF  // ?? RPM / sec
#define XL_320_MAX_PUNCH             0x03FF  // ??

// -----------------------------------------------------------------------------
// Registers meanings
// -----------------------------------------------------------------------------

// Fields definition for the baudrate register
#define XL_320_BAUDRATE_9600BPS     0
#define XL_320_BAUDRATE_57600BPS    1
#define XL_320_BAUDRATE_115200BPS   2
#define XL_320_BAUDRATE_1MBPS       3

// LEDs color
#define XL_320_LED_OFF    0
#define XL_320_LED_RED    1
#define XL_320_LED_GREEN  2
#define XL_320_LED_BLUE   4
#define XL_320_LED_YELLOW  (XL_320_LED_RED   | XL_320_LED_GREEN)
#define XL_320_LED_CYAN    (XL_320_LED_GREEN | XL_320_LED_BLUE)
#define XL_320_LED_MAGENTA (XL_320_LED_RED   | XL_320_LED_BLUE)
#define XL_320_LED_WHITE   (XL_320_LED_RED   | XL_320_LED_GREEN | XL_320_LED_BLUE)

// Motor Mode
#define XL_320_WHEEL_MODE 1
#define XL_320_JOIN_MODE 2

// -----------------------------------------------------------------------------
// Prototypes
// -----------------------------------------------------------------------------

// Initialization functions
void xl_320_init(XL_320_Com_Mode mode);
void xl_320_set_hw_send(void (*hw_send)(uint8_t, uint8_t));
void xl_320_set_hw_receive(uint8_t (*hw_receive)(uint8_t*));
void xl_320_set_hw_flush(void (*hw_flush)(void));
void xl_320_set_hw_switch(void (*hw_switch)(uint8_t));

// UART Layer functions
uint8_t xl_320_is_received(void);

// INSTRUCTION Layer functions - SET
void xl_320_set_id(uint8_t id);
void xl_320_set_baudrate(uint8_t id, uint8_t baudrate);
void xl_320_set_return_delay(uint8_t id, uint8_t return_delay);
void xl_320_set_cw_limit(uint8_t id, uint16_t cw_limit);
void xl_320_set_ccw_limit(uint8_t id, uint16_t ccw_limit);
void xl_320_set_control_mode(uint8_t id, uint8_t mode);
void xl_320_set_temp_limit(uint8_t id, uint8_t temp_limit);
void xl_320_set_min_voltage(uint8_t id, uint8_t min_voltage);
void xl_320_set_max_voltage(uint8_t id, uint8_t max_voltage);
void xl_320_set_torque_limit(uint8_t id, uint16_t torque_limit);
void xl_320_set_return_level(uint8_t id,uint8_t level);
void xl_320_set_alarm_shutdown(uint8_t id, uint8_t shutdown);
void xl_320_set_torque_en(uint8_t id, uint8_t torque_en);
void xl_320_set_led(uint8_t id, uint8_t led);
void xl_320_set_D_gain(uint8_t id, uint8_t D);
void xl_320_set_I_gain(uint8_t id, uint8_t I);
void xl_320_set_P_gain(uint8_t id, uint8_t P);
void xl_320_set_position(uint8_t id, uint16_t position);
void xl_320_set_speed(uint8_t id, uint16_t speed);
void xl_320_set_goal_torque(uint8_t id, uint16_t torque_limit);
void xl_320_set_punch(uint8_t id, uint16_t punch);

//unsigned int xl_320_get_status(xl_320_packet_t* instruction_packet,
//                                xl_320_packet_t* status_packet,
//                                unsigned char expected_param_length);
//unsigned int xl_320_ping(unsigned char id);
//unsigned int xl_320_set_bit_stuff(xl_320_packet_t* packet, unsigned char* parameters);
//unsigned int xl_320_read(unsigned char id, unsigned char address, unsigned char* datas, unsigned char nb_data);
//void xl_320_action(void);
//unsigned int xl_320_reset(unsigned char id);
//
//
////unsigned int xl_320_set_alarm_shutdown(unsigned char id, unsigned char alarm_shutdown);
//
//
//unsigned int xl_320_set_status_return(unsigned char id, unsigned char status_return);
////unsigned int xl_320_set_torque_en(unsigned char id, unsigned char torque_en, unsigned char registered);
//
////unsigned int xl_320_set_position(unsigned char id, unsigned int position, unsigned char registered);
////unsigned int xl_320_set_speed(unsigned char id, unsigned int speed, unsigned char registered);
//
//unsigned int xl_320_get_temperature(unsigned char id, unsigned char *temp);
//unsigned int xl_320_get_voltage(unsigned char id, unsigned char *voltage);
//unsigned int xl_320_get_moving(unsigned char id, unsigned char *moving);
//unsigned int xl_320_get_position(unsigned char id, unsigned int *position);
//unsigned int xl_320_get_speed(unsigned char id, unsigned int *speed);
//unsigned int xl_320_get_load(unsigned char id, unsigned int *load);
//unsigned int xl_320_set_motion_reg(unsigned char id, unsigned int position,
//                                           unsigned int speed, unsigned int torque);
//unsigned int xl_320_set_motions(unsigned char *ids, unsigned int *positions,
//                    unsigned int *speeds, unsigned char *accs, unsigned char *dccs,
//                    unsigned char nb_id);

#endif // _XL_320_H


