#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
 
const char* ssid = "Danush"; // Enter your WiFi name
const char* password =  "dan2thi1985"; // Enter WiFi password
const char* mqttServer = "192.168.1.53";
const int mqttPort = 1883;
const char* mqttUser = "mqtt";
const char* mqttPassword = "dan2thi1985";
 
WiFiClient espClient;
PubSubClient client(espClient);
Servo servoState;

String modes[3] = { "heat", "cool", "off" };
float temps[3] = {25.0, 20.5, 0.0};
int currentStatePointer = 0;
unsigned long lastStateChanged = millis();


void setup() {
  Serial.begin(115200);
  wifiConnect();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  Serial.println("Current Heat Mode = " + modes[currentStatePointer]);
}

void loop() {
  if (!client.connected())
    reconnect();
  client.loop();
  delay(3000);
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
      client.subscribe("heating/mode/set");
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
  if (strcmp(topic,"heating/mode/set") == 0){
    doSetMode(message);
    Serial.print("Current State : ");
    Serial.print(modes[currentStatePointer]);
    String currTemp = String(temps[currentStatePointer]);
    char tempCharArray[ currTemp.length() + 1];
    currTemp.toCharArray(tempCharArray, currTemp.length() + 1);
    Serial.println("");
    Serial.print("Current Temperature = ");
    Serial.println(currTemp);
    client.publish("heating/mode/state", modes[currentStatePointer].c_str());
    client.publish("heating/temp/state", tempCharArray);
  }
   Serial.println("-----------------------");
}
 
void doSetMode(String mode){
  int i;
  for (i = 0; i < 3; i++) {
    if (modes[currentStatePointer].equalsIgnoreCase(mode)){
      break;
    } else {
      currentStatePointer = (++currentStatePointer)%3;
    }
  }
  if (i == 0)
    return;
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - lastStateChanged;
  Serial.print("ElapsedTime : ");
  Serial.println(ElapsedTime);
  if (ElapsedTime > 7000)
    i++;
  doPress(servoState, 24, i);
  if (i > 0){
    lastStateChanged = millis();
  }
  Serial.print("pressing mode button times = ");
  Serial.println(i);
}

void doPress(Servo servo, int pos, int times) {
  servo.attach(D4);
   for (int i = 0; i < times; i++) {
      delay(15);
      servo.write(pos);
      //delay(650);
     // servo.write(90);
      delay(500);
      servo.write(0);
      delay(1000);
   }
   servo.detach();
}
