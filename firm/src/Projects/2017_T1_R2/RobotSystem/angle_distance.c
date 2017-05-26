#include "angle_distance.h"

/**
 * convert the values of wheels encoders (1 to 3) into (distance x, distance y,
 * angle)
 */
void rs_get_polar_from_wheels(struct rs_polar *p_dst, struct rs_wheels *w_src)
{
	p_dst->distance_x = (int32_t)(w_src->wheel_1*-0.4553 + w_src->wheel_2*0.3333 + w_src->wheel_3*0.1220);
	p_dst->distance_y = (int32_t)(w_src->wheel_1*-0.1220 + w_src->wheel_2*-0.3333 + w_src->wheel_3*0.4553);
    p_dst->angle = (int32_t)(w_src->wheel_1*-0.028 + w_src->wheel_2*-0.028 + w_src->wheel_3*-0.028);
}

/**
 * convert (distance x, distance y, angle) into wheels encoders (1 to 3)
 */
void rs_get_wheels_from_polar(struct rs_wheels *w_dst, struct rs_polar *p_src)
{
	w_dst->wheel_1 = (int32_t)(p_src->distance_x*-1.366 + p_src->distance_y*-0.366 + p_src->angle*-12.0115);
	w_dst->wheel_2 = (int32_t)(p_src->distance_x*1 + p_src->distance_y*-1 + p_src->angle*-12.0115);
	w_dst->wheel_3 = (int32_t)(p_src->distance_x*0.366 + p_src->distance_y*1.366 + p_src->angle*-12.0115);
}
