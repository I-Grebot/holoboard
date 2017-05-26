/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       dxl_v2.c
 * @author     Bebop35
 * @date       Apr 17, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   This component implements specific functions of the Dynamixel
 *   Communication Protocol V2
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/blueboard.git
 * -----------------------------------------------------------------------------
 */

#include "dynamixel.h"

/*
 * Some local, private variables
 */

// Headers as an array, for easier handling
static const uint8_t dxl_v2_headers[] = {DXL_V2_HEADER1,
                                         DXL_V2_HEADER2,
                                         DXL_V2_HEADER3,
                                         DXL_V2_HEADER4};
static const size_t dxl_v2_headers_lenght = sizeof(dxl_v2_headers) / sizeof(dxl_v2_headers[0]);

/**
 ********************************************************************************
 **
 **  Framing functions
 **
 ********************************************************************************
 */

// Update the CRC data
uint16_t dxl_v2_compute_crc(uint8_t *data, uint8_t data_size)
{
  extern const uint16_t dxl_v2_crc_table[];
  uint8_t crc_table_idx;
  uint16_t data_idx;
  uint16_t crc_accum = 0;

  for (data_idx = 0; data_idx < data_size; data_idx++)
  {
    crc_table_idx = ((uint16_t)(crc_accum >> 8) ^ data[data_idx]) & 0xFF;
    crc_accum = (crc_accum << 8) ^ dxl_v2_crc_table[crc_table_idx];
  }

  return crc_accum;
}

/* Send a packet through the given interface
 * Since byte-stuffing is inserted, we build a complete buffer before sending
 * anything. This is also required to know the exact length and CRC of the packet.
 *
 * Update the packet's length and CRC values
 */
void dxl_v2_send_packet(dxl_interface_t* itf, dxl_v2_packet_t* packet)
{
  uint8_t idx_param;
  uint8_t buffer[DXL_V2_MAX_LENGTH];

  uint8_t idx = 0;
  uint8_t len_idx;
  uint8_t header1_cnt;
  uint8_t stuffing_cnt;

  // Header (no Byte-Stuffing)
  buffer[idx++] = dxl_v2_headers[0];
  buffer[idx++] = dxl_v2_headers[1];
  buffer[idx++] = dxl_v2_headers[2];
  buffer[idx++] = dxl_v2_headers[3]; // actually reserved

  // ID (no BS)
  buffer[idx++] = packet->id;

  // Length is not known now, 2 bytes are skipped
  // length index position in the buffer is memorized
  len_idx = idx;
  idx+=2;

  // Instruction (no BS)
  buffer[idx++] = packet->content;

  // Start to add parameters
  header1_cnt = 0;
  stuffing_cnt = 0;
  for (idx_param = 0; idx_param < packet->nb_param; idx_param++)
  {
    // Check if the triple byte-stuffing value is matched
    if (packet->parameters[idx_param] == DXL_V2_HEADER3 && header1_cnt >=2 ) {
      header1_cnt = 0;
      buffer[idx++] = DXL_V2_HEADER3;
      buffer[idx++] = DXL_V2_HEADER3;
      stuffing_cnt++;

      // No need to add a byte
    } else {

      // Keep track of consecutive header1 values
      if (packet->parameters[idx_param] == DXL_V2_HEADER1) {
        header1_cnt++;
      } else {
        header1_cnt = 0;
      }

      // Still add data to the buffer
      buffer[idx++] = packet->parameters[idx_param];
    }

  }

  // Compute the packet length and add it to the buffer
  packet->length = DXL_V2_MAKE_LENGTH(packet->nb_param) + stuffing_cnt;
  buffer[len_idx]   = (uint8_t) (  packet->length		  & 0xFF);
  buffer[len_idx+1] = (uint8_t) (( packet->length >> 8 ) & 0xFF);

  // Compute CRC, save the buffer length
  packet->crc = dxl_v2_compute_crc(buffer, idx);
  len_idx = idx;

  // Send the buffer
  for(idx = 0; idx < len_idx; idx++)
  {
    // TODO Check for error
    itf->hw_send_byte(itf->itf_idx, buffer[idx]);
  }

  // Finally send the CRC
  itf->hw_send_byte(itf->itf_idx, ((uint8_t)  packet->crc & 0xFF));
  itf->hw_send_byte(itf->itf_idx, ((uint8_t) (packet->crc >> 8U) & 0xFF));

  // Status
  itf->status = DXL_STATUS_NO_ERROR;

  // Stats
  itf->nb_pkt_tx++;

#ifdef DXL_DEBUG
  DXL_DEBUG_PUTS(DXL_DEBUG_PFX" V2 Send:"DXL_DEBUG_EOL);
  dxl_v2_print_packet(packet);
#endif

}

/* Receive a packet from the given interface.
 * This function must be reentrant in order to work with potential
 * concurrent multi-interfaces.*/
void dxl_v2_receive_packet(dxl_interface_t* itf, dxl_v2_packet_t* packet)
{
  uint8_t idx_param;
  uint8_t buffer[DXL_V2_MAX_LENGTH];
  dxl_status_t status;
  uint8_t idx;
  uint8_t idx_len;
  uint8_t header1_cnt;
  uint8_t stuffing_cnt;

  // Initialize values
  packet->id       = -1;   // Because nobody can have this ID
  packet->length   = 0;
  packet->content  = 0;
  packet->nb_param = 0;
  packet->crc      = 0;

  for(idx_param=0; idx_param < DXL_V2_MAX_PARAMETERS; idx_param++)
    packet->parameters[idx_param] = 0;
  idx_param = 0;

  // Initialize the status buffer
  status = DXL_PASS;
  itf->status = DXL_STATUS_NO_ERROR;

  // Flush interface
  itf->hw_flush(itf->itf_idx);

  // Fetch headers
  for(idx = 0; idx < dxl_v2_headers_lenght; idx++)
  {
    // Detect a timeout header
    if(itf->hw_receive_byte(itf->itf_idx, &buffer[idx]) != DXL_PASS)
    {
      itf->status = DXL_STATUS_ERR_TIMEOUT;
      itf->nb_errors++;
      return;
    }

    /* Detect a header error */
    if(buffer[idx] != dxl_v2_headers[idx])
    {
      itf->status = DXL_STATUS_ERR_HEADER;
      itf->nb_errors++;
      return;
    }

  } // for(headers)

  // Wait for the id and retrieve it
  status |= itf->hw_receive_byte(itf->itf_idx, &buffer[idx]);
  packet->id = buffer[idx];
  idx++;

  // Wait for the length (2 Bytes) and retrieve it
  status |= itf->hw_receive_byte(itf->itf_idx, &buffer[idx]);
  status |= itf->hw_receive_byte(itf->itf_idx, &buffer[idx+1]);
  packet->length = buffer[idx] + (buffer[idx+1] << 8U);
  idx+=2;

  // Wait for the instruction and retrieve it
  status |= itf->hw_receive_byte(itf->itf_idx, &buffer[idx]);
  packet->content = buffer[idx];
  idx++;

  // Detect a timeout error for all previous cases
  if(status != DXL_PASS) {
    itf->status = DXL_STATUS_ERR_TIMEOUT;
    itf->nb_errors++;
    return;
  }

  // Fetch parameters
  header1_cnt = 0;
  stuffing_cnt = 0;
  idx_len = idx;
  idx_param = 0;
  if(idx_len > DXL_V2_PACKET_MIN_LENGTH)
  {
    for(idx_len = 0; idx_len < packet->length-DXL_V2_PACKET_MIN_LENGTH ; idx_len++)
    {
      // Wait for the parameter and retrieve it
      status |= itf->hw_receive_byte(itf->itf_idx, &buffer[idx]);

      // Check if the triple byte-stuffing value is matched
      // If so, ignore the parameter
      if (buffer[idx] == DXL_V2_HEADER3 && header1_cnt >=2 ) {
        header1_cnt = 0;
        stuffing_cnt++;

      // Parameter value is saved
      } else {

        // Keep track of consecutive header1 values
        if (buffer[idx] == DXL_V2_HEADER1) {
          header1_cnt++;
        } else {
          header1_cnt = 0;
        }

        // Always save value
        packet->parameters[packet->nb_param++] = buffer[idx];
      }

      idx++;
    }
  }

  // Wait for the CRC (2 Bytes) and retrieve it
  status |= itf->hw_receive_byte(itf->itf_idx, &buffer[idx]);
  status |= itf->hw_receive_byte(itf->itf_idx, &buffer[idx+1]);
  packet->crc = buffer[idx] + (buffer[idx+1] << 8U);

  // Detect a CRC error
  if(packet->crc != dxl_v2_compute_crc(buffer, idx))
  {
    itf->status = DXL_STATUS_ERR_CHECKSUM;
    itf->nb_errors++;
    return;
  }

#ifdef DXL_DEBUG
  DXL_DEBUG_PUTS(DXL_DEBUG_PFX" V2 Receive:"DXL_DEBUG_EOL);
  dxl_v2_print_packet(packet);
#endif

  // End of RX
  itf->status = status;
  itf->nb_pkt_rx++;
}

// Gather all status: from DXL_V2 but also from the interface layer
uint16_t dxl_v2_get_status(dxl_v2_packet_t* instruction_packet,
                           dxl_v2_packet_t* status_packet,
                           uint8_t expected_param_length)
{
    dxl_status_t status;

    // Check that the status packet id matches the sender
    if(status_packet->id != instruction_packet->id) {
        return DXL_STATUS_ERR_ID;
    }

    // Check that correct instruction was returned
    if(status_packet->content != DXL_V2_INS_STATUS) {
      return DXL_STATUS_ERR_INSTRUCTION;
    }

    // Check that received parameter length is correct
    if(status_packet->nb_param != expected_param_length) {
      return DXL_STATUS_ERR_LENGTH;
    }

    // CRC is checked at RX level; because we need to be aware of the entire buffer

    // Retrieve hardware status from DXL_V2
    return status_packet->parameters[0];

}

/**
 ********************************************************************************
 **
 **  Instructions handlers
 **
 ********************************************************************************
 */

// Ping a servo
void dxl_v2_ping(dxl_servo_t* servo)
{
  dxl_v2_packet_t ping_packet;
  dxl_v2_packet_t status_packet;

  // Build the instruction packet
  ping_packet.id       = servo->id;
  ping_packet.nb_param = 0;
  ping_packet.content  = DXL_V2_INS_PING;

  servo->itf->status = DXL_STATUS_NO_ERROR;

  // Send the ping instruction
  dxl_v2_send_packet(servo->itf, &ping_packet);

  // A status packet is always returned after a ping instruction.
  // However check that id was not broadcast, even if that's a dum operation.
  if((servo->id !=  DXL_ID_BROADCAST))
  {
      // Retrieve a status packet, add the UART error flags
      dxl_v2_receive_packet(servo->itf, &status_packet);

      // Get the overall status, add the error flags
      servo->itf->status |= dxl_v2_get_status(&ping_packet, &status_packet, DXL_V2_LEN_STATUS_PING);
  }

  if(servo->itf->status != DXL_STATUS_NO_ERROR) {
    servo->itf->nb_errors++;

#ifdef DXL_DEBUG
    dxl_print_error(servo->itf->status, servo->itf->protocol);
#endif // DXL_DEBUG
  }
}

// Write instruction
// If registered parameter is set, it'll be a reg_write instruction.
void dxl_v2_write(dxl_servo_t* servo, uint16_t address, uint8_t* parameters, size_t nb_param, bool registered)
{
  uint8_t idx_param;
  dxl_v2_packet_t write_packet;
  dxl_v2_packet_t status_packet;

  // Build the packet
  write_packet.id       = servo->id;
  write_packet.nb_param = nb_param + 2; // +2 for address bytes
  write_packet.content  = registered ? DXL_V2_INS_REG_WRITE:DXL_V2_INS_WRITE;
  write_packet.parameters[0] =  address & 0xFF;
  write_packet.parameters[1] = (address >> 8U) & 0xFF;

  for(idx_param=0; idx_param < nb_param; idx_param++) {
    write_packet.parameters[idx_param+2] = parameters[idx_param];
  }

  // Send the instruction
  dxl_v2_send_packet(servo->itf, &write_packet);

  servo->itf->status = DXL_STATUS_NO_ERROR;

  // A status packet is returned only if address is not broadcast and
  // the status return level is set to "all packets".
  if((servo->id != DXL_ID_BROADCAST) &&
      servo->itf->return_level == DXL_STATUS_EVERYTHING)
  {
      // Retrieve a status packet, add the UART error flags
      dxl_v2_receive_packet(servo->itf, &status_packet);

      // Get the overall status
      servo->itf->status |= dxl_v2_get_status(&write_packet, &status_packet, DXL_V2_LEN_STATUS_WRITE);
  }

  if(servo->itf->status != DXL_STATUS_NO_ERROR) {
    servo->itf->nb_errors++;

  #ifdef DXL_DEBUG
      dxl_print_error(servo->itf->status, servo->itf->protocol);
  #endif // DXL_DEBUG
  }
}

// Read instruction
void dxl_v2_read(dxl_servo_t* servo, uint16_t address, uint8_t* datas, size_t nb_data)
{
  dxl_v2_packet_t read_packet;
  dxl_v2_packet_t status_packet;
  uint8_t idx_param;

  // Build the packet
  read_packet.id            = servo->id;
  read_packet.nb_param      = nb_data + 4; // +2 for address bytes, +2 for data length
  read_packet.content       = DXL_V2_INS_READ;
  read_packet.parameters[0] =  address & 0xFF;
  read_packet.parameters[1] = (address >> 8U) & 0xFF;
  read_packet.parameters[2] =  nb_data & 0xFF;
  read_packet.parameters[3] = (nb_data >> 8U) & 0xFF;

  // Send the instruction
  dxl_v2_send_packet(servo->itf, &read_packet);

  servo->itf->status = DXL_STATUS_NO_ERROR;

  // A read status packet is returned only if address is not broadcast and
  // the status return level is different than "no answer".
  if((servo->id != DXL_ID_BROADCAST) &&
      servo->itf->return_level != DXL_STATUS_NO_AWNSER)
  {
    // Retrieve a status packet, add the UART error flags
    dxl_v2_receive_packet(servo->itf, &status_packet);

    // Get the overall status
    servo->itf->status |= dxl_v2_get_status(&read_packet, &status_packet, DXL_V2_LEN_STATUS_READ(nb_data));

    // Affect read datas if no critical error happened during the
    // status packet reception.
    // Strip 1st byte containing the error status
    if(servo->itf->status == DXL_STATUS_NO_ERROR) {
      for(idx_param = 0; idx_param < nb_data; idx_param++) {
        datas[idx_param] = status_packet.parameters[DXL_V2_LEN_STATUS_READ(0) + idx_param];
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

void dxl_v2_action(dxl_servo_t* servo)
{
  dxl_v2_packet_t action_packet;
  dxl_v2_packet_t status_packet;

  // Build the instruction packet
  action_packet.id       = servo->id;
  action_packet.nb_param = 0;
  action_packet.content  = DXL_V2_INS_ACTION;

  servo->itf->status = DXL_STATUS_NO_ERROR;

  // Send the ping instruction
  dxl_v2_send_packet(servo->itf, &action_packet);

  // A status packet is returned only if address is not broadcast and
  // the status return level is set to "all packets".
  if((servo->id !=  DXL_ID_BROADCAST)  &&
      servo->itf->return_level == DXL_STATUS_EVERYTHING)
  {
      // Retrieve a status packet, add the UART error flags
      dxl_v2_receive_packet(servo->itf, &status_packet);

      // Get the overall status, add the error flags
      servo->itf->status |= dxl_v2_get_status(&action_packet, &status_packet, DXL_V2_LEN_STATUS_ACTION);
  }

  if(servo->itf->status != DXL_STATUS_NO_ERROR) {
    servo->itf->nb_errors++;

#ifdef DXL_DEBUG
    dxl_print_error(servo->itf->status, servo->itf->protocol);
#endif // DXL_DEBUG
  }
}


// Factory Reset instruction
void dxl_v2_reset(dxl_servo_t* servo)
{
  dxl_v2_packet_t reset_packet;
  dxl_v2_packet_t status_packet;

  // Build the instruction packet
  reset_packet.id       = servo->id;
  reset_packet.nb_param = 0;
  reset_packet.content  = DXL_V2_INS_RESET;

  servo->itf->status = DXL_STATUS_NO_ERROR;

  // Send the ping instruction
  dxl_v2_send_packet(servo->itf, &reset_packet);

  // A status packet is returned only if address is not broadcast and
  // the status return level is set to "all packets".
  if((servo->id !=  DXL_ID_BROADCAST)  &&
      servo->itf->return_level == DXL_STATUS_EVERYTHING)
  {
      // Retrieve a status packet, add the UART error flags
      dxl_v2_receive_packet(servo->itf, &status_packet);

      // Get the overall status, add the error flags
      servo->itf->status |= dxl_v2_get_status(&reset_packet, &status_packet, DXL_V2_LEN_STATUS_RESET);
  }

  if(servo->itf->status != DXL_STATUS_NO_ERROR) {
    servo->itf->nb_errors++;

#ifdef DXL_DEBUG
    dxl_print_error(servo->itf->status, servo->itf->protocol);
#endif // DXL_DEBUG
  }
}

// Reboot instruction
void dxl_v2_reboot(dxl_servo_t* servo)
{
  dxl_v2_packet_t reboot_packet;
  dxl_v2_packet_t status_packet;

  // Build the instruction packet
  reboot_packet.id       = servo->id;
  reboot_packet.nb_param = 0;
  reboot_packet.content  = DXL_V2_INS_REBOOT;

  servo->itf->status = DXL_STATUS_NO_ERROR;

  // Send the ping instruction
  dxl_v2_send_packet(servo->itf, &reboot_packet);

  // A status packet is returned only if address is not broadcast and
  // the status return level is set to "all packets".
  if((servo->id !=  DXL_ID_BROADCAST)  &&
      servo->itf->return_level == DXL_STATUS_EVERYTHING)
  {
      // Retrieve a status packet, add the UART error flags
      dxl_v2_receive_packet(servo->itf, &status_packet);

      // Get the overall status, add the error flags
      servo->itf->status |= dxl_v2_get_status(&reboot_packet, &status_packet, DXL_V2_LEN_STATUS_REBOOT);
  }

  if(servo->itf->status != DXL_STATUS_NO_ERROR) {
    servo->itf->nb_errors++;

#ifdef DXL_DEBUG
    dxl_print_error(servo->itf->status, servo->itf->protocol);
#endif // DXL_DEBUG
  }
}

// TODO:
// sync_read()
// sync_write()
// bulk_read()
// bulk_write()

/**
 ********************************************************************************
 **
 **  Service routines
 **
 ********************************************************************************
 */
void dxl_v2_get_error_str(char* status_str, size_t status_str_len, dxl_status_t status)
{
  switch(status)
  {
    case DXL_V2_ERR_RESULT      : snprintf(status_str, status_str_len, "V2: Result fail " ); break;
    case DXL_V2_ERR_INSTRUCTION : snprintf(status_str, status_str_len, "V2: Instruction " ); break;
    case DXL_V2_ERR_CRC         : snprintf(status_str, status_str_len, "V2: CRC "         ); break;
    case DXL_V2_ERR_DATA_RANGE  : snprintf(status_str, status_str_len, "V2: Data range "  ); break;
    case DXL_V2_ERR_DATA_LENGHT : snprintf(status_str, status_str_len, "V2: Data length " ); break;
    case DXL_V2_ERR_DATA_LIMIT  : snprintf(status_str, status_str_len, "V2: Data limit "  ); break;
    case DXL_V2_ERR_ACCESS      : snprintf(status_str, status_str_len, "V2: Access "      ); break;
  }
  status_str += strlen(status_str);
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
void dxl_v2_print_packet(dxl_v2_packet_t* packet)
{
  char str[100];
  uint8_t idx_param;

  sprintf(str,
      "  Id:    %02X"DXL_DEBUG_EOL
      "  Ins:   %02X"DXL_DEBUG_EOL
      "  Par:   %02X"DXL_DEBUG_EOL
      "  Len:   %02X"DXL_DEBUG_EOL
      "  Data:  ",
      packet->id, packet->content, packet->nb_param, packet->length);
  DXL_DEBUG_PUTS(str);

  if(packet->nb_param > 0)
  {
    for(idx_param = 0; idx_param < packet->nb_param ; idx_param++) {
      sprintf(str, "%02X ", packet->parameters[idx_param]);
      DXL_DEBUG_PUTS(str);
    }
  } else {
    DXL_DEBUG_PUTS("(none)");
  }

  sprintf(str, DXL_DEBUG_EOL"  CRC:   %04X"DXL_DEBUG_EOL, packet->crc);
  DXL_DEBUG_PUTS(str);
}

#endif // DXL_DEBUG


