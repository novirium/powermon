#include "Globals.h"

/*
	Sets up the pulse monitor.
*/
void setup(void) {
	// Setup the pulse pin.
	pinMode(2, OUTPUT);

	// Start the serial output for diagnosis.
	Serial.begin(115200);

	#ifdef DEBUG
	Serial.println("Debugging mode active.");
	#endif
	
	// Initialise the EEPROM.
	EEPROM.begin(200);
	#ifdef DEBUG
	Serial.println("EEPROM beginning with 200 bytes.");
	#endif

	// Load General Settings.
	loadDefaultSettings();
	//saveSettings();
	loadSettings();
	
	// Set up the WiFi connections.
	connectToWiFi();

	// Initialise pulse count to zero.
	lPulses = 0;
	cFreeBufferCount = 0;
	
	// Assign HTTP server hooks, start server, and announce it.
	server.onNotFound(handleNotFound);
	server.on("/", handleRoot);
	
		server.on("/settings.html", handleSettings);
			server.on("/general.html", handleGeneralSettings);
			server.on("/client.html", handleWiFiClientSettings);
			server.on("/ap.html", handleWiFiAPSettings);
			server.on("/general.js", handleGeneralScript);
	
		server.on("/connect.html", handleConnect);
		server.on("/style.css", handleCSSRequest);
	
		server.on("/graph.html", handleGraph);
			server.on("/graph.js", handleGraphScript);
			server.on("/values.json", handleValues);
	
	#ifdef DEBUG
	Serial.println("Attaching server handles.");
	#endif
	
	server.begin();
	Serial.println("HTTP Server started.");
	
	// Generate a buffer for the ADC.
	generateADCBuffer();
}

/*
	Performs the loop of the pulse monitor.
*/
void loop(void) {
	// Handles a client.
	server.handleClient();
	
	// Power monitoring functions
	if (bPowerMonEnabled) {
		pulseLightOn();
		delay(20);
		addADCValueToBuffer();
	}
	if (bPulseLight) pulseLightOff();
	
	// Pulse Frequency
	if (bPowerMonEnabled) delay(cPulseFreq);
	else delay(100);
	
	checkWhetherToPushData();
	
	displayFreeBufferInfo();
}