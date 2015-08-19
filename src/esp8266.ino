#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>

#include "wifipass.h"

// add in a file called wifipass.h in /src/ or define here: 
//const char* ssid = "yourssid";
//const char* password = "password";

//void* __dso_handle;

#define BUFFER_SIZE 300
#define LIGHT_ON_WAIT 5

ESP8266WebServer server(80);


 
const int led = 13;

int adc_value;
char strbuffer[7]; 


int bufferIndex=0;
int measureValues[BUFFER_SIZE];
long measureTimes[BUFFER_SIZE];

Ticker measureTick;

int ledDwellTime=1000;
int mperiod=200;

 
 
 
void measure() {
    digitalWrite(led, 0);
    digitalWrite(1, 1);
    delayMicroseconds(ledDwellTime);
    //delay(LIGHT_ON_WAIT);
    if (bufferIndex<BUFFER_SIZE-1){
        bufferIndex++;
    } else {
        bufferIndex=0;
    }
    
    measureValues[bufferIndex]=analogRead(A0);
    measureTimes[bufferIndex]=millis();
    digitalWrite(1, 0);
    digitalWrite(led, 1);
}


void handle_root() {
  
  //digitalWrite(led, 1);
  String valueString = String(measureValues[bufferIndex]);
  String timeString = String(measureTimes[bufferIndex]);
  server.send(200, "text/plain", "Last ADC value: "+valueString+", "+timeString+"ms\r\n");
  //delay(10);
  
}

void handle_raw(){
    
    
    int oldest=bufferIndex;
    
    if (oldest<BUFFER_SIZE-1){
        oldest++;
    } else {
        oldest=0;
    }
    
    String valueString="["+String(measureTimes[oldest])+","+String(measureValues[oldest])+"]";
    
    for (int i=oldest+1;i<BUFFER_SIZE;i++){
        valueString=valueString+",["+String(measureTimes[i])+","+String(measureValues[i])+"]";
    }
    
    for (int i=0;i<oldest;i++){
        valueString=valueString+",["+String(measureTimes[i])+","+String(measureValues[i])+"]";
    }
    
    
    server.send(200, "text/html", String("<!DOCTYPE html>\r\n\
<html>\r\n\
    <head>\r\n\
        <title>Energy Monitor</title>\r\n\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\r\n\
    </head>\r\n\
    <body>\r\n\
        <div data-role=\"page\" id=\"pgeHome\" data-theme=\"e\">\r\n\
            <div data-role=\"content\">\r\n\
                <script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>\r\n\
                <div id=\"chart_div\"></div>\r\n\
                <script type=\"text/javascript\">\r\n\
                    google.load('visualization', '1', {packages: ['corechart', 'line']});\r\n\
                    google.setOnLoadCallback(drawBasic);\r\n\
                    function drawBasic()")+" {\r\n\
                          var data = new google.visualization.DataTable();\r\n\
                          data.addColumn('number', 'X');\r\n\
                          data.addColumn('number', '');\r\n\
                          data.addRows(["+valueString+"]);\r\n\
                          var options = {\r\n\
                            hAxis: {\r\n\
                              title: 'millis'\r\n\
                            },\r\n\
                            vAxis: {\r\n\
                              title: 'ADC'\r\n\
                            }\r\n\
                          };\r\n\
                          var chart = new google.visualization.LineChart(document.getElementById('chart_div'));\r\n\
                          chart.draw(data, options);\r\n\
                        }\r\n\
                </script>\r\n\
            </div>\r\n\
        </div>\r\n\
    </body>\r\n\
</html>\r\n");
}


void handle_settings() {
    String settingsChanged="";
    
    settingsChanged=settingsChanged+"\r\n mperiod: "+String(mperiod)+"ms";
    if (server.hasArg("mperiod")){
        mperiod=server.arg("mperiod").toInt();
        measureTick.detach();
        measureTick.attach_ms(mperiod,measure);
        settingsChanged=settingsChanged+", changed to: "+String(mperiod)+"ms";
    }
    
    settingsChanged=settingsChanged+"\r\n dwelltime: "+String(ledDwellTime)+"us";
    if (server.hasArg("dwelltime")){
        ledDwellTime=server.arg("dwelltime").toInt();
        settingsChanged=settingsChanged+", changed to: "+String(ledDwellTime)+"us";
    }
    
    server.send(200, "text/plain", "Settings\r\n"+settingsChanged+"\r\n");
}





long lastsendtime;

const char* host = "192.168.1.101";
const int httpPort = 80;

int newval=0;
int minval=1000;



 
void setup(void)
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(led, 0);
    Serial.print(".");
    delay(500);
    digitalWrite(led, 1);
  }

  server.on("/", handle_root);
  
  server.on("/raw", handle_raw);
  
  server.on("/settings", HTTP_GET, handle_settings);
  
  server.begin();
  
  measureTick.attach_ms(mperiod,measure);
  
  //lastsendtime=millis();
}




 




 
void loop(void)
{
  server.handleClient();
  
  /*newval=measure();
  if (newval<minval){
      minval=newval;
  }*/
  
  //if (millis()-lastsendtime>2000){
      /*WiFiClient client;
      
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      
      client.println("GET /energy-monitor/add.php?value="+String(minval)+" HTTP/1.0");
      client.println();
      minval=1000;*/
     // lastsendtime=millis();
  //}
} 
