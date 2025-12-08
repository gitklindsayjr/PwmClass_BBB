/*
 * Copyright (c) 2025, Kenneth Lindsay
 * All rights reserved.
 * Author: Kenneth Lindsay
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
 *      following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *      following disclaimer in the documentation and/or other materials provided with the distribution.
 *   3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/****************************************************************************************************
 * Pins are defined by: the /boot/dtbs/6.12.49-bone36/overlays/BB-EHRPWM1-P9_14-P9_16.dtbo
 * and the /boot/dtbs/6.12.49-bone36/overlays/BB-EHRPWM1-P9_14-P9_16.dtbo files.
 *
 * uname_r=6.12.49-bone36
 * To enable the the following pins edit the /boot/uEnv.txt as follows:
 * Remove comment from:
 * #enable_uboot_overlays=1
 * enable_uboot_overlays=1
 *
 * Add the overlays to the following section.
 *   Note: "uname_r=6.12.49-bone36" which is the correct overlay path for the current
 *          installed kernel versione.
 * ###Additional custom Cape section
 * uboot_overlay_addr4=BB-EHRPWM1-P9_14-P9_16.dtbo
 * uboot_overlay_addr5=BB-EHRPWM2-P8_13-P8_19.dtbo                                *
 *
 *                        ----------------------------------
 *                        |                                |
 *                        |        /dev/bone/pwm/1/a P9-14 |-->
 *                        |                                |
 *                        | 	   /dev/bone/pwm/1/b P9-16 |-->
 *                        |                                |
  *                       |        /dev/bone/pwm/2/a P8-19 |-->
 *                        |                                |
 *                        |        /dev/bone/pwm/2/b P8-13 |-->
 *                        |                                |
 *                        |                     gnd  P9-1  |-->
 *                        |                                |
 *                        |                     gnd  P9-2  |-->
 *                        |                                |
 *                        ----------------------------------
 *
 ***************************************************************************************************/

// Beaglebone Black Debian 13 2025-09-05 - am335x-debian-13-base-v6.12-armhf-2025-09-05-4gp.img.xz
// Beaglebone kernel version 6.12.49-bone36 Sep 25 19:56:57 UTC 2025 armv7l GNU/Linux

#ifndef BBB_PWM_H_
#define BBB_PWM_H_

#include <string>
#include "pwm_class.h"

using namespace std;

#define PWM_BONE_PATH     "/dev/bone/pwm"
#define PWM0_SYS_PATH     "/sys/class/pwm/pwmchip0"
#define PWM1_SYS_PATH     "/sys/class/pwm/pwmchip1"
#define NANO_SECONDS      1000000000

/* The pin list enumeration "pwm_pins" was based on the overlays mentioned above
 * to add. To add or delete pins update the enumeration list then add the pin details to the,
 * "pinsArray" initializer below. Each EHRPWMx uses a common period clock and are known as pins
 * A and B.  The order of these pins is defined by the "gpio_class.h/typedef struct pwm_pins_t"
 * In this order: unsigned pinId, unsigned sharedPin (A or B), string chipName, string   path,
 * string   pinName, and pwm_pin_state_t state. The state stores the period, pulse width, and the
 * run/enabled state.
 *
 * This class depends on the kernel using the "sysfs" driver to operate properly. These following
 * command line commands must be working properly, plus the overlays, and being a member of the pwm group.
 *
 * $ cd /dev/bone/pwm/1/a
 * $ ls
 * capture duty_cycle enable period polarity power uevent
 * $ echo 100000 > period
 * $ cat period
 * 100000
 * Alternative is
 * $ cd /sys/class/pwm /pwmchip0
 * device export npwm power pwm0 pwm1 subsystem uevent unexport
 * If pwm0 and pwm1 exists it means that the overlays have already exported our pins.
 * pwmchip0 & pwmchip1 are EHRPWM1 & EHRPWM2 respectively.
 */

enum pwm_      { pwm0=0, pwm1, PWM_DEVICES };
enum pwm_pins_ { NO_PIN, P9_14, P9_16, P8_19, P8_13, PWM_NUM_PINS };

class Pwm : public PwmClass
{
	private:
		// Don't change following const declarations linked to the defined enumerations above
		pwm_pins_t pinsArray[PWM_NUM_PINS] = { { NO_PIN, },
				{ P9_14, P9_16, "EHRPWM1", "/1/a", "P9_14", {0, }, },
				{ P9_16, P9_14, "EHRPWM1", "/1/b", "P9_16", {0, }, },
				{ P8_19, P8_13, "EHRPWM2", "/2/a", "P8_19", {0, }, },
				{ P8_13, P8_19, "EHRPWM2", "/2/b", "P8_13", {0, }, } };
		int    pwmConstructor(void);
	public:
		Pwm(void){ pwmConstructor(); }
		~Pwm(){;}
	};
#endif /* BBB_PWM_H_ */
