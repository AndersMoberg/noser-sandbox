Rayman 2 Input Fix
By Nolan Check
Version 1, released 3/19/2012

How to use:

Copy and paste the included dinput.dll to the Rayman 2 installation directory.
You can now use a gamepad or joystick to play Rayman 2. Be sure to calibrate it
from the menu by going to Options -> Control -> Calibration Pad.

Unfortunately, Rayman 2 uses a fixed button mapping. It cannot be changed.

How it works:

Rayman 2 originally supported joysticks and gamepads, but it has a bug where it
uses the DirectInput API incorrectly. This bug makes it fail to detect input
devices other than the keyboard. When the included dinput.dll file is placed in
Rayman 2's directory, it acts as a layer on top of DirectInput and works around
the bug.

Where to find:

The latest version can be found at
http://code.google.com/p/noser-sandbox/downloads/list .