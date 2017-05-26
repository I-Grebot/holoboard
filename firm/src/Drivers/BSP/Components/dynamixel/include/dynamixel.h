/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       dynamixel.h
 * @author     Bebop35
 * @date       Feb 09, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   General Header for Dynamixel's Digital Servos
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/blueboard.git
 * -----------------------------------------------------------------------------
 */

#ifndef __DYNAMIXEL_H_
#define __DYNAMIXEL_H_

/* Required standard libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
//#include <strings.h>
#include <stdio.h>

/* Protocol-specific definitions & types */
#include "dxl_v1.h"
#include "dxl_v2.h"

/**
 ********************************************************************************
 **
 **  Constants
 **
 ********************************************************************************
 */

/* Constants definition for the TX or RX mode of an interface */
#define DXL_RX_MODE 0U
#define DXL_TX_MODE 1U

/* Generic Fail / Pass error code status */
#define DXL_PASS    0U
#define DXL_FAIL    1U

/* Status constants:
 * 1st byte reserved for protocol-specific error
 * 2nd byte reserved for common DXL errors
 */
#define DXL_STATUS_NO_ERROR             0x0000
#define DXL_STATUS_ERR_TIMEOUT          0x0100  // Returned packet timeout
#define DXL_STATUS_ERR_HEADER           0x0200  // Header error
#define DXL_STATUS_ERR_INSTRUCTION      0x0300  // Returned instruction dos not match
#define DXL_STATUS_ERR_ID               0x0400  // Returned ID does not match
#define DXL_STATUS_ERR_LENGTH           0x0500  // Packet length error
#define DXL_STATUS_ERR_CHECKSUM         0x0600  // Packet checksum (or CRC) is wrong
#define DXL_STATUS_ERR_PROTOCOL         0x1000  // Unknown protocol

/* LEDs color */
#define DXL_LED_OFF    0U
#define DXL_LED_RED    1U
#define DXL_LED_GREEN  2U
#define DXL_LED_BLUE   4U
#define DXL_LED_YELLOW  (DXL_LED_RED   | DXL_LED_GREEN)
#define DXL_LED_CYAN    (DXL_LED_GREEN | DXL_LED_BLUE)
#define DXL_LED_MAGENTA (DXL_LED_RED   | DXL_LED_BLUE)
#define DXL_LED_WHITE   (DXL_LED_RED   | DXL_LED_GREEN | DXL_LED_BLUE)

/* Motor Mode */
#define DXL_WHEEL_MODE   1U
#define DXL_JOIN_MODE    2U

/* Some IDs constants */
#define DXL_ID_BROADCAST      0xFE
#define DXL_ID_DEFAULT        0x00

/* Status answer level definition */
#define DXL_STATUS_NO_AWNSER   0x00 // Except for PING command
#define DXL_STATUS_READ_ONLY   0x01 // Only when a READ command is issued
#define DXL_STATUS_EVERYTHING  0x02

/* Defined when debug mode is required */
//#define DXL_DEBUG

#ifdef DXL_DEBUG
#warning "Dynamixel Library: using debug mode"

#define DXL_DEBUG_PFX "[DXL]" // Debug prefix
#define DXL_DEBUG_EOL "\n\r"  // End-Of-Line character for debug

// Puts() function
#define DXL_DEBUG_PUTS(_str) serial_puts((_str))
extern long serial_puts(const char* str);

#else

#define DXL_DEBUG_PUTS(_str) {};

#endif // DXL_DEBUG

 /**
 ********************************************************************************
 **
 **  Types & Enumerations
 **
 ********************************************************************************
 */

/* Status & Error type */
typedef uint16_t dxl_status_t;

// XL_320 Communication mode
typedef enum {
	XL_320_TX_ONLY 		 =  1,
	XL_320_TXRX_ONE_PIN  = 2,
	XL_320_TXRX_TWO_PINS = 4
} XL_320_Com_Mode;

/* Communication Protocols */
typedef enum {
    DXL_V1 = 1,
    DXL_V2 = 2
} dxl_protocol_e;

/* Registers Control Tables
 * Also serves as implementation masks for the table */
typedef enum {
    DXL_REG1  = 0x01,  /* Legacy table with V1 Communication (e.g. AX-12) */
    DXL_REG2  = 0x02,  /* Table with PID controls */
    DXL_REG3  = 0x04,  /* Table with PID controls and Torque Control */
    DXL_REG4  = 0x08,  /* Table with Control Mode / PID; introduced with V2 Communication */
    DXL_REG5  = 0x10   /* Table with full PID controls, introduced with XM servos */
} dxl_reg_table_e;

/* Area */
typedef enum {
    DXL_EEPROM,  /* Non-Volatile storage */
    DXL_RAM      /* Cleared when powered-off */
} dxl_reg_area_e;

/* Access type */
typedef enum {
    DXL_RW,  /* Read / Write */
    DXL_R    /* Read Only */
} dxl_reg_access_e;

/* Register definition */
typedef struct {
    uint8_t reg_table_mask; /* Implementation mask defining the reg-table */
    dxl_reg_area_e area;
    uint16_t address;
    const char* name;
    dxl_reg_access_e access;
    size_t size;            /* Size in Bytes */
                            /* Values are not stored! */
} dxl_register_t;

/* Servo Models */
typedef struct {
    uint16_t model_id;
    const char* name;
    dxl_protocol_e protocol;
    dxl_reg_table_e reg_table;
} dxl_servo_model_t;

/* Pin mode for half-duplex communication */
typedef enum {
    DXL_MODE_TX = 0U,
    DXL_MODE_RX = 1U
} dxl_switch_mode_e;

/* Dynamixel Interface definition
 * Define hardware handlers (function pointers) as well as
 * other interface-specific constants
 */
typedef struct {

    // Unique index of the interface
    uint8_t itf_idx;

    // Interface protocol to be used
    dxl_protocol_e protocol;

    // Send a byte and return error if not successful
    // An error is a non-zero code
    uint8_t (* hw_send_byte)(uint8_t chan_idx, uint8_t tx_data);

    // Receive a byte and return error if not successful
    // An error is a non-zero code
    uint8_t (* hw_receive_byte) (uint8_t chan_idx, const uint8_t* rx_data);

    // Flush the receiver (cleanup all unprocessed data)
    uint8_t (* hw_flush) (uint8_t chan_idx);

    // Return level that needs to be remembered to know if
    // something has to be expected.
    uint8_t return_level;

    // Return delay also has to be remembered to set an
    // appropriate timeout.
    uint8_t return_delay_ms;

    // Status that can contain various things but mostly errors
    dxl_status_t status;

    // Statistics counters
    uint32_t nb_pkt_tx; // Number of transmitted packets
    uint32_t nb_pkt_rx; // Number of received packets (without errors)
    uint32_t nb_errors; // Number of errors

} dxl_interface_t;

/* Dynamixel Servo control structure
 * Useful to handle dynamic values for each servo
 */
typedef struct {

    // Servo model. Contains protocol and register versions.
    dxl_servo_model_t* model;

    // Interface on which the servo is attached
    dxl_interface_t* itf;

    // Unique ID of the servo on the interface
    uint8_t id;

    // Boundaries
    uint16_t min_position;
    uint16_t max_position;

    // Temporary variables
    uint16_t current_position;

} dxl_servo_t;


/**
********************************************************************************
**
**  Main library functions
**
********************************************************************************
*/

// Servo Models
const dxl_servo_model_t* dxl_find_servo_model_by_name(const char* name);
const dxl_servo_model_t* dxl_find_servo_model_by_id(uint32_t id);

// Registers
const char* dxl_get_area_as_string(const dxl_reg_area_e area);
const char* dxl_get_access_as_string(const dxl_reg_access_e acc);

// Configuration
void dxl_init_itf(dxl_interface_t* itf, uint8_t itf_idx);
void dxl_init_servo(dxl_servo_t* servo, dxl_interface_t* itf, const char* model_name);

// Main handlers
dxl_status_t dxl_ping(dxl_servo_t* servo);
dxl_status_t dxl_reset(dxl_servo_t* servo);
dxl_status_t dxl_write(dxl_servo_t* servo, uint16_t addr, uint8_t* values, size_t size, bool reg);
dxl_status_t dxl_write_int(dxl_servo_t* servo, uint16_t addr, uint32_t value, size_t size, bool reg);
dxl_status_t dxl_read(dxl_servo_t* servo, uint16_t addr, uint8_t* values, size_t size);
dxl_status_t dxl_read_int(dxl_servo_t* servo, uint16_t addr, uint32_t* value, size_t size);
dxl_status_t dxl_action(dxl_servo_t* servo);

// Shorthands
dxl_status_t dxl_get_model(dxl_servo_t* servo, uint16_t* model);
dxl_status_t dxl_set_torque_enable(dxl_servo_t* servo, uint8_t torque_enable);
dxl_status_t dxl_set_position(dxl_servo_t* servo, uint16_t new_position);
dxl_status_t dxl_set_speed(dxl_servo_t* servo, uint16_t new_speed);
dxl_status_t dxl_set_torque(dxl_servo_t* servo, uint16_t new_torque);
dxl_status_t dxl_set_led(dxl_servo_t* servo, uint8_t led);

// Service handlers
void dxl_data_to_bytes_array(uint32_t data, size_t size, uint8_t* data_arr);
void dxl_bytes_array_to_data(uint32_t* data, size_t size, uint8_t* data_arr);
void dxl_get_error_str(char* status_str, size_t status_str_len, dxl_status_t status, dxl_protocol_e protocol);

/* Debug */
#ifdef DXL_DEBUG

void dxl_print_servo(dxl_servo_t* servo);
void dxl_print_error(dxl_status_t status, dxl_protocol_e protocol);


#endif // DXL_DEBUG

/**
********************************************************************************
**
**  Protocol V1 functions
**
********************************************************************************
*/

/* Hardware and low-level routines */
uint8_t dxl_v1_compute_checksum(dxl_v1_packet_t* packet);
void dxl_v1_send_packet(dxl_interface_t* itf, dxl_v1_packet_t* packet);
void dxl_v1_receive_packet(dxl_interface_t* itf, dxl_v1_packet_t* packet);
uint16_t dxl_v1_get_status(dxl_v1_packet_t* instruction_packet,
                           dxl_v1_packet_t* status_packet,
                           uint8_t expected_param_length);

/* Instructions */
void dxl_v1_ping(dxl_servo_t* servo);
void dxl_v1_reset(dxl_servo_t* servo);
void dxl_v1_write(dxl_servo_t* servo, uint8_t address, uint8_t* parameters, size_t nb_param, bool registered);
void dxl_v1_read(dxl_servo_t* servo, uint8_t address, uint8_t* datas, size_t nb_data);
void dxl_v1_action(dxl_servo_t* servo);

/* Service */
void dxl_v1_get_error_str(char* status_str, size_t status_str_len, dxl_status_t status);

/* Debug */
#ifdef DXL_DEBUG
void dxl_v1_print_packet(dxl_v1_packet_t* packet);

#endif // DXL_DEBUG


/**
********************************************************************************
**
**  Protocol V2 functions
**
********************************************************************************
*/

/* Hardware and low-level routines */
uint16_t dxl_v2_compute_crc(uint8_t *data, uint8_t data_size);
void dxl_v2_send_packet(dxl_interface_t* itf, dxl_v2_packet_t* packet);
void dxl_v2_receive_packet(dxl_interface_t* itf, dxl_v2_packet_t* packet);

/* Instructions */
void dxl_v2_ping(dxl_servo_t* servo);
void dxl_v2_write(dxl_servo_t* servo, uint16_t address, uint8_t* parameters, size_t nb_param, bool registered);
void dxl_v2_read(dxl_servo_t* servo, uint16_t address, uint8_t* datas, size_t nb_data);
void dxl_v2_action(dxl_servo_t* servo);
void dxl_v2_reset(dxl_servo_t* servo);
void dxl_v2_reboot(dxl_servo_t* servo);

/* Service */
void dxl_v2_get_error_str(char* status_str, size_t status_str_len, dxl_status_t status);

/* Debug */
#ifdef DXL_DEBUG
void dxl_v2_print_packet(dxl_v2_packet_t* packet);

#endif // DXL_DEBUG

#endif /* __DYNAMIXEL_H_ */
