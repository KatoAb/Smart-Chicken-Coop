#include <WiFi.h>
#include <PubSubClient.h>

#define LED 2   //Defining LED for simulating giving food

// Defining variables used for WiFi and MQTT.

const char* ssid = "Nettverksnavn";
const char* password =  "Nettverkspassort";
const char* mqttServer = "MQTT-IP adresse";
const int mqttPort = "MQTT Port";

unsigned long previousMillis = 0; 	  

//Setting up WiFi and PubSubClient for MQTT
WiFiClient espClient2;
PubSubClient client(espClient2);


void checkTime() {    //Checks if 5 seconds has passed since the LED has turned on. 
  if(millis() > (previousMillis + 5000)) {
    digitalWrite(LED, LOW);
  }
}

//The function runs when subscribed topic is received
void callback(char* topic, byte* payload, unsigned int length) {
 
  // Serial.print("Message arrived in topic: "); 
  // Serial.println(topic); 
  String messageTemp; 

  // Serial.print("Message:"); 
  for (int i = 0; i < length; i++) {    //Message over MQTT is saved

    messageTemp += ((char)payload[i]);   
  }
 
  if(String(topic) == "mating") {      //Checking if the topic and message is correct
    if(messageTemp == "true") {       
      // Serial.println("True");
      digitalWrite(LED, HIGH);        //Turn the LED high. If we had a physical feeding system, the function for activating it would be called here.
      client.publish("matingsHistorikk", "true");
      previousMillis = millis();      //Saving previousMillis as the current time.
      
    }
  }
 
}

//The code that runs when ESP32 starts.
void setup() {
 
  Serial.begin(9600);
 
  pinMode(LED, OUTPUT);    

  WiFi.begin(ssid, password);     //Connecting to WiFi
 
  while (WiFi.status() != WL_CONNECTED) {     
    delay(500);
    // Serial.println("Connecting to WiFi..");
  }
  // Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);       //Initializing MQTT
  client.setCallback(callback);                 //Initializing the callback function, that runs when topic is received.
 
  while (!client.connected()) {                 //Connecting to MQTT
    // Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP32Client")) {
 
      // Serial.println("connected");  
 
    } else {
 
      // Serial.print("failed with state ");
      // Serial.print(client.state());
      delay(100);
 
    }
  }
 
  client.subscribe("mating");  //Subscribing to topic
 
}
 
void loop() {                                  
  client.loop();              //Checking if topic is received on MQTT
  checkTime();                //Checking if 5 seconds has passed since LED has turned on (food simulation)
}
