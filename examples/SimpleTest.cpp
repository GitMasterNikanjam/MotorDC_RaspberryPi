// For Compile Command: mkdir -p ./bin && g++ -o ./bin/SimpleTest SimpleTest.cpp ../MotorDC.cpp -lbcm2835
// For Excute command: sudo ./bin/SimpleTest

// ################################################################################################

#include <iostream>
#include "../MotorDC.h"

using namespace std;

// ######################################################################################

MotorDC motor;

// ###################################################################################

void loop(void);

// ####################################################################################
// main:

int main(void)
{   
    // Initial bcm2835 library.
    if (!bcm2835_init())
        return false;

    motor.parameters.CHANNEL_NUM = 1;
    motor.parameters.CLOCK_DEVIDER = BCM2835_PWM_CLOCK_DIVIDER_32;
    motor.parameters.DIR_PIN = 19; // 1;
    motor.parameters.DIR_POL = 1;
    motor.parameters.OFFSET_DUTY = 4;
    motor.parameters.RANGE = 24000;

    if(!motor.init())
    {
        cout << motor.errorMessage << endl;
        while(1);
    }

    cout << "Initialization was succeeded!" << endl;

    loop();

    bcm2835_close();
    return 0;
}

void loop(void)
{
    float dutyCycleInput = 0;

    while (1)
    {
        printf("Pleas input duty cycle [%]: ");
        cin>> dutyCycleInput;

        motor.setDutyCycle(dutyCycleInput);

        printf("pwmOutput value is: %d\n\n", motor.value.pwm);
    }
}