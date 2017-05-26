/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       dxl_registers.c
 * @author     Bebop35
 * @date       Feb 09, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   Registers definition for the Dynamixel Servos (Protocol v1)
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/blueboard.git
 * -----------------------------------------------------------------------------
 */

#include "dynamixel.h"

/*
 * Control Register Table
 * TODO: V5
 */

const dxl_register_t dxl_registers[] = {

/* -------------------------------------------------------------------------------------------*/
/*   Impl.    Area         Address   Name (no spaces!)            Access      Size  Index */
/* -------------------------------------------------------------------------------------------*/
/* -- REGISTER TABLE COMMON V1/V2/V3/V4 -- */
    {0x0F,    DXL_EEPROM,   0,       "model_number"             , DXL_R,        2}  //  0
   ,{0x0F,    DXL_EEPROM,   2,       "firmware_version"         , DXL_R,        1}  //  1
   ,{0x0F,    DXL_EEPROM,   3,       "id"                       , DXL_RW,       1}  //  2
   ,{0x0F,    DXL_EEPROM,   4,       "baudrate"                 , DXL_RW,       1}  //  3
   ,{0x0F,    DXL_EEPROM,   5,       "return_delay_time"        , DXL_RW,       1}  //  4
   ,{0x0F,    DXL_EEPROM,   6,       "cw_angle_limit"           , DXL_RW,       2}  //  5
   ,{0x0F,    DXL_EEPROM,   8,       "ccw_angle_limit"          , DXL_RW,       2}  //  6
/* -- REGISTER TABLE COMMON V1/V2/V3 -- */
   ,{0x07,    DXL_EEPROM,  11,       "temperature_limit"        , DXL_RW,       1}  //  7
   ,{0x07,    DXL_EEPROM,  12,       "low_voltage_limit"        , DXL_RW,       1}  //  8
   ,{0x07,    DXL_EEPROM,  13,       "high_voltage_limit"       , DXL_RW,       1}  //  9
   ,{0x07,    DXL_EEPROM,  14,       "max_torque"               , DXL_RW,       2}  // 10
   ,{0x07,    DXL_EEPROM,  16,       "status_return_level"      , DXL_RW,       1}  // 11
   ,{0x07,    DXL_EEPROM,  17,       "alarm_led"                , DXL_RW,       1}  // 12
   ,{0x07,    DXL_EEPROM,  18,       "alarm_shutdown"           , DXL_RW,       1}  // 13
/* -- REGISTER TABLE V2/V3 ONLY --*/
   ,{0x06,    DXL_EEPROM,  20,       "multi-turn_offset"        , DXL_RW,       2}  // 14
   ,{0x06,    DXL_EEPROM,  22,       "resolution_divider"       , DXL_RW,       1}  // 15
/* -- REGISTER TABLE V4 ONLY --*/
   ,{0x08,    DXL_EEPROM,  11,       "control_mode"             , DXL_RW,       1}  // 16
   ,{0x08,    DXL_EEPROM,  12,       "temperature_limit"        , DXL_RW,       1}  // 17
   ,{0x08,    DXL_EEPROM,  13,       "low_voltage_limit"        , DXL_RW,       1}  // 18
   ,{0x08,    DXL_EEPROM,  14,       "high_voltage_limit"       , DXL_RW,       1}  // 19
   ,{0x08,    DXL_EEPROM,  15,       "max_torque"               , DXL_RW,       2}  // 20
   ,{0x08,    DXL_EEPROM,  17,       "status_return_level"      , DXL_RW,       1}  // 21
   ,{0x08,    DXL_EEPROM,  18,       "alarm_shutdown"           , DXL_RW,       1}  // 22
/* -- REGISTER TABLE COMMON V1/V2/V3 -- */
   ,{0x07,    DXL_RAM,     24,       "torque_enable"            , DXL_RW,       1}  // 23
   ,{0x07,    DXL_RAM,     25,       "led"                      , DXL_RW,       1}  // 24
/* -- REGISTER TABLE V1 ONLY --*/
   ,{0x01,    DXL_RAM,     26,       "cw_compliance_margin"     , DXL_RW,       1}  // 25
   ,{0x01,    DXL_RAM,     27,       "ccw_compliance_margin"    , DXL_RW,       1}  // 26
   ,{0x01,    DXL_RAM,     28,       "cw_compliance_slope"      , DXL_RW,       1}  // 27
   ,{0x01,    DXL_RAM,     29,       "ccw_compliance_slope"     , DXL_RW,       1}  // 28
/* -- REGISTER TABLE V2/V3 ONLY --*/
   ,{0x06,    DXL_RAM,     26,       "d_gain"                   , DXL_RW,       1}  // 29
   ,{0x06,    DXL_RAM,     27,       "i_gain"                   , DXL_RW,       1}  // 30
   ,{0x06,    DXL_RAM,     28,       "p_gain"                   , DXL_RW,       1}  // 31
    /* -- REGISTER TABLE COMMON V1/V2/V3 -- */
   ,{0x07,    DXL_RAM,     30,       "goal_position"            , DXL_RW,       2}  // 32
   ,{0x07,    DXL_RAM,     32,       "moving_speed"             , DXL_RW,       2}  // 33
   ,{0x07,    DXL_RAM,     34,       "max_torque"               , DXL_RW,       2}  // 34
   ,{0x07,    DXL_RAM,     36,       "present_position"         , DXL_R,        2}  // 35
   ,{0x07,    DXL_RAM,     38,       "present_velocity"         , DXL_R,        2}  // 36
   ,{0x07,    DXL_RAM,     40,       "present_load"             , DXL_R,        2}  // 37
   ,{0x07,    DXL_RAM,     42,       "present_voltage"          , DXL_R,        1}  // 38
   ,{0x07,    DXL_RAM,     43,       "present_temperature"      , DXL_R,        1}  // 39
   ,{0x07,    DXL_RAM,     44,       "registered"               , DXL_R,        1}  // 40
   ,{0x07,    DXL_RAM,     46,       "moving"                   , DXL_R,        1}  // 41
   ,{0x07,    DXL_RAM,     47,       "lock"                     , DXL_RW,       1}  // 42
   ,{0x07,    DXL_RAM,     48,       "punch"                    , DXL_RW,       2}  // 43
/* -- REGISTER TABLE V3 ONLY --*/
   ,{0x04,    DXL_RAM,     68,       "current"                  , DXL_RW,       2}  // 44
   ,{0x04,    DXL_RAM,     70,       "torque_control_enable"    , DXL_RW,       1}  // 45
   ,{0x04,    DXL_RAM,     71,       "goal_torque"              , DXL_RW,       2}  // 46
   ,{0x04,    DXL_RAM,     73,       "goal_acceleration"        , DXL_RW,       1}  // 47
/* -- REGISTER TABLE V4 ONLY --*/
   ,{0x08,    DXL_RAM,     24,       "torque_enable"            , DXL_RW,       1}  // 48
   ,{0x08,    DXL_RAM,     25,       "led"                      , DXL_RW,       1}  // 49
   ,{0x08,    DXL_RAM,     27,       "d_gain"                   , DXL_RW,       1}  // 50
   ,{0x08,    DXL_RAM,     28,       "i_gain"                   , DXL_RW,       1}  // 51
   ,{0x08,    DXL_RAM,     29,       "p_gain"                   , DXL_RW,       1}  // 52
   ,{0x08,    DXL_RAM,     30,       "goal_position"            , DXL_RW,       2}  // 53
   ,{0x08,    DXL_RAM,     32,       "goal_velocity"            , DXL_RW,       2}  // 54
   ,{0x08,    DXL_RAM,     35,       "goal_torque"              , DXL_RW,       2}  // 55
   ,{0x08,    DXL_RAM,     37,       "present_position"         , DXL_R,        2}  // 56
   ,{0x08,    DXL_RAM,     39,       "present_velocity"         , DXL_R,        2}  // 57
   ,{0x08,    DXL_RAM,     41,       "present_load"             , DXL_R,        2}  // 58
   ,{0x08,    DXL_RAM,     45,       "present_voltage"          , DXL_R,        1}  // 59
   ,{0x08,    DXL_RAM,     46,       "present_temperature"      , DXL_R,        1}  // 60
   ,{0x08,    DXL_RAM,     47,       "registered"               , DXL_R,        1}  // 61
   ,{0x08,    DXL_RAM,     49,       "moving"                   , DXL_R,        1}  // 62
   ,{0x08,    DXL_RAM,     50,       "hardware_error_status"    , DXL_RW,       1}  // 63
   ,{0x08,    DXL_RAM,     51,       "punch"                    , DXL_RW,       2}  // 64
/* ------------------------------------------------------------------------------------------------*/
};

const size_t dxl_registers_length = sizeof(dxl_registers) / sizeof(dxl_register_t);

/* Named version for functions handlers */
const dxl_register_t* dxl_reg_model_number             = dxl_registers +  0;
const dxl_register_t* dxl_reg_firmware_version         = dxl_registers +  1;
const dxl_register_t* dxl_reg_id                       = dxl_registers +  2;
const dxl_register_t* dxl_reg_baud_rate                = dxl_registers +  3;
const dxl_register_t* dxl_reg_return_delay_time        = dxl_registers +  4;
const dxl_register_t* dxl_reg_cw_angle_limit           = dxl_registers +  5;
const dxl_register_t* dxl_reg_ccw_angle_limit          = dxl_registers +  6;

const dxl_register_t* dxl_reg_v1_torque_enable         = dxl_registers +  23;
const dxl_register_t* dxl_reg_v4_torque_enable         = dxl_registers +  48;

const dxl_register_t* dxl_reg_v1_position              = dxl_registers +  32;
const dxl_register_t* dxl_reg_v4_position              = dxl_registers +  53;

const dxl_register_t* dxl_reg_v1_speed                 = dxl_registers +  33;
const dxl_register_t* dxl_reg_v4_speed                 = dxl_registers +  54;

const dxl_register_t* dxl_reg_v1_torque                = dxl_registers +  34;
const dxl_register_t* dxl_reg_v4_torque                = dxl_registers +  55;

const dxl_register_t* dxl_reg_v1_led                   = dxl_registers +  24;
const dxl_register_t* dxl_reg_v4_led                   = dxl_registers +  49;
/*
 * Converts an area type into a printable string
 */
const char* dxl_get_area_as_string(const dxl_reg_area_e area)
{
    switch(area)
    {
        case DXL_EEPROM: return "EEPROM";
        case DXL_RAM: 	 return "RAM";
        default:     	 return "NC"; // Error
    }
}

/*
 * Converts an access type into a printable string
 */
const char* dxl_get_access_as_string(const dxl_reg_access_e acc)
{
    switch(acc)
    {
        case DXL_RW: return "RW";
        case DXL_R:  return "R";
        default:     return "NC"; // Error
    }
}
