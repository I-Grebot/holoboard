/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file       motion_cs.c
 * @author     Pierrick
 * @date       May 10, 2017
 * -----------------------------------------------------------------------------
 * @brief
 *   This task handles the motion's control-system
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#include "main.h"
#include "dynamixel.h"
#include "ir_sensors.h"
#include "pid.h"

/* Local definitions */
#define MOTION_CONTROL_PERIOD_TICKS	(MOTION_CONTROL_PERIOD_MS / portTICK_PERIOD_MS)
#define MAX_SPEED 	2047
#define MOTOR1		0x0084
#define MOTOR2 		0x0082
#define MOTOR3		0x0081
#define ENCODER1	0x0004
#define ENCODER2	0x0002
#define ENCODER3	0x0001
#define TICK_PER_REV 1400
#define WHEEL_PERIMETER 188.5

#define CALIB_DISTANCE_X_IN_MM		500
#define CALIB_DISTANCE_X_IN_TICKS	3900
#define CALIB_DISTANCE_Y_IN_MM		500
#define CALIB_DISTANCE_Y_IN_TICKS	3700
#define CALIB_DISTANCE_TETA_IN_DEGREE	360
#define CALIB_DISTANCE_TETA_IN_TICKS	479
#define CALIB_SCALER 100000

#define HAS_TIRETTE	1
#define IS_YELLOW 1

/* Local, Private functions */
static void motion_cs_task(void *pvParameters);
static int16_t encoder1_Old, encoder2_Old, encoder3_Old;
static int32_t encoder1_Value=0,encoder2_Value=0,encoder3_Value=0;

static int32_t ratio_x_mm_per_ticks, ratio_y_mm_per_ticks, ratio_teta_degree_per_ticks;
static int16_t is_started, is_init;

static uint8_t should_use_irsensor_0;
static uint8_t should_use_irsensor_1;
static uint8_t should_use_irsensor_2;

static uint8_t should_use_irsensor_3;
static uint8_t should_use_irsensor_4;
static uint8_t should_use_irsensor_5;


PID_process_t *pPID_1, *pPID_2, *pPID_3;

/* -----------------------------------------------------------------------------
 * Initializations
 * -----------------------------------------------------------------------------
 */

BaseType_t motion_cs_start(void)
{
  BaseType_t ret;

  // Start the motion control task
  ret = xTaskCreate(motion_cs_task, "MOTION_CS", OS_TASK_STACK_MOTION_CS, NULL, OS_TASK_PRIORITY_MOTION_CS, NULL );

  return ret;

}

void motion_cs_init(void)
{

}

/* -----------------------------------------------------------------------------
 * Control system software flags
 * -----------------------------------------------------------------------------
 */



/* -----------------------------------------------------------------------------
 * Position setters
 * -----------------------------------------------------------------------------
 */

void encoder_set_position(int32_t x, int32_t y, int32_t teta) {
    encoder1_Value = (int32_t)(x*-1.366 + y*-0.366 + teta*-12.0115);
    encoder2_Value = (int32_t)(x*1 + y*-1 + teta*-12.0115);
    encoder3_Value = (int32_t)(x*0.366 + y*1.366 + teta*-12.0115);
}

/* -----------------------------------------------------------------------------
 * Position getters
 * -----------------------------------------------------------------------------
 */

int32_t encoder_get_position(uint16_t QEI)
{
	int16_t encoder;
	int16_t delta;

	int32_t output;

	output = 0;

	encoder = hb_lcmxo2_get_qei(QEI)*16;
	switch(QEI)
	{
	case ENCODER1:
	   	delta = encoder - encoder1_Old;
	    encoder1_Old = encoder;
	    encoder1_Value += (int32_t)delta/16;
	    output = encoder1_Value;
	    break;
	case ENCODER2:
	   	delta = encoder - encoder2_Old;
	    encoder2_Old = encoder;
	    encoder2_Value += (int32_t)delta/16;
	    output = encoder2_Value;
	    break;
	case ENCODER3:
	   	delta = encoder - encoder3_Old;
	    encoder3_Old = encoder;
	    encoder3_Value += (int32_t)delta/16;
	    output = encoder3_Value;
	    break;
	default :
		output = 0;
	}

	return output;
}



/* -----------------------------------------------------------------------------
 * Speed setters
 * -----------------------------------------------------------------------------
 */
void motor1_set_speed(int speed)
{
	if(speed >= MAX_SPEED)
	{
		speed = MAX_SPEED;
	}

	if(speed <= -MAX_SPEED)
	{
		speed = -MAX_SPEED;
	}

	hb_lcmxo2_set_pwm(MOTOR1, speed);
}

void motor2_set_speed(int speed)
{
	if(speed >= MAX_SPEED)
	{
		speed = MAX_SPEED;
	}

	if(speed <= -MAX_SPEED)
	{
		speed = -MAX_SPEED;
	}

	hb_lcmxo2_set_pwm(MOTOR2, speed);
}

void motor3_set_speed(int speed)
{
	if(speed >= MAX_SPEED)
	{
		speed = MAX_SPEED;
	}

	if(speed <= -MAX_SPEED)
	{
		speed = -MAX_SPEED;
	}

	hb_lcmxo2_set_pwm(MOTOR3, speed);
}


void motors_set_speed(int speed_motor1, int speed_motor2, int speed_motor3)
{
	motor1_set_speed(speed_motor1);
	motor2_set_speed(speed_motor2);
	motor3_set_speed(speed_motor3);
}

void robot_set_speed(int speed_x, int speed_y, int speed_teta)
{
	int16_t speed_motor1;
	int16_t speed_motor2;
	int16_t speed_motor3;

	speed_motor1 = (int16_t)(speed_x*-1.366 + speed_y*-0.366 + speed_teta*20.1261);
	speed_motor2 = (int16_t)(speed_x*1 + speed_y*-1 + speed_teta*20.1261);
	speed_motor3 = (int16_t)(speed_x*0.366 + speed_y*1.366 + speed_teta*20.1261);

	motors_set_speed(speed_motor1,speed_motor2,speed_motor3);
}
/* -----------------------------------------------------------------------------
 * Speed getters
 * -----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------
 * Aversive mutexes management
 * -----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------
 * Helpers GoTo Position
 * -----------------------------------------------------------------------------
 */



void set_ratio_x_mm_per_ticks(int32_t ticks_for_distance, int32_t distance_mm) {
	ratio_x_mm_per_ticks = (CALIB_SCALER * distance_mm) / ticks_for_distance;
}

void set_ratio_y_mm_per_ticks(int32_t ticks_for_distance, int32_t distance_mm) {
	ratio_y_mm_per_ticks = (CALIB_SCALER * distance_mm) / ticks_for_distance;
}

void set_ratio_teta_degree_per_ticks(int32_t ticks_for_rotation, int32_t rotation_deg)
{
	ratio_teta_degree_per_ticks = (CALIB_SCALER * rotation_deg) / ticks_for_rotation;
}

void go_to_position_ticks(int32_t x, int32_t y, int32_t teta)
{
	PID_Set_Ref_Position(pPID_1, x);
	PID_Set_Ref_Position(pPID_2, y);
	PID_Set_Ref_Position(pPID_3, teta);
}

void go_to_position_relative_metric(int32_t x, int32_t y, int32_t teta)
{
	int32_t x_in_ticks = (x * CALIB_SCALER) / ratio_x_mm_per_ticks;
	int32_t y_in_ticks = (y * CALIB_SCALER) / ratio_y_mm_per_ticks;
	int32_t teta_in_ticks = (teta * CALIB_SCALER) / ratio_teta_degree_per_ticks;

	go_to_position_ticks(x_in_ticks, y_in_ticks, teta_in_ticks);
}

void go_to_position_absolute_metric(int32_t x, int32_t y, int32_t teta)
{
}

void set_irsensors_for_moving_toward_y_axis(){
	should_use_irsensor_0 = 1;
	should_use_irsensor_1 = 1;
	should_use_irsensor_2 = 0;

	should_use_irsensor_3 = 0;
	should_use_irsensor_4 = 1;
	should_use_irsensor_5 = 0;
}

void set_irsensors_for_moving_backward_y_axis(){
	should_use_irsensor_0 = 0;
	should_use_irsensor_1 = 0;
	should_use_irsensor_2 = 1;

	should_use_irsensor_3 = 0;
	should_use_irsensor_4 = 0;
	should_use_irsensor_5 = 0;
}

void set_irsensors_for_moving_toward_x_axis(){
	should_use_irsensor_0 = 0;
	should_use_irsensor_1 = 1;
	should_use_irsensor_2 = 0;

	should_use_irsensor_3 = 0;
	should_use_irsensor_4 = 1;
	should_use_irsensor_5 = 0;
}

void set_irsensors_for_moving_backward_x_axis(){
	should_use_irsensor_0 = 1;
	should_use_irsensor_1 = 0;
	should_use_irsensor_2 = 0;

	should_use_irsensor_3 = 0;
	should_use_irsensor_4 = 0;
	should_use_irsensor_5 = 1;
}

void deactivate_all_sensors(){
	should_use_irsensor_0 = 0;
	should_use_irsensor_1 = 0;
	should_use_irsensor_2 = 0;

	should_use_irsensor_3 = 0;
	should_use_irsensor_4 = 0;
	should_use_irsensor_5 = 0;
}

void activate_all_sensors(){
	should_use_irsensor_0 = 1;
	should_use_irsensor_1 = 1;
	should_use_irsensor_2 = 1;

	should_use_irsensor_3 = 1;
	should_use_irsensor_4 = 1;
	should_use_irsensor_5 = 1;
}

void set_limitation_normal() {
  PID_Set_limitation(pPID_1,700,75);
  PID_Set_limitation(pPID_2,700,75);
  PID_Set_limitation(pPID_3,500,20);
}

void set_limitation_slow() {
  PID_Set_limitation(pPID_1,350,75);
  PID_Set_limitation(pPID_2,350,75);
  PID_Set_limitation(pPID_3,250,20);
}


/* -----------------------------------------------------------------------------
 * Main Motion Control System Managment Task
 * TODO: handle re-init of the task
 * -----------------------------------------------------------------------------
 */

void motion_cs_task(void *pvParameters)
{
  /* Static local variables */
  static int32_t old_a        = 0;
  static int32_t old_d        = 0;
  static int32_t old_speed_a  = 0;
  static int32_t old_speed_d  = 0;

  TickType_t xNextWakeTime;

  uint16_t timer=0;
  uint16_t timer_delay = 0;
  uint16_t start_delay = 0;
  uint16_t timer_offset = 0;

  uint8_t is_yellow = 0;

  should_use_irsensor_0 = 0;
  should_use_irsensor_1 = 0;
  should_use_irsensor_2 = 0;

  should_use_irsensor_3 = 0;
  should_use_irsensor_4 = 0;
  should_use_irsensor_5 = 0;

  is_started = 0;
  is_init = 0;

  /* Initialise xNextWakeTime - this only needs to be done once. */
  xNextWakeTime = xTaskGetTickCount();

  /* Wait for 1s to let lcmxo2 startup */
  LCMXO2_RESET_WRITE(LCMXO2_RESET_ON);
  vTaskDelayUntil( &xNextWakeTime, MOTION_CONTROL_PERIOD_TICKS*10);

  /* Init motors and PID */
  motor1_set_speed(0);
  motor2_set_speed(0);
  motor3_set_speed(0);
  pPID_1 = pid_init();		//position PID 1
  pPID_2 = pid_init();
  pPID_3 = pid_init();
  PID_Set_Pwm(pPID_1,(void*)hb_lcmxo2_set_pwm,(void*)MOTOR1);
  PID_Set_Pwm(pPID_2,(void*)hb_lcmxo2_set_pwm,(void*)MOTOR2);
  PID_Set_Pwm(pPID_3,(void*)hb_lcmxo2_set_pwm,(void*)MOTOR3);
  PID_Set_Encoder(pPID_1, (void*)encoder_get_position,(void*)ENCODER1);
  PID_Set_Encoder(pPID_2, (void*)encoder_get_position,(void*)ENCODER2);
  PID_Set_Encoder(pPID_3, (void*)encoder_get_position,(void*)ENCODER3);
  PID_Set_Coefficient(pPID_1->PID,1,0,0,0);
  PID_Set_Coefficient(pPID_2->PID,1,0,0,0);
  PID_Set_Coefficient(pPID_3->PID,1,0,0,0);

  /* Init PID limitation */
  PID_Set_limitation(pPID_1,700,75);
  PID_Set_limitation(pPID_2,700,75);
  PID_Set_limitation(pPID_3,500,20);

  set_ratio_x_mm_per_ticks(CALIB_DISTANCE_X_IN_TICKS, CALIB_DISTANCE_X_IN_MM);
  set_ratio_y_mm_per_ticks(CALIB_DISTANCE_Y_IN_TICKS, CALIB_DISTANCE_Y_IN_MM);
  set_ratio_teta_degree_per_ticks(CALIB_DISTANCE_TETA_IN_TICKS, CALIB_DISTANCE_TETA_IN_DEGREE);


  /* Remove compiler warning about unused parameter. */
  ( void ) pvParameters;
  led_set_mode(HB_LED_BLINK_FAST);
  led_set_color(HB_LED_RED);
  while(is_init == 0)
  {
	  if(ENDSTOP1_VALUE == HAS_TIRETTE)
	  {
		  //wait and check tirette status again to debounce
		  vTaskDelayUntil( &xNextWakeTime, 100);
		  if(ENDSTOP1_VALUE == HAS_TIRETTE)
		  {
			  led_set_color(HB_LED_BLUE);
			  is_init = 1;

			  if(ENDSTOP5_VALUE == IS_YELLOW) {
				  is_yellow = 1;
				  dsv_set_led(DXL_LED_YELLOW);
			  } else {
				  is_yellow = 0;
				  dsv_set_led(DXL_LED_BLUE);
			  }
		  }
	  }

	  vTaskDelayUntil( &xNextWakeTime, MOTION_CONTROL_PERIOD_TICKS);
  }

  while(is_started == 0)
  {
	  if(ENDSTOP1_VALUE != HAS_TIRETTE)
	  {
		  is_started = 1;
		  LCMXO2_RESET_WRITE(LCMXO2_RESET_OFF);
		  vTaskDelayUntil( &xNextWakeTime, MOTION_CONTROL_PERIOD_TICKS*50);
	  }

	  vTaskDelayUntil( &xNextWakeTime, MOTION_CONTROL_PERIOD_TICKS);
  }
  for( ;; )
  {
	//  led_set_mode(HB_LED_BLINK_SLOW);

	  //sensors position
	  //	  5
	  //    2/_\0   X is parallel to 2 (</), Y is perpendicular to X
	  //      1 4

	  timer_offset = 0;
      // Manage Robot System
     // rs_update(&robot.cs.rs);

	  if(timer == 0) {
		  deactivate_all_sensors();
		  go_to_position_relative_metric(0, 0, 0);
		  dsv_open_grabber();
	  }
	  timer_offset += 100;

//	  //go out of departure area
//	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
//	  {
//		  set_irsensors_for_moving_toward_y_axis();
//
//		  //sould not detect wall
//		  should_use_irsensor_0 = is_yellow ? 1 : 0;
//		  should_use_irsensor_1 = is_yellow ? 0 : 1;
//
//		  go_to_position_relative_metric(0, 900, 0);
//	  }
//	  timer_offset += 4000;
//
//	  //offset from the rocket
//	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
//	  {
//		  if (is_yellow)
//		  {
//			  set_irsensors_for_moving_backward_x_axis();
//		  }
//		  else
//		  {
//			  set_irsensors_for_moving_toward_x_axis();
//		  }
//
//		  go_to_position_relative_metric(is_yellow ? -100 : 100, 900, 0);
//	  }
//	  timer_offset += 2000;
//
//	  //go to middle of the table (1,5m)
//	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
//	  {
//		  set_irsensors_for_moving_toward_y_axis();
//
//		  //sould not detect wall
//		  should_use_irsensor_0 = is_yellow ? 1 : 0;
//		  should_use_irsensor_1 = is_yellow ? 0 : 1;
//
//		  go_to_position_relative_metric(is_yellow ? -100 : 100, 1500, 0);
//	  }
//	  timer_offset += 3000;
//
//	  //turn 90deg
//	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
//	  {
//		  deactivate_all_sensors();
//
//		  go_to_position_relative_metric(is_yellow ? -100 : 100, 1500, is_yellow ? -90 : 90);
//	  }
//	  timer_offset += 2000;
//
//	  //fix position using wall
//	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
//	  {
//		  deactivate_all_sensors();
//
//		  go_to_position_relative_metric(is_yellow ? -100 : 100, 0, is_yellow ? -90 : 90);
//	  }
//	  timer_offset += 3000;
//
//	  //fix position using rocket
//	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
//	  {
//		  set_limitation_slow();
//		  encoder_set_position(0, 0, 0);
//
//		  deactivate_all_sensors();
//
//		  go_to_position_relative_metric(is_yellow ? -500 : 500, 0, 0);
//	  }
//	  timer_offset += 3000;
//
//	  //fix position using wall again
//	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
//	  {
//		  set_limitation_normal();
//		  encoder_set_position(0, 0, 0);
//		  deactivate_all_sensors();
//
//		  go_to_position_relative_metric(is_yellow ? 10 : -10,-100,0);
//	  }
//	  timer_offset += 3000;

	  //offset from rocket
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  set_limitation_normal();
		  encoder_set_position(0, 0, 0);

		  deactivate_all_sensors();

		  go_to_position_relative_metric(is_yellow ? 50 : -50, 100, 0);
	  }
	  timer_offset += 3000;

	  //face tube
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  encoder_set_position(0, 0, 0);

		  deactivate_all_sensors();
		  go_to_position_relative_metric(0, 0, is_yellow ? 80 : -80);
	  }
	  timer_offset += 1000;

	  //go near tube
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  encoder_set_position(0, 0, 0);

			should_use_irsensor_0 = 0;
			should_use_irsensor_1 = 1;
			should_use_irsensor_2 = 0;

			should_use_irsensor_3 = 0;
			should_use_irsensor_4 = 1;
			should_use_irsensor_5 = 0;

		  PID_Set_limitation(pPID_1,350,75);
		  PID_Set_limitation(pPID_2,700,75);

		  go_to_position_relative_metric(is_yellow ? -570 : 570, 1225, 0);
	  }
	  timer_offset += 5000;

	  //turn to face the tube
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  encoder_set_position(0, 0, 0);

		  activate_all_sensors();

		  go_to_position_relative_metric(0, 0, is_yellow ? 150 : -150);
	  }
	  timer_offset += 3000;

	  //little finger touch the tube
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  encoder_set_position(0, 0, 0);

		  PID_Set_limitation(pPID_1,700,75);
		  PID_Set_limitation(pPID_2,350,75);
		  set_irsensors_for_moving_toward_x_axis();

		  go_to_position_relative_metric(is_yellow ? 150 : -150, -50, 0);
	  }
	  timer_offset += 3000;


	  //close little finger
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  dsv_close_grabber();
	  }
	  timer_offset += 1000;

	  //face team zone
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  encoder_set_position(0, 0, 0);

		  deactivate_all_sensors();

		  go_to_position_relative_metric(0, 0, is_yellow ? 95 : -95);
	  }
	  timer_offset += 1000;

	  //go near team zone
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  encoder_set_position(0, 0, 0);

		  set_irsensors_for_moving_toward_y_axis();
		  PID_Set_limitation(pPID_1,700,75);
		  PID_Set_limitation(pPID_2,350,75);
		  go_to_position_relative_metric(is_yellow ? 720 : -720, 1640, 0);
	  }
	  timer_offset += 8000;

	  //go near near near near team zone ignoring rocket sided sensor
//	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
//	  {
//		  encoder_set_position(0, 0, 0);
//
//		  set_irsensors_for_moving_toward_y_axis();
//
//		  //sould not detect wall
//		  should_use_irsensor_0 = is_yellow ? 0 : 1;
//		  should_use_irsensor_1 = is_yellow ? 1 : 0;
//
//		  go_to_position_relative_metric(0, 400, 0);
//	  }
//	  timer_offset += 3000;

	  //turn 180
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  encoder_set_position(0, 0, 0);

		  deactivate_all_sensors();

		  go_to_position_relative_metric(0, 0, 150);
	  }
	  timer_offset += 3000;

	  //drop the bass
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  dsv_open_grabber();
	  }
	  timer_offset += 1000;

	  //turn a little bit
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  encoder_set_position(0, 0, 0);

		  set_irsensors_for_moving_toward_y_axis();

		  go_to_position_relative_metric(0, 0, is_yellow ? -30 : 30);
	  }

	  //run away
	  if(timer == ((timer_offset+start_delay)/MOTION_CONTROL_PERIOD_TICKS) + timer_delay)
	  {
		  encoder_set_position(0, 0, 0);

		  set_irsensors_for_moving_toward_y_axis();

		  go_to_position_relative_metric(0, 500, 0);
	  }

	  //
	  //encoder_set_position(int32_t x, int32_t y, int32_t teta)



	  //pause the robot when a valid obstacle is detected
	  if((should_use_irsensor_0 && sensor0detect) || (should_use_irsensor_1 && sensor1detect) || (should_use_irsensor_2 && sensor2detect)
			  || (should_use_irsensor_3 && sensor3detect) || (should_use_irsensor_4 && sensor4detect) || (should_use_irsensor_5 && sensor5detect))
	  {
		  PID_Pause_holonomic(pPID_1, pPID_2, pPID_3);
		  timer_delay++;
	  }
	  else // run when no valid obstacle
	  {
		  PID_Process_holonomic(pPID_1, pPID_2, pPID_3);
	  }


	  timer++;

	  // end of game
	  if(timer> (90000/MOTION_CONTROL_PERIOD_TICKS))
	  {
		  PID_Pause_holonomic(pPID_1, pPID_2, pPID_3);
		  led_set_color(HB_LED_RED);
		  while(1);
	  }

	  // sprintf(str,"dummy3=%u \t old_dummy3=%u \t delta3=%u\n\r",dummy3, old_dummy3, (dummy3-old_dummy3)&0x0FFF);
	  //serial_puts(str);
	  /* Wakes-up when required */
	  vTaskDelayUntil( &xNextWakeTime, MOTION_CONTROL_PERIOD_TICKS);
  }
}

