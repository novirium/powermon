# powermon
powermon is an ESP8266 based application which counts the number of pulses from either a rotary disc-based electricity meter or a LED pulsing electricity meter.

# Screenshots
powermon Interface:

![alt tag](/screenshots/Interface.png)

powermon Settings:

![alt tag](/screenshots/Settings.png)

powermon ADC Interface:

![alt tag](/screenshots/ADC.png)

#To get started:

The application has been built using the platformio platform.

Steps:

*** Please Note: The IP Address mentioned in the steps below are for the Access Point mode of the ESP8266. If using the Client Mode, it is best to find the 
correct IP Address for the device and substitute it.

1) Download a clone of the application and extract it.
2) The Default Access Point name and password are defined in the Globals.h file; it is best to change these before proceeding.
3) Navigate to the base path using command prompt or terminal.
4) Run "platformio run"
5) Upon downloading the application to the ESP8266, look for the WiFi Access Point details which were specified earlier in step 2.
6) Navigate to http://192.168.4.1/settings.html
7) Adjust the settings as required:

	Client SSID: The Access Point the ESP8266 is to connect to in Client Mode.
	Client Password: The Password for the Access Point mentioned above.
	Client Mode Active: This Enables/Disables the Client Mode for the ESP8266.
	
	Access Point SSID: The SSID for the ESP8266 in AP Mode.
	Access Point Password: The Password for the ESP8266 in AP Mode.
	Access Point Mode Active: This Enables/Disables the AP Mode for the ESP8266.
	
	Power monitoring Active: This Enables/Disables the ADC monitoring on the ESP8266.
	Pulse Light: This pulses the light connected on GPIO2. Needed for the rotary disc-based electricity meter.
	Measurement Frequency: This sets the delay for each cycle. Set this to zero to have no delay between cycles.
	ADC Limit: The minimum analog amount recorded for a pulse to be counted.
	ADC High Count: The number of cycles in which the analog value has been recorded above the ADC Limit for a pulse to be recorded.
	ADC High Debounce: The number of cycles in which the analog value has been recorded below the ADC Limit for another pulse to be recorded.
	Pulse multiplication factor: The multiplication factor for the measured value. If one pulse equals 17 Wh, then the pmf should be 17.
	Push Data(URL): The URL to push the calculated value to.
	Push Argument: The GET attribute in which to link the calculated value to.
	Push Frequency: The number of cycles to pass for each push of the calculated values to the server.

#Notes:	
1) You can view debug information via the Serial Port on the ESP8266. It runs at 115200bps.
2) To view the ADC via a graph/chart, navigate to http://192.168.4.1/graph.html
3) To view the current pulse count, and the calculated value, navigate to http://192.168.4.1/
