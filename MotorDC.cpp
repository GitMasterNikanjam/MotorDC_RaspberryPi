#include "MotorDC.h"


MotorDC::MotorDC(uint8_t channel, uint32_t divisor, uint32_t range, uint8_t dir_pin, uint8_t dir_pol)
{
   _channel = channel;
   _divisor = divisor;
   parameters.RANGE = range;
   parameters.DIR_PIN = dir_pin;
   
   switch(dir_pol)
   {
    case 0:
        parameters.DIR_POL = 0;
    break;
    default:
       parameters. DIR_POL = 1;
   }
}

bool MotorDC::begin(void)
{
    if( !( (_channel == 0) || (_channel == 1) ) )
    {
        errorMessage = "Error MotorDC object. Channel range not correct. please select channel 0 or 1.";
        return false;
    }

    if(parameters.DIR_PIN > 30)
    {
        errorMessage = "Error MotorDC object. pin configuration for direction control is in wrong range.";
        return false;
    }
    bcm2835_gpio_fsel(parameters.DIR_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(parameters.DIR_PIN, parameters.DIR_POL);

    if(_channel == 0)
    {
        bcm2835_gpio_fsel(12, BCM2835_GPIO_FSEL_ALT0);
    }
    else if(_channel == 1)
    {
        bcm2835_gpio_fsel(13, BCM2835_GPIO_FSEL_ALT0);
    }

    // Set pwm clock devider.
    bcm2835_pwm_set_clock(_divisor);
    
    // Set pwm mode.
    bcm2835_pwm_set_mode(_channel, 1, 1);

    // Set Range value for pwm control.
    bcm2835_pwm_set_range(_channel, parameters.RANGE); 

    // Set pwm or dutycycle value to 0.
    bcm2835_pwm_set_data(_channel, 0);

    return true;
}

void MotorDC::control(float dutyCycleInput)
{
    if(dutyCycleInput >= 0)
    {
        dir = HIGH;
    }
    else
    {
        dir = LOW;
    }

    float pwm_temp = ( abs(dutyCycleInput) - _dutyOffset ) * parameters.RANGE / 100.0;

    if(pwm_temp < 0)
        pwm = 0;

    if(pwm_temp >= parameters.RANGE)
        pwm = parameters.RANGE;

    bcm2835_gpio_write(parameters.DIR_PIN, dir);
    bcm2835_pwm_set_data(_channel, pwm);
}

void MotorDC::clean(void)
{
    switch(_channel)
    {
        case 0:
            bcm2835_gpio_fsel(12, BCM2835_GPIO_FSEL_INPT);
            bcm2835_gpio_set_pud(12, BCM2835_GPIO_PUD_OFF);
        break;
        case 1:
            bcm2835_gpio_fsel(13, BCM2835_GPIO_FSEL_INPT);
            bcm2835_gpio_set_pud(13, BCM2835_GPIO_PUD_OFF);
        break;
    }

    bcm2835_gpio_fsel(parameters.DIR_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(parameters.DIR_PIN, BCM2835_GPIO_PUD_OFF);
}