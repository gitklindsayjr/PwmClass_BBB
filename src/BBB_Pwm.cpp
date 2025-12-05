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
 * dtb_overlay=/boot/dtbs/6.12.49-bone36/overlays/BB-EHRPWM1-P9_14-P9_16.dtbo
 * dtb_overlay=/boot/dtbs/6.12.49-bone36/overlays/BB-EHRPWM2-P8_13-P8_19.dtbo                                *
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

#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "BBB_Pwm.h"
#include "pwm_class.h"

int Pwm::pwmConstructor(void)
{
	// Initialize the device paths
	int polarity, period, pulseWidth, enabled;
	numPins = PWM_NUM_PINS;
	numDevices = PWM_DEVICES;
	devPath = PWM_BONE_PATH;
	pins = pinsArray;
	for(unsigned i=1; i<PWM_NUM_PINS; i++)
	{   // Search for a match in our pins array
		if((i != pins[i].pinId))
		{
			cerr << "ERROR: Pin enumerations are out of order" << endl;
			return ERROR;
		}
		if(pinExists(i))
		{
			polarity = getPolarity(i);
			period = getPeriod(i);
			pulseWidth = getPulseWidth(i);
			enabled = getEnabledState(i);
			if((polarity < 0) || (period < 0) || (pulseWidth < 0) || (enabled < 0))
				cerr << "ERROR: Failed to initialize pin " << pins[i].pinName << endl;
		}
	}
	return false;
}
