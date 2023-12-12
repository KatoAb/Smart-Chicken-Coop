  /*This code is for streaming and camera-setting changes directly
  using only mqtt and Node-RED. Using Ai-thinker ESP32cam the performance due to
  hardware limitations may be slow. 
  Code that is commented out are for controlling servos, due to the high workload and temperature
  it is not recommended using. 
  See the webserver solution for the most smooth experience with servo control in file "WebServerCam".
  */

  #include "esp_camera.h"
  #include <WiFi.h>
  #include <PubSubClient.h>
  #include <ArduinoJson.h>
  
  /*#include <ESP32Servo.h>


  #define SERVO_X 15
  #define SERVO_Y 14
  Servo servoX;
  Servo servoY;
  bool autoControl;
  */

  // Declares CONFIG variable with class DynamicJsonDocument and size 2048 bytes
  DynamicJsonDocument CONFIG(2048); 

  // Wi-Fi credential
  const char* ssid = "your network ssid";
  const char* password = "your password";

  // MQTT server information
  const char* mqtt_server = "178.164.41.198";

  // Hostname for the device
  const char* HostName = "choose hostname";

  
  const char* streaming = "streaming"; //declare this topic/variable globally so it can be used in the take_picture() function
  const char* mqttUser = "admin";
  const char* mqttPassword = "yuiooiuy";
  
  // Wi-Fi client and MQTT client
  WiFiClient espClient;
  PubSubClient client(espClient);

  // Function to apply camera settings based on the provided JSON configuration
  void applyCameraSettings(const JsonObject &config) {
  sensor_t * s = esp_camera_sensor_get();
  if (!s) { // Check if camera sensor is available
    Serial.println("Failed to get camera sensor");
    return;
  }

  // Apply settings if they are present in the JSON
  if (config.containsKey("framesize")) s->set_framesize(s, (framesize_t)config["framesize"].as<int>());
  if (config.containsKey("vflip")) s->set_vflip(s, config["vflip"]);
  if (config.containsKey("hmirror")) s->set_hmirror(s, config["hmirror"]);
  if (config.containsKey("colorbar")) s->set_colorbar(s, config["colorbar"]);
  if (config.containsKey("special_effect")) s->set_special_effect(s, config["special_effect"]);
  if (config.containsKey("quality")) s->set_quality(s, config["quality"]);
  if (config.containsKey("brightness")) s->set_brightness(s, config["brightness"]);
  if (config.containsKey("contrast")) s->set_contrast(s, config["contrast"]);
  if (config.containsKey("saturation")) s->set_saturation(s, config["saturation"]);
  if (config.containsKey("sharpness")) s->set_sharpness(s, config["sharpness"]);
  if (config.containsKey("denoise")) s->set_denoise(s, config["denoise"]);
  if (config.containsKey("awb_gain")) s->set_awb_gain(s, config["awb_gain"]);
  if (config.containsKey("wb_mode")) s->set_wb_mode(s, config["wb_mode"]);
}

// Initial position for the servo
int pos = 90;

// MQTT callback function for processing incoming messages
void callback(char* topic, byte* message, unsigned int length) {
  // Convert the incoming topic to a String for easier comparison
  String topicStr = String(topic);
   // Print information about the received message
  Serial.print("Message arrived on topic: ");
  Serial.print(topicStr);
  Serial.print(". Message: ");
  String messageTemp;
  // Process the bytes of the incoming message
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  //Uncomment following code for servo control:
   /*
   
   // Check if the topic is "autoControl"
   if (topicStr == "autoControl") {
    // If the message indicates "true", set autoControl to true
    if (messageTemp == "true") {
      autoControl = true;
    } 
    // If the message indicates "false", set autoControl to false
    else if (messageTemp == "false") {
      autoControl = false;
    }
  }
  
  
  // Check if autoControl is false
  if(!autoControl){
    // If the topic is "servoX"
    if (topicStr == "servoX") {
        // Convert the message to an integer and assign it to pos
         pos = messageTemp.toInt();
         // Move servoX to the specified position
         servoX.write(pos);
    } 
    // If the topic is "servoY"
    else if (topicStr == "servoY") {
      // Convert the message to an integer and assign it to pos
      int pos = messageTemp.toInt();
      // Move servoY to the specified position
      servoY.write(pos);
    }
  } 

*/


 // Check if the received topic is "config"
 if (topicStr == "config") {
  Serial.println("Received CONFIG command");
  //Create a JSON document for parsing the configuration message
  StaticJsonDocument<512> doc; 
  //Parse the incoming JSON message
  DeserializationError error = deserializeJson(doc, message, length);
  if (error) {
    Serial.println("Failed to parse config message");
    return;
  }
  //Extract the JsonObject from the parsed JSON document
  JsonObject config = doc.as<JsonObject>();
  //Apply camera settings based on the received configuration
  applyCameraSettings(config);
    }
}

// Function to initialize the camera with specified configuration
  void camera_init() {
    // Configure camera settings
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = 5;
    config.pin_d1       = 18;
    config.pin_d2       = 19;
    config.pin_d3       = 21;
    config.pin_d4       = 36;
    config.pin_d5       = 39;
    config.pin_d6       = 34;
    config.pin_d7       = 35;
    config.pin_xclk     = 0;
    config.pin_pclk     = 22;
    config.pin_vsync    = 25;
    config.pin_href     = 23;
    config.pin_sscb_sda = 26;
    config.pin_sscb_scl = 27;
    config.pin_pwdn     = 32;
    config.pin_reset    = -1;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    config.frame_size   = FRAMESIZE_QVGA; // QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;           
    config.fb_count     = 1;

    // Initialize the camera with the specified configuration
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
    }

  }

// Function to capture and publish a photo from the camera
  void take_picture() {
    // Attempt to get a camera frame buffer
    camera_fb_t *fb = esp_camera_fb_get();
    // Check if capturing the frame buffer was successful
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }
    // Attempt to publish the captured photo to the MQTT topic
    if (client.publish_P(streaming, fb->buf, fb->len, false)) { //the publish_P means the image is stored in the flash memory of esp32cam before sending, this is to save RAM.
      Serial.println("Published photo successfully");           
    } else {
      Serial.println("Failed to publish photo");
    }
    // Return the camera frame buffer to the camera driver
    esp_camera_fb_return(fb);
  }

// Function to set up Wi-Fi connection
  void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    // Set Wi-Fi mode to Station (client) mode
    WiFi.mode(WIFI_STA);
    // Set the hostname for the ESP32
    WiFi.setHostname(HostName);
    // Begin Wi-Fi connection with the specified SSID and password
    WiFi.begin(ssid, password);
    // Wait for the Wi-Fi connection to be established
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    //servoX.attach(SERVO_X); //uncomment for servo control
    //servoY.attach(SERVO_Y); //uncomment for servo control
  }

// Function to reconnect to the MQTT broker
  void reconnect() {
    // Continue attempting to connect until successful
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect to the MQTT broker with specified client ID, username, and password
      if (client.connect(HostName, mqttUser, mqttPassword)) {
        Serial.println("connected");

        // Uncomment the following lines for servo control
        /*client.subscribe("servoX");
        client.subscribe("servoY");
        client.subscribe("autoControl");*/
        client.subscribe("config");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait for 5 seconds before attempting to reconnect
        delay(5000);
      }
    }
  }

// Setup function executed once on program start
  void setup() {
  // Begin serial communication at a baud rate of 115200
  Serial.begin(115200);
  // Initialize the camera settings
  camera_init();
  // Set up the Wi-Fi connection
  setup_wifi();
  // Set the MQTT server and port for the MQTT client
  client.setServer(mqtt_server, 1883);
  // Set the callback function to handle incoming MQTT messages
  client.setCallback(callback);
  // Make sure to subscribe to the config topic
}
//Uncomment this chunk of code if you want to use servos:
/*

int intervall = 1; // Steps per servo moment
unsigned long servoTimer = 0; 

int dir =1; // Direction of servo movement (1 for increasing, -1 for decreasing)

// Function to control servo position automatically based on a timer
void autoFunction(){
  // Check if autoControl mode is true
  if (autoControl) {
    // Check if enough time has elapsed since the last servo movement
    if (millis() - servoTimer >= 70) {
      // Set the servo position
      servoX.write(pos);
      // Increment or decrement the servo position based on the direction and step size
      pos = pos + (intervall * dir);
      if (pos >= 180) {
        dir = -1; // Change direction if the servo reaches its maximum angle
      }
      if (pos <= 0) {
        dir = 1; // Change direction if the servo reaches its minimum angle
      }
      const char* xPos = "xPos";
      // Publish the current servo position to the "xPos" MQTT topic
      client.publish(xPos, String(pos).c_str());
      // Reset the timer for the next servo movement
      servoTimer = millis();
    }
}

*/
unsigned long capturetimer = 0;


  void loop() {
    // Check if the MQTT client is not connected, attempt to reconnect
    if (!client.connected()) {
      reconnect();
    }
    // Allow the MQTT client to handle incoming messages
    client.loop();
     // Check if enough time has elapsed since the last image capture
    if(millis()-capturetimer>=200){
      // Trigger the function to capture and publish a photo
      take_picture();
      // Reset the timer for the next image capture
      capturetimer = millis(); 
  }
    /*Uncomment the following line if you want to use servos: */
    //autoFunction();
  
  }
