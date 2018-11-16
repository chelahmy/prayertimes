# Prayer Times
A tool and a C library to calculate muslim prayer times based on observable approximation of sunrise and noon. As for sunset, the time different from sunrise to noon is almost the same as from noon to sunset. Sunrise, noon and sunset are daily solar events that are predictable. All muslim prayer times refer to those daily solar events by applying relative values such as degrees or minutes before sunrise or after sunset.

However, some schools of thoughts or locality do apply minor adjustments to the relative values based on their individual methods. Hence, there are a number of methods of prayer times in the world.

The events of sunrise and sunset are due to the Earth rotation on its axis. The Sun does not always exactly rise from the east and set to the west. Sometimes through out a year it declines towards north and sometimes towards south. This is due to the tilting of the Earth's axis. And the duration of a sunrise to the next day sunrise is not always 24 hours because of the motion of the Earth's movement around the Sun while it rotates on its axis. The motion is due to the Earth's orbit around the Sun that is not an exact circle. Thus, there is a daily equation of time through out the year. And most importantly to note is that the relationship between the Earth and the Sun is not constant in celestial measures. Hence, any calculation is only approximation.

That is why our prayer times keep on changing on daily basis. We just noticed this since the invention of the clock. Now we are relying more on the clock rather than observing the Sun for prayer times.

The Wikipedia has the method to calculate the [position of the Sun](https://en.wikipedia.org/wiki/Position_of_the_Sun). We can use the [NOAA Solar Calculator](https://www.esrl.noaa.gov/gmd/grad/solcalc/) to compare results with our own culculation. 

The algorithm to calculate solar events is still evolving for accuracy, speed and simplicity. Some tools use altered algorithms for reasons of limitations in computing processing. Hence, there are variations in minutes and seconds to the calculated prayer times by different tools. Yet, the variations can still be minimized by readjusting the relative values. Anyway, the exact minutes and seconds are not mandatory in muslim prayer times. We are encouraged to observe the Sun.

This tool allows prayer times relative values to be adjusted.

The C library was adapted from [PrayTimes.js](http://praytimes.org/code/). This library also implements the required trignometry and square root routines so that it does not depend on the standard C math library.

The library which is made of two files, [prayertimes.c](https://github.com/chelahmy/prayertimes/blob/master/prayertimes.c) and [atan.c](https://github.com/chelahmy/prayertimes/blob/master/atan.c), has been tested to work with [ESP8266](https://en.wikipedia.org/wiki/ESP8266) using [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk). Thus, it can be used in embedded systems.

Credits to [PrayTimes.org](http://praytimes.org) and to all those who had contributed, is contributing and will contribute to the works of calculating the Sun's position as seen from anywhere on the Earth at any given time through out our lives now and into the future. May Allah bless us all.
