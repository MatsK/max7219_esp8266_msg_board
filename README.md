# MAX7219 RDA Message Board

Forked from https://github.com/rdeangel/esp8266_max7219_rda_msg_board

This is an ESP8266 based message board and it has been mainly put togheter to display scrolling messages from remote systems or users such as:
1. Home Assistant or Node-Red using HTTP or MQTT
2. Linux / Windows using curl
3. Any programming language that has MQTT support
4. A browser URL Link
5. A built-in webgui

## Key Features:

* HTTP webserver / message board web interface
* Send messages via HTTP using automation systems or scripts ("URI" or "Json api" parameters supported) 
* Send messages via MQTT Server (User Authentication or Anonymous) ("Json" parameters or "Plain" messages supported)
* Support for UTF8 Extended ASCII Characters (see https://www.utf8-chartable.de/)
* Change/Store HTTP credentials
* Change/Store MQTT Config (enable/disable MQTT and connect/disconnect alerting)
* mDNS Supported (browse and send messages via http to mdns name (eg. ESP-MSG-ABCDEF.local) or to selected IP address (future improvement ability to change hostname)
* WifiManager provides a web portal to configure WiFi SSID and Password when one hasn't been previously configured
* Press ESP8266 FLASH button (or browse to /factoryreset) to wipe WiFi SSID Config, HTTP credentials and MQTT Setting.

[Initial setup of the message board](/doc/init.md)

