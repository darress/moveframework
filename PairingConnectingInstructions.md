## This is for the PS Move only, the Navigation Controller works only with MIJ ##

**A played a bit around with a win7 x64 system, and I could manage it too. So it seems that all windows version will work. Also tested with internal BT adapter, and it worked too.**

**[Please check this thread](http://code.google.com/p/moveframework/issues/detail?id=1) for other people's experience with the pairing process!**

# Instructions to pair and connect the Moves to a Windows based computer #

## Seting the Bluetooth address of your dongle in the Move's memory (pairing) ##
The Move connects always to the host address, that is stored in the it's memory. So if you pair it with a host, several other hosts can be around, the Move will ignore them.
  1. Make sure MotionInJoy is not installed (or removed properly)
  1. Connect your Move to the PC via USB (you can connect more at once, all will be paired)
  1. Start the Calibration Tool, and press P to pair all devices.
    * If you have more BT dongles, all Moves will be paired to the first dongle found.


## Connecting the paired Moves to you PC for the first time ##
**I paired two devices with more operating systems and PCs! In most cases it works, just don't give up! If you did it once, the Move will always automatically connect by pressing the PS button!**
**Tested on Windows 7 32 and 64 bit**
  1. Make sure MotionInJoy is not installed!
  1. Open your Bluetooth settings, and make sure "Allow devices to find this computer" is checked
  1. Click "Add a Bluetooth device"
  1. Press the PS button on your Move while the BT discovery is in progress
  1. "Motion Controller" will appear, wait until the blinking is over
    * Method 1: Press the PS button, than immediately double click on the device
    * Method 2: Double click on the device, wait 2 sec, press the PS button
  1. Pair the Move without a oode
    * Sometimes it is necessary to press the PS button just after clicking the "pair without code", and also try to press it if you stuck at some step.
    * If you can't pair it, try an other dongle, some dongles just don't work
  1. Open the Properties window of the Move (if you are here, you will probably able to pair the device, dont give up, play around a little, and report how you could manage it! :))
  1. Open the properties of the device, go to the "Services" tab and you have to see the checkbox for the HID service!
    * If the checkbox does not appear, try to press the PS button a bit before clicking the services tab. If it appeared once, it will not desappear any more.
    * Or try this method:
      * Right click on the device. Submenu appears. Wait.
      * Hit the ps button and, quickly:
      * Click properties
      * Click services tab
  1. You see the checkbox, now you have 3 methods to try
    * Method 1: Tick the checkbox, press the PS button and immediately hit Apply. If it doesn't work for first, uncheck the checkbox, close the window, open it again, and repeat this step.
    * Method 2: Make sure the checkbox is not checked. Close the window. Open it again, but don't click on the services tab yet. Press the PS button, and immediately click the services tab. If it doesn't work, repeat it.
    * Method 3: During HID device installation stage, press PS button immediately after led goes off, just to keep the installation going. Then when that's done, the led will light up on its own, and don't press PS button any more, or Move goes into pairing mode again and have to start all over. (Comment: During HID device installation stage, every time the Move LED goes off, press PS button immediately so the LED starts blinking again, this is just to keep the installation going. Also make sure you fully charge the Move, it will greatly prolong the blinking period.)
  1. If the led on your Move is on and not blinking, you successfully connected your Move to your PC, and you will be able to pair it very fast every other time by simple pushing the PS button.
  1. If you could not manage to connect the Move to the PC, please play around a bit, and if you found a new method, let me know!


## Connecting the paired Moves to your PC (not the first time) ##
**Only tested with Windows 7 and Windows 8, please comment if it works or not with other versions!**
  1. Press the PS button on your Move, and it will be paired

## Disconnecting your Move ##
  1. Press and hold the PS button until the LED goes out.