/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_PWM_H
#define AOS_PWM_H

typedef struct {
    float    duty_cycle;  /* the pwm duty_cycle */
    uint32_t freq;        /* the pwm freq */
} pwm_config_t;

typedef struct {
    uint8_t      port;    /* pwm port */
    pwm_config_t config;  /* spi config */
    void        *priv;    /* priv data */
} pwm_dev_t;

/**@brief Initialises a PWM pin
 *
 * @note  Prepares a Pulse-Width Modulation pin for use.
 * Does not start the PWM output (use @ref MicoPwmStart).
 *
 * @param     pwm    : the PWM device
 * @return    0      : on success.
 * @return    EIO    : if an error occurred with any step
 */
int32_t hal_pwm_init(pwm_dev_t *pwm);


/**@brief Starts PWM output on a PWM interface
 *
 * @note  Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param     pwm  : the PWM device
 * @return    0    : on success.
 * @return    EIO  : if an error occurred with any step
 */
int32_t hal_pwm_start(pwm_dev_t *pwm);


/**@brief Stops output on a PWM pin
 *
 * @note  Stops Pulse-Width Modulation signal output on a PWM pin
 *
 * @param     pwm : the PWM device
 * @return    0   : on success.
 * @return    EIO : if an error occurred with any step
 */
int32_t hal_pwm_stop(pwm_dev_t *pwm);

#endif
