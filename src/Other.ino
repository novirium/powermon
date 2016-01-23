#include "Globals.h"

/*
	Generates a buffer to store the ADC values in.
*/
void generateADCBuffer() {
	char cMultiplier;
	int bSize;

	iABufferPos = 0;
	cMultiplier = 10;
	cABuffer = NULL;
	
	// Generate buffer for ADC values.
	while ((cABuffer == NULL) || (cMultiplier < 100)) {
		bSize = (ESP.getFreeHeap() / cMultiplier);
		if (bSize > 255) iABufferSize = 255;
		else iABufferSize = bSize;
		
		cABuffer = (char *) clear_alloc(iABufferSize, 1);
		
		// Increment the multiplier.
		cMultiplier = cMultiplier + 20;
	}
	
	// Give it a one byte buffer on failure.
	if (cABuffer==NULL) {
		cABuffer = (char *) clear_alloc(1, 1);
		iABufferSize = 1;
	}
}

void displayFreeBufferInfo() {
	if (cFreeBufferCount==0) {
		Serial.print("Free heap: ");
		Serial.println(ESP.getFreeHeap());
	}
	
	cFreeBufferCount++;
	
	if (cFreeBufferCount>9) cFreeBufferCount = 0;
}

/*
	Adds an ADC Value to the buffer.
*/
void addADCValueToBuffer() {
	cABuffer[iABufferPos] = (analogRead(A0) >> 2);
	if (cABuffer[iABufferPos] >= cLimit) {
		cLimitCountHi++;
		cLimitCountLo = 0;
		
		if (!bLimitDebounce) {
			if (cLimitCountHi>cLimitHigh) {
				lPulses++;
				bLimitDebounce = true;
			}
		}
	} else {
		cLimitCountLo++;
		cLimitCountHi = 0;
		if (cLimitCountLo>cLimitDebounce) {
			bLimitDebounce = false;
		}
	}
	
	iABufferPos++;
	if (iABufferPos>=iABufferSize) iABufferPos = 0;
}

/*
	Turns the pulse light on.
*/
void pulseLightOn() {
	digitalWrite(2, HIGH);
}

/*
	Turns the pulse light off.
*/
void pulseLightOff() {
	digitalWrite(2, LOW);
}

/*
	Checks whether the data needs to be pushed to the server.
*/
void checkWhetherToPushData() {
	int bHasResponse = false;
	
	// Write incoming data to the serial port.
	
	while (client.available()) {
		if (!bHasResponse) Serial.println("Response received from server: ");
		bHasResponse = true;
		
		char c = client.read();
		Serial.write(c);
	}

	if (millis() - lLastConnectionTime > (iPushFreq*1000)) {
		pushDataToServer();
	}
}

/*
	Attempts to push the data to the server.
	Example taken from WiFiWebClientRepeating Arduino example.
*/
void pushDataToServer() {
	String strHost, strServer, strURL;
	char strConnect[100];

	client.stop();
	
	strServer = sPushURL.substring(sPushURL.indexOf("//")+2);
	strHost = strServer.substring(0, strServer.indexOf("/"));
	strURL = strServer.substring(strServer.indexOf("/"));
	strHost.toCharArray((char *) &strConnect, 100);
	
	if (client.connect(strConnect, 80)) {
	
		Serial.println("Connecting to "+strHost+" on Port 80, requesting "+strURL);

		client.println("GET "+strURL+"?"+sPushArg+"="+String(lPulses*iPulseMultiFactor, DEC)+" HTTP/1.1");
		client.println("Host: www.arduino.cc");
		client.println("User-Agent: ArduinoWiFi/1.1");
		client.println("Connection: close");
		client.println();

		lLastConnectionTime = millis();
	}
	else {
		Serial.println("Error: Could not connect to the server.");
	}
}

/*
	Updates the General settings.
*/
boolean updateGeneralSettings() {
	int iPlaceholder;
	double dPlaceholder;
	
	// Set the default values;
	bPowerMonEnabled = false;
	bPulseLight = false;
	
	for (char i=0; i<server.args(); i++) {	
		switch (server.argName(i)[1]) {				
			// Pulse Frequency
			case 'm':
				bPowerMonEnabled = (server.arg(i).toInt() == 1);
				break;
			case 'l':
				bPulseLight = (server.arg(i).toInt() == 1);
				break;
			case 't':
				iPlaceholder = server.arg(i).toInt();
				if (iPlaceholder > 255) iPlaceholder = 255;
				if (iPlaceholder < 0) iPlaceholder = 0;
				
				cPulseFreq = iPlaceholder;
				break;
			
			// ADC Values
			case 'd':
				iPlaceholder = server.arg(i).toInt();
				if (iPlaceholder > 255) iPlaceholder = 255;
				if (iPlaceholder < 0) iPlaceholder = 0;
				
				cLimit = iPlaceholder;
				break;
			case 'h':
				iPlaceholder = server.arg(i).toInt();
				if (iPlaceholder > 255) iPlaceholder = 255;
				if (iPlaceholder < 0) iPlaceholder = 0;
				
				cLimitHigh = iPlaceholder;
				break;
			case 'i':
				iPlaceholder = server.arg(i).toInt();
				if (iPlaceholder > 255) iPlaceholder = 255;
				if (iPlaceholder < 0) iPlaceholder = 0;
				
				cLimitDebounce = iPlaceholder;
				break;
			
			// Push Information
			case 'c':
				dPlaceholder = server.arg(i).toInt();
				iPulseMultiFactor = dPlaceholder;
				if (iPulseMultiFactor <= 0) iPulseMultiFactor = 1;
				
				break;
			case 'u':
				// Copy the setting across.
				sPushURL = escape_parameter(String(server.arg(i)));
				break;
			case 'a':
				sPushArg = String(server.arg(i));
				break;
			
			// Push Frequency
			case 'f':
				iPushFreq = server.arg(i).toInt();
				if (iPushFreq < 5) iPushFreq = 5;
				else if (iPushFreq > 1000) iPushFreq = 1000;

				break;
			default:
				break;
		}
	}
}