#include <GyverOLED.h>
#include <Wire.h>
//GyverOLED<SSD1306_128x32, OLED_BUFFER> oled;
//GyverOLED<SSD1306_128x32, OLED_NO_BUFFER> oled;
//GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
//GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;
//GyverOLED<SSD1306_128x64, OLED_BUFFER, OLED_SPI, 8, 7, 6> oled;
GyverOLED<SSH1106_128x64> oled;
#include "FS.h"
#include <Adafruit_GFX.h>    // Core graphics library for OLED
#include <Adafruit_SSD1306.h> // OLED driver
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// I2C address of the display, commonly 0x3C or 0x3D for many SSD1306 OLEDs
#define OLED_RESET    -1  // Reset pin not used for most I2C OLEDs
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <Encoder.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h> // Include WiFiClient
#include <ESP8266WiFi.h> // Include WiFi library
#include <ArduinoJson.h> // Include ArduinoJson library
#include <TimeLib.h> // Include Time library if you're using the Arduino environment

#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFiManager.h>
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 0, 60000); // Adjust timezone as needed (0 is UTC)
// Wi-Fi Credentials
//const char* ssid = " "; // Replace with your SSID
//const char* password = " "; // Replace with your password

// Define the character set for input
const String characterSet = "|abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,:;@!#$%&*()[]{}_-+=<>?/|\\^~`";

// Define maximum input length and EOF trigger
const int maxLength = 32;  // Adjust as needed for your input size
const char EOF_CHAR = '|';  // Arbitrary character to signify end of input

Encoder encoder(5, 4); // D1 (GPIO 5) for DT, D2 (GPIO 4) for CLK
const int buttonPin = D7; // D6 (GPIO 12) for the button

int error;
long lastPosition = 0;
String stationNames[400]; // Array to hold station names
String mapIDs[400]; // Array to hold map IDs
int stationCount = 0;
unsigned long updateInterval = 10000; // Interval in milliseconds (30 seconds)
unsigned long lastUpdateTime = 0;     // To keep track of the last update time

const String API_KEY = " "; // Your CTA API key
// Store SSID and password
String ssid = "";
String password = "";
String currentInput = "";
bool wifiConnected = false;
int scalefactor = 4;
// Limit on input length (e.g., 10 characters)
char enteredChar;
void setup() {// Initialize the OLED display
  Wire.begin(D4, D3);  // Set up I2C
  Serial.begin(9600); oled.init();
  delay(1000);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Set button pin as input with pull-up
  Serial.println("WiFi Configuration");

  // Initialize the OLED display here, if not done
  oled.clear();

//  // Prompt to collect SSID and password using the encoder
//  oled.setCursorXY(0, 0);
//  oled.print("Enter SSID:");
//  oled.update();
//  currentInput = "SSID";  // Start with SSID input
//
//  // Collect SSID first
//  collectInput("SSID");
//
//  // Once SSID is collected, prompt for password input
//  switchInputMode();
//  collectInput("PWD");

  //  // Now you can connect to WiFi
  //  Serial.println("Connecting to WiFi with SSID: " + ssid + " and Password: " + password);
  //  WiFi.begin(ssid.c_str(), password.c_str());
  //
  //  while (WiFi.status() != WL_CONNECTED) {
  //    delay(500);
  //    Serial.print(".");
  //  }
  Serial.println("Connected!");
  //  // Connect to Wi-Fi
  //  WiFi.begin(ssid, password);
  //  Serial.print("Connecting to WiFi");
  //  while (WiFi.status() != WL_CONNECTED) {
  //    delay(500);
  //    Serial.print(".");
  //  }
  //  Serial.println("Connected to WiFi");
  // Initialize the SPIFFS file system
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  oled.clear();
  oled.setScale(1);
  oled.setCursor(0, 1);
//  oled.print("Enter SSID & Password");
  oled.update();
  delay(2000);  // Wait for 2 seconds

  // Load station data from SPIFFS
  loadStationData();
  Serial.println("station data loaded");

 ///*  
// Set WiFi mode explicitly (optional for ESP, defaults to STA+AP mode)
  WiFi.mode(WIFI_STA); 

  // Create an instance of WiFiManager
  WiFiManager wm;

  // Uncomment the following line to reset stored Wi-Fi settings (for testing)
  //wm.resetSettings();

  // Try to connect using saved Wi-Fi credentials, if any
  bool res;
  res = wm.autoConnect("AutoConnectAP", "password"); // Access point with password

  // If Wi-Fi connection is successful, print success message
  if (res) {
      Serial.println("Connected to Wi-Fi");
  } else {
      // If connection fails, print failure message
      Serial.println("Failed to connect to Wi-Fi");
  }

  // Now you're connected to the Wi-Fi, you can continue with your application
  // For example, you can call your functions to interact with APIs, etc.
  // Initialize the OLED display
  
   oled.clear();   // Clear the display buffer
   oled.update();  
//*/ 





}
//    // Connect to Wi-Fi
//    void connectToWiFi(const String &ssid, const String &password) {
//      WiFi.begin(ssid.c_str(), password.c_str());
//      Serial.print("Connecting to WiFi");
//
//      while (WiFi.status() != WL_CONNECTED) {
//        delay(500);
//        Serial.print(".");
//      }
//      Serial.println("Connected!");
//
//      wifiConnected = true;  // Set Wi-Fi connection status to true
//    }

void loop() {
//  currentInput = "SSID";  // Start with SSID input
//
//  // Collect SSID first
//  collectInput("SSID");
//
//  // Once SSID is collected, prompt for password input
//  switchInputMode();
//  collectInput("PWD");


  // Main loop runs once WiFi is connected and the app starts
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected, Running App...");
    runApp();  // Replace with your app logic
  } else {
    Serial.println("WiFi not connected. Try again.");
  }

  delay(1000);  // Optional delay to reduce output rate
}



void runApp() {
  unsigned long currentMillis = millis(); // Get the current time in milliseconds
  Serial.println("Started loop");

  // Check if enough time has passed for an update
  if (currentMillis - lastUpdateTime >= updateInterval) {
    // It's time to update the data
    int index = lastPosition % stationCount; // Get the index
    getArrivalData(mapIDs[index], stationNames[index]); // Update the arrival data

    // Update the last update time
    lastUpdateTime = currentMillis;
            // Optional: Show a message indicating that the data has been updated
            oled.clear();
            oled.setCursor(0, 0);
            oled.print("Data Updated!");
            delay(1000); // Show update message for 1 second
  }

  // Check if the button is pressed
  if (digitalRead(buttonPin) == LOW) {
    Serial.println("Button Pressed!");
    int index = lastPosition % stationCount; // Get the index

    oled.clear();  // Clear the display
    oled.setCursor(0, 0);  // Set the cursor to the top-left corner
    oled.print("Selected Station:");
    oled.setCursor(0, 10); // Move down a line
    oled.print(stationNames[index].substring(0, 16)); // Display selected station name
    oled.update();
    //        oled.print(" - ");
    //         oled.update();
    //        oled.print(mapIDs[index].substring(0, 16)); // Display selected map ID
    //        oled.update();
    // Debugging output to Serial Monitor
    Serial.print("Button Pressed! Index: ");
    Serial.print(index);
    Serial.print(", Station: ");
    Serial.print(stationNames[index]);
    Serial.print(", Map ID: ");
    Serial.println(mapIDs[index]);

    // Call function to get arrival data initially
    getArrivalData(mapIDs[index], stationNames[index]);

    // Loop to keep updating data until the button is pressed again
    bool buttonPressedAgain = false;
    while (!buttonPressedAgain) {
      currentMillis = millis();  // Recalculate currentMillis inside the loop

      // Update arrival data if the time interval has passed
      if (currentMillis - lastUpdateTime >= updateInterval) {
        getArrivalData(mapIDs[index], stationNames[index]); // Update arrival data
        lastUpdateTime = currentMillis; // Update last update time
      }

      // Check if the button is pressed again to exit the loop
      if (digitalRead(buttonPin) == LOW) {
        buttonPressedAgain = true; // Break the loop
        Serial.println("Button Pressed Again! Exiting update loop.");
        oled.clear(); // Clear the display
        oled.setCursor(0, 0);
        oled.print("Ready for Select");
        oled.update();
      }

      delay(100); // A short delay to prevent bouncing issues (adjust if necessary)
    }

    // Prevent further prints until the button is released
    while (digitalRead(buttonPin) == LOW) {
      Serial.println("Waiting for button release...");
      delay(10); // Wait for button release
    }
  }

  // Read the current position from the encoder
  long newPosition = encoder.read();
  if (newPosition != lastPosition) {
    lastPosition = newPosition;
    int index = lastPosition % stationCount; // Loop through stations
    oled.clear();
    oled.setScale(1);
    oled.setCursorXY(0, 0);
    oled.print(stationNames[index].substring(0, 16)); // Display station name
    oled.update();
    // Clear line 1 before updating
    //        oled.setCursorXY(0, 10);
    //        oled.print("                "); // Clear line
    //        oled.update();
    //        oled.setCursorXY(0, 20);
    //        oled.print(mapIDs[index].substring(0, 16)); // Display map ID
    //        oled.update();
  }

  delay(1000); // Delay for smoother scrolling
}




















// Function to convert ISO 8601 to UNIX timestamp
#include <TimeLib.h> // Include Time library

// Function to convert ISO 8601 to UNIX timestamp
long isoToUnixTimestamp(const char* isoTime) {
  String arrivalTime(isoTime);

  // Extract components
  int year = arrivalTime.substring(0, 4).toInt();
  int month = arrivalTime.substring(5, 7).toInt();
  int day = arrivalTime.substring(8, 10).toInt();
  int hour = arrivalTime.substring(11, 13).toInt();
  int minute = arrivalTime.substring(14, 16).toInt();
  int second = arrivalTime.substring(17, 19).toInt();

  // Debugging output
  Serial.print("Parsed Time: ");
  Serial.print(year);
  Serial.print("-");
  Serial.print(month);
  Serial.print("-");
  Serial.print(day);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

  // Create a tmElements_t structure
  tmElements_t tm;
  tm.Year = year - 1970; // Year since 1970
  tm.Month = month; // Month (1-12)
  tm.Day = day; // Day of the month (1-31)
  tm.Hour = hour; // Hour (0-23)
  tm.Minute = minute; // Minute (0-59)
  tm.Second = second; // Second (0-59)

  // Convert to UNIX timestamp
  return makeTime(tm); // Use the tmElements_t structure
}
// Function to load station data from SPIFFS
void loadStationData() {
  Serial.println("Started SPIFFS load");
  File configurations = SPIFFS.open("/data.txt", "r"); // Open the config file
  if (!configurations) {
    Serial.println("Failed to open config file");
    return;
  }

  bool headerSkipped = false; // To skip the header row
  while (configurations.available() && stationCount < 350) {
    String line = configurations.readStringUntil('\n');
    if (!headerSkipped) {
      headerSkipped = true; // Skip the header row
      continue;
    }

    // Step 1: Find the location (coordinates), which is the last part of the line
    int lastComma = line.lastIndexOf(',');  // Last comma separates Map ID and Coordinates
    int secondLastComma = line.lastIndexOf(',', lastComma - 1);  // Second last comma separates Map ID
    int thirdLastComma = line.lastIndexOf(',', secondLastComma - 1);  // Second last comma separates Map ID
    String location = line.substring(secondLastComma + 1);  // Coordinates after the last comma
    location.trim();  // Clean up any spaces
    // Example: "(41.879507, -87.626037)"

    // Step 2: Extract the Map ID (this is just before the coordinates)
    String mapID = line.substring(thirdLastComma + 1, secondLastComma); // Extract Map ID (between second last and last comma)
    mapID.trim();  // Clean up any spaces

    // Step 3: Find the Station Name, which is enclosed in quotes and before the first comma
    int firstComma = line.indexOf(',');  // Find the first comma
    int secondComma = line.indexOf(',', firstComma + 1);
    int thirdComma = line.indexOf(',', secondComma + 1);
    String stationName = line.substring(secondComma + 1, thirdComma);  // Extract Station Name
    stationName.trim();  // Clean up any extra spaces

    // Check if the Station Name is enclosed in quotes and remove the quotes if present
    if (stationName.startsWith("\"") && stationName.endsWith("\"")) {
      stationName = stationName.substring(1, stationName.length() - 1);  // Remove quotes
    }

    // Store the Map ID and Station Name in their respective arrays
    mapIDs[stationCount] = mapID; // Store Map ID
    stationNames[stationCount++] = stationName; // Store Station Name

    // Optional: Print out to verify the data parsing works
    //    Serial.print("Station Name: ");
    //    Serial.print(stationNames[stationCount - 1]);
    //    Serial.print(", Map ID: ");
    //    Serial.println(mapIDs[stationCount - 1]);
  }

  configurations.close();
}
void getArrivalData(String mapID, String stationName) {
  // Clear the screen for the new data
  oled.clear();

  // Print new arrival data
  oled.setCursorXY(0, 0);
  //oled.print("Loading data for: ");
  oled.update();
  // oled.setCursorXY(0,20);
  oled.setScale(1);
  oled.print(stationName); oled.print("  ");
  oled.print(mapID);
  oled.update();
  // Fetch and display the actual arrival data as you did before
  // (just replace lcd.print() with oled.print())

  // Example:
  // oled.setCursor(0, 10);
  // oled.print(eastDest); // Print East destination
  // oled.print(" ");
  // oled.print(eastBoundArrivals.substring(0, 16));
  // Repeat for west bound arrivals



  HTTPClient http;
  WiFiClient client; // Create a WiFiClient instance

  //  // Check Wi-Fi connection
  //  if (WiFi.status() != WL_CONNECTED) {
  //    Serial.println("WiFi not connected. Attempting to reconnect...");
  //    WiFi.begin(ssid, password); // Attempt to reconnect
  //    // Wait until we're connected
  //    while (WiFi.status() != WL_CONNECTED) {
  //      delay(500);
  //      Serial.print(".");
  //    }
  //    Serial.println("Connected to WiFi");
  //  }
  //  Serial.print("WiFi Status: ");
  //  Serial.println(WiFi.status());

  String url = "http://lapi.transitchicago.com/api/1.0/ttarrivals.aspx?key=" + API_KEY + "&mapid=" + mapID + "&outputType=JSON";

  Serial.print("Requesting URL: "); // Debugging output
  Serial.println(url); // Print the constructed URL
  http.setTimeout(5000); // Set timeout to 5 seconds
  http.begin(client, url); // Use the WiFiClient instance
  int httpCode = http.GET();

  // Check for HTTP response
  if (httpCode > 0) { // HTTP response code
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Arrival Data: ");
      Serial.println(payload);

      // Parse JSON
      DynamicJsonDocument doc(8192); // Adjust size as needed
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        JsonArray arrivals = doc["ctatt"]["eta"]; // Get the array of arrivals
        const char* currentTimeIso = doc["ctatt"]["tmst"]; // Get the current time in ISO format
        long currentTime = isoToUnixTimestamp(currentTimeIso); // Convert "cmst" to UNIX timestamp

        Serial.print("Current Time from JSON: ");
        Serial.println(currentTime); // Debugging output

        String eastBoundArrivals = ""; // For direction trDr = 1
        String westBoundArrivals = ""; // For direction trDr = 5

        String eastDest = ""; // Store East destination name
        String westDest = ""; // Store West destination name

        for (JsonVariant arrival : arrivals) {
          const char* direction = arrival["trDr"]; // Get direction
          long arrivalTimestamp = isoToUnixTimestamp(arrival["arrT"]); // Convert arrival time

          Serial.print("Arrival Timestamp: ");
          Serial.println(arrivalTimestamp);

          long minutesAway = (arrivalTimestamp - currentTime) / 60; // Calculate minutes away
          if (minutesAway < 0) {
            minutesAway = 0; // Avoid negative values
          }

          Serial.print("Minutes Away: ");
          Serial.println(minutesAway);

          // Get destination name for display
          String destName = arrival["destNm"].as<String>();
          if (destName.length() > 0) {
            Serial.print("Destination Name: ");
            Serial.println(destName); // Print destination name for debugging
          } else {
            Serial.println("Destination Name is empty");
          }

          if (strcmp(direction, "1") == 0 && eastDest.isEmpty()) { // East bound
            eastDest = destName.substring(0, 4); // Get abbreviated destination
          } else if (strcmp(direction, "5") == 0 && westDest.isEmpty()) { // West bound
            westDest = destName.substring(0, 4); // Get abbreviated destination
          }

          if (strcmp(direction, "1") == 0) { // Check for East bound
            eastBoundArrivals += String(minutesAway) + ",";
          } else if (strcmp(direction, "5") == 0) { // Check for West bound
            westBoundArrivals += String(minutesAway) + ",";
          }
        }

        //oled.clear();
        oled.setScale(1);
        oled.setCursorXY(0, 20); oled.update();
        oled.print(eastDest); // Print abbreviated East destinationoled.update();
        oled.print(" ");
        oled.update();
        oled.print(eastBoundArrivals.substring(0, 16)); // Limit to 16 characters
        oled.update();
        oled.setCursorXY(0, 40);; oled.update();
        oled.print(westDest); // Print abbreviated West destinationoled.update();
        oled.print(" ");
        oled.update();
        oled.print(westBoundArrivals.substring(0, 16)); // Limit to 16 characters
        oled.update();
      } else {
        Serial.print("JSON Deserialization failed: ");
        Serial.println(error.f_str());
      }
    } else {
      Serial.printf("HTTP GET failed: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end(); // Close connection
  }
}
void collectInput(const String &label) {
  String input = "";  // String to hold input
  int position = 0;   // Encoder position
  long lastEncoderPos = encoder.read();  // Keep track of encoder position to prevent flickering

  while (true) {
    long newPosition = encoder.read() / scalefactor;  // Read encoder position and scale
    if (newPosition != position) {
      position = newPosition;
      int charIndex = position % characterSet.length();
      enteredChar = characterSet.charAt(charIndex);  // Get the character
      Serial.println("Character viewed " + String(enteredChar)); // Debugging statement
      oled.clear();
      oled.setCursorXY(0, 0);
      oled.print(label);  // Show prompt for SSID or Password
      oled.setCursorXY(0, 10);
      oled.print("Char: ");
      oled.print(enteredChar);  // Display current character
      oled.update();
    }

    // Check if button is pressed to select character
    if (digitalRead(buttonPin) == LOW) {
      Serial.println("Button Pressed!");  // Debugging statement
      //scalefactor = 2;  // Adjust scaling factor
      delay(200);  // Simple debounce
      if (currentInput == "SSID") {
        Serial.print("enteredchar "); Serial.println(enteredChar);
        if (enteredChar == EOF_CHAR) {
          Serial.print("reached EOF ");
          break;
        }
        ssid += characterSet.charAt(position % characterSet.length()); // Add character to SSID
        Serial.print("SSID in progress: "); Serial.print(ssid);
        //      oled.clear();
        //      oled.setCursorXY(0, 10);
        //      oled.print(label);
        oled.setCursorXY(0, 10); oled.print(ssid); oled.update();
        Serial.println(ssid);  // Debugging statement to show accumulated SSID
        delay(500);

      } else if (currentInput == "PWD") {
        password += characterSet.charAt(position % characterSet.length());  // Add character to Password
        Serial.print("PWD in progress: ");
        Serial.println(password);  // Debugging statement to show accumulated Password
        delay(500);
      }

      // Update OLED display with the selected SSID and password
      oled.clear();
      oled.setCursorXY(0, 1);
      oled.print("SSID: ");
      oled.print(ssid);  // Display the current SSID


      // Check if SSID and Password are complete, break the loop if both are filled
      if (enteredChar == EOF_CHAR) {

        Serial.print("reached EOF ");
        oled.clear();
        oled.setCursorXY(0, 0);
        oled.print("Input complete!");
        oled.update();
        delay(1000);  // Show "complete" message for a moment
        break;  // Exit the loop after completion
      }

      // Switch to password input after SSID is filled
      if (enteredChar == EOF_CHAR) {

        currentInput = "PWD";  // Change to password mode
        oled.clear();
        oled.setCursorXY(0, 0);
        oled.print("Enter Password: ");
        oled.update();
      }
    }

    delay(100);  // Delay to prevent flickering and allow time to process encoder
  }
}

// Function to switch between SSID and Password input
void switchInputMode() {
  // Switch between SSID and password input by updating the currentInput string
  if (currentInput == "SSID") {
    currentInput = "PWD";  // Now input the password
    oled.setCursorXY(0, 0);
    oled.print("Enter Password:");
    oled.update();
  } else if (currentInput == "PWD") {
    currentInput = "SSID";  // Now input the SSID
    oled.setCursorXY(0, 0);
    oled.print("Enter SSID:");
    oled.update();
  }
}
