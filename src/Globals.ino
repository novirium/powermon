#include "Globals.h"

char *charToIntStr(char cIn) {
	char *strOut;
	
	strOut = (char *)malloc(4);
	if (strOut == NULL) return NULL;
	
	sprintf(strOut, "%d", cIn);
	
	return strOut;
}

char *intToStr(int iIn) {
	char *strOut;
	
	strOut = (char *)malloc(9);
	if (strOut == NULL) return NULL;
	
	sprintf(strOut, "%d", iIn);
	
	return strOut;
}

void shiftString(char *strIn, int offset) {
	int i;
	
	#ifdef DEBUG
	char strMessage[50];
	sprintf((char *) &strMessage, "Offsetting array by %d, starting at %d.", offset, (strlen(strIn)+offset));
	Serial.println(strMessage);
	#endif
	
	strIn[strlen(strIn)+offset] = 0;
	for (i=(strlen(strIn)+offset)-1; i>=offset; i--) {
		strIn[i] = strIn[i-offset];
	}
	
	#ifdef DEBUG
	Serial.println("Offsetting string complete.");
	#endif
}

/*
	Allocates and clears the memory.
*/
void *clear_alloc(char cLength, char cSize) {
	char *cOut;
	char i;
	char cLen;
	
	// Allocate the memory.
	cLen = cLength * cSize;
	cOut = (char *) malloc(cLen);
	
	// Check for nullness.
	if (cOut == NULL) return NULL;
	
	// Clear the memory.
	for (i=0; i<cLen; i++) cOut[i] = 0;
	
	return (void *) cOut;
}

/*
	Clears the specified buffer.
*/
void clear_buffer(char *bIn, int size) {
	for (int i=0; i<size; i++) bIn[i] = 0;
}

/*
	Escapes the parameters in the URL.
*/
String escape_parameter(String param) {
	param.replace("+"," ");
	param.replace("%21","!");
	param.replace("%23","#");
	param.replace("%24","$");
	param.replace("%26","&");
	param.replace("%27","'");
	param.replace("%28","(");
	param.replace("%29",")");
	param.replace("%2A","*");
	param.replace("%2B","+");
	param.replace("%2C",",");
	param.replace("%2F","/");
	param.replace("%3A",":");
	param.replace("%3B",";");
	param.replace("%3D","=");
	param.replace("%3F","?");
	param.replace("%40","@");
	param.replace("%5B","[");
	param.replace("%5D","]");

	return param;
}