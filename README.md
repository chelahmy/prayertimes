# Prayer Times
A tool and a C library to calculate muslim prayer times based on sundial approximation of sunrise and noon. As for sunset, the time different from sunrise to noon is almost the same as from noon to sunset. All prayer times can be referred to those daily events by applying standard constant values such as degrees or minutes before sunrise or after sunset.

However, some schools of thoughts or locality do apply minor adjustments to the constants according to their methods. Hence, there are a number of prayer time methods in the world.

The Sun rises and sets due to the Earth rotation on its axis. The Sun does not always exactly rise and set from the east and to the west. Sometimes in a year it declines a bit north and sometimes a bit south. This is due to the tilting of the Earth's axis. And the duration of a sunrise to another day sunrise is not always 24 hours because the Earth is also orbiting the Sun while it rotates on its axis. And the orbit is not an exact circle. Thus, there is a daily equation of time through out the year. And the relationship between the Earth and the Sun is not constant in celestial time. Hence, any calculation is only approximation.

The algorithm to calculate sunrise and noon is still evolving for accuracy, speed and simplicity. Some tools use altered algorithms for reasons of limitations in computing processing. Hence, there are variations in minutes and seconds to the calculated prayer times by different tools. Yet, the variations can still be minimized by readjusting the constants. Anyway, the exact minutes and seconds are not mandatory in muslim prayer times. However, we are encouraged to observe the Sun.

This tool allows prayer times constants to be adjusted.

The C library was adapted from [PrayTimes.js](http://praytimes.org/code/). Apart from the prayer times calculations, this library also implements the required trignometry and square root routines so that it does not depend on the standard C math library.

The library which is made of two files, [prayertimes.c](https://github.com/chelahmy/prayertimes/blob/master/prayertimes.c) and [atan.c](https://github.com/chelahmy/prayertimes/blob/master/atan.c), has been tested to work with [ESP8266](https://en.wikipedia.org/wiki/ESP8266) using [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk).

Credits to [PrayTimes.org](http://praytimes.org) and to all those who had contributed to the works of calculating the Sun's position as seen from the Earth. May Allah bless we all.
