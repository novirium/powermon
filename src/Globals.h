#ifndef _GLOBALS_H
#define _GLOBALS_H

	#include <ESP8266WiFi.h>
	#include <WiFiClient.h>
	#include <ESP8266WebServer.h>
	#include <ESP8266mDNS.h>
	#include <EEPROM.h>

	String WIFI_AP_SSID = "pulsemon-3AD1";	// The WiFi Access Point SSID.
	String WIFI_AP_PASSWORD = "Test1234";	// The WiFi Access Point Password.

	ESP8266WebServer server(80);				// The webserver.
	MDNSResponder mdns;							// The MDNS responder.
	WiFiClient client;							// Web Client

	volatile unsigned long lPulses;				// The number of pulses since startup.
	volatile unsigned long lLast_ms;			// The last milliseconds seen by the interrupt.
	
	boolean bPowerMonEnabled;					// Power Monitoring enabled.
	boolean bPulseLight;						// Pulse Light Enabled
	char cPulseFreq;							// Pulse Frequency			(milliseconds)
	int iPulseMultiFactor;						// Pulse Multiplication Factor.
	
	char cLimit;								// ADC Limit
	char cLimitHigh;							// Cycles for ADC to go high.
	char cLimitDebounce;						// Cycles to wait for next high cycle.
	char cLimitCountHi;							// The count of the limit being high.
	char cLimitCountLo;							// The count of the limit being low.
	boolean bLimitDebounce;						// Debounce active.
	
	String sPushURL;								// URL to push the data to.
	String sPushArg;								// Argument to push to.
	int iPushFreq;								// Frequency to push data. (seconds)
	long lLastConnectionTime;					// Last time the server was connected to.

	char *cABuffer;								// Buffer for ADC values.
	char iABufferSize;							// Buffer size.
	char iABufferPos;							// Buffer position.
	
	char cFreeBufferCount;						// Counter used to display the free buffer information.

	typedef struct {
		char ssid[16];
		char password[16];
		boolean useWiFi;
	} SETTINGS;
	
	#define DEBUG 1
	
#endif