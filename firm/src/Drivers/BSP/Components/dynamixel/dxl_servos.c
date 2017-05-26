/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       dxl_servos.c
 * @author     Bebop35
 * @date       Feb 09, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   Implements Dynamixel Servo Specific functions
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/blueboard.git
 * -----------------------------------------------------------------------------
 */

#include "dynamixel.h"


const dxl_servo_model_t dxl_servo_models[] = {

        /* ID      Model          Com. Protocol     Reg. Table  */
        // AX Series
        {   12,    "AX-12"          , DXL_V1,        DXL_REG1}, /* AX-12 / AX-12+ / AX-12A */
        {  300,    "AX-12W"         , DXL_V1,        DXL_REG1},
        {   18,    "AX-18"          , DXL_V1,        DXL_REG1}, /* AX-18F / AX-18A */
        // EX Series
        {  107,    "EX-106"         , DXL_V1,        DXL_REG1}, /* EX-106+ */
        // RX Series
        {   10,    "RX-10"          , DXL_V1,        DXL_REG1},
        {   24,    "RX-24"          , DXL_V1,        DXL_REG1}, /* RX-24F */
        {   28,    "RX-28"          , DXL_V1,        DXL_REG1},
        {   64,    "RX-64"          , DXL_V1,        DXL_REG1},
        // DX Series
        {  113,    "DX-113"         , DXL_V1,        DXL_REG1},
        {  116,    "DX-116"         , DXL_V1,        DXL_REG1},
        {  117,    "DX-117"         , DXL_V1,        DXL_REG1},
        // MX Series
        {  360,    "MX-12"          , DXL_V1,        DXL_REG2}, /* MX-12W */
        {   29,    "MX-28"          , DXL_V1,        DXL_REG2}, /* MX-28T / MX-28R / MX-28AT / MX-28AR */
        {  310,    "MX-64"          , DXL_V1,        DXL_REG3}, /* MX-64T / MX-64R / MX-64AT / MX-64AR */
        {  320,    "MX-106"         , DXL_V1,        DXL_REG3}, /* MX-106T / MX-106R */
        // XL Series
        {  350,    "XL320"          , DXL_V2,        DXL_REG4},
        // XH Series
        { 1000,    "XH430-W350"     , DXL_V2,        DXL_REG5}, /* XH430-W350-T / XH430-W350-R */
        { 1010,    "XH430-W210"     , DXL_V2,        DXL_REG5}, /* XH430-W210-T / XH430-W210-R */
        // XM Series
        { 1020,    "XM430-W350"     , DXL_V2,        DXL_REG5}, /* XM430-W350-T / XM430-W350-R */
        { 1030,    "XM430-W210"     , DXL_V2,        DXL_REG5}  /* XM430-W210-T / XM430-W210-R */
};

static const size_t dxl_nb_servo_models = sizeof(dxl_servo_models) / sizeof(dxl_servo_models[0]);

const uint32_t dxl_baudrates[] = {
           9600,
          19200,
          57600,
         115200,
         200000,
         250000,
         400000,
         500000,
        1000000
};

const size_t dxl_baudrates_size = sizeof(dxl_baudrates)/sizeof(uint32_t);

/* Find a corresponding DXL servo by its name. Returns NULL if not found.
 * This function is not really optimized, so it shouldn't be called too often
 */
const dxl_servo_model_t* dxl_find_servo_model_by_name(const char* name)
{
    dxl_servo_model_t* model = dxl_servo_models;
    uint16_t model_idx;
    // FIXME
/*
    for(model_idx = 0;
        model_idx < dxl_nb_servo_models;
        model_idx++, model++)
    {
        if(!strcasecmp(name, model->name)) {
            return model;
        }
    }

    return NULL;*/

    return dxl_servo_models + 6;

}

const dxl_servo_model_t* dxl_find_servo_model_by_id(uint32_t id)
{
  uint16_t model_idx;

  for(model_idx = 0; model_idx < dxl_nb_servo_models; model_idx++)
  {
    if(id == dxl_servo_models[model_idx].model_id) {
      return &dxl_servo_models[model_idx];
    }
  }

  return NULL;
}

#ifdef DXL_DEBUG
void dxl_print_servo(dxl_servo_t* servo)
{
    char str[100];
    sprintf(str, DXL_DEBUG_PFX" Servo:"DXL_DEBUG_EOL
                 "  ITF:%u"DXL_DEBUG_EOL
                 "  ID:%u"DXL_DEBUG_EOL
                 "  Model:%u (%s)"DXL_DEBUG_EOL,
            servo->itf->itf_idx, servo->id, servo->model->model_id, servo->model->name);
    serial_puts(str);
}

#endif // DXL_DEBUG
