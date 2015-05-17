dht22_display
=============

A simple sketch that displays temperature and humidity from a DHT22
sensor to a TM1637 display.


### Commands for RRDtool ###

1. Create an RRD database.

    rrdtool create test.rrd --step 5 \
        DS:temp:GAUGE:20:0:U \
        DS:hum:GAUGE:20:0:100 \
        DS:hi:GAUGE:20:0:U \
        RRA:AVERAGE:0.5:1:1000 \

2. Use the following script to insert data from the Arduino to the DB.

    while true
    do 
        rrdtool update test.rrd $(curl  -q http://arduino.local/data/get/ 2>/dev/null |./readSensor.py) && \
        echo "Updated" && sleep 5 
    done

3. Plot a graph using the following command.

    rrdtool graph test.png --rigid --lower-limit 0 --upper-limit 100 \
    --vertical-label 'Temperature in C' --right-axis-label 'Percent' \
    --right-axis 2.5:0 --start -900 --end now \
    DEF:temp=test.rrd:temp:AVERAGE LINE:temp#54EC48:"Temperature":STACK \
    GPRINT:temp:MIN:"min\: %3.2lf %s" \
    GPRINT:temp:AVERAGE:"average\: %3.2lf %s" \
    GPRINT:temp:MAX:"max\:%3.2lf %s\n" \
    DEF:hum=test.rrd:hum:AVERAGE LINE:hum#8BBFFF:"Humidity":STACK \
    GPRINT:hum:MIN:"min\: %3.2lf %s" \
    GPRINT:hum:AVERAGE:"average\: %3.2lf %s" \
    GPRINT:hum:MAX:"max\: %3.2lf %s\n" \
    DEF:hi=test.rrd:hi:AVERAGE LINE:hi#F29D00:"Heat Index":STACK \
    GPRINT:hi:MIN:"min\: %3.2lf %s" \
    GPRINT:hi:AVERAGE:"average\: %3.2lf %s" \
    GPRINT:hi:MAX:"max\: %3.2lf %s\n"

<!-- vi: tw=72
-->
