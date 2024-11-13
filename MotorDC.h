#ifndef MOTOR_DC_H
#define MOTOR_DC_H

// ###############################################################################
// Include libraries:

#include <iostream>
#include <bcm2835.h>

// ##############################################################################

/**
 * @class MotorDC
 *  */
class MotorDC
{
    public:

        /// @brief Last error accured for object.
        std::string errorMessage;

        /**
         * @struct ParametersStructure
         * @brief Parameters structure.
         */
        struct ParametersStructure
        {
            /**
             * @brief The pwm value range for bcm2835_pwm_set_range() function.
             * @note - PWM_frequency = BASE_CPU_CLOCK / (PWM_CLOCK_DIVIDER * PWM_RANGE)
             */
            uint32_t RANGE;

            /**
             * @brief The GPIO pin number for direction control of DC motor driver.
             */
            int8_t DIR_PIN;

            /**
             * @brief The polaririty of input pwm pulse direction. 0: normal, 1: reverse.
             */
            uint8_t DIR_POL;

            /**
             * @brief The pwm channel number for raspberryPi. It can be 0 or 1. 
             * @note - Channel 0 assigned to pin GPIO 12. 
             * 
             * @note - Channel 1 assigned to pin GPIO 13.
             *  */ 
            int8_t CHANNEL_NUM;

            /**
             * @brief The PWM clock divider can be one of the values in the bcm2835PWMClockDivider enum. 
             * @note - Base CPU clock frequency depends on kernel settings. 
             * 
             * @note - PWM_frequency = BASE_CPU_CLOCK / (PWM_CLOCK_DIVIDER * PWM_RANGE)
             */
            uint32_t CLOCK_DEVIDER;

            /**
             * @brief Offset or deadzone value for duty cycle. [%].
             */
            float OFFSET_DUTY;

        }parameters;

        struct ValuesStructure
        {
            /**
             * @brief Motors Amper consumption. [mA] 
             */
            float amp;  

            /**
             * @brief Motor PWM Value. [0, MOTOR_PWM_RANGE]
             *  */                          
            uint32_t pwm;   

            /**
             * @brief Motor Duty Cycle Value. [-100, 100] %
             *  */                    
            float duty;  

            /**
             * @brief Motor Rotation direction. 1:normal , -1:reverse, 0:stop
             *  */                              
            int8_t dir;                         
        }value;

        /** 
         * @brief Default constructor. Init some varibles and parameters.
        */ 
        MotorDC();

        /**
         * @brief Destructor.
         */
        ~MotorDC();

        /**
         * @brief Init object. Check parameters. 
         */
        bool init(void);

        /**
         * @brief Clean setting on hardware. Stop MotorDC action.
         *  */ 
        void clean(void);

        /**
         * @brief Set duty cycle of motor. [%]
         * @note Its value can be at range: [-100, 100]
         */
        void setDutyCycle(float dutyCycleInput);

    private:
        
        /**
         * @brief Check parameters validations.
         * @return true if succeeded.
         */
        bool _checkParameters(void);

        /**
         * @brief The value gain for direction command correction. It depends on DIR_POL parameters.
         */
        int8_t _dirFactor;

        float _mapSlope;
};

#endif  // MOTOR_DC_H



