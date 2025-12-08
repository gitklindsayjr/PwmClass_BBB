#ifndef PWM_CLASS_H_
#define PWM_CLASS_H_
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

#include <string>
#include <vector>

using namespace std;

#define ERROR        -1
#define NO_ERROR     0
#define NANO_SECONDS      1000000000

typedef struct
{
	unsigned period;
	unsigned pulseWidth;
	unsigned polarity;
	unsigned enabled;
	bool     exists;
} pwm_pin_state_t;
typedef struct
{
	unsigned pinId;
	unsigned sharedPin;
	string   chipName;
	string   path;
	string   pinName;
	pwm_pin_state_t state;
} pwm_pins_t;

enum run_      { STOP=0, RUN=1 };
enum enabled_  { DISABLED = 0, ENABLED = 1 };
enum polarity_ { POS_PULSE=1, NEG_PULSE=0 };

class PwmClass
{
	private: // Helper methods to read/write /dev/bone/... paths
		int  writePath(string path, int n);
		int  writePath(string path, string s);
		int  readPath(string path);
		int  readPath(string path, string *str);
	public:
		PwmClass(void);
		unsigned numDevices;
		unsigned numPins;
		string   devPath;
		pwm_pins_t *pins;
		int  setPolarity(unsigned pinId, unsigned polarity);
		int  setPulseWidth(unsigned pinId, unsigned pulseWidth);
		int  setPeriod(unsigned pinId, unsigned period);
		int  setEnabledState(unsigned pinId, unsigned state);
		bool pinExists(unsigned pinId);
	public: // These gets return the information in the pwm_pins_t *pins array
		unsigned get_PinId(string pinName);
		string   get_PinPath(unsigned pinId);
		string   get_PinName(unsigned pinId);
		string   get_ChipName(unsigned pinId);
		unsigned get_Period(unsigned pinId);
		unsigned get_PulseWidth(unsigned pinId);
		unsigned get_Polarity(unsigned pinId);
		unsigned get_EnabledState(unsigned pinId);
		~PwmClass();
	protected:
		int  getPolarity(unsigned pinId);
		int  getPulseWidth(unsigned pinId);
		int  getPeriod(unsigned pinId);
		int  getEnabledState(unsigned pinId);
	private:
		string getDirPath(string dirPath, string findStr, unsigned *error);
};

#endif /* PWM_CLASS_H_ */
