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

/* Local, Private functions */
static void motion_cs_task(void *pvParameters);

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


/* -----------------------------------------------------------------------------
 * Position getters
 * -----------------------------------------------------------------------------
 */

int16_t encoder1_get_speed(void)
{
	int16_t value;
	value = hb_lcmxo2_get_qei(ENCODER1);
	if(value&0x0800)
		return value|0xF800;
	else
		return value;
}

int16_t encoder2_get_speed(void)
{
	int16_t value;
	value = hb_lcmxo2_get_qei(ENCODER2);
	if(value&0x0800)
		return value|0xF800;
	else
		return value;
}

int16_t encoder3_get_speed(void)
{
	int16_t value;
	value = hb_lcmxo2_get_qei(ENCODER3);
	if(value&0x0800)
		return value|0xF800;
	else
		return value;
}

/* -----------------------------------------------------------------------------
 * Speed setters
 * -----------------------------------------------------------------------------
 */
void motor1_set_speed(int speed)
{
	if(speed >= MAX_SPEED)
		speed = MAX_SPEED;
	if(speed <= -MAX_SPEED)
		speed = -MAX_SPEED;
	hb_lcmxo2_set_pwm(MOTOR1, speed);
}

void motor2_set_speed(int speed)
{
	if(speed >= MAX_SPEED)
		speed = MAX_SPEED;
	if(speed <= -MAX_SPEED)
		speed = -MAX_SPEED;
	hb_lcmxo2_set_pwm(MOTOR2, speed);
}

void motor3_set_speed(int speed)
{
	if(speed >= MAX_SPEED)
		speed = MAX_SPEED;
	if(speed <= -MAX_SPEED)
		speed = -MAX_SPEED;
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
 * Main Motion Control System Managment Task
 * TODO: handle re-init of the task
 * -----------------------------------------------------------------------------
 */

void motion_cs_task(void *pvParameters)
{
  TickType_t xNextWakeTime;
  PID_process_t *pPID_1, *pPID_2, *pPID_3;
  static int16_t dummy1=0, old_dummy1=0;
  static int16_t dummy2=0, old_dummy2=0;
  static int16_t dummy3=0, old_dummy3=0;
  static int32_t position1=0, position2=0,position3=0;
  int16_t speed_motor1;
  int16_t speed_motor2;
  int16_t speed_motor3;
  int16_t speed_x=0, speed_y=0, speed_teta=100;
  char str[60];
  /* Initialise xNextWakeTime - this only needs to be done once. */
  xNextWakeTime = xTaskGetTickCount();
  /* Wait for 1s to let lcmxo2 startup*/
  vTaskDelayUntil( &xNextWakeTime, MOTION_CONTROL_PERIOD_TICKS*10);
  LCMXO2_RESET_WRITE(LCMXO2_RESET_OFF);
  vTaskDelayUntil( &xNextWakeTime, MOTION_CONTROL_PERIOD_TICKS*10);
  motor1_set_speed(0);
  motor2_set_speed(0);
  motor3_set_speed(0);
  pPID_1 = pid_init((void*)hb_lcmxo2_set_pwm,(void*)MOTOR1,0,1);		//position PID 1
  pPID_2 = pid_init((void*)hb_lcmxo2_set_pwm,(void*)MOTOR2,0,1);
  pPID_3 = pid_init((void*)hb_lcmxo2_set_pwm,(void*)MOTOR3,0,1);
  PID_Set_Coefficient(pPID_1->PID,4,0,1,0);
  PID_Set_Coefficient(pPID_2->PID,4,0,1,0);
  PID_Set_Coefficient(pPID_3->PID,4,0,1,0);
  PID_Set_limitation(pPID_1,1000,0);
  PID_Set_limitation(pPID_2,1000,0);
  PID_Set_limitation(pPID_3,1000,0);

  speed_motor1 = (int16_t)(speed_x*-1.366 + speed_y*-0.366 + speed_teta*20.1261);
  speed_motor2 = (int16_t)(speed_x*1 + speed_y*-1 + speed_teta*20.1261);
  speed_motor3 = (int16_t)(speed_x*0.366 + speed_y*1.366 + speed_teta*20.1261);
  PID_Set_Ref_Position(pPID_1,speed_motor1);
  PID_Set_Ref_Position(pPID_2,speed_motor2);
  PID_Set_Ref_Position(pPID_3,speed_motor3);
  /* Remove compiler warning about unused parameter. */
  ( void ) pvParameters;

  for( ;; )
  {
	  old_dummy1 = dummy1;
	  dummy1=encoder1_get_speed();
	  position1=position1+(dummy1-old_dummy1);
	  old_dummy2 = dummy2;
	  dummy2=encoder2_get_speed();
	  position2=position2+(dummy2-old_dummy2);
	  old_dummy3 = dummy3;
	  dummy3=encoder3_get_speed();
	  position3=position3+(dummy3-old_dummy3);
	 // sprintf(str,"dummy1=%i \t old_dummy1=%i \t position1=%i\n\r",dummy1, old_dummy1, position1);
	 // serial_puts(str);
	  //old_dummy2 = dummy2;
	  //old_dummy3 = dummy3;
	  PID_Process_Position(pPID_1, NULL, position1);
	  PID_Process_Position(pPID_2, NULL, position2);
	  PID_Process_Position(pPID_3, NULL, position3);
	  sprintf(str,"position1=%i \t position2=%i \t position3=%i\n\r",position1, position2, position3);
	  serial_puts(str);
	//	  motor1_set_speed(1023-dummy1);
//	  sprintf(str,"dummy1=%u \t old_dummy1=%u \t delta1=%u\n\r",dummy1, old_dummy1, (dummy1-old_dummy1)&0x0FFF);
//	  serial_puts(str);
//	  dummy2 = encoder2_get_speed();
	//  sprintf(str,"dummy2=%u \t old_dummy2=%u \t delta2=%u\n\r",dummy2, old_dummy2, (dummy2-old_dummy2)&0x0FFF);
	//  serial_puts(str);
	 // dummy3 = encoder3_get_speed();
	 // sprintf(str,"dummy3=%u \t old_dummy3=%u \t delta3=%u\n\r",dummy3, old_dummy3, (dummy3-old_dummy3)&0x0FFF);
	  //serial_puts(str);
	  /* Wakes-up when required */
	  vTaskDelayUntil( &xNextWakeTime, MOTION_CONTROL_PERIOD_TICKS);
  }
}

