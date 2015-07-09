# Framework for Playstation Move on PC (Windows) #

## Project Description ##
The Playstation Move is a very good motion tracking device with it's glowing bulb and precise sensors. But unfortunately Sony made no SDK to directly use it with PC. You can connect the PC to the PS3 though (if you buy their program), and access the tracking data. But maybe there are people, who do not own a PS3 or the Move.Me but want precise real-time motion tracking.

This framework is for these people.

You are not a programmer? No problem, just use the JoyEmu and play games with MoveFramework.

With the Calibration Tool you can calibrate your device, so the framework can track it very accurate. You can test it as well with Calibration Tool.

With the Move Framework, you can integrate all the possibilities of motion tracking in your programs and games!

It is developed in C++.

There are SDK's for C++ and C# developers too! :)

## Licensing ##
The license of the framework is GPLv2, so you may only use the dll, the source or any other part of the framework, if the derived work is also free and comes with source code too.

If you want to use it any other way than this, please contact me for the commercial version.

## Requests ##
I'm sorry, but I don't have any free time to fullfill them, so you are on your own. But if you are programmer, you can extend or improve it any way you want.

## Videos ##
[A video was uploaded by tcboy88 that shows the calibration tool (an older version) while tracking.](http://www.youtube.com/watch?v=VeJ0n0hY1OQ)

[An other video by mudemoniac playing Most Wanted with the JoyEmu tool.](http://www.youtube.com/watch?v=VXXItLS0cU4)

## Donation ##
Please donate if this project was helpful or you want to support future developing! Any amount is welcome and deeply appreciated!

[![](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=3BFPJXWVPGEHW)

## Supporters ##
Peter Giokaris

**Thank you!**

## Features ##
  * Joystick emulation! PPJoy needed, check the download section!
  * Supports the native BT stack and MotionInJoy too
    * Native BT stack:
      * You can use other BT devices with a single BT adapter
      * Supports multiple Moves
      * Complicated pairing process with random success
      * Once you paired, no bugs, no errors
      * Instructions: PairingConnectingInstructions
    * MotionInJoy
      * You have to have 2 BT adapters in order to use other BT devices
      * Doesn't work with multiple Moves
      * Easy to set up
      * Supports the Navigation controller
      * The driver may contain bugs
      * Instructions: MotionInJoySetup
  * Orientation tracking and position tracking can be used independently
  * Fast and automatic bulb tracking with fairly good accuracy
    * Sub-pixel precision
    * Robust masking, it only processes a small part of the pixels (0.05%)
    * Also works for half hidden bulb!
    * No external library is used, no whole image operations
    * Reset the position by pressing the PS button for 2 seconds.
  * Calibrated orientation tracking using [the algorithm of Sebastian Madgwick](http://www.x-io.co.uk/node/8)
    * The algorithm is better than a normal Kalman-filter
    * You can turn the use of magnetic sensors on or off (press M)
    * Reset the orientation with the PS button while pointing to the monitor with the device, so the orientation will be tracked relative to your monitor
    * You can apply a prediction filter to smooth the jitter without drift
  * Calibration Tool that uses the calibration data that is hard-coded in the device (for the accelerometer and gyroscope) + additional motion data to calibrate the magnetic sensors
  * Automatic bulb color calibration
    * You can turn it off to set the colors manually
  * Automatic calibration of the accelerometers and gyroscopes
  * Navigation Controller support (MIJ only)
  * Easy to use framework
  * You can track multiple Moves!
  * Clean architecture, so you can modify it easily
  * Built in pairing tool (using instructions: CalibrationTool)
  * Config file to set parameters without a rebuild

## Prerequisites ##
A PS Eye camera must be connected to your computer if you want position tracking.

You also need the CL Eye driver to be able to use the camera on Windows. (You can also use it as a webcam with that driver. :))

[CL Eye driver](http://codelaboratories.com/products/eye/driver/)

MotionInJoy is supported again, because the creator of MIJ fixed all the issues why I didn't support it! Thank you! More information: MotionInJoySetup

Or if you don't want to use it, remove it to be able to use the framework with native BT stack.
[Instructions here](http://www.motioninjoy.com/help/how-uninstall-motioninjoy)
And you can try the native pairing process. PairingConnectingInstructions

To be able to compile and run the calibration tool project, you must have the [Ogre3D SDK 1.7.4](http://www.ogre3d.org/download/sdk/) on your PC,
and the OGRE\_HOME environment variable set to the path of the SDK.
  * command line, go to the sdk root directory
  * setx OGRE\_HOME %cd%

If you have trouble setting up the OgreSDK, [you may find further information here.](http://www.ogre3d.org/tikiwiki/Setting+Up+An+Application+-+Visual+Studio?page=Setting+Up+An+Application+-+Visual+Studio&comments_parentId=77&comments_per_page=1&thread_style=commentStyle_threaded#Environment_Variables)

The framework part has no dependencies except the Eye driver.

## Use of the framework ##
  * You don't need the calibration tool any more if you don't want to use the magnetometers, because the calibration is automatic
    * Or use the calibration tool for magnetic calibration, and replace the settings.cfg
  * Read the [MoveFrameworkSDK](MoveFrameworkSDK.md) wiki page for more information.


## Other Move on PC projects ##
[Move on PC](http://code.google.com/p/moveonpc/) Maybe the first Move project on Windows. I also began there. :)

[Thomas' multiplatform framework](http://thp.io/2010/psmove/)