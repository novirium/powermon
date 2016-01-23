#include "Globals.h"

/*
	Generates a page for the user.
*/
void generatePage(int iRetCode, String strTitle, String strHeader, char *strBuffer) {
	String strOut;

	// Error handling.
	if ((strTitle == NULL) || (strBuffer == NULL)) return;
	
	#ifdef DEBUG
	Serial.println("The page content length is "+String(strlen(strBuffer)));
	#endif
	
	// Prep webpage.
	strOut = String("<html><head>")+strHeader+String("<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\"/><title>")+strTitle+String("</title></head><body><div id='main_outer'><div id='main_inner'>");
	strOut += String(strBuffer)+String("</div></div></body></html>");
	
	// Inform user.
	Serial.println(String("Sending ")+strTitle+" page to client.");
	
	// Send string.
	server.send(iRetCode, "text/html", strOut);
}

/*
	Generates a JSON array and sends it for the user.
*/
void generateJSONArray(int iRetCode, char *arrIn, int iBufferSize) {
	String strOut;
	int i;
	
	strOut = String('[');
	for (i=0; i<iBufferSize; i++) {
		strOut += String(arrIn[i], DEC);
		if (i!=(iBufferSize-1)) strOut += ", ";
	}
	strOut += "]";

	Serial.println("Sending JSON to client.");
	server.send(iRetCode, "application/json", strOut);
}

/*
	Generates the CSS style page.
*/
void generateCSS() {
	String strOut;
	
	strOut = String("#chart_div,#main _outer,iframe{width:100%}body{background-color:#2eb82e;color:#fff}span{width:300px;display:inline-block}iframe{border:.5px solid}fieldset{width:99%;border:0}input{width:400px;padding:10px;border:1px solid #dcdcdc;transition:box-shadow .3s,border .3s}input.focus,input:focus{border:1px solid #707070;box-shadow:0 0 5px 1px #969696}#main_inner{width:100%;margin:0 auto}#chart_div{height:80%}");
	
	Serial.println("Sending CSS style sheet to client.");
	server.send(200, "text/css", strOut);
}

/*
	Generates the Javascript data.
*/
void generateScript(String strIn) {
	server.send(200, "text/javascript", strIn);
}

/*
	Generates the HTML for a input.
*/
String appendInputElement(String strIn, String strLabel, String strType, String strName, String strValue) {
	return strIn+"<br/><span>"+strLabel+"</span><input type=\""+strType+"\" name=\""+strName+"\" value=\""+strValue+"\"/>";
}

/*
	Generates the HTML for a checkbox.
*/
String appendCheckboxElement(String strIn, String strLabel, String strName, String strValue) {
	return strIn+"<br/><span>"+strLabel+"</span><input type=\"checkbox\" name=\""+strName+"\" value=\"1\" "+strValue+">";
}

/*
	Generates a link.
*/
String appendLink(String strIn, String strLabel, String strLink) {
	return strIn+"<br/><a href=\""+strLink+"\">"+strLabel+"</a>";
}

/*
	Generates a script tag.
*/
String appendScript(String strIn, String strSource) {
	return strIn+"<script type=\"text/javascript\" src=\""+strSource+"\"></script>";
}

/*
	Generates an element.
*/
String appendElement(String strIn, String strElement) {
	return strIn+strElement;
}

/*
	Handles the root request.
*/
void handleRoot() {
	String strOut;
	char cStrOut[1024];

	strOut = String("<label class='indexLbl'><h5>Total Pulses: ");
	strOut += String(lPulses, DEC);
	strOut += "</label><br/><label class='indexLbl'>Calculated Usage: ";
	strOut += String(lPulses*iPulseMultiFactor, DEC);
	strOut += "</label><br/><br/><a href=\"graph.html\">View ADC Graph</a><br/><a href=\"settings.html\">Settings</a></h5>";
	
	strOut.toCharArray((char *)&cStrOut, 1024);
	generatePage(200, "Home", "", cStrOut);
}

/*
	Handles the connect button request.
*/
void handleConnect() {
	SETTINGS *settings;
	String strOut;
	char cStrOut[1024];
	
	settings = getWiFiClientSettings();
	if (settings != NULL) {
		strOut = String("Connecting to WiFi client AP with SSID '");
		strOut += String(settings->ssid);
		strOut += String("'...<br/><br/><a href=\"/\">Home</a>");
		
		strOut.toCharArray((char *)&cStrOut, 1024);
		generatePage(200, "Connecting", "",cStrOut);
		connectToClientWiFi(settings);
	} else {
		strOut = String("There are no valid WiFi Client settings in memory.<br/><br/><a href=\"/\">Home</a>");
		
		strOut.toCharArray((char *)&cStrOut, 1024);
		generatePage(200, "Connecting", "",cStrOut);
	}
}

/*
	Handles the WiFi Client Settings page.
*/
void handleWiFiClientSettings() {
	SETTINGS *settingsClient = NULL;
	String strOut;
	String strMessage;
	char cStrOut[1024];
	clear_buffer((char *)cStrOut, 1024);
	
	if (server.method() == HTTP_POST) {
		if (updateWiFiSettings()) connectToWiFi();
		strMessage = String("<label style=\"color:#FF0000\">Updated settings.</label>");
	} else strMessage = String();
	  
	settingsClient = getWiFiClientSettings();
	
	if (settingsClient == NULL) {
		settingsClient = generateNewSettings();
		#ifdef DEBUG
		Serial.println("Generated new settings for WiFi client.");
		#endif
	}

	// Generate the initial string.
	strOut = String();
	
	// Generate the webpage.
	strOut = appendElement(strOut, "<h5>");
		strOut = appendElement(strOut, strMessage);
		strOut = appendElement(strOut, "<form method=\"POST\">");
			strOut = appendElement(strOut, "<fieldset>");
				strOut = appendInputElement(strOut, "Client SSID:", "text", "cs", settingsClient->ssid);
				strOut = appendInputElement(strOut, "Client Password:", "password", "cp", settingsClient->password);
				strOut = appendCheckboxElement(strOut, "Client Mode Active", "ce", (settingsClient->useWiFi ? "checked" : ""));
			strOut = appendElement(strOut, "</fieldset>");
			strOut = appendInputElement(strOut, "", "submit", "su", "Update");
		strOut = appendElement(strOut, "</form>");
		strOut = appendLink(strOut, "Connect to WiFi", "connect.html");
	strOut = appendElement(strOut, "</h5>");

	#ifdef DEBUG
	Serial.println("Elements added to the WiFi Client Settings webpage.");
	#endif
	
	strOut.toCharArray((char *)&cStrOut, 1024);
	generatePage(200, "Settings", "",cStrOut);
	
	cleanupSettings(settingsClient);
}

/*
	Handles the WiFi AP Settings request.
*/
void handleWiFiAPSettings() {
	SETTINGS *settingsAP = NULL;
	String strOut;
	String strMessage;
	char cStrOut[1024];
	
	if (server.method() == HTTP_POST) {
		if (updateWiFiSettings()) connectToWiFi();
		strMessage = String("<label style=\"color:#FF0000\">Updated settings.</label>");
	} else strMessage = String();
	  
	settingsAP = getWiFiAPSettings();
	
	if (settingsAP == NULL) {
		settingsAP = generateNewSettings();
		#ifdef DEBUG
		Serial.println("Generated new settings for WiFi AP.");
		#endif
	} else {
		#ifdef DEBUG
		Serial.println("Displaying retrieved settings for WiFi AP.");
		#endif	
	}

	// Initialise the string.
	strOut = String();
	
	// Generate the webpage.
	strOut = appendElement(strOut, "<h5>");
		strOut = appendElement(strOut, strMessage);
		strOut = appendElement(strOut, "<form method=\"POST\">");
			strOut = appendElement(strOut, "<fieldset>");
				strOut = appendInputElement(strOut, "Access Point SSID:", "text", "as", settingsAP->ssid);
				strOut = appendInputElement(strOut, "Access Point Password:", "password", "ap", settingsAP->password);
				strOut = appendCheckboxElement(strOut, "Access Point Mode Active", "ae", (settingsAP->useWiFi ? "checked" : ""));
			strOut = appendElement(strOut, "</fieldset>");
			strOut = appendInputElement(strOut, "", "submit", "su", "Update");
		strOut = appendElement(strOut, "</form>");
	strOut = appendElement(strOut, "</h5>");

	#ifdef DEBUG
	Serial.println("Elements added to the AP Settings webpage.");
	#endif
	
	strOut.toCharArray((char *)&cStrOut, 1024);
	generatePage(200, "Settings", "",cStrOut);
	
	cleanupSettings(settingsAP);
}

/*
	Handles the General Settings request.
*/
void handleGeneralSettings() {
	String strOut;
	String strMessage;
	String strInt;
	char cStrOut[1024];
	
	if (server.method() == HTTP_POST) {
		updateWiFiSettings();
		updateGeneralSettings();
		saveSettings();
		
		strMessage = String("<label style=\"color:#FF0000\">Updated settings.</label>");
	} else strMessage = String();

	loadSettings();

	// Initialise the initial string.
	strOut = String();
	
	// Generate the webpage.
	strOut = appendElement(strOut, "<h5>");
		strOut = appendElement(strOut, strMessage);
		strOut = appendElement(strOut, "<form method=\"POST\">");
			strOut = appendElement(strOut, "<fieldset>");
				strOut = appendCheckboxElement(strOut, "Power monitoring Active", "pm", (bPowerMonEnabled ? "checked" : ""));
				strOut = appendCheckboxElement(strOut, "Pulse Light", "pl", (bPulseLight ? "checked" : ""));
				strOut = appendElement(strOut, "<br/>");
				strOut = appendInputElement(strOut, "Measurement Frequency", "text", "pt", String(cPulseFreq, DEC));
				strOut = appendElement(strOut, "<br/>");
				
				strOut = appendInputElement(strOut, "ADC Limit", "text", "ad", String(cLimit,DEC));
				strOut = appendInputElement(strOut, "ADC High Count", "text", "ah", String(cLimitHigh,DEC));
				strOut = appendInputElement(strOut, "ADC High Debounce", "text", "ai", String(cLimitDebounce,DEC));
				strOut = appendElement(strOut, "<br/>");
				
				strOut = appendInputElement(strOut, "Pulse multiplication factor", "text", "pc", String(iPulseMultiFactor,DEC));
				strOut = appendInputElement(strOut, "Push Data(URL)", "text", "pu", sPushURL);
				strOut = appendInputElement(strOut, "Push Argument", "text", "pa", sPushArg);
				strOut = appendElement(strOut, "<br/>");
				
				strOut = appendInputElement(strOut, "Push Frequency", "text", "pf", String(iPushFreq, DEC));

			strOut = appendElement(strOut, "</fieldset>");
			strOut = appendInputElement(strOut, "", "submit", "sv", "Update");
		strOut = appendElement(strOut, "</form>");
	strOut = appendElement(strOut, "</h5>");

	#ifdef DEBUG
	Serial.println("Elements added to the General Settings webpage.");
	#endif

	strOut.toCharArray((char *)&cStrOut, 1024);
	generatePage(200, "Settings", "",cStrOut);
}

/*
	Handles the Settings page request.
*/
void handleSettings() {
	String strOut;
	String strHeader;
	char cStrOut[1024];
	
	// Generate the webpage.
	strOut = appendElement(strOut, "<iframe src=\"client.html\" scrolling=\"yes\" id=\"csi\" onload=\"javascript:resizeFrame(this)\"></iframe><br/><iframe src=\"ap.html\" scrolling=\"yes\" id=\"asi\" onload=\"javascript:resizeFrame(this)\"></iframe><br/><iframe src=\"general.html\" scrolling=\"yes\" id=\"gsi\" onload=\"javascript:resizeFrame(this)\"></iframe><br/>");
	strHeader = appendScript(strHeader, "https://ajax.googleapis.com/ajax/libs/jquery/2.1.3/jquery.min.js");
	strHeader = appendScript(strHeader, "general.js");

	#ifdef DEBUG
	Serial.println("Elements added to the Settings Overview webpage.");
	#endif
	
	strOut.toCharArray((char *)&cStrOut, 1024);
	generatePage(200, "Settings", strHeader, cStrOut);
}

/*
	Handle the graphing page.
*/
void handleGraph() {
	String strOut;
	String strHeader;
	char cStrOut[1024];
	
	// Generate the webpage.
	strOut = appendElement(strOut, "Raw Values<div id=\"chart_div\"></div>");
	strHeader = appendScript(strHeader, "https://ajax.googleapis.com/ajax/libs/jquery/2.1.3/jquery.min.js");
	strHeader = appendScript(strHeader, "https://www.google.com/jsapi");
	strHeader = appendScript(strHeader, "graph.js");

	#ifdef DEBUG
	Serial.println("Elements added to the Graph webpage.");
	#endif
	
	strOut.toCharArray((char *)&cStrOut, 1024);
	generatePage(200, "Graph", strHeader, cStrOut);
}

/*
	Generates the Graphing script.
*/
void handleGraphScript() {
	String strOut;
	
	strOut = "function updateGraph(){$.ajax({url:\"values.json\",success:function(a){drawGraph(organiseData(a))},complete:function(){setTimeout(updateGraph,1e3)}})}function organiseData(a){var e,n;for(e=new Array,n=0;n<a.length;n++)e[n]=new Array,e[n][0]=n,e[n][1]=a[n];return e}function drawGraph(a){var e=new google.visualization.DataTable;e.addColumn(\"number\",\"X\"),e.addColumn(\"number\",\"Value\"),e.addRows(a);var n={hAxis:{title:\"Time\"},vAxis:{title:\"Value\"}},t=new google.visualization.LineChart(document.getElementById(\"chart_div\"));t.draw(e,n)}function resizeFrame(a){a.style.height=a.contentWindow.document.body.scrollHeight+\"px\"}google.load(\"visualization\",\"1\",{packages:[\"corechart\",\"line\"]}),google.setOnLoadCallback(function(){$(function(){updateGraph()})});";
	
	generateScript(strOut);
}

/*
	Generates the General script.
*/
void handleGeneralScript() {
	String strOut;
	
	strOut = "function resizeFrame(e){e.style.height=(e.contentWindow.document.body.scrollHeight+100)+\"px\"}";
	
	generateScript(strOut);
}

/*
	Handles the request for values from the ADC buffer.
*/
void handleValues() {
	generateJSONArray(200, cABuffer, iABufferSize);
}

/*
	Handles the request for the CSS style sheet.
*/
void handleCSSRequest() {
	generateCSS();
}

/*
	Handles a request for when a page is not found.
*/
void handleNotFound() {
	String strOut;
	char cStrOut[1024];

	// Generate page content.
	strOut = String("<h3>File not Found</h3><br/><h5>The file '")+server.uri()+"' cannot be found on the server.</h5>";
	
	// Generate page.
	strOut.toCharArray((char *)&cStrOut, 1024);
	generatePage(404, "File not Found", "", cStrOut);
}