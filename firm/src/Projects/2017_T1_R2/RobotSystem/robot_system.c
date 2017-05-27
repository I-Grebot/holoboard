#include <string.h>
#include <stdio.h>

#include "motion.h"
#include "robot_system.h"
#include "angle_distance.h"
#include "holoboard.h"


/** Call a pwm() pointer : 
 * - lock the interrupts
 * - read the pointer to the pwm function
 * - unlock the interrupts
 * - if pointer is null, don't do anything
 * - else call the pwm with the parameters
 */
static inline void
safe_setpwm(void (*f)(void *, int32_t), void * param, int32_t value)
{
	void (*f_tmp)(void *, int32_t);
	void * param_tmp;
	f_tmp = f;
	param_tmp = param;
	if (f_tmp) {
		f_tmp(param_tmp, value);
	}
}

/** Call a encoder() pointer : 
 * - lock the interrupts
 * - read the pointer to the encoder function
 * - unlock the interrupts
 * - if pointer is null, return 0
 * - else return the value processed by the function
 */
static inline uint32_t
safe_getencoder(int32_t (*f)(void *), void * param)
{
	int32_t (*f_tmp)(void *);
	void * param_tmp;
	f_tmp = f;
	param_tmp = param;
	if (f_tmp) {
		return f_tmp(param_tmp);
	}
	return 0;
}

/** Set the structure to 0 */
void rs_init( struct robot_system * rs)
{
	memset(rs, 0, sizeof(struct robot_system));
}

/** define left PWM function and param */
void rs_set_motor1_pwm(struct robot_system * rs, void (*motor1_pwm)(void *, int32_t), void *motor1_pwm_param)
{
	rs->pwm_motor1 = motor1_pwm;
	rs->pwm_motor1_param = motor1_pwm_param;
}

void rs_set_motor2_pwm(struct robot_system * rs, void (*motor2_pwm)(void *, int32_t), void *motor2_pwm_param)
{
	rs->pwm_motor2 = motor2_pwm;
	rs->pwm_motor2_param = motor2_pwm_param;
}

void rs_set_motor3_pwm(struct robot_system * rs, void (*motor3_pwm)(void *, int32_t), void *motor3_pwm_param)
{
	rs->pwm_motor3 = motor3_pwm;
	rs->pwm_motor3_param = motor3_pwm_param;
}

/** define left motor encoder function and param */
void rs_set_motor1_encoder(struct robot_system * rs, int32_t (*motor1_encoder)(void *),
			     void *motor1_encoder_param, double gain)
{
	rs->wheel1_encoder = motor1_encoder;
	rs->wheel1_encoder_param = motor1_encoder_param;
	rs->wheel1_gain = gain;
}
void rs_set_motor2_encoder(struct robot_system * rs, int32_t (*motor2_encoder)(void *),
			     void *motor2_encoder_param, double gain)
{
	rs->wheel1_encoder = motor2_encoder;
	rs->wheel1_encoder_param = motor2_encoder_param;
	rs->wheel1_gain = gain;
}
void rs_set_motor3_encoder(struct robot_system * rs, int32_t (*motor3_encoder)(void *),
			     void *motor3_encoder_param, double gain)
{
	rs->wheel1_encoder = motor3_encoder;
	rs->wheel1_encoder_param = motor3_encoder_param;
	rs->wheel1_gain = gain;
}


/**** Virtual encoders and PWM */

/** 
 * set the real pwms according to the specified angle (it also
 * depends on the last distance command sent) 
 */
void rs_set_angle(void * data, int32_t angle)
{
	struct rs_polar p;
	struct rs_wheels w;
	struct robot_system * rs = data;

	p.distance_x = rs->virtual_pwm.distance_x ;
	p.distance_y = rs->virtual_pwm.distance_y ;
	rs->virtual_pwm.angle = angle;

	p.angle = angle;
	rs_get_wheels_from_polar(&w, &p);
	
	safe_setpwm(rs->pwm_motor1, rs->pwm_motor1_param, w.wheel_1);
	safe_setpwm(rs->pwm_motor2, rs->pwm_motor2_param, w.wheel_2);
	safe_setpwm(rs->pwm_motor3, rs->pwm_motor3_param, w.wheel_3);
}

/** 
 * set the real pwms according to the specified distance (it also
 * depends on the last angle command sent) 
 */
void rs_set_distance_x(void * data, int32_t distance)
{
	struct robot_system * rs = data;
	struct rs_polar p;
	struct rs_wheels w;

	p.angle = rs->virtual_pwm.angle ;
	p.distance_y = rs->virtual_pwm.distance_y ;
	rs->virtual_pwm.distance_x = distance;

	p.distance_x = distance;
	rs_get_wheels_from_polar(&w, &p);
	
	safe_setpwm(rs->pwm_motor1, rs->pwm_motor1_param, w.wheel_1);
	safe_setpwm(rs->pwm_motor2, rs->pwm_motor2_param, w.wheel_2);
	safe_setpwm(rs->pwm_motor3, rs->pwm_motor3_param, w.wheel_3);
}

void rs_set_distance_y(void * data, int32_t distance)
{
	struct robot_system * rs = data;
	struct rs_polar p;
	struct rs_wheels w;

	p.angle = rs->virtual_pwm.angle ;
	p.distance_x = rs->virtual_pwm.distance_x ;
	rs->virtual_pwm.distance_y = distance;

	p.distance_y = distance;
	rs_get_wheels_from_polar(&w, &p);

	safe_setpwm(rs->pwm_motor1, rs->pwm_motor1_param, w.wheel_1);
	safe_setpwm(rs->pwm_motor2, rs->pwm_motor2_param, w.wheel_2);
	safe_setpwm(rs->pwm_motor3, rs->pwm_motor3_param, w.wheel_3);
}

/** 
 * get the virtual angle according to real encoders value. 
 */
int32_t rs_get_angle(void * data)
{
	struct robot_system * rs = data;
	int32_t angle;
	
	//vLockEncoderAngle();
	angle = rs->virtual_encoders.angle ;	
	//vUnlockEncoderAngle();
	return angle;
}

/** 
 * get the virtual distance according to real encoders value. 
 */
int32_t rs_get_distance_x(void * data)
{
	struct robot_system * rs = data;
	int32_t distance;

	//vLockEncoderDistance();
	distance = rs->virtual_encoders.distance_x ;
	//vUnlockEncoderDistance();
	return distance;
}

int32_t rs_get_distance_y(void * data)
{
	struct robot_system * rs = data;
	int32_t distance;
	
	//vLockEncoderDistance();
	distance = rs->virtual_encoders.distance_y ;
	//vUnlockEncoderDistance();
	return distance;
}

void rs_set_flags(struct robot_system * rs, uint8_t flags)
{
	rs->flags = flags;
}

/** 
 * Read the encoders, and update internal virtual counters. Call this
 * function is needed before reading the virtual encoders.The program
 * will decide if it the external encoders or the motor encoders are
 * taken in account (depending on flags, but not yet)
 */
void rs_update(void * data)
{
	struct robot_system * rs = data;
	struct rs_wheels wmot;
	struct rs_polar pmot;

	int32_t delta_angle, delta_distance_x, delta_distance_y;
	
	/* read encoders */
	wmot.wheel_1 = safe_getencoder(rs->pwm_motor1, rs->pwm_motor1_param);
	wmot.wheel_2 = safe_getencoder(rs->pwm_motor2, rs->pwm_motor2_param);
	wmot.wheel_3 = safe_getencoder(rs->pwm_motor3, rs->pwm_motor3_param);
	
	/* apply gains to each wheel */
//	if (! (rs->flags & RS_IGNORE_MOT_GAIN )) {
		wmot.wheel_1 = wmot.wheel_1*rs->wheel1_gain;
		wmot.wheel_2 = wmot.wheel_2*rs->wheel2_gain;
		wmot.wheel_3 = wmot.wheel_3*rs->wheel3_gain;
//	}

	rs_get_polar_from_wheels(&pmot, &wmot);

	delta_angle = pmot.angle - rs->pmot_prev.angle;
	delta_distance_x = pmot.distance_x - rs->pmot_prev.distance_x;
	delta_distance_y = pmot.distance_y - rs->pmot_prev.distance_y;

//	vLockEncoderAngle();
//	vLockEncoderDistance();
	rs->virtual_encoders.angle += delta_angle;
	rs->virtual_encoders.distance_x += delta_distance_x;
	rs->virtual_encoders.distance_y += delta_distance_y;

//	vUnlockEncoderAngle();
//	vUnlockEncoderDistance();

	/* don't lock too much time */
	rs->pmot_prev = pmot;
	rs->wmot_prev = wmot;
}
