/* ESP8266 AWS IoT example by Evandro Luis Copercini
   Public Domain - 2017
   But you can pay me a beer if we meet someday :D
   This example needs https://github.com/esp8266/arduino-esp8266fs-plugin.,m.......................................................................................................,
  It connects to AWS IoT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
  -WARNING: this example doesn't verify the server CA due low heap, this can be a security issue
*/
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards


#include <ArduinoJson.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
int pos = 0;    // variable to store the servo position
// Update these with values suitable for your network.
const char* ssid = "xxx";int count=0;
const char* password = "xx";
const char* AWS_endpoint = "xx"; //MQTT broker ip

int desval = 0; //desired (for control)value received from shadow corresponding to led value
#define relay D5  //ALERT ALERT messing with this. it should be D2
#define sensor D2  //ALERT ALERT messing with this. it should be D5
#define Ground D8
#define interval 3000
int sensorstate;
char current_doorstate, last_doorstate;
/////////////////////\\\\\\\\\\callback\\\\\\\\\///////////////////
void callback(char* topic, byte* payload, unsigned int length) {
  desval = 0;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 31; i < 33; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.print("     des_val=");
  for (int i = 31; i < 33; i++) {
    desval = desval * 10 + payload[i] - '0';
  }
  Serial.print("Callback:Desired value received via shadow =");
  //Serial.println(desval);
  if (desval == 66) {
    toggle();
   Serial.println(desval);
    desval = 0;
  } else desval = 0;
}
/////////////////////\\\\\\\\\\callback end\\\\\\\\\///////////////////
WiFiClientSecure espClient20;
PubSubClient client(AWS_endpoint, 8883, callback, espClient20); //set  MQTT port number to 8883 as per //standard
long lastMsg = 0;
char msg[50];
int value = 0;
/////////////////////\\\\\\\\\\wifi setup \\\\\\\\\///////////
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
/////////////////////\\\\\\\\\\wifi setup end\\\\\\\\\///////////////////
/////////////////////\\\\\\\\\\THE setup \\\\\\\\\///////////
void setup() {
myservo.attach(D5);  // attaches the servo on pin 9 to the servo object
  pinMode(relay, OUTPUT);
  pinMode(sensor, INPUT_PULLUP); pinMode(Ground, OUTPUT); digitalWrite(Ground, LOW);
  last_doorstate = 'C'; //arbitrary initial value.



  Serial.begin(115200);
  Serial.setDebugOutput(true);
  setup_wifi(); Serial.println("got here");
  delay(1000);
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
  // Load certificate file
  File cert = SPIFFS.open("/5e0fec0b44-certificate.pem.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert) {
    Serial.println("Failed to open cert file");
  }
  else
    Serial.println("Success to open cert file");
  delay(1000);
  if (espClient20.loadCertificate(cert))
    Serial.println("cert loaded");
  else
    Serial.println("cert not loaded");
  // Load private key file
  File private_key = SPIFFS.open("/5e0fec0b44-private.pem.der", "r"); //replace private eith your uploaded file name
  if (!private_key) {
    Serial.println("Failed to open private cert file");
  }
  else
    Serial.println("Success to open private cert file");
  delay(1000);
  if (espClient20.loadPrivateKey(private_key))
    Serial.println("private key loaded");

  Serial.println("private key not loaded");
  /*
    // Load CA file
    File ca = SPIFFS.open("/ca.der", "r"); //replace ca eith your uploaded file name
    if (!ca) {
      Serial.println("Failed to open ca ");
    }
    else
    Serial.println("Success to open ca");
    delay(1000);
    if(espClient20.loadCACert(ca))
    Serial.println("ca loaded");
    else
    Serial.println("ca failed");
  */
  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
  
}
/////////////////////\\\\\\\\\\THE setup end \\\\\\\\\///////////
/////////////////////\\\\\\\\\\THE LOOP \\\\\\\\\///////////
void loop() {
  if (!client.connected()) {
    count=count+1;
    reconnect();Serial.print("COUNT =");Serial.println(count);
  }

  client.loop();
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    // ++value;
    //  snprintf (msg, 75, "hello world #%ld", value);
    //  Serial.print("Publish message: ");
    // Serial.println(msg);

    if ((client.subscribe("$aws/things/iot2/shadow/update"))) {
      //Serial.print("Recvd     desval=");

     //Serial.println(desval);
    }
    else {
      Serial.println("nothing");
    }

    //  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap()); //Low heap can cause problems
  }
  sensval();
  if (last_doorstate != current_doorstate) {
    last_doorstate = current_doorstate;
    Serial.print(" doorstate changed="); Serial.println(last_doorstate);
    if (last_doorstate == 'C') {
      client.publish("$aws/things/iot2/shadow/update", " {\"state\":{\"reported\":{\"value\":11}}} ");
    }
    else {
      client.publish("$aws/things/iot2/shadow/update", " {\"state\":{\"reported\":{\"value\":99}}} ");
    }

  }

}
/////////////////////\\\\\\\\\\THE LOOP END \\\\\\\\\///////////
/////////////////////\\\\\\\\\\Reconnect \\\\\\\\\///////////
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("espClient202")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      // client.subscribe("$aws/thing/iot2/shadow/update");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
/////////////////////\\\\\\\\\\Reconnect END \\\\\\\\\///////////
/////////////////////\\\\\\\\\\toggle\\\\\\\\\///////////
void toggle()
{/*
  digitalWrite(relay, HIGH);
  delay(interval);
  digitalWrite(relay, LOW);
  delay(interval);
*/

Serial.print("Toggle2 value received =");Serial.println(desval);
grabber();
}
/////////////////////\\\\\\\\\\toggle END \\\\\\\\\///////////
/////////////////////\\\\\\\\\\detects current state of door \\\\\\\\\///////////
void sensval()
{
  sensorstate = digitalRead(sensor);
  if (sensorstate == LOW) {
    current_doorstate = 'O';
  }
  else {
    current_doorstate = 'C';
  }
  //Serial.println(current_doorstate);
  digitalWrite(Ground, LOW);
}
/////////////////////\\\\\\\\\\detects current state of door \\\\\\\\\///////////
void grabber() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }

}


