/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       dynamixel.c
 * @author     Bebop35
 * @date       Feb 09, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   This component implements the control of Dynamixel Servos
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/blueboard.git
 * -----------------------------------------------------------------------------
 */

#include "dynamixel.h"

/**
********************************************************************************
**
**  Initialization and configuration
**
********************************************************************************
*/

/* @brief: Initialize a DXL Interface with default values.
 * @param itf: Dynamixel Interface to write into
 */
void dxl_init_itf(dxl_interface_t* itf, uint8_t itf_idx)
{
    itf->itf_idx = itf_idx;
    itf->protocol = DXL_V1;
    itf->hw_send_byte = NULL;
    itf->hw_receive_byte = NULL;
    itf->hw_flush = NULL;
    itf->nb_pkt_tx = 0;
    itf->nb_pkt_rx = 0;
    itf->nb_errors = 0;
    itf->status = DXL_STATUS_NO_ERROR;


    itf->return_level = DXL_STATUS_EVERYTHING;
    itf->return_delay_ms = 1;

    // Protocol-dependent values
    if(itf->protocol == DXL_V1) {
        // ..
    }

}

/* Initialize a servo, by model name attach it to a DXL Interface */
void dxl_init_servo(dxl_servo_t* servo, dxl_interface_t* itf, const char* model_name)
{
    servo->model = (dxl_servo_model_t*) dxl_find_servo_model_by_name(model_name);
/*
    // Check that model exists
    if(servo->model == NULL)
    {
        // TODO error mgmt
        return;
    }

    // Check that the protocol is correct
    if(servo->model->protocol != itf->protocol)
    {
        // TODO error mgmt
        return;
    }*/

    servo->id = DXL_ID_DEFAULT;
    servo->itf = itf;

    servo->min_position = 0;
    servo->max_position = 1023; // TEMP
    servo->current_position = 0;

#ifdef DXL_DEBUG
    dxl_print_servo(servo);
#endif

}

/**
********************************************************************************
**
**  Main handlers
**
********************************************************************************
*/

/* @brief: Ping the selected servo
 * @param servo: Servo to send the reset access
 */
dxl_status_t dxl_ping(dxl_servo_t* servo)
{
    if(servo->itf->protocol == DXL_V1) {
        dxl_v1_ping(servo);

    } else if(servo->itf->protocol == DXL_V2) {
        dxl_v2_ping(servo);

    // Error
    } else {
        return DXL_STATUS_ERR_PROTOCOL;
    }

    return servo->itf->status;
}

/* @brief: Factory-reset of the selected servo
 * @param servo: Servo to send the reset access
 */
dxl_status_t dxl_reset(dxl_servo_t* servo)
{

    if(servo->itf->protocol == DXL_V1) {
        dxl_v1_reset(servo);

    } else if(servo->itf->protocol == DXL_V2) {
    	dxl_v2_reset(servo);

    // Error
    } else {
        return DXL_STATUS_ERR_PROTOCOL;
    }

    return servo->itf->status;
}

/* @brief: Basic write access
 * @param servo: Servo to send the write access
 * @param addr: Register address
 * @param values: Array of values to write
 * @param size: Size of the value array in number of bytes
 * @param reg: True if the write is registered
 */
dxl_status_t dxl_write(dxl_servo_t* servo, uint16_t addr, uint8_t* values, size_t size, bool reg)
{
    if(servo->itf->protocol == DXL_V1) {
        dxl_v1_write(servo, (uint8_t) addr, values, size, reg);

    } else if(servo->itf->protocol == DXL_V2) {
    	dxl_v2_write(servo, (uint16_t) addr, values, size, reg);

    // Error
    } else {
        return DXL_STATUS_ERR_PROTOCOL;
    }

    return servo->itf->status;
}

/* @brief: Write access with 32-bits cast
 * @param servo: Servo to send the write access
 * @param addr: Register address
 * @param value: Value to write
 * @param size: Size of the value in number of bytes
 * @param reg: True if the write is registered
 */
dxl_status_t dxl_write_int(dxl_servo_t* servo, uint16_t addr, uint32_t value, size_t size, bool reg)
{
	uint8_t value_bytes[4];
	dxl_data_to_bytes_array(value, size, value_bytes);
	return dxl_write(servo, addr, value_bytes, size, reg);
}

/* @brief: Basic read access
 * @param servo: Servo to send the read access
 * @param addr: Register address
 * @param values: Array of values to store the read values
 * @param size: Size of the value array in number of bytes
 */
dxl_status_t dxl_read(dxl_servo_t* servo, uint16_t addr, uint8_t* values, size_t size)
{
    if(servo->itf->protocol == DXL_V1) {
        dxl_v1_read(servo, (uint8_t) addr, values, size);

    } else if(servo->itf->protocol == DXL_V2) {
    	dxl_v2_read(servo, (uint16_t) addr, values, size);

    // Error
    } else {
        return DXL_STATUS_ERR_PROTOCOL;
    }

    return servo->itf->status;
}

/* @brief: Read access with 32-bits cast
 * @param servo: Servo to send the read access
 * @param addr: Register address
 * @param value: Read value
 * @param size: Size of the value in number of bytes
 */
dxl_status_t dxl_read_int(dxl_servo_t* servo, uint16_t addr, uint32_t* value, size_t size)
{
	uint8_t value_bytes[4];
	dxl_status_t ret;
	ret = dxl_read(servo, addr, value_bytes, size);

	if(ret == DXL_PASS) {
		dxl_bytes_array_to_data(value, size, value_bytes);
	}

	return ret;
}

/* @brief: Trigger action
 * @param servo: Servo to send the action command
 */
dxl_status_t dxl_action(dxl_servo_t* servo)
{
    if(servo->itf->protocol == DXL_V1) {
        dxl_v1_action(servo);

    } else if(servo->itf->protocol == DXL_V2) {
    	dxl_v2_action(servo);

    // Error
    } else {
        return DXL_STATUS_ERR_PROTOCOL;
    }

    return servo->itf->status;
}

/**
********************************************************************************
**
**  Common short-hands
**
********************************************************************************
*/

dxl_status_t dxl_get_model(dxl_servo_t* servo, uint16_t* model)
{
    extern const dxl_register_t* dxl_reg_model_number;
    return dxl_read_int(servo, dxl_reg_model_number->address, model, dxl_reg_model_number->size);
}


dxl_status_t dxl_set_torque_enable(dxl_servo_t* servo, uint8_t torque_enable)
{
    extern const dxl_register_t* dxl_reg_v1_torque_enable;
    extern const dxl_register_t* dxl_reg_v4_torque_enable;
    dxl_register_t* reg;

    if(servo->model->reg_table == DXL_REG4)
      reg = dxl_reg_v4_torque_enable;
    else
      reg = dxl_reg_v1_torque_enable;

    return dxl_write_int(servo, reg->address, torque_enable, reg->size, false);
}

dxl_status_t dxl_set_position(dxl_servo_t* servo, uint16_t new_position)
{
  extern const dxl_register_t* dxl_reg_v1_position;
  extern const dxl_register_t* dxl_reg_v4_position;
  dxl_register_t* reg;

  if(servo->model->reg_table == DXL_REG4)
    reg = dxl_reg_v4_position;
  else
    reg = dxl_reg_v1_position;

  return dxl_write_int(servo, reg->address, new_position, reg->size, false);
}

dxl_status_t dxl_set_speed(dxl_servo_t* servo, uint16_t new_speed)
{
  extern const dxl_register_t* dxl_reg_v1_speed;
  extern const dxl_register_t* dxl_reg_v4_speed;
  dxl_register_t* reg;

  if(servo->model->reg_table == DXL_REG4)
    reg = dxl_reg_v4_speed;
  else
    reg = dxl_reg_v1_speed;

  return dxl_write_int(servo, reg->address, new_speed, reg->size, false);
}

dxl_status_t dxl_set_torque(dxl_servo_t* servo, uint16_t new_torque)
{
  extern const dxl_register_t* dxl_reg_v1_torque;
  extern const dxl_register_t* dxl_reg_v4_torque;
  dxl_register_t* reg;

  if(servo->model->reg_table == DXL_REG4)
    reg = dxl_reg_v4_torque;
  else
    reg = dxl_reg_v1_torque;

  return dxl_write_int(servo, reg->address, new_torque, reg->size, false);
}

dxl_status_t dxl_set_led(dxl_servo_t* servo, uint8_t led)
{
  extern const dxl_register_t* dxl_reg_v1_led;
  extern const dxl_register_t* dxl_reg_v4_led;
  dxl_register_t* reg;

  if(servo->model->reg_table == DXL_REG4)
    reg = dxl_reg_v4_led;
  else
    reg = dxl_reg_v1_led;

  return dxl_write_int(servo, reg->address, led, reg->size, false);

}


/**
********************************************************************************
**
**  High-level procedures
**
********************************************************************************
*/

// Interface scan (baudrate + protocol)



/**
********************************************************************************
**
**  Some service routines
**
********************************************************************************
*/

/* @brief: Convert a data into a byte array of the correct endianess.
 * @param data: data to be converted
 * @param size: data size in bytes
 * @param data: returned data in bytes array
 */
void dxl_data_to_bytes_array(uint32_t data, size_t size, uint8_t* data_arr)
{
    switch(size) {
        default:
        case 1:
            data_arr[0] = (uint8_t) (data & 0x00FF) & 0xFF;
            break;
        case 2:
            data &= 0x0000FFFF;
            data_arr[0] = (uint8_t) (data & 0xFF);
            data_arr[1] = (uint8_t) (data >> 8L) & 0xFF;
            break;
        case 4:
            data_arr[0] = (uint8_t) (data & 0xFF);
            data_arr[1] = (uint8_t) (data >> 8L) & 0xFF;
            data_arr[2] = (uint8_t) (data >> 16L) & 0xFF;
            data_arr[3] = (uint8_t) (data >> 24L) & 0xFF;
            break;
    }
}

/* @brief: Convert a byte array into a data with the correct endianess
 * @param data: returned data
 * @param size: data size in bytes
 * @param data: data in bytes array to be converted
 */
void dxl_bytes_array_to_data(uint32_t* data, size_t size, uint8_t* data_arr)
{
    switch(size) {
        default:
        case 1:
            *data = data_arr[0];
            break;
        case 2:
            *data = data_arr[0] + (data_arr[1] << 8L);
            break;
        case 4:
            *data = data_arr[0] + (data_arr[1] << 8L) + (data_arr[2] << 16L) + (data_arr[3] << 24L);
            break;
    }
}

/* @brief: Get an error message as a string. If there is no error,
 *         the written string is empty.
 * @param status_str: string in which the error is returned as a string
 * @param status_str_len: maximum length of the status string
 * @param status: status to print (does not print anything if no error)
 * @param protocol: interface protocol
 */

void dxl_get_error_str(char* status_str, size_t status_str_len, dxl_status_t status, dxl_protocol_e protocol) {

    /* Common DXL errors */
    if(status & DXL_STATUS_ERR_TIMEOUT) {
        snprintf(status_str, status_str_len, "Timeout ");
        status_str += strlen(status_str);
    }

    if(status & DXL_STATUS_ERR_HEADER) {
        snprintf(status_str, status_str_len, "Header ");
        status_str += strlen(status_str);
    }

    if(status & DXL_STATUS_ERR_ID) {
        snprintf(status_str, status_str_len, "ID ");
        status_str += strlen(status_str);
    }

    if(status & DXL_STATUS_ERR_LENGTH) {
        snprintf(status_str, status_str_len, "Data length ");
        status_str += strlen(status_str);
    }

    if(status & DXL_STATUS_ERR_CHECKSUM) {
        snprintf(status_str, status_str_len, "Checksum ");
        status_str += strlen(status_str);
    }

    if(status & DXL_STATUS_ERR_PROTOCOL) {
        snprintf(status_str, status_str_len, "Protocol ");
        status_str += strlen(status_str);
    }


    /* Protocol-Specific errors */
    if(protocol == DXL_V1) {
        dxl_v1_get_error_str(status_str, status_str_len, status);

    } else if(protocol == DXL_V2) {
        dxl_v2_get_error_str(status_str, status_str_len, status);
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

/* @brief: Print a DXL error
 * @param status: status to print (does not print anything if no error)
 */
void dxl_print_error(dxl_status_t status, dxl_protocol_e protocol)
{
    char error_str[128];
    char error_msg[128];

    if(status != DXL_PASS) {
        dxl_get_error_str(error_str, sizeof(error_str), status, protocol);
        sprintf(error_msg, DXL_DEBUG_PFX" Error: %s"DXL_DEBUG_EOL, error_str);
        serial_puts(error_msg);
    }

}

#endif // DXL_DEBUG
