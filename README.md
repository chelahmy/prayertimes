# Prayer Times
A tool and a C library to calculate muslim prayer times based on sundial approximation of sunrise and noon. As for sunset, the time different from sunrise to noon is almost the same as from noon to sunset. All prayer times can be referred to those daily events by applying standard constant values such as degrees or minutes before sunrise or after sunset.

However, some schools of thoughts or locality do apply minor adjustments to the constants according to their methods. Hence, there are a number of prayer time methods in the world.

The algorithm to calculate sunrise and noon is still evolving for accuracy, speed and simplicity. Some tools use altered algorithms for reasons of limitations in computing processing. Hence, there are variations in minutes and seconds to the calculated prayer times by different tools. Yet, the variations can still be minimized by readjusting the constants.

This tool allows prayer times constants to be adjusted.

The C library was adapted from [PrayTimes.js](http://praytimes.org/code/). Apart from the prayer times calculations, this library also implements the required trignometry and square root routines so that it does not depend on the standard C math library.

The library which is made of two files, [prayertimes.c](https://github.com/chelahmy/prayertimes/blob/master/prayertimes.c) and [atan.c](https://github.com/chelahmy/prayertimes/blob/master/atan.c), has been tested to work with [ESP8266](https://en.wikipedia.org/wiki/ESP8266) using [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk).

Credits to [PrayTimes.org](http://praytimes.org) and to all those who had contributed to the works of calculating the Sun's position as seen from the Earth. May Allah bless we all.
