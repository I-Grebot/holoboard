#ifndef _PID_H
#define _PID_H

/*
 * PID Library dependencies
 * ------------------------
 */

// Include the board-specific hardware configuration,
// such as clock frequencies definition, modules used etc.
//#include "board_cfg.h"
//#include "dc_motor.h"
#include "main.h"
/*
 * Check board config
 * ------------------
 */

//#if ! defined DC_MOTOR_USE_PWM1H1 && ! defined DC_MOTOR_USE_PWM1H2 && ! defined DC_MOTOR_USE_PWM1H3 && ! defined DC_MOTOR_USE_PWM2H1
//    #error "PID Library: Error - At least one DC_MOTOR_USE_PWMxxx fuse should be set."
//#endif

/*#ifndef PID_GET_POSITION
    #warning "PID Library: No user defined PID_GET_POSITION macro. Only QEI peripheral will be used to get motor position."
	#define PID_GET_POSITION(_motor_channel) {_motor_channel}
	#include "encoders.h"
#endif*/

/*
 * Structures definition
 * ---------------------
 */

/* Memory 152 bytes */
typedef struct PID_struct_t{
    int8_t KP;     	// Proportional gain
    int8_t KI;       	// Integrative gain
    int8_t KD;       	// Derivative gain
    int32_t err;
    int32_t last_err;
    int32_t err_I;
    int32_t I_limit;
}PID_struct_t;

/* Memory 184 bytes */
typedef struct PID_process_t{
	void (*set_pwm)(void *, int32_t);
	void *pwm_channel;
//    uint8_t  DC_Motor_Channel;	// PID DC motor channel 0->3
    uint8_t  encoder_Channel; 	// QEI channel 0->1
    uint8_t  use_QEI;         	// Use QEI peripheral? 1:Yes 0:No
    int32_t curr;
    int32_t last;
    int32_t ref;
    int32_t last_ref;
    // PID structure
    PID_struct_t *PID;
    int32_t speed_Limit;     	// Speed saturation, 0 => no limit
    int32_t acceleration_Limit; // Acceleration saturation, 0 => no limit
}PID_process_t;


/*
 * PID Functions Prototypes
 */

int32_t PID_Process(PID_struct_t *PID, int32_t error);
void PID_Process_Speed(PID_process_t *sPID, uint32_t position);
void PID_Process_Position(PID_process_t *pPID, PID_process_t *sPID, int32_t position);
void PID_Set_Coefficient(PID_struct_t *PID,int8_t KP,int8_t KI,int8_t KD,uint32_t I_limit);
void PID_Reset(PID_process_t *xPID);
void PID_Set_limitation(PID_process_t *xPID,int32_t S_limit, int32_t A_limit);
PID_process_t* pid_init(void (*set_pwm)(void *, int32_t), void *pwm_channel,uint8_t E_channel,uint8_t use_QEI);
void PID_Set_Cur_Position(PID_process_t *pPID, int32_t position);
void PID_Set_Ref_Position(PID_process_t *pPID, int32_t position);
int32_t PID_Get_Cur_Position(PID_process_t *pPID);
void PID_Set_Ref_Speed(PID_process_t *sPID, int16_t speed);
int32_t PID_Get_Cur_Speed(PID_process_t *sPID);

#endif /* ! _PID_H */
