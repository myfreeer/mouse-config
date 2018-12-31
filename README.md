# mouse-config

Switch between different mouse speed and acceleration configures for different mouse.

## Programs

* mouse-config-once: parse config file and execute and exit.
* mouse-config-daemon: parse config file and execute every time usb device changes.

## Config File

### Config File Name & Path

Config file should be located in the same folder of mouse-config executable,
and has the same file name of the executable with `.ini` as extension.

Example: Config file for `mouse-config-daemon.exe` should be `mouse-config-daemon.ini`

### Config File Format

```ini
[Mouse]
Some_Device_Id=10,0,0,0
Default=15
```

Key of the ini entry is device id of a mouse,
which can be found in `Device Manager`,
fuzzy matching one by one with currently working devices.
The `Default` key is used when none of other device ids matched,
or do nothing if the `Default` key not exist.
Max length of key is 256 chars.

Value of the ini entry is parameters of mouse configure,
formatted as `Mouse speed,[Mouse threshold1,Mouse threshold1,Mouse acceleration]`.
If the last three parameters not provided, they would be inited as `0` by default.

## Extra Info

### Mouse speed

Link: <https://docs.microsoft.com/windows/desktop/api/winuser/nf-winuser-systemparametersinfow>

Retrieves the current mouse speed which ranges between 1 (slowest) and 20 (fastest).
The mouse speed determines how far the pointer will move based on the distance the mouse moves.
The default value is 10, which results in no additional modification to the mouse motion.

### Mouse threshold & acceleration

Link: <https://docs.microsoft.com/windows/desktop/api/winuser/nf-winuser-mouse_event>

Sets the two mouse threshold values and the mouse acceleration.
If the specified distance along either the x or y axis is greater than the first mouse threshold value,
and the mouse acceleration level is not zero, the operating system doubles the distance.

If the specified distance along either the x- or y-axis is greater than the second mouse threshold value,
and the mouse acceleration level is equal to two,
the operating system doubles the distance that resulted from applying the first threshold test.
It is thus possible for the operating system to multiply relatively-specified mouse motion
along the x- or y-axis by up to four times.