#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>  // Include ArduinoJson library
// Replace with your network credentials
const char* ssid = " ";  // your Wi-Fi SSID
const char* password = " ";  // your Wi-Fi password
// GPIO where the DS18B20 is connected to
const int oneWireBus = D2;   
OneWire oneWire(oneWireBus);
int s1Value;
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
// GroveStreams API URL template (with Unix timestamp for startDate)
d
WiFiClient client;

void setup() {
  Serial.begin(115200);  // Start the serial monitor at 115200 baud rate
  pinMode(oneWireBus, INPUT_PULLUP);
// Start the DS18B20 sensor
  sensors.begin();
  // Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Wi-Fi connected");

  // Print IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
}

 void loop() {sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  Send(temperatureF);
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");
  int delayvalue = s1Value*60*1000;
  Serial.print("delay value mts  ="); Serial.println(s1Value);
  delay(delayvalue);//in millisec


 
}
void Send(float temperatureF){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String groveStreamURL = "http://www.grovestreams.com/api/feed?compTmplId=myorg.mysensor&api_key=xxxxxxxxxxxxxxxxxx&s3=" + String(temperatureF) + "&compName=8533489&compId=17066978&rsid=s1";  // Use Unix timestamp


    // Send PUT request instead of GET
    Serial.print("Sending request to GroveStreams...");
    http.begin(client, groveStreamURL);  // Specify the URL
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Set content type for PUT request

    // Send PUT request
    int httpCode = http.PUT("");  // Send the PUT request (with empty body, since the parameters are in the URL)

    // Check for response
    if (httpCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpCode);
      String payload = http.getString();  // Get the response payload
      Serial.println("Response: ");
      Serial.println(payload);  // Print the respon
      // Parse the JSON response
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.f_str());
        return;
      }

      // Extract the 's1' value from the JSON object
      s1Value = doc["s1"];  // Extract the value of 's1'
      Serial.print("Extracted s1 value: ");
      Serial.println(s1Value);

    } else {
      Serial.println("Error in HTTP request");
    }
    http.end();  // Close the connection
  }
}
