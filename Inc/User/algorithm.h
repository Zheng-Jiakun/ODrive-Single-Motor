#ifndef ALGORITHM
#define ALGORITHM

#include "main.h"

enum
{
    LAST = 0,
    NOW = 1,
};

typedef struct
{
    float p;
    float i;
    float d;

    float set;
    float get;
    float err[2];

    float pout;
    float iout;
    float dout;

    float out;

    float min_output;
    float max_output;

    float integral_limit;

} pid_t;

void pid_init(pid_t *pid, float min_out, float max_out, float intergral_limit, float kp, float ki, float kd);

void pid_reset(pid_t *pid, float kp, float ki, float kd);

float pid_calculate(pid_t *pid, float get, float set);

#endif
