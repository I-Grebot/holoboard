/* -----------------------------------------------------------------------------
 * pid
 * I-Grebot Dc motor PID library
 * -----------------------------------------------------------------------------
 * File        : pid.c
 * Language    : C
 * Author      : Sebastien Brulais
 * Creation    : 2013-06-07
 * -----------------------------------------------------------------------------
 * Description
 *   This is a double PID (position and speed) control library for dc motors.
 *   With dspi33F, it can handle up to 4 independant motor controls
 * -----------------------------------------------------------------------------
 * Dependency:
 *   dc_motor library
 *   encoders library
 * -----------------------------------------------------------------------------
 * Library usage
 *
 * - If you're not using QEI peripheral, a PID_GET_POSITION(_motor_channel) macro
 * has to be defined and the pPID->cfg->use_QEI should be reset.
 * - If you're using QEI peripheral, make sure the encoders library is included
 * - In the user defined program declare a PID process structure and configure it.
 * - Then the PID_Process() function should be called at constant rate, for 
 * instance by a Timer
 * - Update reference position of the motor with PID_Set_Ref_Postion()
 *
 *  /!\ Keep in mind that PID gains are sample time dependent
 *
 *  /!\ Do not forget to set a heap size in your linker for mallocs
 *  /!\ At least 4096 bytes /!\
 *
 * See ./example for more informations
 *
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: http://svn2.assembla.com/svn/paranoid_android/
 * -----------------------------------------------------------------------------
 * $Rev: 1320 $
 * $LastChangedBy: Pierrick_Boissard $
 * $LastChangedDate: 2015-04-30 23:13:02 +0200 (jeu., 30 avr. 2015) $
 * -----------------------------------------------------------------------------
 * Version     Comment                                   Author       Date
 * 1.0         Initial release                           Seb B.      2013-06-07
 * 1.1	       Separation speed/position update		 	 Pierrick B. 2013-12-11
 * 1.2         Adding PID Process + Testing              Pierrick B. 2014-01-04
 * -----------------------------------------------------------------------------
 */

#include "pid.h"
#include <stdlib.h>

static inline void
safe_setpwm(void (*f)(void *, int32_t), void * param, int32_t value)
{
	void (*f_tmp)(void *, int32_t);
	void * param_tmp;
	f_tmp = f;
	param_tmp = param;

	if (f_tmp)
	{
		f_tmp(param_tmp, value);
	}
}

static inline int32_t
safe_getencoder(int32_t (*f)(void *), void * param)
{
	int32_t (*f_tmp)(void *);
	void * param_tmp;
	f_tmp = f;
	param_tmp = param;

	if (f_tmp)
	{
		return f_tmp(param_tmp);
	}

	return 0;
}

int32_t PID_Process(PID_struct_t *PID, int32_t error){
    int32_t command;
    int32_t err_D;

    PID->last_err = PID->err;
    PID->err = error;
    PID->err_I += error;
    err_D = PID->err - PID->last_err;

    if(PID->KI!=0)
    {
        if(PID->err_I > PID->I_limit)
        {
            PID->err_I = PID->I_limit;
        }
        else if(PID->err_I < -PID->I_limit)
        {
            PID->err_I = -PID->I_limit;
        }
    }

    command = PID->err*PID->KP + PID->err_I*PID->KI/100 - err_D*PID->KD;

    return command;
 }

void PID_Process_Speed(PID_process_t *sPID, uint32_t position){
    int32_t command=0;

    sPID->curr = (position - sPID->last);

    // Compute Speed errors
    command = PID_Process(sPID->PID, sPID->ref - sPID->curr);
   
    // Speed saturation
    if(sPID->speed_Limit)
    {
    	if (command > sPID->speed_Limit)
    	{
            command = sPID->speed_Limit;
        }
    	else if (command < - sPID->speed_Limit)
    	{
            command = - sPID->speed_Limit;
    	}
    }

    // Acceleration saturation
    if(sPID->acceleration_Limit)
    {
        if ( (command - sPID->last_ref) > sPID->acceleration_Limit)
        {
            command = sPID->last_ref + sPID->acceleration_Limit;
        }
        else if (  (sPID->last_ref - command )  >  sPID->acceleration_Limit )
        {
            command = sPID->last_ref - sPID->acceleration_Limit;
        }
    }

    // Send new motor reference
    safe_setpwm(sPID->set_pwm, sPID->pwm_channel,command);
    //dc_motor_set_speed(sPID->DC_Motor_Channel,command);
    sPID->last_ref = command;
    sPID->last = position;
}

void PID_Process_Position(PID_process_t *pPID, PID_process_t *sPID, int32_t position){
    
    int32_t ref_speed;

    pPID->curr = position;
	
    // Compute position errors
    ref_speed = PID_Process(pPID->PID, pPID->ref - pPID->curr);
 
    // Speed saturation
    if(pPID->speed_Limit)
    {
    	if (ref_speed > pPID->speed_Limit)
    	{
            ref_speed = pPID->speed_Limit;
        }
    	else if (ref_speed < - pPID->speed_Limit)
    	{
            ref_speed = - pPID->speed_Limit;
    	}
    }
	
    // Acceleration saturation
    if(pPID->acceleration_Limit)
    {
        if ( (ref_speed - pPID->last_ref) > pPID->acceleration_Limit)
        {
            ref_speed = pPID->last_ref + pPID->acceleration_Limit;
        }
        else if (  (pPID->last_ref - ref_speed )  >  pPID->acceleration_Limit )
        {
            ref_speed = pPID->last_ref - pPID->acceleration_Limit;
        }
    }
    
    if (sPID != NULL)
    {
        sPID->ref=ref_speed;
    	PID_Process_Speed(sPID,position);
    }
    else
    {
        // Send new motor reference
    	safe_setpwm(pPID->set_pwm, pPID->pwm_channel,ref_speed);
    	//dc_motor_set_speed(pPID->DC_Motor_Channel,ref_speed);
        pPID->last_ref = ref_speed;
    }
}

void PID_Process_holonomic(PID_process_t *pPIDx,PID_process_t *pPIDy,PID_process_t *pPIDteta)
{
    int32_t ref_speedx=0,ref_speedy=0,ref_speedteta=0;
    int32_t motor1_pos,motor2_pos,motor3_pos;
    int32_t motor1_speed,motor2_speed,motor3_speed;
    int32_t posx=0, posy=0, posteta=0;
    char str[60];

    motor1_pos = safe_getencoder(pPIDx->get_encoder,pPIDx->encoder_Channel);
    motor2_pos = safe_getencoder(pPIDy->get_encoder,pPIDy->encoder_Channel);
    motor3_pos = safe_getencoder(pPIDteta->get_encoder,pPIDteta->encoder_Channel);

    // Compute current position
    posx = (int32_t)(motor1_pos*-0.4553 + motor2_pos*0.3333 + motor3_pos*0.1220);
    posy = (int32_t)(motor1_pos*-0.1220 + motor2_pos*-0.3333 + motor3_pos*0.4553);
    posteta = (int32_t)(motor1_pos*-0.028 + motor2_pos*-0.028 + motor3_pos*-0.028);

	sprintf(str,"Posx=%i \t Posy=%i \t Posteta=%i \n\r",posx, posy, posteta);
	serial_puts(str);

    // Compute position errors
    ref_speedx = PID_Process(pPIDx->PID, pPIDx->ref - posx);
    ref_speedx = PID_Manage_limitation(pPIDx, ref_speedx);
    ref_speedy = PID_Process(pPIDy->PID, pPIDy->ref - posy);
    ref_speedy = PID_Manage_limitation(pPIDy, ref_speedy);
    ref_speedteta = PID_Process(pPIDteta->PID, pPIDteta->ref - posteta);
    ref_speedteta = PID_Manage_limitation(pPIDteta, ref_speedteta);

    motor1_speed = (int32_t)(ref_speedx*-1.366 + ref_speedy*-0.366 + ref_speedteta*-12.0115);
    motor2_speed = (int32_t)(ref_speedx*1 + ref_speedy*-1 + ref_speedteta*-12.0115);
    motor3_speed = (int32_t)(ref_speedx*0.366 + ref_speedy*1.366 + ref_speedteta*-12.0115);

    if(motor1_speed >= 1000)
    	motor1_speed = 1000;
    if(motor1_speed <= -1000)
    	motor1_speed = -1000;
    if(motor2_speed >= 1000)
    	motor2_speed = 1000;
    if(motor2_speed <= -1000)
    	motor2_speed = -1000;
    if(motor3_speed >= 1000)
    	motor3_speed = 1000;
    if(motor3_speed <= -1000)
    	motor3_speed = -1000;

    // Send new motor reference
    safe_setpwm(pPIDx->set_pwm, pPIDx->pwm_channel,(int16_t)motor1_speed);
    safe_setpwm(pPIDy->set_pwm, pPIDy->pwm_channel,(int16_t)motor2_speed);
    safe_setpwm(pPIDteta->set_pwm, pPIDteta->pwm_channel,(int16_t)motor3_speed);
}

void PID_Set_Coefficient(PID_struct_t *PID,int8_t KP,int8_t KI,int8_t KD,uint32_t I_limit){
    // Set coefficients
    PID->KP = KP;
    PID->KI = KI;
    PID->KD = KD;
    PID->I_limit = I_limit;
}

void PID_Reset(PID_process_t *xPID){
    xPID->PID->I_limit = 0;
    xPID->PID->err = 0;
    xPID->PID->err_I = 0;
    xPID->PID->last_err = 0;
    xPID->last = 0;
    xPID->last_ref = 0;

    // Reset PID gains
    PID_Set_Coefficient(xPID->PID,0,0,0,0);
	
    PID_Set_limitation(xPID,0,0);
}

void PID_Set_limitation(PID_process_t *xPID,int32_t S_limit, int32_t A_limit){
    // Set Saturations
    xPID->speed_Limit = S_limit;
    xPID->acceleration_Limit = A_limit;
    xPID->last_ref = 0;
}

int32_t PID_Manage_limitation(PID_process_t *xPID, int32_t param){
	int32_t value;
	value = param;

	    // Speed saturation
	    if(xPID->speed_Limit)
	    {
			if ( value > xPID->speed_Limit)
			{
				value = xPID->speed_Limit;
			}
			else if (value < -xPID->speed_Limit)
			{
				value = -xPID->speed_Limit;
			}
	    }

	    // Acceleration saturation
	    if(xPID->acceleration_Limit)
	    {
	        if ( (value - xPID->last_ref) > xPID->acceleration_Limit)
	        {
	        	value = xPID->last_ref + xPID->acceleration_Limit;
	        }
	        else if (  (xPID->last_ref - value )  >  xPID->acceleration_Limit )
	        {
	        	value = xPID->last_ref - xPID->acceleration_Limit;
	        }
	    }

	    xPID->last_ref = value;
	    return value;
}

PID_process_t* pid_init(void){
    PID_process_t *xPID;
    xPID =(PID_process_t *) malloc(sizeof(PID_process_t));
    xPID->PID=(PID_struct_t *) malloc(sizeof(PID_struct_t));
    PID_Reset(xPID);
    return xPID;
}

void PID_Set_Pwm(PID_process_t *xPID, void (*set_pwm)(void *, int32_t), void *pwm_channel){
    xPID->set_pwm = set_pwm;
    xPID->pwm_channel = pwm_channel;
}

void PID_Set_Encoder(PID_process_t *xPID, int32_t (*get_encoder)(void *), void *encoder_channel){
    xPID->get_encoder = get_encoder;
    xPID->encoder_Channel = encoder_channel;
}

void PID_Set_Cur_Position(PID_process_t *pPID, int32_t position) {
    pPID->curr = position;
}

void PID_Set_Ref_Position(PID_process_t *pPID, int32_t position) {
    pPID->ref = position;
}

int32_t PID_Get_Cur_Position(PID_process_t *pPID){
    return pPID->curr;
}

void PID_Set_Ref_Speed(PID_process_t *sPID, int16_t speed) {
    sPID->ref = speed;
}

int32_t PID_Get_Cur_Speed(PID_process_t *sPID){
    return sPID->curr;
}
