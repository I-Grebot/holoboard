/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       dxl_v1.c
 * @author     Bebop35
 * @date       Feb 15, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   This component implements specific functions of the Dynamixel
 *   Communication Protocol V1
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/blueboard.git
 * -----------------------------------------------------------------------------
 */

#include "dynamixel.h"

 /**
 ********************************************************************************
 **
 **  Framing functions
 **
 ********************************************************************************
 */

/* Return the checksum of a packet */
uint8_t dxl_v1_compute_checksum(dxl_v1_packet_t* packet)
{
    uint8_t i;
    uint8_t checksum;

    // Init checksum with the 'static' values
    checksum = packet->id + packet->content + packet->length;

    // Add parameters value to checksum
    if(packet->length > 2) {
        for(i=0; i<(packet->length)-2; i++) {
            checksum += (packet->parameters)[i];
        }
    }

    return ~checksum;
}

/* Send a packet through the given interface */
void dxl_v1_send_packet(dxl_interface_t* itf, dxl_v1_packet_t* packet)
{
  uint8_t idx_param;

  itf->hw_send_byte(itf->itf_idx, DXL_V1_HEADER);
  itf->hw_send_byte(itf->itf_idx, DXL_V1_HEADER);
  itf->hw_send_byte(itf->itf_idx, packet->id);
  itf->hw_send_byte(itf->itf_idx, packet->length);
  itf->hw_send_byte(itf->itf_idx, packet->content);

  // Parameters
  if(packet->length > DXL_V1_PACKET_MIN_LENGTH) {
      for(idx_param = 0; idx_param < packet->length-DXL_V1_PACKET_MIN_LENGTH ; idx_param++) {
          itf->hw_send_byte(itf->itf_idx, packet->parameters[idx_param]);
      }
  }

  // Checksum
  itf->hw_send_byte(itf->itf_idx, packet->checksum);

  itf->status = DXL_STATUS_NO_ERROR;

  // Stats
  itf->nb_pkt_tx++;

  #ifdef DXL_DEBUG
    DXL_DEBUG_PUTS(DXL_DEBUG_PFX" V1 Send:"DXL_DEBUG_EOL);
    dxl_v1_print_packet(packet);
  #endif
}

/* Receive a packet from the given interface.
 * This function must be reentrant in order to work with potential
 * concurrent multi-interfaces.*/
void dxl_v1_receive_packet(dxl_interface_t* itf, dxl_v1_packet_t* packet)
{
    uint8_t idx_param;
    uint8_t rx_buffer;
    dxl_status_t status;

    // Initialize values
    packet->id      = -1;   // Because nobody can have this ID
    packet->length  = 0;
    packet->content = 0;

    for(idx_param=0; idx_param < DXL_V1_MAX_PARAMETERS; idx_param++)
        packet->parameters[idx_param] = 0;
    idx_param = 0;

    // Initialize the status buffer
    status = DXL_PASS;
    itf->status = DXL_STATUS_NO_ERROR;

    itf->hw_flush(itf->itf_idx);

    // Wait for the 1st header and retrieve it
    // Detect a timeout error
    if(itf->hw_receive_byte(itf->itf_idx, &rx_buffer) != DXL_PASS) {
      itf->status = DXL_STATUS_ERR_TIMEOUT;
      itf->nb_errors++;
      return;
    }

    // Wait for the 2nd header and retrieve it
    // Detect a timeout error
    if(itf->hw_receive_byte(itf->itf_idx, &rx_buffer) != DXL_PASS) {
      itf->status = DXL_STATUS_ERR_TIMEOUT;
      itf->nb_errors++;
      return;
    }

    /* Detect a header error */
    if(rx_buffer != DXL_V1_HEADER) {
        itf->status = DXL_STATUS_ERR_HEADER;
        itf->nb_errors++;
        return;
    }

    // Wait for the id and retrieve it
    status |= itf->hw_receive_byte(itf->itf_idx, &rx_buffer);
    packet->id = rx_buffer;

    // Wait for the length and retrieve it
    status |= itf->hw_receive_byte(itf->itf_idx, &rx_buffer);
    packet->length = rx_buffer;

    // Wait for the status and retrieve it
    status |= itf->hw_receive_byte(itf->itf_idx, &rx_buffer);
    packet->content = rx_buffer;

    /* Detect a timeout error */
    if(status != DXL_PASS) {
      itf->status = DXL_STATUS_ERR_TIMEOUT;
      itf->nb_errors++;
      return;
    }

    // Retrieve parameters, length must be greater than a given value
    if(packet->length > DXL_V1_PACKET_MIN_LENGTH) {
        for(idx_param=0; idx_param < packet->length-DXL_V1_PACKET_MIN_LENGTH ; idx_param++)
        {
            // Wait for the parameter and retrieve it
            status |= itf->hw_receive_byte(itf->itf_idx, &rx_buffer);
            (packet->parameters)[idx_param] = rx_buffer;
        }
    }

    // Wait for the checksum and retrieve it
    status |= itf->hw_receive_byte(itf->itf_idx, &rx_buffer);
    packet->checksum = rx_buffer & 0xFF;

    // Detect a checksum error
    if(packet->checksum != dxl_v1_compute_checksum(packet)) {
        itf->status = DXL_STATUS_ERR_CHECKSUM;
        itf->nb_errors++;
        return;
    }

#ifdef DXL_DEBUG
    DXL_DEBUG_PUTS(DXL_DEBUG_PFX" V1 Receive:"DXL_DEBUG_EOL);
    dxl_v1_print_packet(packet);
#endif

    // End of RX
    itf->status = status;
    itf->nb_pkt_rx++;
}

// Gather all status: from DXL_V1 but also from the interface layer
uint16_t dxl_v1_get_status(dxl_v1_packet_t* instruction_packet,
                           dxl_v1_packet_t* status_packet,
                           uint8_t expected_param_length)
{
    dxl_status_t status;

    // Retrieve hardware status from DXL_V1
    status = (status_packet->content) & 0xFF;

    // Check that the status packet id matches the sender
    if(status_packet->id != instruction_packet->id)
        status |= DXL_STATUS_ERR_ID;

    // Check that received parameter length is correct
    if(status_packet->length != DXL_V1_MAKE_LENGTH(expected_param_length)) {
        status |= DXL_STATUS_ERR_LENGTH;
    }

    // Check that checksum is correct, this is not relevant if length is incorrect
    if(status_packet->checksum != dxl_v1_compute_checksum(status_packet)) {
        status |= DXL_STATUS_ERR_CHECKSUM;
    }

    return status;
}

/**
********************************************************************************
**
**  Instructions handlers
**
********************************************************************************
*/

// Ping a servo
void dxl_v1_ping(dxl_servo_t* servo)
{
    dxl_v1_packet_t ping_packet;
    dxl_v1_packet_t status_packet;

    // Build the instruction packet
    ping_packet.id       = servo->id;
    ping_packet.length   = DXL_V1_MAKE_LENGTH(0);
    ping_packet.content  = DXL_V1_INS_PING;
    ping_packet.checksum = dxl_v1_compute_checksum(&ping_packet);

    servo->itf->status = DXL_STATUS_NO_ERROR;

    // Send the ping instruction
    dxl_v1_send_packet(servo->itf, &ping_packet);

    // A status packet is always returned after a ping instruction.
    // However check that id was not broadcast, even if that's a dum operation.
    if((servo->id !=  DXL_ID_BROADCAST))
    {
        // Retrieve a status packet, add the UART error flags
        dxl_v1_receive_packet(servo->itf, &status_packet);

        // Get the overall status, add the error flags
        servo->itf->status |= dxl_v1_get_status(&ping_packet, &status_packet, 0);
    }

    if(servo->itf->status != DXL_STATUS_NO_ERROR) {
        servo->itf->nb_errors++;

#ifdef DXL_DEBUG
        dxl_print_error(servo->itf->status, servo->itf->protocol);
#endif // DXL_DEBUG
    }
}

// Reset instruction
void dxl_v1_reset(dxl_servo_t* servo)
{
    dxl_v1_packet_t reset_packet;

    // Build the instruction packet
    reset_packet.id       = servo->id;
    reset_packet.length   = DXL_V1_MAKE_LENGTH(0);
    reset_packet.content  = DXL_V1_INS_RESET;
    reset_packet.checksum = dxl_v1_compute_checksum(&reset_packet);

    servo->itf->status = DXL_STATUS_NO_ERROR;

    // Send the reset instruction
    dxl_v1_send_packet(servo->itf, &reset_packet);

    // Never check for return status packet

    if(servo->itf->status != DXL_STATUS_NO_ERROR) {
        servo->itf->nb_errors++;
    }
}

// Write instruction
// If registered parameter is set, it'll be a reg_write instruction.
void dxl_v1_write(dxl_servo_t* servo, uint8_t address, uint8_t* parameters, size_t nb_param, bool registered)
{
    uint8_t idx_param;
    dxl_v1_packet_t write_packet;
    dxl_v1_packet_t status_packet;

    // Build the packet
    write_packet.id            = servo->id;
    write_packet.length        = 3+nb_param;
    write_packet.content       = registered ? DXL_V1_INS_REG_WRITE:DXL_V1_INS_WRITE;
    write_packet.parameters[0] = address;

    for(idx_param=0; idx_param < nb_param; idx_param++) {
        write_packet.parameters[idx_param+1] = parameters[idx_param];
    }

    write_packet.checksum = dxl_v1_compute_checksum(&write_packet);

    // Send the instruction
    dxl_v1_send_packet(servo->itf, &write_packet);

    servo->itf->status = DXL_STATUS_NO_ERROR;

    // A status packet is returned only if address is not broadcast and
    // the status return level is set to "all packets".
    if((servo->id != DXL_ID_BROADCAST) &&
            servo->itf->return_level == DXL_STATUS_EVERYTHING)
    {
        // Retrieve a status packet, add the UART error flags
        dxl_v1_receive_packet(servo->itf, &status_packet);

        // Get the overall status
        servo->itf->status |= dxl_v1_get_status(&write_packet, &status_packet, 0);
    }

    if(servo->itf->status != DXL_STATUS_NO_ERROR) {
        servo->itf->nb_errors++;

#ifdef DXL_DEBUG
        dxl_print_error(servo->itf->status, servo->itf->protocol);
#endif // DXL_DEBUG
    }


}

// Read
void dxl_v1_read(dxl_servo_t* servo, uint8_t address, uint8_t* datas, size_t nb_data)
{
    dxl_v1_packet_t read_packet;
    dxl_v1_packet_t status_packet;
    uint8_t idx_data;

    // Build the packet
    read_packet.id            = servo->id;
    read_packet.length        = 4;
    read_packet.content       = DXL_V1_INS_READ;
    read_packet.parameters[0] = address;
    read_packet.parameters[1] = nb_data;
    read_packet.checksum      = dxl_v1_compute_checksum(&read_packet);

    // Send the instruction
    dxl_v1_send_packet(servo->itf, &read_packet);

    servo->itf->status = DXL_STATUS_NO_ERROR;

    // A read status packet is returned only if address is not broadcast and
    // the status return level is different than "no answer".
    if((servo->id != DXL_ID_BROADCAST) &&
            servo->itf->return_level != DXL_STATUS_NO_AWNSER)
    {
        // Retrieve a status packet, add the UART error flags
        dxl_v1_receive_packet(servo->itf, &status_packet);

        // Get the overall status, add the CDS55XX and APP error flags
        servo->itf->status |= dxl_v1_get_status(&read_packet, &status_packet, nb_data);

        // Affect read datas if no critical error happened during the
        // status packet reception
        if(servo->itf->status == DXL_STATUS_NO_ERROR) {
            for(idx_data=0; idx_data < status_packet.length-2; idx_data++) {
                datas[idx_data] = status_packet.parameters[idx_data];
            }
        }
    }

    if(servo->itf->status != DXL_STATUS_NO_ERROR) {
        servo->itf->nb_errors++;

#ifdef DXL_DEBUG
        dxl_print_error(servo->itf->status, servo->itf->protocol);
#endif // DXL_DEBUG
    }

}

void dxl_v1_action(dxl_servo_t* servo)
{
    dxl_v1_packet_t action_packet;

    // Build the packet
    action_packet.id        = DXL_ID_BROADCAST;
    action_packet.length    = 2;
    action_packet.content   = DXL_V1_INS_ACTION;
    action_packet.checksum  = dxl_v1_compute_checksum(&action_packet);

    // Send the instruction
    dxl_v1_send_packet(servo->itf, &action_packet);

    if(servo->itf->status != DXL_STATUS_NO_ERROR) {
        servo->itf->nb_errors++;

#ifdef DXL_DEBUG
        dxl_print_error(servo->itf->status, servo->itf->protocol);
#endif // DXL_DEBUG
    }
}


/**
********************************************************************************
**
**  Service routines
**
********************************************************************************
*/

void dxl_v1_get_error_str(char* status_str, size_t status_str_len, dxl_status_t status)
{

    if(status & DXL_V1_ERR_INPUT_VOLTAGE) {
        snprintf(status_str, status_str_len, "V1: Input voltage ");
        status_str += strlen(status_str);
    }

    if(status & DXL_V1_ERR_ANGLE_LIMIT) {
        snprintf(status_str, status_str_len, "V1: Angle limit ");
        status_str += strlen(status_str);
    }

    if(status & DXL_V1_ERR_OVERHEATING) {
        snprintf(status_str, status_str_len, "V1: Overheating ");
        status_str += strlen(status_str);
    }

    if(status & DXL_V1_ERR_RANGE) {
        snprintf(status_str, status_str_len, "V1: Range ");
        status_str += strlen(status_str);
    }

    if(status & DXL_V1_ERR_CHECKSUM) {
        snprintf(status_str, status_str_len, "V1: Checksum ");
        status_str += strlen(status_str);
    }

    if(status & DXL_V1_ERR_OVERLOAD) {
        snprintf(status_str, status_str_len, "V1: Overload ");
        status_str += strlen(status_str);
    }

    if(status & DXL_V1_ERR_INSTRUCTION) {
        snprintf(status_str, status_str_len, "V1: Instruction ");
        status_str += strlen(status_str);
    }
}

/**
********************************************************************************
**
**  Debug
**
********************************************************************************
*/
#ifdef DXL_DEBUG

// Print a packet content in the given string
void dxl_v1_print_packet(dxl_v1_packet_t* packet)
{
    char str[100];
    uint8_t idx_param;

    sprintf(str, "  Id:   %02X"DXL_DEBUG_EOL
                 "  Ins:  %02X"DXL_DEBUG_EOL
                 "  Len:  %02X"DXL_DEBUG_EOL
                 "  Data: ",
            packet->id, packet->content, packet->length);
    DXL_DEBUG_PUTS(str);

    if(packet->length > DXL_V1_PACKET_MIN_LENGTH) {
        for(idx_param = 0; idx_param < packet->length-DXL_V1_PACKET_MIN_LENGTH ; idx_param++) {
            sprintf(str, "%02X ", packet->parameters[idx_param]);
            DXL_DEBUG_PUTS(str);
        }
    } else {
      DXL_DEBUG_PUTS("(none)");
    }

    sprintf(str, DXL_DEBUG_EOL"  Chk:  %02X"DXL_DEBUG_EOL, packet->checksum);
    DXL_DEBUG_PUTS(str);
}

#endif // DXL_DEBUG


