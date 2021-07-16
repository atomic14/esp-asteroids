[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/Z8Z734F5Y)
# ESP32 Asteroids

A simple implementation of the classic arcade game Asteroids on the ESP32 using a laser and mirrors.

Although the code is based around driving a laser, you can use if with a normal display if you have one.

You can watch a video on how it all works [here](https://www.youtube.com/watch?v=pPh3_ciEmzs)

[![Video](https://img.youtube.com/vi/LXDwGygCokU/0.jpg)](https://www.youtube.com/watch?v=LXDwGygCokU)

You can find more details on the schematics and how the laser galvos work in this video - https://youtu.be/bl1e54QGJk4.

The project is built using [PlatformIO](https://platformio.org/) and uses the Esspressif IDF. I've been testing on a
WROVER module, but it should work on a WROOM as well - though there may be some issues with running out of memory.

The code is still under development. There are some bugs and issues to be found.
