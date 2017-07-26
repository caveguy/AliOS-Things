/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file hal/soc/pwm.h
 * @brief PWM HAL
 * @version since 5.5.0
 */

#ifndef YOS_PWM_H
#define YOS_PWM_H


/**@brief Initialises a PWM pin
 *
 * @note  Prepares a Pulse-Width Modulation pin for use.
 * Does not start the PWM output (use @ref MicoPwmStart).
 *
 * @param pwm        : the PWM interface which should be initialised
 * @param frequency  : Output signal frequency in Hertz
 * @param duty_cycle : Duty cycle of signal as a floating-point percentage (0.0 to 100.0)
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
int32_t hal_pwm_init(uint8_t pwm, uint32_t freq, float duty_cycle);


/**@brief Starts PWM output on a PWM interface
 *
 * @note  Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param pwm        : the PWM interface which should be started
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
int32_t hal_pwm_start(uint8_t pwm);


/**@brief Stops output on a PWM pin
 *
 * @note  Stops Pulse-Width Modulation signal output on a PWM pin
 *
 * @param pwm        : the PWM interface which should be stopped
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
int32_t hal_pwm_stop(uint8_t pwm);

/** @} */
/** @} */

#endif
