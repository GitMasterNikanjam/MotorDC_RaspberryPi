// ##############################################################################################
// Include libraries:

#include "MotorDC.h"

// #################################################################################################

MotorDC::MotorDC()
{
 parameters.CHANNEL_NUM = -1;
 parameters.CLOCK_DEVIDER = BCM2835_PWM_CLOCK_DIVIDER_128;
 parameters.DIR_PIN = -1;
 parameters.OFFSET_DUTY = 0;
 parameters.RANGE = 1000;

 value.amp = 0;
 value.duty = 0;
 value.dir = 0;
 value.pwm = 0;

 _mapSlope = 1;

}

MotorDC::~MotorDC()
{
    setDutyCycle(0);
    clean();
}

bool MotorDC::init(void)
{
    if(_checkParameters() == false)
    {
        return false;
    }

    _mapSlope = (100.0 - parameters.OFFSET_DUTY) / 100.0;

    bcm2835_gpio_fsel(parameters.DIR_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(parameters.DIR_PIN, parameters.DIR_POL);

    if(parameters.CHANNEL_NUM == 0)
    {
        bcm2835_gpio_fsel(12, BCM2835_GPIO_FSEL_ALT0);
    }
    else if(parameters.CHANNEL_NUM == 1)
    {
        bcm2835_gpio_fsel(13, BCM2835_GPIO_FSEL_ALT0);
    }

    // Set pwm clock devider.
    bcm2835_pwm_set_clock(parameters.CLOCK_DEVIDER);
    
    // Set pwm mode.
    bcm2835_pwm_set_mode(parameters.CHANNEL_NUM, 1, 1);

    // Set Range value for pwm control.
    bcm2835_pwm_set_range(parameters.CHANNEL_NUM, parameters.RANGE); 

    // Set pwm or dutycycle value to 0.
    bcm2835_pwm_set_data(parameters.CHANNEL_NUM, 0);

    return true;
}

void MotorDC::setDutyCycle(float dutyCycleInput)
{
    float pwm_temp = (_mapSlope * abs(dutyCycleInput) + parameters.OFFSET_DUTY ) * (float)parameters.RANGE / 100.0;

    if(pwm_temp < 0)
    {
        value.pwm = 0;
    }  
    else if(pwm_temp >= parameters.RANGE)
    {
        value.pwm = parameters.RANGE;
    }
    else
    {
        value.pwm = pwm_temp;
    }

    uint8_t dir_temp = parameters.DIR_POL;

    if(dutyCycleInput > 0)
    {
        value.dir = 1;
    }
    else if(dutyCycleInput < 0)
    {
        value.dir = -1;
        dir_temp = !parameters.DIR_POL;
    }
    else
    {
        value.dir = 0;
        value.pwm = 0;
    }
    
    bcm2835_gpio_write(parameters.DIR_PIN, dir_temp);
    bcm2835_pwm_set_data(parameters.CHANNEL_NUM, value.pwm);
}

void MotorDC::clean(void)
{
    setDutyCycle(0);
    
    switch(parameters.CHANNEL_NUM)
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

    value.amp = 0;
    value.duty = 0;
    value.dir = 0;
    value.pwm = 0;
}

bool MotorDC::_checkParameters(void)
{
    bool state = true;

    if( !( (parameters.CHANNEL_NUM == 0) || (parameters.CHANNEL_NUM == 1) ) )
    {
        errorMessage = "Error MotorDC: Channel range not correct. please select channel 0 or 1.";
        state = false;
    }

    if(parameters.DIR_PIN > 30)
    {
        errorMessage = "Error MotorDC: Pin configuration for direction control is in wrong range.";
        state = false;
    }

    state = state && ( (parameters.CHANNEL_NUM == 0) || (parameters.CHANNEL_NUM == 1) ) &&
                 (parameters.DIR_PIN >= 0) &&
                 (parameters.DIR_POL <= 1) &&
                 (parameters.OFFSET_DUTY >= 0) &&
                 ((parameters.CLOCK_DEVIDER % 2 == 0) || (parameters.CLOCK_DEVIDER == 1)) &&
                 (parameters.CLOCK_DEVIDER <= 2048);

    if(state == false)
    {
        errorMessage = "Error MotorDC: One or some parameters are not correct.";
        return false;
    }

    return true;
}