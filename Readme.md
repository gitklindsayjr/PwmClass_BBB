This software was created for imbedding in applications that require the use of the PWM interface on Debian based Linux SOC's. 
The Beaglebone Black was the SOC for which this Class was initially developed. The Linux enviroment is:<br>
image -- Beaglebone Black Debian 13 2025-09-05 - am335c-debian-13-base-v6.12-armhf-2025-09-05-4gp.img.xz<br>
kernel -- version 6.12.49-bone36 Sep 25 19:56:57 UTC 2025 armv7l GNU/Linux<br>

This software expects that the following pins:<br>
P9_14, P9_16, P8_19, P8_13 be mapped to /dev/bone/pwm/1/a&b and /dev/bone/pwm/2/a/b by adding the 2 overlays.<br>
Are located at /boot/dtbs/6.12.49-bone36/overlays<br>
BB-EHRPWM1-P9_14-P9_16.dtbo and BB-EHRPWM1-P9_14-P9_16.dtbo files.<br>

Edit the following information to your /boot/uEnv.txt file<br>
$ sudo nano /boot/uEnv.txt<br>
Remove comment from:<br>
enable_uboot_overlays=1<br>
 
Add the overlays to the following section. ###Additional custom Cape section<br>
uboot_overlay_addr4=BB-EHRPWM1-P9_14-P9_16.dtbo<br>
uboot_overlay_addr5=BB-EHRPWM2-P8_13-P8_19.dtbo<br>

The include file that holds the PWM Pin information is "BBB_Pwm.h" in a array named "pinsArray".
The pin list enumeration "pwm_pins" was based on the overlays mentioned above. To add (requires a new overlay)
orr delete pins update the enumeration list then add the pin details to the, "pinsArray" initializer below.

Each EHRPWMx uses a common period clock and are known as pins "a" and "b". The order of these pins is defined by
the "gpio_class.h/typedef struct pwm_pins_t"<br>
In this order: unsigned pinId, unsigned sharedPin (A or B), string chipName, string path, string pinName,
and pwm_pin_state_t state. The state stores the period, pulse width, and the run/enabled state.

This class depends on the kernel using the "sysfs" driver to operate properly. These following
command line commands must be working properly, plus the overlays added to the /boot/uEnv.txt file, and being a member
of the pwm group.

If not installed install the following:<br>
$ sudo apt update<br>
$ sudo apt install build-essential<br>
$ sudo apt install g++ gcc cmake<br>
$ sudo apt install git

Initial development was cross compiled from a Ubuntu 20.04 PC.<br>

Install software on the Beaglebone Black:<br>
$ git clone http://github.com/gitklindsayjr/PwmClass_BBB

The following should be the result of the clone operation<br>
$ cd PwmClass_BBB<br>
$ ls<br>
CMakeLists.txt src<br>
$ ls src<br>
BBB_Pwm.cpp BBB_Pwm.h pwm_class.cpp pwm_class.h main.cpp<br>

Begin the build process:<br>
$ mkdir build<br>
$ cmake -DNATIVE=1 ..<br>
$ cmake --build . -- VERBOSE=1<br>

Build process results:<br>
$ ls<br>
CMakeCache.txt CMakeFiles Makefile cmake_install.cmake pwm-test

The executable is "pwm-test" which uses the pins P9_14 and P9_16 as /dev/bone/pwm/1a & b respecitvely. Also
pins P8_19 and P8_13 as /dev/bone/pwm2/a & b respectively.

Example command line use, all pins the default is to specify no pins in which P9_14 is used:<br>
$ ./gpiod-test P9_14 P9_16 P8_19 P8_13 
	
