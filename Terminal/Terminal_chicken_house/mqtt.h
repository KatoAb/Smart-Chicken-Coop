
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  String messageTemp;

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {

    messageTemp += ((char)payload[i]);
  }
  Serial.println(messageTemp);
 
  if(String(topic) == "tempOut") {
    airTemp = atof(messageTemp.c_str());
    
  }

  if(String(topic) == "humOut") {
    airHum = atof(messageTemp.c_str());
  }
  
  if(String(topic) == "gasOut") { 
    airVOC = atof(messageTemp.c_str());
    //set text color
    if(airVOC >= 75){ //set color to text depending on value
      IAQtextColor = GREEN;
  }
    else if (airVOC < 75 && airVOC >= 40){
      IAQtextColor = ORANGE;
  }
    else{
      IAQtextColor = RED;
  }
  }

  if(String(topic) == "waterTempOut") {
    waterTemp = atof(messageTemp.c_str());
  }

  if(String(topic) == "PHOut") {
    PHValue = atof(messageTemp.c_str());

    if(PHValue < 8.5 && PHValue > 6.5){ //set color to text depending on value
      PHtextColor = GREEN;
    }
    else {
      PHtextColor = RED;
    }
    
  }

  if(String(topic) == "foodTimeOut") {
    foodTime = (messageTemp.c_str());
  }

  if(String(topic) == "waterLevelOut") {
    waterLevel = atof(messageTemp.c_str());

  if(waterLevel >= 75){ //set color to text depending on value
    waterLevelTextColor = GREEN;
  }
  else if (waterLevel < 75 && waterLevel >= 40) {
    waterLevelTextColor = ORANGE;
  }
  else{
    waterLevelTextColor = RED;
  }
  }
 
}

void subToClient(){ //subscribe to nodered client
  client.subscribe("humOut");
  client.subscribe("gasOut");
  client.subscribe("tempOut");
  client.subscribe("waterTempOut");
  client.subscribe("PHOut");
  client.subscribe("foodTimeOut");
  client.subscribe("waterLevelOut");
  client.subscribe("rLED");
  client.subscribe("gLED");
  client.subscribe("setTempOut");

}


void wifiConnect() {
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("eggCounter")) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(100);
 
    }
  }
}

void sendData(){  
  Serial.print("Publishing number of eggs: ");   
  Serial.println(inputString);   
  client.publish("eggInn", inputString.c_str());
}

 
 
 
