#include "angle_distance.h"

#ifndef _ROBOT_SYSTEM_H_
#define _ROBOT_SYSTEM_H_



struct robot_system
{
	uint8_t flags;

	struct rs_polar virtual_pwm;
	struct rs_polar virtual_encoders;

	struct rs_polar pmot_prev;
	struct rs_wheels wmot_prev;

	/* Motors encoders */
	int32_t (*wheel1_encoder)(void *);
	void* wheel1_encoder_param;
	float wheel1_gain;
	
	int32_t (*wheel2_encoder)(void *);
	void* wheel2_encoder_param;
	float wheel2_gain;

	int32_t (*wheel3_encoder)(void *);
	void* wheel3_encoder_param;
	float wheel3_gain;

	/* PWM */
	void (*pwm_motor1)(void *, int32_t);
	void *pwm_motor1_param;
	void (*pwm_motor2)(void *, int32_t);
	void *pwm_motor2_param;
	void (*pwm_motor3)(void *, int32_t);
	void *pwm_motor3_param;
};

/** Set the structure to 0 */
void rs_init( struct robot_system * );

/**** ACCESSORS */

/** define PWM functions and params */
void rs_set_motor1_pwm(struct robot_system * rs, void (*motor1_pwm)(void *, int32_t), void *motor1_pwm_param);
void rs_set_motor2_pwm(struct robot_system * rs, void (*motor2_pwm)(void *, int32_t), void *motor2_pwm_param);
void rs_set_motor3_pwm(struct robot_system * rs, void (*motor3_pwm)(void *, int32_t), void *motor3_pwm_param);


/** define motors encoder functions and params */
void rs_set_motor1_encoder(struct robot_system * rs, int32_t (*motor1_encoder)(void *),
			     void *motor1_encoder_param, double gain);
void rs_set_motor2_encoder(struct robot_system * rs, int32_t (*motor2_encoder)(void *),
			     void *motor2_encoder_param, double gain);
void rs_set_motor3_encoder(struct robot_system * rs, int32_t (*motor3_encoder)(void *),
			     void *motor3_encoder_param, double gain);

/**** Virtual encoders and PWM */

/** 
 * set the real pwms according to the specified angle (it also
 * depends on the last distance command sent) 
 */
void rs_set_angle(void * rs, int32_t angle);

/** 
 * set the real pwms according to the specified distance in x
 * (it also depends on the last angle command sent)
 */
void rs_set_distance_x(void * rs, int32_t distance);

/**
 * set the real pwms according to the specified distance in y
 * (it also depends on the last angle command sent)
 */
void rs_set_distance_y(void * rs, int32_t distance);

/** 
 * get the virtual angle according to real encoders value. 
 */
int32_t rs_get_angle(void * rs);

/** 
 * get the virtual distance according to real encoders value. 
 */
int32_t rs_get_distance_x(void * rs);

/** 
 * get the virtual distance according to real encoders value.
 */
int32_t rs_get_distance_y(void * rs);


/** 
 * Read the encoders, and update internal virtual counters. Call this
 * function is needed before reading the virtual encoders. The program
 * will decide if it the external encoders or the motor encoders are
 * taken in account (depending on flags, but not yet)
 */
void rs_update(void * rs);

void rs_set_flags(struct robot_system * rs, uint8_t flags);


#endif /* #ifndef _ROBOT_SYSTEM */
