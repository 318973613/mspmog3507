#include "pid.h"

static float pid_sanitize_float(float value)
{
    if (value != value) {
        return 0.0f;
    }
    return value;
}

void pid_init(PID_TypeDef *PID, float kp, float ki, float kd)
{
    PID->kp = kp;
    PID->ki = ki;
    PID->kd = kd;
    PID->target = 0.0f;
    PID->error0 = 0.0f;
    PID->error1 = 0.0f;
    PID->error2 = 0.0f;
    PID->errorint = 0.0f;
}

void pid_changeTarget(PID_TypeDef *PID, float target)
{
    PID->target = pid_sanitize_float(target);
}

float pid_compute(PID_TypeDef *PID, float actual)
{
    actual = pid_sanitize_float(actual);
    PID->error0 = PID->target - actual;
    PID->errorint += PID->error0;

    if (PID->errorint > 1000.0f) {
        PID->errorint = 1000.0f;
    }
    if (PID->errorint < -1000.0f) {
        PID->errorint = -1000.0f;
    }

    float COp = PID->kp * PID->error0;
    float COi = PID->ki * PID->errorint;
    float COd = PID->kd * (PID->error0 - PID->error1);
    float CO = pid_sanitize_float(COp + COi + COd);

    PID->error1 = PID->error0;

    if (CO > 1000.0f) {
        CO = 1000.0f;
    }
    if (CO < -1000.0f) {
        CO = -1000.0f;
    }
    return CO;
}

float pid_angle(PID_TypeDef *PID, float actual)
{
    actual = pid_sanitize_float(actual);
    PID->error0 = PID->target - actual;
    PID->errorint += PID->error0;

    if (PID->errorint > 1000.0f) {
        PID->errorint = 1000.0f;
    }
    if (PID->errorint < -1000.0f) {
        PID->errorint = -1000.0f;
    }

    float COp = PID->kp * PID->error0;
    float COi = PID->ki * PID->errorint;
    float COd = PID->kd * (PID->error0 - PID->error1);
    float CO = pid_sanitize_float(COp + COi + COd);

    PID->error1 = PID->error0;

    if (CO > 20.0f) {
        CO = 20.0f;
    }
    if (CO < -20.0f) {
        CO = -20.0f;
    }
    return CO;
}

float pid_duoji(PID_TypeDef *PID, float actual)
{
    actual = pid_sanitize_float(actual);
    PID->error2 = PID->error1;
    PID->error1 = PID->error0;
    PID->error0 = PID->target - actual;

    float COp = PID->kp * (PID->error0 - PID->error1);
    float COi = PID->ki * PID->error0;
    float COd = PID->kd * (PID->error0 - 2.0f * PID->error1 + PID->error2);

    return pid_sanitize_float(COp + COi + COd);
}

void App_duoji_angle(PID_TypeDef *PID, float Omega)
{
    pid_changeTarget(PID, Omega);
}
