dht22_display
=============

A simple sketch that displays temperature and humidity from a DHT22
sensor to a TM1637 display.
Data from the sensor are also logged into an RRD database.

### Introduction ###
My first simple project in Arduino that was made just for fun.
If you have any suggetions for improvements or any bugfixes, please feel
free to create an issue.

### Hardware requirements ###

 - Arduino Yun
 - 4 digit display driven by a TM1637 chip
 - DHT22 sensor
 - SD card (in case you need logging to be performed on the Arduino Yun)

### Library requirements ###

 - TM1637Display (https://github.com/avishorp/TM1637/)
 - DHTLib (https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib)

### Installation ###
Copy ```arduino``` folder to an SD card.

### Commands for RRDtool ###

1. Create an RRD database (most preferably inside ```arduino/www``` folder).

```rrdtool create temp.rrd --step 5 \
    DS:temp:GAUGE:20:0:U \
    DS:hum:GAUGE:20:0:100 \
    DS:hi:GAUGE:20:0:U \
    RRA:AVERAGE:0.5:1:1000```

2. Use the following script to insert data from the Arduino to the DB.

```while true
do 
    rrdtool update temp.rrd $(curl  -q http://localhost/data/get/ 2>/dev/null |./readSensor.py) && \
    echo "Updated" && sleep 5 
done```

3. Plot a graph using the following command.

```rrdtool graph temp.png --rigid --lower-limit 0 --upper-limit 50 \
--vertical-label 'Temperature in C' --right-axis-label 'Percent' \
--right-axis 2:0 --start -900 --end now \
DEF:temp=temp.rrd:temp:AVERAGE LINE:temp#54EC48:"Temperature":STACK \
GPRINT:temp:MIN:"min\: %3.2lf %s" \
GPRINT:temp:AVERAGE:"average\: %3.2lf %s" \
GPRINT:temp:MAX:"max\:%3.2lf %s\n" \
DEF:hum=temp.rrd:hum:AVERAGE CDEF:s_hum=hum,0.15,* \
LINE1:s_hum#8BBFFF:"Humidity":STACK \
GPRINT:hum:MIN:"min\: %3.2lf %s" \
GPRINT:hum:AVERAGE:"average\: %3.2lf %s" \
GPRINT:hum:MAX:"max\: %3.2lf %s\n" \
DEF:hi=temp.rrd:hi:AVERAGE CDEF:s_hi=hum,0.15,* \
LINE1:s_hi#F29D00:"Heat Index":STACK \
GPRINT:hi:MIN:"min\: %3.2lf %s" \
GPRINT:hi:AVERAGE:"average\: %3.2lf %s" \
GPRINT:hi:MAX:"max\: %3.2lf %s\n" ```

4. Visit http://arduino.local/sd/temp.png and check the result.

<!-- vi: tw=72
-->
