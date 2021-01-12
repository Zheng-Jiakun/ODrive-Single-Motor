#include "algorithm.h"

static void abs_limit(float *a, float ABS_MAX)
{
    if (*a > ABS_MAX)
        *a = ABS_MAX;
    if (*a < -ABS_MAX)
        *a = -ABS_MAX;
}

void pid_init(pid_t *pid, float min_out, float max_out, float intergral_limit,
              float kp, float ki, float kd)
{
    pid->integral_limit = intergral_limit;
    pid->min_output = min_out;
    pid->max_output = max_out;

    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
}

float pid_calc(pid_t *pid, float get, float set)
{
    pid->get = get;
    pid->set = set;
    pid->err[NOW] = set - get;

    pid->pout = pid->p * pid->err[NOW];
    pid->iout += pid->i * pid->err[NOW];
    pid->dout = pid->d * (pid->err[NOW] - pid->err[LAST]);

    abs_limit(&(pid->iout), pid->integral_limit);
    pid->out = pid->pout + pid->iout + pid->dout;

    if (pid->out > pid->max_output)
        pid->out = pid->max_output;
    else if (pid->out < pid->min_output)
        pid->out = pid->min_output;

    pid->err[LAST] = pid->err[NOW];

    return pid->out;
}

void pid_reset(pid_t *pid, float kp, float ki, float kd)
{
    pid->p = kp;
    pid->i = ki;
    pid->d = kd;

    pid->pout = 0;
    pid->iout = 0;
    pid->dout = 0;
    pid->out = 0;
}
