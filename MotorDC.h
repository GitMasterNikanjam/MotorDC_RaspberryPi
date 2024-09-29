#ifndef _MOTOR_DC_H_
#define _MOTOR_DC_H_

    #include <iostream>
    #include <bcm2835.h>

    // MotorDC object.
    class MotorDC
    {
        public:

            struct Parameters
            {
                uint32_t RANGE;
                uint8_t DIR_PIN;
                uint8_t DIR_POL = 0;
            }parameters;

            float amp;                          // Motors Amper consumption. [mA]  
            uint32_t pwm;                       // Motor PWM Value. [0, MOTOR_PWM_RANGE]
            float duty;                         // Motor Duty Cycle Value. [-100, 100] %       
            int8_t dir;                         // Motor Rotation direction. 1:CW, -1:CCW
            


            std::string errorMessage;

            /** 
             * @brief object constructor. set pwm channel, pwm clock divisor, input range, direction control pin, direction polarity.
             * @param channel: can be 0 or 1. channel 0 assigned to pin GPIO 12. channel 1 assigned to pin GPIO 13.
             * @param divisor: is pwm clock divider. can be in bcm2835PWMClockDivider enum values. Base cpu clock frequency depends on kernel settings. 
             * PWM_frequency = BASE_CPU_CLOCK / (MOTOR_PWM_CLOCK_DIVIDER * MOTOR_PWM_RANGE)
             * @param range: is the range value for pwm control input value.
             * @param dir_pin: is the gpio pin number for motor direction control by motor driver.
             * @param dir_pol: is the motor direction polarity. CW/CCW is depends on hardware wiring configuration. 
            */ 
            MotorDC(uint8_t channel, uint32_t divisor, uint32_t range, uint8_t dir_pin, uint8_t dir_pol = 0);

            bool begin(void);

            // Clean setting on hardware. Stop MotorDC action.
            void clean(void);

            void control(float dutyCycleInput);

        private:
            
            uint8_t _channel;

            uint32_t _divisor;

            float _dutyOffset;
    };


#endif  // _MOTOR_DC_H_



