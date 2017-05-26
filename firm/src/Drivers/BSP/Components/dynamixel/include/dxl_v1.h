/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       dxl_v1.h
 * @author     Bebop35
 * @date       Feb 09, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   Dynamixel's Digital Servo Protocol v1 framing description
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/blueboard.git
 * -----------------------------------------------------------------------------
 */

#ifndef ___DXL_PROTOCOL_V1_H_
#define ___DXL_PROTOCOL_V1_H_

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
#define DXL_V1_HEADER            0xFF

/* Minimum length of a packet */
#define DXL_V1_PACKET_MIN_LENGTH 2U

/* Calculate the length of a packet based on the number of parameters */
#define DXL_V1_MAKE_LENGTH(_n_param) ((_n_param) + DXL_V1_PACKET_MIN_LENGTH)

/* Calculate the baudrate register value based on the required BPS */
#define DXL_V1_MAKE_BAUDRATE(_br_bps) (2000000/((_br_bps)+1))

/* Maximum number of parameters in a packet */
#define DXL_V1_MAX_PARAMETERS   8U

/* Instructions identifiers */
#define DXL_V1_INS_PING          0x01 // Nb. Param: 0
#define DXL_V1_INS_READ          0x02 // Nb. Param: 2
#define DXL_V1_INS_WRITE         0x03 // Nb. Param: 2
#define DXL_V1_INS_REG_WRITE     0x04 // Nb. Param: 0
#define DXL_V1_INS_ACTION        0x05 // Nb. Param: 0
#define DXL_V1_INS_RESET         0x06 // Nb. Param: 0
#define DXL_V1_INS_SYNC_WRITE    0x83 // Nb. Param: N
#define DXL_V1_INS_BULK_READ     0x92 // Nb. Param: N

/**
 ********************************************************************************
 **
 **  Data content constants
 **
 ********************************************************************************
 */

/* Error flags definition */
#define DXL_V1_ERR_INPUT_VOLTAGE    1U
#define DXL_V1_ERR_ANGLE_LIMIT      2U
#define DXL_V1_ERR_OVERHEATING      4U
#define DXL_V1_ERR_RANGE            8U
#define DXL_V1_ERR_CHECKSUM        16U
#define DXL_V1_ERR_OVERLOAD        32U
#define DXL_V1_ERR_INSTRUCTION     64U

 /**
 ********************************************************************************
 **
 **  Types & Enumerations
 **
 ********************************************************************************
 */


/* DXL Communication Protocol V1 Packets
 * Double-byte header is not represented,
 * Instruction and status packets only depends on their contents:
 *  - An instruction for the instruction packets (write, read, action etc.)
 *  - Servo's status for the status packets
 */
typedef struct {
    uint8_t id;
    uint8_t length;
    uint8_t content;
    uint8_t parameters[DXL_V1_MAX_PARAMETERS];
    uint8_t checksum;
} dxl_v1_packet_t;


#endif /* ___DXL_PROTOCOL_V1_H_ */
