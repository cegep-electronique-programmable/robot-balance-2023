#include "Arduino.h"
#include "StepperNB.h"

StepperNB::StepperNB(int pin_direction, int pin_step, int pin_ms1, int pin_ms2, int pin_ms3, int number_of_steps, bool direction_inverted)
{
    this->pin_direction = pin_direction;
    this->pin_step = pin_step;
    this->pin_ms1 = pin_ms1;
    this->pin_ms2 = pin_ms2;
    this->pin_ms3 = pin_ms3;
    this->number_of_steps = number_of_steps;
    this->direction_inverted = direction_inverted;

    this->direction = 0;
    this->ratio = 1;

    this->position_steps = 0;
    this->position_degrees = 0;

    this->target_speed_degrees_per_second = 0;
}

float StepperNB::getSpeed(void) {
    return this->target_speed_degrees_per_second;
}

void StepperNB::setSpeed(float target_speed_degrees_per_second)
{
    this->target_speed_degrees_per_second = target_speed_degrees_per_second;

    // Adujst direction
    if (this->target_speed_degrees_per_second > 0)
    {
        this->setDirection(0);
    }
    else
    {
        this->setDirection(1);
    }

    // TODO: adjust ratio according to speed range
    if (target_speed_degrees_per_second < 45 && target_speed_degrees_per_second > -45) {
        this->setRatio(16);
    }
    else if (target_speed_degrees_per_second < 90 && target_speed_degrees_per_second > -90) {
        this->setRatio(8);
    }
    else if (target_speed_degrees_per_second < 180 && target_speed_degrees_per_second > -180) {
        this->setRatio(4);
    }
    else if (target_speed_degrees_per_second < 360 && target_speed_degrees_per_second > -360) {
        this->setRatio(2);
    }
    else {
        this->setRatio(1);
    }

    // Compute timer period
    float step_per_second = abs(this->target_speed_degrees_per_second) / 360.0 * this->number_of_steps * this->ratio;
    int timer_period = 1000000.0 / step_per_second; // Nombre de micro secondes entre deux impulsions
    if (timer_period < 100) // saturation at 100 us
    {
        timer_period = 100;
    }

    
    
    this->timer_period = timer_period;
}

int StepperNB::getDirection(void)
{
    return this->direction;
}

void StepperNB::setDirection(int direction)
{
    this->direction = direction;
    if (this->direction_inverted)
    {
        digitalWrite(this->pin_direction, !direction);
    }
    else 
    {
        digitalWrite(this->pin_direction, direction);  
    }
    
    
}

void StepperNB::setRatio(int ratio)
{
    switch (ratio)
    {
    case 1:
        this->ratio = ratio;
        digitalWrite(this->pin_ms1, LOW);
        digitalWrite(this->pin_ms2, LOW);
        digitalWrite(this->pin_ms3, LOW);
        break;
    case 2:
        this->ratio = ratio;
        digitalWrite(this->pin_ms1, HIGH);
        digitalWrite(this->pin_ms2, LOW);
        digitalWrite(this->pin_ms3, LOW);
        break;
    case 4:
        this->ratio = ratio;
        digitalWrite(this->pin_ms1, LOW);
        digitalWrite(this->pin_ms2, HIGH);
        digitalWrite(this->pin_ms3, LOW);
        break;
    case 8:
        this->ratio = ratio;
        digitalWrite(this->pin_ms1, HIGH);
        digitalWrite(this->pin_ms2, HIGH);
        digitalWrite(this->pin_ms3, LOW);
        break;
    case 16:
        this->ratio = ratio;
        digitalWrite(this->pin_ms1, HIGH);
        digitalWrite(this->pin_ms2, HIGH);
        digitalWrite(this->pin_ms3, HIGH);
        break;
    default:
        break;
    }
}

int StepperNB::getRatio(void)
{
    return this->ratio;
}

uint64_t StepperNB::getTimerPeriod(void)
{
    
    return this->timer_period;
}

int StepperNB::version(void)
{
    return 0;
}