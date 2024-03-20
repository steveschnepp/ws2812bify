# The NUA format

## Origin

The NUA format was invented by [Josh Levine](http://josh.com) in his [NeoUart](https://github.com/bigjosh/NeoUart) project.

It is a textual format that has 1 event per line. The event/line has the following format:

    [DD]RRGGBB where [DD] is an optional duration in 1/100s of seconds, and
    and RR, GG, and BB are the brightness levels for red, green, and blue
    respecively.

All values are hex numbers in the range 00-ff. The case is not relevant.

Examples of pixelspecs:

* 000000=Black
* 05FFFF=white for .05s
* 800080=50% blue for 1.28s

It is used to drive a single neopixel RGB led.

## Extension

In order to be able to write whole strips, I need to extend the protocol a little.

Each line still represents an event, but it can be of unlimited length.

It is enough to simply add a space than another RRGGBB hex numbers.
A line represents a single "transaction", that is send to the line without any RESET in between.

RESETS are sent at the end of each line. The duration is what is waited before emitting the next line.
Note that it is an "at least" duration. Due to the non-realtime nature of the
host OS, delays can eventually be longer.

    [DD]RRGGBB[ RRGGBB[ RRGGBB] ... ]
    [DD]RRGGBB[ RRGGBB[ RRGGBB] ... ]
    [DD]RRGGBB[ RRGGBB[ RRGGBB] ... ]
    [DD]RRGGBB[ RRGGBB[ RRGGBB] ... ]

So a typical file for a 8 neopixels rain fall is


    05000000 000000 000000 000000 000000 000000 000000 000000
    15FFFFFF 000000 000000 000000 000000 000000 000000 000000
    05000000 FFFFFF 000000 000000 000000 000000 000000 000000
    15000000 000000 FFFFFF 000000 000000 000000 000000 000000
    05000000 000000 000000 FFFFFF 000000 000000 000000 000000
    05000000 000000 000000 000000 FFFFFF 000000 000000 000000
    05000000 000000 000000 000000 000000 FFFFFF 000000 000000
    05000000 000000 000000 000000 000000 000000 FFFFFF 000000
    05000000 000000 000000 000000 000000 000000 000000 FFFFFF
    05000000 000000 000000 000000 000000 000000 000000 000000
