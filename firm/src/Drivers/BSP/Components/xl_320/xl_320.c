#include "xl_320.h"

xl_320_cfg_t xl_320_cfg;

/* Local, Private functions */
static void xl_320_send_packet(xl_320_packet_t* packet);
static unsigned short update_crc(unsigned short crc_accum, uint8_t *data_blk_ptr, unsigned short data_blk_size);
static void xl_320_write(uint8_t id, uint8_t address, uint8_t* parameters, uint8_t nb_param, uint8_t instruction);
static uint8_t xl_320_receive_packet(xl_320_packet_t* packet);

// -----------------------------------------------------------------------------
// First layer: handles UART data
// -----------------------------------------------------------------------------

void xl_320_init(XL_320_Com_Mode mode){
    xl_320_cfg.mode = mode;
    switch(mode)
    {
        case XL_320_TX_ONLY:
        case XL_320_TXRX_ONE_PIN:
        case XL_320_TXRX_TWO_PINS:
        default:
            xl_320_cfg.hw_send_byte = NULL;
            xl_320_cfg.hw_receive_byte = NULL;
            xl_320_cfg.hw_flush = NULL;
            xl_320_cfg.hw_switch = NULL;
        break;
    }
}
void xl_320_set_hw_send(void (*hw_send)(uint8_t, uint8_t))
{
    xl_320_cfg.hw_send_byte = hw_send;
}
void xl_320_set_hw_receive(uint8_t (*hw_receive)(uint8_t*))
{
    xl_320_cfg.hw_receive_byte = hw_receive;
}
void xl_320_set_hw_flush(void (*hw_flush)(void))
{
    xl_320_cfg.hw_flush = hw_flush;
}
void xl_320_set_hw_switch(void (*hw_switch)(uint8_t))
{
    xl_320_cfg.hw_switch = hw_switch;
}

// Write instruction
unsigned short update_crc(unsigned short crc_accum, uint8_t *data_blk_ptr, unsigned short data_blk_size){
    unsigned short i, j;
    unsigned short crc_table[256] = {
        0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
        0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
        0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
        0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
        0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
        0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
        0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
        0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
        0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
        0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
        0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
        0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
        0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
        0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
        0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
        0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
        0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
        0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
        0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
        0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
        0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
        0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
        0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
        0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
        0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
        0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
        0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
        0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
        0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
        0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
        0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
        0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
    };

    for(j = 0; j < data_blk_size; j++)
    {
        i = ((unsigned short)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
        crc_accum = (crc_accum << 8) ^ crc_table[i];
    }

    return crc_accum;
}

void xl_320_write(uint8_t id, uint8_t address, uint8_t* parameters, uint8_t nb_param, uint8_t instruction){
    unsigned char idx_param;
    xl_320_packet_t write_packet;

    // Build the packet
    write_packet.id            = id;
    write_packet.instruction   = instruction;
    write_packet.parameter_nb  = nb_param + 2;
    write_packet.packet_length = write_packet.parameter_nb + 3;
    write_packet.parameters[0] = (uint8_t)(address&0xFF); // LSW
    write_packet.parameters[1] = (uint8_t)((address>>8)&0xFF); // MSW

    for(idx_param=2; idx_param < write_packet.parameter_nb; idx_param++)
    {
        write_packet.parameters[idx_param] = parameters[idx_param-2];
    }

    // Send the instruction
    xl_320_send_packet(&write_packet);
}

void xl_320_send_packet(xl_320_packet_t* packet){
	char str[20];
    unsigned short crc16;
    uint8_t buffer[XL_320_MAX_PACKET];
    uint8_t idx_buffer=0;
    uint8_t idx_param;
    uint8_t length;
    uint8_t stuffing=0;
    uint8_t ff = 0;
    buffer[idx_buffer++]=XL_320_HEADER;
    buffer[idx_buffer++]=XL_320_HEADER;
    buffer[idx_buffer++]=XL_320_HEADER_2;
    buffer[idx_buffer++]=0x00;          // Reserved
    buffer[idx_buffer++]=packet->id;
    length = idx_buffer;
    idx_buffer +=2;
    buffer[idx_buffer++]=packet->instruction;
    for (idx_param=0; idx_param<packet->parameter_nb; idx_param++){
        if (packet->parameters[idx_param] == 0xfd && ff>=2) {
            ff = 0;
            buffer[idx_buffer++]=0xfd;
            buffer[idx_buffer++]=0xfd;
            stuffing++;
        } else {
            if (packet->parameters[idx_param] == 0xff) {
                ff++;
            } else {
                ff = 0;
            }
            buffer[idx_buffer++] = packet->parameters[idx_param];
        }
    }

    buffer[length]=(uint8_t)((packet->packet_length+stuffing)&0xFF);
    buffer[length+1]=(uint8_t)(((packet->packet_length+stuffing)>>8)&0xFF);

    crc16 = update_crc(0, buffer,idx_buffer);
    length = idx_buffer;

    if(xl_320_cfg.mode==XL_320_TXRX_ONE_PIN)
        xl_320_cfg.hw_switch(XL_320_TX);

    sprintf(str, "XL Send: ");
	serial_puts(str);

    for(idx_buffer = 0; idx_buffer<length; idx_buffer++)
    {
        xl_320_cfg.hw_send_byte(0, buffer[idx_buffer]);
        sprintf(str, "%02X ", buffer[idx_buffer]);
        serial_puts(str);
    }
    xl_320_cfg.hw_send_byte(0, crc16 & 0xFF); // CRC_L
    xl_320_cfg.hw_send_byte(0, (crc16>>8)&0xFF); // CRC_H

    sprintf(str, "%02X ", crc16 & 0xFF);
    serial_puts(str);
    sprintf(str, "%02X\n", (crc16>>8)&0xFF);
    serial_puts(str);

    if(xl_320_cfg.mode==XL_320_TXRX_ONE_PIN)
        xl_320_cfg.hw_switch(XL_320_RX);
}

//// Status check to be added
uint8_t xl_320_receive_packet(xl_320_packet_t* packet){
    uint8_t rx_buffer;
    uint8_t buffer[XL_320_MAX_PACKET];
    uint8_t idx_buffer = 0;
    uint8_t idx_param = 0;
    uint8_t status;

    // Initialize values
    packet->id = XL_320_ID_IMPOSSIBLE;   // Because nobody can have this ID
    packet->packet_length = 0;
    packet->instruction = 0;
    for(idx_param=0; idx_param<XL_320_MAX_PARAM; idx_param++)
        packet->parameters[idx_param] = 0;
    idx_param = 0;
    status = 0;

    // Flush RX before receiving anything
    xl_320_cfg.hw_flush();

    // Wait for the 1rst header and retrieve
    status = xl_320_cfg.hw_receive_byte(&rx_buffer);
    if(status&0x01)
        return XL_320_ERROR_UART_TIMEOUT;

    if(rx_buffer != (uint8_t)XL_320_HEADER)
        return XL_320_ERROR_UART_HEADER;

    buffer[idx_buffer++] = rx_buffer;

    do {
        status = xl_320_cfg.hw_receive_byte(&rx_buffer);
        if(!(status&0x01))
            buffer[idx_buffer++] = rx_buffer;
    }while(!(status&0x01));

    if(buffer[8])       // If error flag is set
        return buffer[8];

    packet->id = buffer[4];
    packet->packet_length = buffer[5];
    packet->parameter_nb = packet->packet_length-3;
    packet->instruction = buffer[7];

    for(idx_param=0; idx_param<packet->parameter_nb; idx_param++)
    packet->parameters[idx_param] = buffer[9];
    return status;
}
uint8_t xl_320_is_received(void){
    xl_320_packet_t packet;
    xl_320_receive_packet(&packet);
    if(packet.instruction==XL_320_STATUS)
        return XL_320_RECEIVED;
     else
        return 0;
}                                              //Tested

/****************/
/* Instructions */
/****************/

/**
  * @brief Set the ID of all XL320, it is recommended to have only one XL320 plugged when calling this method.
  * @param id: the new ID that will be written on all XL320.
  */
void xl_320_set_id(uint8_t id) {
     xl_320_write(XL_320_ID_BROADCAST, Xl_320_ADR_ID, &id, 1, XL_320_INS_WRITE);
}                                                //Tested

/**
  * @brief Set the baud rate of the specified XL320.
  * @param id: the ID of the targeted XL320.
  * @param baudrate: the baudrate identifier (0: 9600, 1:57600, 2:115200, 3:1Mbps).
  */
void xl_320_set_baudrate(uint8_t id, uint8_t baudrate) {
     xl_320_write(id, Xl_320_ADR_BAUDRATE, &baudrate, 1, XL_320_INS_WRITE);
}                        //Tested

/**
  * @brief Set the delay time per data value that takes from the transmission of Instruction Packet until the return of Status Packet.
  * @param id: the ID of the targeted XL320.
  * @param return_delay: the delay multiplier (0 to 254). Calculated delay is 2usec * return_delay.
  */
void xl_320_set_return_delay(uint8_t id, uint8_t return_delay) {
     xl_320_write(id, Xl_320_ADR_RET_DELAY, &return_delay, 1, XL_320_INS_WRITE);
}

void xl_320_set_cw_limit(uint8_t id, uint16_t cw_limit) {
    uint8_t angle[2];
    angle[0]=(uint8_t)(cw_limit&0xFF);
    angle[1]=(uint8_t)((cw_limit>>8)&0xFF);
     xl_320_write(id, Xl_320_ADR_CW_LIMIT, angle, 2, XL_320_INS_WRITE);
}
void xl_320_set_ccw_limit(uint8_t id, uint16_t ccw_limit) {
    uint8_t angle[2];
    angle[0]=(uint8_t)(ccw_limit&0xFF);
    angle[1]=(uint8_t)((ccw_limit>>8)&0xFF);
     xl_320_write(id, Xl_320_ADR_CCW_LIMIT, angle, 2, XL_320_INS_WRITE);
}

/**
  * @brief Set control mode of the specified XL320.
  * @param id: the ID of the targeted XL320.
  * @param mode: the desired mode (1: Wheel Mode, 2:Join Mode).
  */
void xl_320_set_control_mode(uint8_t id, uint8_t mode) {
    xl_320_write(id, Xl_320_ADR_CONTROL_MODE, &mode, 1, XL_320_INS_WRITE);
}

//void xl_320_set_temp_limit(uint8_t id, uint8_t temp_limit) {
//    xl_320_write(id, Xl_320_ADR_TEMP_LIMIT, &temp_limit, 1, XL_320_INS_WRITE);
//}


void xl_320_set_min_voltage(uint8_t id, uint8_t min_voltage) {
    xl_320_write(id, Xl_320_ADR_LOW_VOLTAGE, &min_voltage, 1, XL_320_INS_WRITE);
}
void xl_320_set_max_voltage(uint8_t id, uint8_t max_voltage) {
    xl_320_write(id, Xl_320_ADR_HIGH_VOLTAGE, &max_voltage, 1, XL_320_INS_WRITE);
}

/**
  * @brief Set the torque value of maximum output.
  * @param id: the ID of the targeted XL320.
  * @param torque_limit: the desired maximum torque value (0 to 1023). Calculated maximum torque is ~0.1% * torque_limit.
  */
void xl_320_set_torque_limit(uint8_t id, uint16_t torque_limit) {
    uint8_t torque[2];
    torque[0]=(uint8_t)(torque_limit&0xFF);
    torque[1]=(uint8_t)((torque_limit>>8)&0xFF);
     xl_320_write(id, Xl_320_ADR_MAX_TORQUE, torque, 2, XL_320_INS_WRITE);
}

/**
  * @brief Set when return status are send by the specified XL320.
  * @param id: the ID of the targeted XL320.
  * @param level: the desired policy (0: never returns status, 1: returns status only for READ commands, 2: returns status for all commands).
  */
void xl_320_set_return_level(uint8_t id, uint8_t level) {
     xl_320_write(id, Xl_320_ADR_RET_LEVEL, &level, 1, XL_320_INS_WRITE);
}                       //Tested

void xl_320_set_alarm_shutdown(uint8_t id, uint8_t shutdown) {
     xl_320_write(id, Xl_320_ADR_ALARM_SHUTDOWN, &shutdown, 1, XL_320_INS_WRITE);
}

/**
  * @brief Enable or disable torque on the specified XL320.
  * @param id: the ID of the targeted XL320.
  * @param torque_en: the Torque Enable value (0: torque disabled, 1: torque enabled).
  */
void xl_320_set_torque_en(uint8_t id, uint8_t torque_en) {
     xl_320_write(id, Xl_320_ADR_TORQUE_EN, &torque_en, 1, XL_320_INS_WRITE);
}

/**
  * @brief Set the LED color on the specified XL320.
  * @param id: the ID of the targeted XL320.
  * @param led: the LED color (0b111: white, 0b101: pink, 0b110: cyan, 0b011: yellow, 0b100: blue, 0b010: green, 0b001: red).
  */
void xl_320_set_led(uint8_t id , uint8_t led) {
    xl_320_write(id, XL_320_ADR_LED, &led, 1, XL_320_INS_WRITE);
}                                 //Tested

/**
  * @brief Set the D gain of the specified XL320's PID.
  * @param id: the ID of the targeted XL320.
  * @param D_gain: the D gain (0 to 254). Calculated D value is D Gain / 8.
  */
void xl_320_set_D_gain(uint8_t id, uint8_t D_gain) {
    xl_320_write(id, XL_320_ADR_D_GAIN, &D_gain, 1, XL_320_INS_WRITE);
}

/**
  * @brief Set the I gain of the specified XL320's PID.
  * @param id: the ID of the targeted XL320.
  * @param I_gain: the I gain (0 to 254). Calculated I value is I Gain * 1000 / 2048.
  */
void xl_320_set_I_gain(uint8_t id, uint8_t I_gain) {
    xl_320_write(id, XL_320_ADR_I_GAIN, &I_gain, 1, XL_320_INS_WRITE);
}

/**
  * @brief Set the P gain of the specified XL320's PID.
  * @param id: the ID of the targeted XL320.
  * @param P_gain: the P gain (0 to 254). Calculated P value is P Gain * 4 / 1000.
  */
void xl_320_set_P_gain(uint8_t id, uint8_t P_gain) {
    xl_320_write(id, XL_320_ADR_P_GAIN, &P_gain, 1, XL_320_INS_WRITE);
}

/**
  * @brief Set the desired position of the specified XL320. In order to use this method, the XL320 mode must be 'Join Mode'
  * @param id: the ID of the targeted XL320.
  * @param position: the desired position (0 to 1023). Calculated goal position is position * 0.29deg.
  */
void xl_320_set_position(uint8_t id, uint16_t position) {
    uint8_t pos[2];
    pos[0]=(uint8_t)(position&0xFF);
    pos[1]=(uint8_t)((position>>8)&0xFF);
     xl_320_write(id, XL_320_ADR_GOAL_POSITION, pos, 2, XL_320_INS_WRITE);
}

/**
  * @brief Set the desired speed of the specified XL320. In order to use this method, the XL320 mode must be 'Join Mode'
  * @param id: the ID of the targeted XL320.
  * @param speed: the desired speed (0 to 1023 for Join Mode, 0 to 2048 for Wheel Mode). Calculated speed is (speed * 0.111rpm) for Join Mode or ((speed - 1023) * 0.1% of the maximum speed) in Wheel mode.
  */
void xl_320_set_speed(uint8_t id, uint16_t speed) {
    uint8_t velocity[2];
    velocity[0]=(uint8_t)(speed&0xFF);
    velocity[1]=(uint8_t)((speed>>8)&0xFF);
     xl_320_write(id, XL_320_ADR_GOAL_VELOCITY, velocity, 2, XL_320_INS_WRITE);
}

/**
  * @brief Set the desired torque of the specified XL320.
  * @param id: the ID of the targeted XL320.
  * @param goal_torque: the desired goal torque (0 to 1023). Calculated goal torque is (goal_torque * 0.1% of the maximum torque).
  */
void xl_320_set_goal_torque(uint8_t id, uint16_t goal_torque) {
    uint8_t torque[2];
    torque[0]=(uint8_t)(goal_torque&0xFF);
    torque[1]=(uint8_t)((goal_torque>>8)&0xFF);
    xl_320_write(id, XL_320_ADR_GOAL_TORQUE, torque, 2, XL_320_INS_WRITE);
}


void xl_320_set_punch(uint8_t id, uint16_t punch) {
    uint8_t min_current[2];
    min_current[0]=(uint8_t)(punch&0xFF);
    min_current[1]=(uint8_t)((punch>>8)&0xFF);
    xl_320_write(id, Xl_320_ADR_MAX_TORQUE, min_current, 2, XL_320_INS_WRITE);
}


//// -----------------------------------------------------------------------------
//// INSTRUCTION layer: handles XL-320 instruction set
//// -----------------------------------------------------------------------------
//
//// Gather all status: from XL-320 but also from the UART layer
unsigned int xl_320_get_status(xl_320_packet_t* instruction_packet,
                                xl_320_packet_t* status_packet,
                                unsigned char expected_param_length){
    unsigned int status;

    // Retrieve hardware status from XL-320
    status = (status_packet->instruction) & 0xFF;

    // Check that the status packet id matches the sender
    if(status_packet->id != instruction_packet->id)
        status |= XL_320_ERROR_APP_ID;

    // Check that received parameter length is correct
    if(status_packet->packet_length != expected_param_length+2)
        status |= XL_320_ERROR_APP_LENGTH;

    // Check that checksum is correct, this is not
    // revealant if length is incorrect
    //if (status_packet->checksum != xl_320_compute_checksum(status_packet)) {
        status |= XL_320_ERROR_APP_CHECKSUM;
        //printf("Checksums : 0x%02X vs 0x%02X\n", status_packet->checksum ,xl_320_compute_checksum(status_packet)  );
    //}

    //xl_320_print_packet(&status_packet);

    return status;
}
//// Ping instruction
//unsigned int xl_320_ping(unsigned char id){
//    unsigned int status;
//    xl_320_packet_t ping_packet;
//    xl_320_packet_t status_packet;
//
//    // Build the instruction packet
//    ping_packet.id       = id;
//    ping_packet.packet_length   = 3; // 0 parameters
//    ping_packet.instruction  = XL_320_INS_PING;
//    ping_packet.checksum = xl_320_compute_checksum(&ping_packet);
//
//    status = 0;
//
//    // Send the ping instruction
//    xl_320_send_packet(&ping_packet);
//
//    // A status packet is always returned after a ping instruction.
//    // However check that id was not broadcast, even if that's a dum operation.
//    if((id !=  XL_320_ID_BROADCAST))
//    {
//        // Retrieve a status packet, add the UART error flags
//        status = xl_320_receive_packet(&status_packet);
//
//        // Get the overall status, add the CDS55XX and APP error flags
//        status |= xl_320_get_status(&ping_packet, &status_packet, 0);
//    }
//
//    return status;
//}
//// Read instruction
//unsigned int xl_320_read(unsigned char id, unsigned char address, unsigned char* datas, unsigned char nb_data){
//    unsigned int status;
//    xl_320_packet_t read_packet;
//    xl_320_packet_t status_packet;
//    unsigned char idx_data;
//
//    // Build the packet
//    read_packet.id            = id;
//    read_packet.packet_length        = 4;
//    read_packet.instruction       = XL_320_INS_READ;
//    read_packet.parameters[0] = address;
//    read_packet.parameters[1] = nb_data;
//    read_packet.checksum      = xl_320_compute_checksum(&read_packet);
//
//    // Send the instruction
//    xl_320_send_packet(&read_packet);
//
//    status = 0;
//
//    // A read status packet is returned only if address is not broadcast and
//    // the status return level is different than "non".
//    if((id != (unsigned char) XL_320_ID_BROADCAST) &&
//        xl_320_cfg.return_level != XL_320_STATUS_NO_ANSWER)
//    {
//        // Retrieve a status packet, add the UART error flags
//        status = xl_320_receive_packet(&status_packet);
//
//        // Get the overall status, add the CDS55XX and APP error flags
//        status |= xl_320_get_status(&read_packet, &status_packet, nb_data);
//
//        // Affect read datas if no critical error happened during the
//        // status packet reception
//        if(!(status & 0x00FF))
//            for(idx_data=0; idx_data<status_packet.packet_length-2; idx_data++)
//                datas[idx_data] = status_packet.parameters[idx_data];
//
//    }
//
//    //xl_320_print_packet(&status_packet);
//    return status;
//}
//// Action instruction
//void xl_320_action(void){
//    xl_320_packet_t action_packet;
//
//    // Build the packet
//    action_packet.id        = XL_320_ID_BROADCAST;
//    action_packet.packet_length    = 2;
//    action_packet.instruction   = XL_320_INS_ACTION;
//    action_packet.checksum  = xl_320_compute_checksum(&action_packet);
//
//    // Send the instruction
//    xl_320_send_packet(&action_packet);
//}
//// Reset instruction: /!\ carrefull this will factory reset everything including baudrate and IDs
//unsigned int xl_320_reset(unsigned char id){
//    unsigned int status;
//    xl_320_packet_t reset_packet;
//    xl_320_packet_t status_packet;
//
//    // Build the instruction packet
//    reset_packet.id       = id;
//    reset_packet.packet_length   = 2; // 0 parameters
//    reset_packet.instruction  = XL_320_INS_RESET;
//    reset_packet.checksum = xl_320_compute_checksum(&reset_packet);
//
//    // Send the instruction
//    xl_320_send_packet(&reset_packet);
//
//    // A status packet is returned only if address is not broadcast and
//    // the status return level is set to "all packets".
//    if((id != (unsigned char) XL_320_ID_BROADCAST) &&
//        xl_320_cfg.return_level == XL_320_STATUS_EVERYTHING)
//    {
//        // Retrieve a status packet, add the UART error flags
//        status = xl_320_receive_packet(&status_packet);
//
//        // Get the overall status, add the CDS55XX and APP error flags
//        status |= xl_320_get_status(&reset_packet, &status_packet, 0);
//    }
//
//    return status;
//}
//// Synchronized write instruction
//void xl_320_sync_write(unsigned char* id, unsigned char address, unsigned char** parameters,
//                        unsigned char nb_id, unsigned char nb_param){
//    unsigned char idx_id;
//    unsigned char idx_param;
//    xl_320_packet_t sync_packet;
//
//    // Build the packet
//    sync_packet.id            = XL_320_ID_BROADCAST;
//    sync_packet.packet_length        = (nb_param + 1) * nb_id + 4;
//    sync_packet.instruction       = XL_320_INS_WRITE;
//    sync_packet.parameters[0] = address;
//    sync_packet.parameters[1] = nb_param;
//
//    for(idx_id=0; idx_id < nb_id; idx_id++)
//    {
//        sync_packet.parameters[idx_id+2] = id[idx_id];
//
//        for(idx_param=0; idx_param < nb_param; idx_param++)
//            sync_packet.parameters[idx_id*nb_param + idx_param] = parameters[idx_id][idx_param];
//
//    }
//    sync_packet.checksum = xl_320_compute_checksum(&sync_packet);
//
//    // Send the instruction
//    xl_320_send_packet(&sync_packet);
//
//}

// -----------------------------------------------------------------------------
// Application layer: single access function set
// -----------------------------------------------------------------------------

//
// Shortcut  functions
//

//
// EEPROM set functions
// They all take effect immediatly (not registered).
//

// As we dont know the device's id, it is broadcast to everyone,
// so all connected devices will now have the requested id.
// RAM set functions
// Revealant functions have a registered possibility; calling xl_320_action()
// is then necessarry to trigger any effect.
// Otherwise, they take effect immediatly.
// RAM get functions
//
// unsigned int xl_320_get_temperature(unsigned char id, unsigned char *temp) {
// //   return xl_320_read(id, XL_320_ADDR_CUR_TEMP, temp, 1); }
//
// unsigned int xl_320_get_voltage(unsigned char id, unsigned char *voltage) {
////    return xl_320_read(id, XL_320_ADDR_CUR_VOLT, voltage, 1); }
//
// unsigned int xl_320_get_moving(unsigned char id, unsigned char *moving) {
//  //  return xl_320_read(id, XL_320_ADDR_MOVING, moving, 1); }
//
// unsigned int xl_320_get_position(unsigned char id, unsigned int *position) {
//    //return xl_320_read_int(id, XL_320_ADDR_CUR_POS_L, position); }
//
// unsigned int xl_320_get_speed(unsigned char id, unsigned int *speed) {
//    //return xl_320_read_int(id, XL_320_ADDR_CUR_SPEED_L, speed); }
//
// unsigned int xl_320_get_load(unsigned char id, unsigned int *load) {
//   // return xl_320_read_int(id, XL_320_ADDR_CUR_LOAD_L, load); }
//
//// -----------------------------------------------------------------------------
//// Application layer: burst access (multiple registers), synchronized motion
//// -----------------------------------------------------------------------------
//
//// Set the motion parameters of one servo but register the motion.
//// Calling xl_320_action() is necessary to trigger the motion
// unsigned int xl_320_set_motion_reg(unsigned char id, unsigned int position,
//                                           unsigned int speed, unsigned int torque)
//{
//    unsigned char params[6];
//    params[0] = position & 0x00FF;
//    params[1] = position >> 8;
//    params[2] = speed & 0x00FF;
//    params[3] = speed >> 8;
//    params[4] = torque & 0x00FF;
//    params[5] = torque >> 8;
//
//   // return xl_320_write(id, XL_320_ADDR_GOAL_POS_L, params, 6, 1);
//
//}
//// Set the motion parameters (goal, speed, acceleration) of an array of servos.
//// This functions can handle a maximum of 16 servos.
// unsigned int xl_320_set_motions(unsigned char *ids, unsigned int *positions,
//                    unsigned int *speeds, unsigned char *accs, unsigned char *dccs,
//                    unsigned char nb_id)
//{
//    unsigned char idx_id;
//    unsigned char params[16][6]; // 16 servos x 6 parameters
//    unsigned int *cur_position;
//    unsigned int *cur_speed;
//    unsigned char *cur_acc;
//    unsigned char *cur_dcc;
//
//    // Init array pointers
//    cur_position = positions;
//    cur_speed = speeds;
//    cur_acc = accs;
//    cur_dcc = dccs;
//
//    // Build the data table
//    for(idx_id=0; idx_id<nb_id; idx_id++)
//    {
//        params[idx_id][0] = (*cur_position) & 0x00FF;
//        params[idx_id][1] = (*cur_position) >> 8;
//        params[idx_id][2] = (*cur_speed) & 0x00FF;
//        params[idx_id][3] = (*cur_position) >> 8;
//        params[idx_id][4] = (*cur_acc);
//        params[idx_id][5] = (*cur_dcc);
//
//        // Increment indexes
//        cur_position++;
//        cur_speed++;
//        cur_acc++;
//        cur_dcc++;
//    }
//
// //   xl_320_sync_write(ids, XL_320_ADDR_GOAL_POS_L, (unsigned char**) params, nb_id, 6);
//
//    // All packets were broadcasted
//    return 0;
//}
