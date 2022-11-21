# Cyrillonumeric Electroluminescent Display driver

If you happen to have some [IEL-0-VI](http://lampes-et-tubes.info/cd/cd113.php?l=e) Cyrillonumeric EL displays and some (discontinued) [EL Sequencers](https://www.sparkfun.com/products/retired/12781) from Sparkfun, this code should get you going with displaying whatever you want on them.

![Pic1](https://user-images.githubusercontent.com/20519442/203099698-aa573b9a-bc8b-4a4e-954f-b6dceff32a4a.jpg)![12781-01a](https://user-images.githubusercontent.com/20519442/203099536-d18a9923-55e9-4dd0-9007-f6df6e6c7449.jpg)

Different EL displays will probably work too, I just havent tested on them (I haven't even tested on IEL-0-VIs, I wrote this code for someone else who has them, but I'm pretty sure it will work) 

Here's how you wire them up for multiple displays

![IMG_2054 Large](https://user-images.githubusercontent.com/20519442/203095459-5a677e81-6dfc-433a-b6cf-1dc24b546c5c.jpeg)

Upload the Master code to one of them and the slave code to the rest, then attach the CS pins on each of the slaves to ADC 2, 3, and 4 on the master. Connect all the MOSI, MISO, SCK, and GND pins together between each board.

Uncomment one of the #defines at the top to choose what to have it show, your options are: a string you enter into the quote[] array, cycle through all the characters, or send 4 chars through the serial monitor and it will show them. 

I'm using [MiniCore](https://github.com/MCUdude/MiniCore) and choosing ATMega328P and external 8MHz as the clock speed (the EL Sequencers have an 8MHz crystal instead of 16MHz like an Arduino)

If you need any help getting it working, send me a message and I'll help you with getting it set up.
