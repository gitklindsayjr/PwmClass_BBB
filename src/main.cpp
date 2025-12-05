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

// Used Debian image am335x-debian-11.6-minimal-armhf-2023-01-02
// using kernel version 5.10.145-ti-r55

/* Warning: You can't change the period if you have used both pins on ehrpwm0, ehrpwm1, or ehrpwm2.  You will
 * need to reboot to change the period. Reverting back to a gpio pin does not allow period changing either.
*/

/****************************************************************************************************
 * Pins are defined by: the /boot/dtbs/6.12.49-bone36/overlays/BB-EHRPWM1-P9_14-P9_16.dtbo
 * and the /boot/dtbs/6.12.49-bone36/overlays/BB-EHRPWM1-P8_13-P8_19.dtbo files.
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

#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <get_line_class.h>
#include <BBB_Pwm.h>

#define NS                   1000000000    // 10^9
#define SERVO_FREQ           50            // 50 Hz
#define SERVO_PERIOD         20000000      // 20 ms
#define SERVO_MAX            2000000       // 2 ms
#define SERVO_ZERO           1500000       // 1.5 ms
#define SERVO_MIN            1000000       // 1.0 ms
#define SERVO_ZERO_STR       "1500000"
#define QUIT                 'q'
#define NEXT_PIN             'n'

using namespace std;

// Globals
Pwm *pwm;
GetLineClass *line;

int    setPeriod(unsigned pin);
int    setPulseWidth(unsigned pin);
void   setEnableState_Polarity(unsigned pin);
int    changePeriod_PulseWidth(unsigned pin);
bool   strToInt(string input, int *value);
string parseCommandLine(char *argv);

int main(int argc, char *argv[])
{
	int pin[PWM_NUM_PINS]= {0,};
	int error = NO_ERROR, setupRet =  NEXT_PIN;
	string pinName[PWM_NUM_PINS], input;
    unsigned nextPin, numPins;
    bool loop = true;

	pwm =  new Pwm();
	line = new GetLineClass();
    if((argc > 1) && (argc <= PWM_NUM_PINS))
	{
    	unsigned i;
		for(i=1; i<(unsigned)argc; i++)
		{
			pinName[i] = parseCommandLine(argv[i]);
			pin[i] = pwm->get_PinId(pinName[i]);
			if(pin[i] == NO_PIN)
			{
				if(pinName[i].front() != 'P')
		    	{
					cout << "Usage: gpio-test [--help] " << pwm->get_PinName(1) << " " << pwm->get_PinName(2) << " <...>" << endl;
		    		cout << "The pin names are those defined in the /boot/dtbs/6.12.49-bone36/overlays folder" << endl;
		    		cout << "Files BB-EHRPWM1-P9_14-P9_16.dtbo, and BB-EHRPWM2-P8_13-P8_19.dtbo" << endl;
		    		cout << "The order of the pins is not important. The max number of pins that can be listed is - " << PWM_NUM_PINS - 1 << "." << endl;
				}
				else
					cout << "ERROR: Not a valid pwm pin - " << pinName[i] << endl;
				goto exitTest;
			}
		    error = setPeriod(pin[i]);
		    error = setPulseWidth(pin[i]);
		    if(error < 0)
		    	goto exitTest;
		    setEnableState_Polarity(pin[i]);
		}
		numPins = i;
	}
    else
    {
    	numPins = 1;
		pin[1] = P9_14; // Default pin
		pinName[1] = pwm->get_PinName(pin[1]);
    	error = setPeriod(pin[1]);
    	error = setPulseWidth(1);
    	if(error < 0)
    		goto exitTest;
    	setEnableState_Polarity(1);
	}
    nextPin = 1; // Start with second pin
    while(loop)
    {
    	switch(setupRet)
    	{
    	case NEXT_PIN:
    		if(pin[nextPin] != 0) // If their is a pin assigned, increment nextPin
    			setupRet = changePeriod_PulseWidth(pin[nextPin]);
    		nextPin++;
    		if(nextPin >= numPins)
    			nextPin = 1;
    		break;
    	case QUIT:
    		cout << "Do you want to disable pins " << " (y/n) -- " << flush;
    		input = line->getLineBlock();
    	    if((input == "Y") || (input == "y"))
    	    {
   	    		for(unsigned i = 1; i<=numPins; i++)
   	    			pwm->setEnabledState(pin[i], DISABLED);
    	    }
    	    loop = false;
    		break;;
    	case ERROR:
    	case NO_ERROR:
    	default:
    		delete pwm;
    		return 0;
    	}
    }
exitTest:
	delete pwm;
    delete line;
    return 0;
}
int setPeriod(unsigned pin)
{
	int  period;
	bool ret;
	string input = "";
	line->flush();
	cout << "Input " << pwm->get_PinName(pin) << " period (nano seconds) -- " << flush;
	do
	{
		input = line->getLineBlock();
		ret = strToInt(input, &period);
	}
	while(!ret);
	if(pwm->setPeriod(pin, period) < 0)
		return ERROR;
	return NEXT_PIN;
}
int setPulseWidth(unsigned pin)
{
	int pulseWidth, ret;
	string input = "";
	cout << "Input " << pwm->get_PinName(pin) << " pulse width (nano seconds) -- " << flush;
	do
	{
		input = line->getLineBlock();
		ret = strToInt(input, &pulseWidth);
	}while(!ret);
	if(pwm->setPulseWidth(pin, pulseWidth) < 0)
		return ERROR;
	return NEXT_PIN;
}
void setEnableState_Polarity(unsigned pin)
{
	cout << pwm->get_PinName(pin) << ": Period = " << pwm->get_Period(pin) << " nano secs." << endl;
	cout << pwm->get_PinName(pin) << ": Pulse width = " << pwm->get_PulseWidth(pin) << " nano secs." << endl;
	pwm->setPolarity(pin, POS_PULSE);
	pwm->setEnabledState(pin, ENABLED);
	cout << "------------------------------------------------------------------" << endl;
}

int changePeriod_PulseWidth(unsigned pin)
{
	int period, pulseWidth;
	string input = "";
	pulseWidth = pwm->get_PulseWidth(pin);
	do
	{
		cout << "Input " << pwm->get_PinName(pin) << " period (nano seconds <ENTER> to skip) -- " << flush;
		input = line->getLineBlock();
		if(strToInt(input, &period))
		{
			if(period >= pulseWidth)
				pwm->setPeriod(pin, period);
			else
				cout << "ERROR: Period must be greater then pulse  width" << endl;
		}
		period = pwm->get_Period(pin);
		cout << pwm->get_PinName(pin) << ": Period is set to: " << period << "(ns)" << endl;
		cout << "Input " << pwm->get_PinName(pin) << " pulse width (nano seconds <ENTER> to skip, Q to quit) -- " << flush;
		input = line->getLineBlock();
		if(strToInt(input, &pulseWidth))
		{
			if(pulseWidth < period)
				pwm->setPulseWidth(pin, pulseWidth);
			else
				cout << "ERROR: Pulse width must be less then period" << endl;
		}
		if((input == "Q") || (input == "q"))
			return QUIT;
		cout << pwm->get_PinName(pin) << ": Period = " << pwm->get_Period(pin) << " nano secs." << endl;
		cout << pwm->get_PinName(pin) << ": Pulse Width = " << pwm->get_PulseWidth(pin) << " nano secs." << endl;

	}
	while(input != "");
  	return NEXT_PIN;
}
string parseCommandLine(char *argv)
{
	int  n=0;
	string pinName;
	while(argv[n] !=0)
	{
		argv[n] = toupper(argv[n]);
		n++;
	}
	pinName = argv;
	return pinName;
}
bool strToInt(string input, int *value)
{
	try
	{
		*value = stoi(input);
		return true;
	}
	catch(const std::invalid_argument &ia)
	{
		return false;
	}
}


