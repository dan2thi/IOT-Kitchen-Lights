#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
 
const char* ssid = "Danush"; // Enter your WiFi name
const char* password =  "dan2thi1985"; // Enter WiFi password
const char* mqttServer = "192.168.1.53";
const int mqttPort = 1883;
const char* mqttUser = "mqtt";
const char* mqttPassword = "dan2thi1985";
const char* subscribedChannel = "kitchen/lights/set";
const char* publishChannel = "kitchen/lights/state";
 
WiFiClient espClient;
PubSubClient client(espClient);
Servo servoState;

unsigned long lastStateChanged = millis();

void setup() {
  Serial.begin(115200);
  wifiConnect();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected())
    reconnect();
  client.loop();
  delay(2000);
}

void wifiConnect(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}

void reconnect(){
  if(WiFi.status() != WL_CONNECTED){
    wifiConnect();
  }
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("connected");  
      client.subscribe(subscribedChannel);
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(5000);
    }
  }
}
 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String message = String("");
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  if (strcmp(topic, subscribedChannel) == 0){
    String state = doChangeSwitch(message);
    Serial.print("State = ");
    Serial.println(state);
    if (!String("-1").equals(state))
      client.publish(publishChannel, state.c_str());
  }
   Serial.println("-----------------------");
}
 
String doChangeSwitch(String message) {
  String ret;
  servoState.attach(D4);
  delay(15);
  if (String("ON").equalsIgnoreCase(message)){
    servoState.write(130);
    ret = "ON";
  } else if (String("OFF").equalsIgnoreCase(message)){
    servoState.write(15);
    ret = "OFF";
  } else {
    ret = "-1";
  }
  delay(1000);
  servoState.detach();

  return ret;
}
