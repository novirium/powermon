#include "Globals.h"

/*
	Reads a string from EEPROM.
*/
String readString(char cStartPosition, char cLength) {
	String strOut;
	char i;
	char strCalcXOR;
	char strRecXOR;
	char cIn;
	boolean bEnd;
	
	#ifdef DEBUG
	String strMessage;
	strMessage = String("Reading string from (")+String(cStartPosition, DEC)+", "+String(cLength, DEC)+").";
	Serial.println(strMessage);
	//Serial.print("Character sequence: ");
	#endif
	
	// Read each char from memory.
	strOut = String("");
	strCalcXOR = 0;
	bEnd = false;
	for (i=0; i<cLength; i++) {
		cIn = EEPROM.read(cStartPosition+i);
		
		// Check for the end of the string.
		if (cIn==0) bEnd = true;
		
		// Append to string and XOR
		if (!bEnd) strOut = strOut + String(cIn);
		if (i!=(cLength-1)) strCalcXOR ^= cIn;
		else strRecXOR = cIn;
		
		#ifdef DEBUG	
		//strMessage = String(cIn, DEC)+", ";
		//Serial.print(strMessage);
		#endif
	}

	#ifdef DEBUG	
	Serial.println("");
	#endif	
	
	// Return the null on invalid value.
	if (strRecXOR!=strCalcXOR) {
		#ifdef DEBUG	
		strMessage = String("Invalid string read from EEPROM(")+String(cStartPosition, DEC)+", "+String(cLength, DEC)+"),("+String(strCalcXOR, DEC)+", "+String(strRecXOR, DEC)+").";
		Serial.println(strMessage);
		#endif
		return String();
	}
	
	return strOut;
}

/*
	Writes a string to EEPROM.
*/
void writeString(char cStartPosition, char cLength, String strIn) {
	char strXOR;
	char i;
	char cEnd, cTEnd;
	
	#ifdef DEBUG
	String strMessage;
	if (strIn.length()==0) {
		strMessage = String("Cannot write string to (")+String(cStartPosition, DEC)+", "+String(cLength, DEC)+").";
		Serial.println(strMessage);
	}
	#endif
	if (strIn.length()==0) return;
	
	// Initialise values and placeholders.
	strXOR = 0;
	cEnd = cStartPosition+strIn.length();
	cTEnd = cStartPosition+cLength;
	if (cEnd>=cTEnd) cEnd = cTEnd-1;
		
	// Write the string to EEPROM.
	for (i=cStartPosition; i<cEnd; i++) {
		EEPROM.write(i, strIn.charAt(i-cStartPosition));
		strXOR ^= strIn.charAt(i-cStartPosition);
	}
	
	// Pad the end with blank values.
	for (i=cEnd; i<cTEnd; i++) EEPROM.write(i, 0);
	
	// Write the XOR'd value to the end.
	#ifdef DEBUG
	strMessage = String("Writing string to (")+String(cStartPosition, DEC)+", "+String(cLength, DEC)+"),("+String(strXOR, DEC)+", "+String(strIn.charAt(cLength-1), DEC)+").";
	Serial.println(strMessage);
	#endif
	EEPROM.write((cStartPosition+cLength)-1, strXOR);
	EEPROM.commit();
}

/*
	Reads an integer from EEPROM.
*/
int readInt(char cStartPosition) {
	String cReadBuffer;
	int iOut;

	cReadBuffer = readString(cStartPosition, 6);
	
	if (cReadBuffer.length()>0) {
		iOut = cReadBuffer.toInt();
	} else iOut = 0;
	
	return iOut;
}

/*
	Writes an integer to EEPROM.
*/
void writeInt(char cStartPosition, int iIn) {
	String cWriteBuffer;
	
	if (iIn > 99999) iIn = 99999;
	else if (iIn < 0) iIn = 0;
	
	cWriteBuffer = String(iIn, DEC);
	writeString(cStartPosition, 6, cWriteBuffer);
}

/*
	Reads a boolean from EEPROM.
*/
boolean readBoolean(char cBytePosition, char cBitPosition) {
	char byteIn;
	char bMask;
	boolean bOut;
	
	#ifdef DEBUG
	String strMessage;
	strMessage = String("Reading boolean from (")+String(cBytePosition, DEC)+", "+String(cBitPosition, DEC)+").";
	Serial.println(strMessage);
	#endif
	
	// Read byte from EEPROM.
	bMask = (1 << cBitPosition);
	byteIn = EEPROM.read(cBytePosition);
	bOut = ((byteIn & bMask) > 0);

	return bOut;
}

/*
	Writes a boolean to EEPROM.
*/
void writeBoolean(char cBytePosition, char cBitPosition, boolean bValue) {
	char byteIn;
	char bMask;
	char byteOut;

	#ifdef DEBUG
	String strMessage;
	strMessage = String("Writing ");
	if (bValue) strMessage = strMessage + "true ";
	else strMessage = strMessage + "false ";
	strMessage = strMessage + String("boolean to (")+String(cBytePosition, DEC)+", "+String(cBitPosition, DEC)+").";
	Serial.println(strMessage);
	#endif
	
	// Write byte to EEPROM.
	bMask = (1 << cBitPosition);
	byteIn = EEPROM.read(cBytePosition);

	// Apply the change to the byte.
	if (bValue) byteOut = (byteIn | bMask);
	else byteOut = (byteIn & (~bMask));
	
	EEPROM.write(cBytePosition, byteOut);
	EEPROM.commit();
}

/*
	Gets the WiFi settings from memory, or returns NULL on failure.
*/
SETTINGS *getWiFiSettings(char cStartPos) {
	SETTINGS *settingsOut = NULL;
	String strSSID;
	String strPassword;
	boolean bUseWiFi;
	
	// Malloc the memory.
	settingsOut = (SETTINGS *) malloc(sizeof(SETTINGS));
	if (settingsOut == NULL) return NULL;
	
	// Read the SSID and password from EEPROM.
	strSSID = readString(cStartPos, 16);
	strPassword = readString(cStartPos+16, 16);
	bUseWiFi = readBoolean(cStartPos+32, 0);
	
	if ((strSSID.length() > 0) && (strPassword.length() > 0)) {
		#ifdef DEBUG
		String strMessage;
		strMessage = String("Retrieved the following settings from memory at offset: ")+String(cStartPos, DEC)+".";
		Serial.println(strMessage);
		Serial.println("	SSID: "+strSSID);
		Serial.println("	Password: "+strPassword);
		if (bUseWiFi) Serial.println("	Use WiFi: Yes");
		else Serial.println("	Use WiFi: No");
		#endif
	
		strSSID.toCharArray(settingsOut->ssid, 15);
		strPassword.toCharArray(settingsOut->password, 15);
		settingsOut->useWiFi = bUseWiFi;
		return settingsOut;
	}
	
	#ifdef DEBUG
	String strMessage;
	strMessage = String("No valid settings were found at offset ")+String(cStartPos, DEC)+".";
	Serial.println(strMessage);
	#endif
	
	// Free memory on failure.
	free(settingsOut);
	
	// Return NULL on failure.
	return NULL;
}

/*
	Writes the WiFi settings to EEPROM.
*/
void putWiFiSettings(char cStartPos, SETTINGS *settings) {
	if (settings == NULL) {
		#ifdef DEBUG
		Serial.println("An empty settings object was passed to the putWiFiSettings function.");
		#endif	
		return;
	}

	#ifdef DEBUG
	Serial.println("Writing settings to the EEPROM.");
	#endif	
	
	writeString(cStartPos, 16, String(settings->ssid));
	writeString(cStartPos+16, 16, String(settings->password));
	writeBoolean(cStartPos+32, 0, settings->useWiFi);
}

/*
	Gets the WiFi Client settings from memory, or returns NULL on failure.
*/
SETTINGS *getWiFiClientSettings() {
	SETTINGS *settingsOut;
	
	settingsOut = getWiFiSettings(0);
	
	return settingsOut;
}

/*
	Puts the WiFi Client settings into memory.
*/
void putWiFiClientSettings(SETTINGS *settings) {
	putWiFiSettings(0, settings);
}

/*
	Gets the default WiFi Access Point settings from memory.
*/
SETTINGS *getDefaultWiFiAPSettings() {
	SETTINGS *settingsOut = NULL;

	#ifdef DEBUG
	Serial.println("Default WiFi Access Point settings requested.");
	#endif
	
	// Allocate memory for settings.
	settingsOut = (SETTINGS *)malloc(sizeof(SETTINGS));
	
	// If the memory cannot be allocated, return NULL.
	if (settingsOut == NULL) return NULL;
		
	// If the memory can be allocated, the copy the constants into memory.
	String("pulsemon-3AD1").toCharArray(settingsOut->ssid, 15);
	String("Test1234").toCharArray(settingsOut->password, 15);
	settingsOut->useWiFi = true;
	
	#ifdef DEBUG
	Serial.println("Default WiFi Access Point settings retrieved.");
	#endif
	
	return settingsOut;
}

/*
	Gets the WiFi Access Point settings from memory, or uses the default.
*/
SETTINGS *getWiFiAPSettings() {
	SETTINGS *settingsOut;
	
	// Get the AP WiFi settings from memory, defaults on failure.
	settingsOut = getWiFiSettings(33);
	if (settingsOut == NULL) settingsOut = getDefaultWiFiAPSettings();
	
	return settingsOut;
}

/*
	Puts the WiFi Access Point settings into memory.
*/
void putWiFiAPSettings(SETTINGS *settings) {
	putWiFiSettings(33, settings);
}

/*
	Cleans up the SETTINGS object.
*/
void cleanupSettings(SETTINGS *settings) {
	if (settings != NULL) {
		#ifdef DEBUG
		Serial.println("Freeing a settings object.");
		#endif
		free(settings);
	}
}

/*
	Generates a new SETTINGS object with blank values.
*/
SETTINGS *generateNewSettings() {
	SETTINGS *settings;
	#ifdef DEBUG
	Serial.println("Generating a new settings object.");
	#endif
	settings = (SETTINGS *)malloc(sizeof(SETTINGS));
	String().toCharArray(settings->ssid, 15);
	String().toCharArray(settings->password, 15);
	settings->useWiFi = false;
	
	return settings;
}

/*
	Loads the general settings.
*/
void loadSettings() {
	String cPlaceholder;

	bPowerMonEnabled = readBoolean(66, 0);
	bPulseLight = readBoolean(66, 1);
	
	cPlaceholder = readString(67, 2);
		cPulseFreq = cPlaceholder[0];
	
	iPulseMultiFactor = readInt(69);
	iPulseMultiFactor = iPulseMultiFactor / 10;
	
	cPlaceholder = readString(75, 2);
		cLimit = cPlaceholder[0];
	cPlaceholder = readString(77, 2);
		cLimitHigh = cPlaceholder[0];
	cPlaceholder = readString(79, 2);
		cLimitDebounce = cPlaceholder[0];
	
	sPushURL = readString(81, 40);
	
	sPushArg = readString(121, 10);
	
	iPushFreq = readInt(131);
}

/*
	Saves the general settings to EEPROM.
*/
void saveSettings() {
	char cPlaceholder[2];
	int dPlaceholder;

	writeBoolean(66, 0, bPowerMonEnabled);
	writeBoolean(66, 1, bPulseLight);
	
	cPlaceholder[0] = cPulseFreq;
	writeString(67, 2, cPlaceholder);
	
	dPlaceholder = iPulseMultiFactor * 10;
	writeInt(69, dPlaceholder);
	
	cPlaceholder[0] = cLimit;
	writeString(75, 2, cPlaceholder);
	cPlaceholder[0] = cLimitHigh;
	writeString(77, 2, cPlaceholder);
	cPlaceholder[0] = cLimitDebounce;
	writeString(79, 2, cPlaceholder);
	
	writeString(81, 40, sPushURL);
	writeString(121, 10, sPushArg);
	
	writeInt(131, iPushFreq);
}

/*
	Loads the default general settings.
*/
void loadDefaultSettings() {
	bPowerMonEnabled = false;
	bPulseLight = false;
	
	cPulseFreq = 100;
	iPulseMultiFactor = 5660;
	
	cLimit = 120;
	cLimitHigh = 5;
	cLimitDebounce = 5;
	cLimitCountHi = 0;
	cLimitCountLo = 0;
	bLimitDebounce = false;
	
	sPushURL = String("http://192.168.1.101");
	sPushArg = String("value");
	iPushFreq = 600;
}