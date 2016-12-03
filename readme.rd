This sketch allows a SmartEverything SigFox node to request current time from the network and set its internal clock.

The time taken to send and receive the time will lead to a delta of time up to a couple of minute (usualy significantly less than a minute).

The update request is sent every 24 hours.

The sketch do nothing else than printing the time on serial usb port every minute.

The node is blocked when time update, but this could be adjusted with additional development.

Improvement can also include the use of the RTC embedded on the board to allow the MCU to go to sleep and save power.

To properly work, the SigFox backend have to be configured to respond to downlonk data in direct mode with the data set to `{time}00000000`. details can be found on http://miscellany.bouillot.org/post/153988507825/sigfox-two-way-communications-setting-nodes
