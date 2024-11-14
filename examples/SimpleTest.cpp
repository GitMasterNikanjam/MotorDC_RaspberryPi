// For Compile Command: mkdir -p ./bin && g++ -o ./bin/SimpleTest SimpleTest.cpp ../MotorDC.cpp -lbcm2835
// For Excute command: sudo ./bin/SimpleTest

// ################################################################################################

#include <iostream>
#include "../MotorDC.h"

using namespace std;

// ######################################################################################

MotorDC motorAZ;
MotorDC motorEL;

// ###################################################################################

void loop(void);

// ####################################################################################
// main:

int main(void)
{   
    // Initial bcm2835 library.
    if (!bcm2835_init())
        return false;

    motorAZ.parameters.CHANNEL_NUM = 0;
    motorAZ.parameters.CLOCK_DEVIDER = BCM2835_PWM_CLOCK_DIVIDER_32;
    motorAZ.parameters.DIR_PIN = 1;
    motorAZ.parameters.DIR_POL = 0;
    motorAZ.parameters.OFFSET_DUTY = 4;
    motorAZ.parameters.RANGE = 24000;

    motorEL.parameters = motorAZ.parameters;
    motorEL.parameters.DIR_PIN = 19;
    motorEL.parameters.CHANNEL_NUM = 1;


    if(!motorAZ.init() || !motorEL.init())
    {
        cout << "Error: MotorDC init failed." << endl;
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

        // motorAZ.setDutyCycle(dutyCycleInput);
        motorEL.setDutyCycle(dutyCycleInput);

        printf("Motor duty cycle value is: %f\n\n", motorEL.value.duty);
    }
}