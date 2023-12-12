
/* Parts of this code have been taken from Espressif Systems' documentation code for ESP32cam. 
This code runs the most smooth for our purposes. For code that lets you change camera settings
and stream directly through MQTT without a web browser see file "MQTTCam";
due to hardware limitations it runs slower, however you'll benefit from being able to send videostreams and have images be displayed in
the Node-RED dashboard to a port forwarded Raspberry Pi outside the local network.
*/

//Defining variables and including necessary libraries
#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

#define SERVO_X 15
#define SERVO_Y 14


Servo servoX;
Servo servoY;
bool autoControl;
int pos = 90; //Initial position of the servos attached to the camera

#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"

//credentials
const char* ssid = "your wifi ssid";
const char* password = "your network password";
const char* mqtt_server = "your ip adress";
const char* mqttUser = "mqtt username";
const char* mqttPassword = "mqtt password";


WiFiClient espClient;
PubSubClient client(espClient);

void startCameraServer();
void setupLedFlash(int pin);

void setup_wifi(){
  // Begin Wi-Fi connection with provided SSID and password
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  // Wait for the Wi-Fi connection to be established
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();  // Start the camera server

  // Print the IP address for accessing the camera
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Attaching servos to pins
  servoX.attach(SERVO_X);
  servoY.attach(SERVO_Y);

  // Setting up WiFi and MQTT connection
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Configuring camera settings
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(config.pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

  // Initializing the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if(config.pixel_format == PIXFORMAT_JPEG){
    s->set_framesize(s, FRAMESIZE_QVGA);
  }



// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif
}


// MQTT message callback
void callback(char* topic, byte* message, unsigned int length) {
  String topicStr = String(topic);
  Serial.print("Message arrived on topic: ");
  Serial.print(topicStr);
  Serial.print(". Message: ");
  String messageTemp;
  
  // Processing the received message
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  
  // Handling what to do when autocontrol is turned on by user
   if (topicStr == "autoControl") {
    if (messageTemp == "true") {
      autoControl = true; //autoControl variable determines what if statesment should run
    } else if (messageTemp == "false") {
      autoControl = false;
    }
  }
  
  
  /* If autoControl is false then the position of the servos are
  changed by the user in the Node-RED dashboard where you can alter
  the servos controling the camera in x position and/or y position*/
  if(!autoControl){      
    // Check the received MQTT topic
    if (topicStr == "servoX") {
        pos = messageTemp.toInt();
        servoX.write(pos); // Set the servoX position to the received value
      } 
    else if (topicStr == "servoY") {
        int pos = messageTemp.toInt();
        servoY.write(pos); //Set the servoY position to the received value
        }

  }
}

// Reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    if (client.connect("ESP32Cam", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT");
      client.subscribe("servoX");
      client.subscribe("servoY");
      client.subscribe("autoControl");
    } else {
      Serial.print("Failed to connect to MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 5 seconds...");
      delay(5000);
    }
  }
}


int intervall = 1; // Steps per increment
unsigned long servoTimer = 0; //Timer to control the interval between servo movements
int dir =1; // Direction of servo movement (1 for increasing, -1 for decreasing)

void autoFunction(){
  // Control servo position based on a timer
  if (millis() - servoTimer >= 70) {
      servoX.write(pos); // Set the servo position
      pos = pos + (intervall * dir);
      if (pos >= 180) {
        dir = -1; // Change the direction to decrease when reaching the maximum angle (180 degrees)
      }
      if (pos <= 0) {
        dir = 1; // Change the direction to increase when reaching the minimum angle (0 degrees)
      }
      const char* xPos = "xPos";
      client.publish(xPos, String(pos).c_str()); // Publish the current servo position to MQTT
      servoTimer = millis();   // Reset the timer for the next servo movement 
    }
}

void loop() {
  // Reconnect to MQTT if not connected
   if (!client.connected()) {
    reconnect();
  }
  // Handle MQTT messages
  client.loop();
  
  /*if autoControl is true then run the autofunction repeatedly,
  effectively making the servos change position automatically until turned off*/
  if (autoControl) {
    autoFunction();
  }
}
