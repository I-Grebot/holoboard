/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       dxl_v2.h
 * @author     Bebop35
 * @date       Apr 17, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   Dynamixel's Digital Servo Protocol v2 framing description
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/blueboard.git
 * -----------------------------------------------------------------------------
 */

#ifndef ___DXL_PROTOCOL_V2_H_
#define ___DXL_PROTOCOL_V2_H_

#include <stdint.h>
#include <stdbool.h>

/**
 ********************************************************************************
 **
 **  Framing Constants & Macros
 **
 ********************************************************************************
 */

/* Packet Header */
#define DXL_V2_HEADER1            0xFF
#define DXL_V2_HEADER2            0xFF
#define DXL_V2_HEADER3            0xFD
#define DXL_V2_HEADER4            0x00

/* Minimum length of a packet */
#define DXL_V2_PACKET_MIN_LENGTH 3U

/* Calculate the length of a packet based on the number of parameters */
#define DXL_V2_MAKE_LENGTH(_n_param) ((_n_param) + DXL_V2_PACKET_MIN_LENGTH)

/* Calculate the baudrate register value based on the required BPS */
#define DXL_V2_MAKE_BAUDRATE(_br_bps) (2000000/((_br_bps)+1)) // TODO: check value

/* Maximum number of parameters in a packet */
#define DXL_V2_MAX_PARAMETERS   8U

/* Maximum length of a packet. 2 Byte stuffing max are considered */
#define DXL_V2_MAX_LENGTH		(10U + 2U + DXL_V2_MAX_PARAMETERS)

/* Instructions identifiers */
#define DXL_V2_INS_PING          0x01 // Nb. Param: 0
#define DXL_V2_INS_READ          0x02 // Nb. Param: 2
#define DXL_V2_INS_WRITE         0x03 // Nb. Param: 2
#define DXL_V2_INS_REG_WRITE     0x04 // Nb. Param: 0
#define DXL_V2_INS_ACTION        0x05 // Nb. Param: 0
#define DXL_V2_INS_RESET         0x06 // Nb. Param: 0
#define DXL_V2_INS_REBOOT        0x08 // Nb. Param: TODO
#define DXL_V2_INS_STATUS        0x55 // Nb. Param: 4
#define DXL_V2_INS_SYNC_READ     0x82 // Nb. Param: N
#define DXL_V2_INS_SYNC_WRITE    0x83 // Nb. Param: N
#define DXL_V2_INS_BULK_READ     0x92 // Nb. Param: N
#define DXL_V2_INS_BULK_WRITE    0x93 // Nb. Param: N

/* Status packets lengths */
#define DXL_V2_LEN_STATUS_PING            4U                // Error (1B) + Model (2B) + Firmware (1B)
#define DXL_V2_LEN_STATUS_WRITE           1U                // Error (1B)
#define DXL_V2_LEN_STATUS_READ(_nb_data) (1U + (_nb_data))  // Error (1B) + nb of requested data
#define DXL_V2_LEN_STATUS_ACTION          1U                // Error (1B)
#define DXL_V2_LEN_STATUS_RESET           1U                // Error (1B)
#define DXL_V2_LEN_STATUS_REBOOT          1U                // Error (1B)

/**
 ********************************************************************************
 **
 **  Data content constants
 **
 ********************************************************************************
 */

/* Error number definition */
#define DXL_V2_ERR_RESULT           1U
#define DXL_V2_ERR_INSTRUCTION      2U
#define DXL_V2_ERR_CRC              3U
#define DXL_V2_ERR_DATA_RANGE       4U
#define DXL_V2_ERR_DATA_LENGHT      5U
#define DXL_V2_ERR_DATA_LIMIT       6U
#define DXL_V2_ERR_ACCESS           7U

 /**
 ********************************************************************************
 **
 **  Types & Enumerations
 **
 ********************************************************************************
 */


/* DXL Communication Protocol V2 Packets
 * Triple-byte header is not represented,
 * Instruction and status packets only depends on their contents:
 *  - An instruction for the instruction packets (write, read, action etc.)
 *  - Servo's status for the status packets
 */
typedef struct {
    uint8_t id;
    uint8_t content;
    uint16_t length;
    uint8_t parameters[DXL_V2_MAX_PARAMETERS];
    uint8_t nb_param;
    uint16_t crc;
} dxl_v2_packet_t;


#endif /* ___DXL_PROTOCOL_V2_H_ */
