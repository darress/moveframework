# Setting up your project #

  * Download and unzip the latest release of the SDK

  * If you want to add the framework to an existing project:
    * add the SDK's include folder to the additional include libraries (C/C++/General)
    * add the SDK's lib folder to the additional library directories (linker/general)
    * add MoveManager.lib to the additional depedencies (linker/input)
    * set $(TargetDir) for the working directory (debugging) (or if you want to use a custom working directory, use that folder to copy the .cfg file to)
    * copy the DLL-s to your target directory from the SDK's debug folder

  * Or use the SDK's project for reference

  * **Use the Calibration Tool to get the accurate calibration in the settings.cfg file**
  * **Copy that file to your working directory**

# Using the SDK #

You can find an example in the example project.
Everything is in the "Move" namespace!
(Except the low-level device code.)

> ## PS Button ##
  * Reset the orientation with the PS button while pointing to the monitor with the device, so the orientation will be tracked relative to your monitor
  * Reset the position by pressing the PS button for 2 seconds

# API #
API comming soon.

# Config file #

The filename is always settings.cfg

The most of the file is generated through the calibration process (Move`_` sections), but you can set a few things manually.
These things are in the [Tracking](Tracking.md) section.

**AHRSalgorithmGain** (default: 0.1): This is the gain of the orientation tracking algorithm. If you set this high, there will be less drift but the jitter will be higher. With a low value there will be more drift but less jitter.

**CameraExposure** (default: 50): This sets the camera exposure. It has an effect on the position tracking. You check the effect by looking at the mask (press 1), it's the best if the ball is very bright and everything else is dark.

**BallColorX** (default: not set): The color of the x.th Move. It automatically turns of the automatic color management. Format: RGB

**ReturnZeroIfNotFound** (default: 0):
> if 0: the position will be the last position if the ball is not found
> if not 0: the position will be 0, if not found

**PredictionBufferSize** (default: not set): if set, there will be a prediction filter on the orientation (less jitter, but slower tracking)

**UseMagnetometers** (default: 1): if 0, no Move uses the magnetometers, if not 0, all Moves uses them, if they are calibrated.

Example file:
```
[Tracking]
AHRSalgorithmGain=0.3
CameraExposure=50
BallColor0=255 0 0
BallColor1=0 255 0
ReturnZeroIfNotFound=0
PredictionBufferSize=100
UseMagnetometers=1

[Move_04:76:6E:C5:8C:3A]
gyroGain=0.00159983 6.01081e-005 9.95319e-006 -5.44653e-005 0.00161772 9.04184e-006 -1.20184e-005 2.40131e-006 0.00217727
accBias=0.000180657 6.40918e-005 0.000832511
accGain=0.00225479 -4.00231e-005 -1.83652e-005 3.20757e-005 0.00225905 7.05441e-006 1.36879e-005 -4.58274e-006 0.00228297
magBias=-138.876 -37.3236 100.772
magGain=0.00598726 0.000530953 -0.000252551 -0.000496338 0.00632303 -9.21145e-005 9.75411e-005 -0.000130615 0.00606146
[Move_00:06:F7:DA:B4:20]
gyroGain=0.00148034 1.65837e-005 -2.71994e-005 -1.86888e-005 0.00164413 1.54699e-005 -3.59728e-006 -1.19669e-005 0.00180474
accBias=0.000508314 -6.31258e-005 0.000206684
accGain=0.00226056 5.22263e-005 -1.51919e-005 -8.27179e-006 0.00228614 -8.82555e-006 1.38849e-005 4.14575e-005 0.00223976
```