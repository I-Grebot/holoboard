/* -----------------------------------------------------------------------------
 * HoloBoard
 * I-Grebot
 * -----------------------------------------------------------------------------
 * @file    motion.h
 * @author  Pierrick
 * @date    2017/05/26
 * -----------------------------------------------------------------------------
 * @brief
 *    This file contains motion related definitions
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/holoboard.git
 * -----------------------------------------------------------------------------
 */

#ifndef _MOTION_H_
#define _MOTION_H_

#include <stdint.h>
#include <stdbool.h>

//#include "aversive.h"
//#include "pid.h"
//#include "biquad.h"
//#include "quadramp.h"
//#include "quadramp_derivate.h"
//#include "ramp.h"
#include "angle_distance.h"
//#include "blocking_detection_manager.h"
//#include "control_system_manager.h"
//#include "position_manager.h"
#include "robot_system.h"
//#include "trajectory_manager.h"

/**
********************************************************************************
**
**  Definitions
**
********************************************************************************
*/

// Number of waypoints that can be stored in the motion controller's FIFO
// We don't need to have a large amount since we feed it one after the other
#define MOTION_MAX_WP_IN_QUEUE    5U // Maximum amount of waypoints in the queue

// Pre-defined speeds
#define SPEED_FAST_D         1200L // For long motions only
#define SPEED_FAST_A          500L
#define SPEED_NORMAL_D        800L // Normal and moderate motions
#define SPEED_NORMAL_A        300L
#define SPEED_SLOW_D          400L // Actuator handling, narrow path or avoidance
#define SPEED_SLOW_A          200L
#define SPEED_VERY_SLOW_D     200L // Position reset for example
#define SPEED_VERY_SLOW_A     100L

/**
********************************************************************************
**
**  Types & Enumerations
**
********************************************************************************
*/

/* Control System manager structure
 * Contains all required control variables for the robot motion
 */

typedef struct
{

  /* General */
  int8_t cs_events;

  /* Positioning */
  struct robot_system rs;
//  struct robot_position pos;
//  struct trajectory traj;

  /* Control system in Angle */
//  struct cs cs_a;
 // struct pid_filter pid_a;
 // struct quadramp_filter qr_a;

  /* Control system in Distance */
//  struct cs cs_d;
//  struct pid_filter pid_d;
//  struct quadramp_filter qr_d;

  /* Blocking detection */
//  struct blocking_detection bd_l;
//  struct blocking_detection bd_r;

  /* Control variables */
  int32_t pwm_l;
  int32_t pwm_r;

  /* Speed */
  volatile int16_t speed_a;
  volatile int16_t speed_d;

  /* Acceleration */
  volatile int16_t acceleration_a;
  volatile int16_t acceleration_d;

} avs_cs_t;


/* Waypoint type (kind of trajectory) */
typedef enum
{
  WP_NO_MOTION, // isn't it explicit?
  WP_GOTO_AUTO,
  WP_GOTO_FWD,
  WP_GOTO_BWD,
  WP_MOVE_REL,
  WP_ORIENT_FRONT,
  WP_ORIENT_BEHIND
} wp_type_e;

/* Waypoint nominal speed */
typedef enum
{
  WP_SPEED_FAST ,
  WP_SPEED_NORMAL,
  WP_SPEED_SLOW,
  WP_SPEED_VERY_SLOW
} wp_speed_e;

// Coordinates of a POI + its orientation
// TODO: move me
typedef struct
{
  int16_t x;
  int16_t y;
  int16_t a;
} poi_t;

/* Waypoint definition */
typedef struct
{

  // Type of waypoint
  wp_type_e type;

  // Context-dependant offset to apply to the coordinates
  poi_t offset;

  // Related coordinates: Absolute or Relative
  struct {
    poi_t abs;
    struct {
      int16_t d;
      int16_t a;
    } rel;
  } coord;

  // Associated Vectorial and Angular speeds
  wp_speed_e speed;

  // Flags
  bool trajectory_must_finish;

} wp_t;


#endif /* _MOTION_H_ */
