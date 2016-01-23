#include "Globals.h"

/*
	Connects to client WiFi.
*/
void connectToClientWiFi(SETTINGS *settings) {
	char cCount;

	WiFi.begin(settings->ssid, settings->password);

	// Wait for connection.
	Serial.print("Connecting to WiFi");
	cCount = 0;
	while ((WiFi.status() != WL_CONNECTED) && (cCount < 30)) {
		delay(500);
		Serial.print(".");
		cCount++;
	}
	
	if (WiFi.status() == WL_CONNECTED) {
		// Announce connecting to WiFi successful.
		Serial.println("");
		Serial.print("Connected to ");
		Serial.println(settings->ssid);
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
		
		// Start up the MDNS server.
		if (mdns.begin(settings->ssid, WiFi.localIP())) {
			Serial.println("MDNS Responder started.");
		}
	} else {
		Serial.println("");
		Serial.print("Timed out attempting to connect to ");
		Serial.println(settings->ssid);
	}
}

/*
	Sets up the Access Point WiFi.
*/
void setupAPWiFi(SETTINGS *settings) {
	WiFi.softAP(settings->ssid, settings->password);

	// Display message to user.
	Serial.println("Soft AP started.");
	Serial.print("	SSID: ");
	Serial.println(settings->ssid);
	
	#ifdef DEBUG
	Serial.print("	Password: ");
	Serial.println(settings->password);
	#endif
	
	// Announce AP IP Address.
	Serial.print("AP IP address: ");
	Serial.println(WiFi.softAPIP());
}

/*
	Sets up the WiFi for the pulsemon.
*/
void connectToWiFi() {
	SETTINGS *settingsAP = NULL;
	SETTINGS *settingsClient = NULL;
	boolean bUseClient, bUseAP;
	
	// Load the settings.
	settingsAP = getWiFiAPSettings();
	settingsClient = getWiFiClientSettings();
	
	// Set the default.
	bUseClient = bUseAP = false;
	
	if (settingsAP!=NULL) {
		if (settingsAP->useWiFi) bUseAP = true;
	}
	if (settingsClient!=NULL) {
		if (settingsClient->useWiFi) bUseClient = true;
	}	
	
	// Setup WiFi Access Point.
	if (bUseAP && bUseClient) {
		Serial.println("Using the AP and Client settings from memory.");
		
		WiFi.mode(WIFI_AP_STA);
		setupAPWiFi(settingsAP);
		connectToClientWiFi(settingsClient);
	} else {
		if (bUseAP) {
			Serial.println("Using the AP settings from memory.");
			
			WiFi.mode(WIFI_AP);
			setupAPWiFi(settingsAP);
		} else if (bUseClient) {
			Serial.println("Using the Client settings from memory.");
			
			WiFi.mode(WIFI_STA);
			connectToClientWiFi(settingsClient);
		} else {
			Serial.println("Error: No WiFi access point to connect to, and no SoftAP to setup.");
			Serial.println("       Using SoftAP Defaults.");
			
			// Get the new default SoftAP Settings.
			cleanupSettings(settingsAP);			
			settingsAP = getDefaultWiFiAPSettings();
			
			// Set the mode, and setup the WiFi.
			WiFi.mode(WIFI_AP);
			setupAPWiFi(settingsAP);
		}
	}
	
	// Perform memory cleanup.
	cleanupSettings(settingsClient);
	cleanupSettings(settingsAP);
}

/*
	Prints the new WiFi settings to Serial.
*/
void printWiFiSettings(String strMode, SETTINGS *settings) {
	Serial.println("New settings have been set for the "+strMode);
	Serial.print("	SSID: ");
	Serial.println(settings->ssid);
	#ifdef DEBUG
	Serial.print("	Password: ");
	Serial.println(settings->password);
	#endif
	if (settings->useWiFi) Serial.println("	Using WiFi: Yes");	
	else Serial.println("	Using WiFi: No");
}

/*
	Updates the WiFi settings.
*/
boolean updateWiFiSettings() {
	SETTINGS settingsClient;
	SETTINGS settingsAP;
	SETTINGS *settings;
	boolean bSettingsChanged;
	
	// Initialise all settings to NULL.
	String().toCharArray(settingsClient.ssid, 15);
	String().toCharArray(settingsClient.password, 15);
	settingsClient.useWiFi = false;
	
	String().toCharArray(settingsAP.ssid, 15);
	String().toCharArray(settingsAP.password, 15);
	settingsAP.useWiFi = false;
	bSettingsChanged = false;
	
	for (char i=0; i<server.args(); i++) {
		settings = NULL;

		// Determine if its a Client or AP based setting.
		if (server.argName(i)[0]=='c') settings = &settingsClient;
		else if (server.argName(i)[0]=='a') settings = &settingsAP;

		// Determine if its a SSID or Password setting.
		if (settings != NULL) {
			switch (server.argName(i)[1]) {
				case 's':				
					// Copy the setting across.
					String(server.arg(i)).toCharArray(settings->ssid, 15);
					break;
				case 'p':
					// Copy the setting across.
					String(server.arg(i)).toCharArray(settings->password, 15);
					break;
				case 'e':
					settings->useWiFi = (server.arg(i).toInt() == 1);
					break;
				default:
					break;
			}
		}
	}
	
	// Write settings to EEPROM if needed.
	if ((strlen(settingsClient.ssid) > 0) && (strlen(settingsClient.password) > 0)) {
		putWiFiClientSettings(&settingsClient);
		printWiFiSettings("Client", &settingsClient);
		bSettingsChanged = true;
	}
	if ((strlen(settingsAP.ssid) > 0) && (strlen(settingsAP.password) > 0)) {
		putWiFiAPSettings(&settingsAP);
		printWiFiSettings("AP", &settingsAP);
		bSettingsChanged = true;
	}
		
	return bSettingsChanged;
}