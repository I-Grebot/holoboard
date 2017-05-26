#ifndef _ANGLE_DISTANCE_H_
#define _ANGLE_DISTANCE_H_

#include "main.h"

struct rs_wheels {
	int32_t wheel_1;
	int32_t wheel_2;
	int32_t wheel_3;
};


struct rs_polar {
	int32_t distance_x;
	int32_t distance_y;
	int32_t angle;
};

/**
 * convert the values of wheels encoders (left, right) into (distance,
 * angle)
 */
void rs_get_polar_from_wheels(struct rs_polar * p_dst, struct rs_wheels * w_src);

/**
 * convert (distance, angle) into (left, right)
 */
void rs_get_wheels_from_polar(struct rs_wheels * w_dst, struct rs_polar * p_src);

#endif
