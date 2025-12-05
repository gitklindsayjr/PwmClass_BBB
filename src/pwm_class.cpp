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
#include "pwm_class.h"
#include "BBB_Pwm.h"

PwmClass::PwmClass(void)
{
	pins = 0;
	numDevices = 0;
	numPins = 0;
}
PwmClass::~PwmClass()
{
}
unsigned PwmClass::get_PinId(string pinName)
{
	for(unsigned i=1; i<numPins; i++)
	{
		if(pinName == pins[i].pinName)
			return pins[i].pinId;
	}
	return 0;
}
string PwmClass::get_PinName(unsigned pin)
{
	string name;
	for(unsigned i=1; i<numPins; i++)
	{
		if(pin == pins[i].pinId)
			return pins[i].pinName;
	}
	return "";
}
string PwmClass::get_PinPath(unsigned pinId)
{
	string path = devPath + pins[pinId].pinName;
	return path;
}
string PwmClass::get_ChipName(unsigned pinId)
{
	string chip = pins[pinId].chipName;
	return chip;
}
unsigned PwmClass::get_Period(unsigned pinId)
{
	return pins[pinId].state.period;
}
unsigned PwmClass::get_PulseWidth(unsigned pinId)
{
	return pins[pinId].state.pulseWidth;
}
unsigned PwmClass::get_Polarity(unsigned pinId)
{
	return pins[pinId].state.polarity;
}
unsigned PwmClass::get_EnabledState(unsigned pinId)
{
	return pins[pinId].state.enabled;
}
bool PwmClass::pinExists(unsigned pinId)
{
    DIR *dir;
    string dirPath = devPath;
   	dirPath+= pins[pinId].path;
    dir = opendir((char *)dirPath.c_str());
    if(dir != NULL)
    {
        (void) closedir(dir);
        return true;
    }
    return false;
}
int PwmClass::setPolarity(unsigned pinId, unsigned polarity)
{
	string path;
	string strPolarity;
	path = devPath + pins[pinId].pinName +"/" + "polarity";
	if(polarity)
		strPolarity = "normal";
	else
		strPolarity = "inversed";
	if(writePath(path, strPolarity) == ERROR)
		return ERROR;
	// Check to see if polarity was set correctly
	if(getPolarity(pinId) != (int)polarity)
		return ERROR;
	return NO_ERROR;
}
int PwmClass::getPolarity(unsigned pinId)
{
	string path = devPath;
	unsigned input = 0;
	string polarity;
	path+= pins[pinId].path;
	path+= "/polarity";
	input = readPath(path, &polarity);

	if(polarity == "normal")
		input = POS_PULSE;
	else if(polarity == "inversed")
		input = NEG_PULSE;
	else
		return ERROR;
	pins[pinId].state.polarity = input;
	return NO_ERROR;
}
int PwmClass::setPulseWidth(unsigned pinId, unsigned pulseWidth)
{
	string path = devPath;
	unsigned period = get_Period(pinId);
	if(pulseWidth >= period || pulseWidth == 0)
	{
		cerr << "ERROR: Pulse width must be < " <<  period << "(ns)" << endl;
		return ERROR;
	}
	path+= pins[pinId].path + "/duty_cycle";
	if(writePath(path, pulseWidth) == ERROR)
		return ERROR;
	if(getPulseWidth(pinId) < 0)
		return ERROR;
	return NO_ERROR;
}
int PwmClass::getPulseWidth(unsigned pinId)
{
	string path = devPath;
	int input;
	path+= pins[pinId].path + "/duty_cycle";
	input = readPath(path);
	if(input < 0)
		return ERROR;
	pins[pinId].state.pulseWidth = input;
	return input;
}
int PwmClass::setPeriod(unsigned pinId, unsigned period)
{
	string path = devPath;
	unsigned sharedPeriod;
	unsigned sharedPin;

	usleep(500000);
	sharedPin = pins[pinId].sharedPin;
	sharedPeriod = get_Period(sharedPin);
	if((sharedPeriod == 0) || (sharedPeriod == period))
	{ // Ok to set the period for this pin shared pin not set
		if(period < 100)
		{
			cerr << "ERROR: PWM period less then 100ns" << endl;
			return ERROR;
		}
		path+= pins[pinId].path +"/period";
		if(writePath(path, period) == ERROR)
			return ERROR;
	}
	else
	{   // Can't change the period both need to be the same
			cerr << "ERROR: PWM shared pin must be 0 or same as -- " << sharedPeriod << " reboot to set to 0" << endl;
			return ERROR;
	}
	if(getPeriod(pinId) != (int)period)
		return ERROR;
	return NO_ERROR;
}
int PwmClass::getPeriod(unsigned pinId)
{
	string path = devPath;
	unsigned input;
	path+= pins[pinId].path +"/" + "period";
	input = readPath(path);
	if(input < 0)
		return ERROR;
	pins[pinId].state.period = input;
	return input;
}

int PwmClass::setEnabledState(unsigned pinId, unsigned state)
{
	string path = devPath;
	unsigned input, trys = 0;
	path+= pins[pinId].path +"/enable";
	do
	{   // Loop here until set
		if(writePath(path, state) == ERROR)
			return ERROR;
		usleep(10000);  // Wait for kernel
		trys++; // Allow 5 trys
		if(trys > 4)
			return ERROR;
		input = getEnabledState(pinId);
	}
	while(input != state);
	return input;
}
int PwmClass::getEnabledState(unsigned pinId)
{
	string path = devPath;
	unsigned input;
	path+= pins[pinId].path +"/" + "enable";
	input = readPath(path);
	if(input < 0)
		return ERROR;
	pins[pinId].state.enabled = ENABLED;
	return NO_ERROR;
}

int PwmClass::readPath(string path)
{
	int   input;
	ifstream in;
	int   errorFlag = ERROR;
	in.open(path.c_str());
	if(in.fail())
		return errorFlag;
	in >> input;
	in.close();
	return input;
}
int PwmClass::readPath(string path, string *str)
{
	ifstream in;
	int   errorFlag = ERROR;
	in.open(path.c_str());
	if(in.fail())
		return errorFlag;
	in >> *str;
	in.close();
	return 0;
}
int PwmClass::writePath(string path, int n)
{
	ofstream out;
	out.open(path.c_str());
	if(out.fail())
		return ERROR;
	out << n;
	out.close();
	return false;
}
int PwmClass::writePath(string path, string s)
{
	ofstream out;
	out.open(path.c_str());
	if(out.fail())
		return ERROR;
	out << s;
	out.close();
	return false;
}
string PwmClass::getDirPath(string dirPath, string findStr, unsigned *error)
{
    vector<string> files = vector<string>();
    string subDirName = "";

    DIR *dir;
    struct dirent *dirp;
    size_t found;
    dir = opendir((char *)dirPath.c_str());
    *error = NO_ERROR;
   	if(!dir)
   		return subDirName;
   	while((dirp = readdir(dir)) != NULL)
   		files.push_back(string(dirp->d_name));
   	closedir(dir);
   	for (unsigned int i = 0; i < files.size(); i++)
   	{
   		subDirName = files[i];
   		found = subDirName.find(findStr);
   		if(found!=string::npos)
   			return subDirName;
   	}
   	*error = ERROR;
    return "";
}


