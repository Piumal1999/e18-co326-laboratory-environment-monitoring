/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
// #include <Adafruit_BME280.h>
// #include <Adafruit_Sensor.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "J-A-R-V-I-S";
const char* password = "11111111";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
// const char* mqtt_server = "65.0.173.201";
IPAddress mqtt_server(65, 0, 173, 201);//MQTT server IP



// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient22;
PubSubClient client(espClient22);


// Lamp - LED - GPIO 4 = D2 on ESP-12E NodeMCU board
const int led = 4;


//Connect your NodeMCU to your router
void setup_wifi() {
  delay(10);
  
  Serial.println();
 
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - NodeMCU IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your NodeMCU is subscribed to

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageInfo;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageInfo += (char)message[i];
  }
  Serial.println();

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if(topic=="room/light"){
      Serial.print("Changing Room Light to ");
      if(messageInfo == "on"){
        digitalWrite(led, HIGH);
        Serial.print("On");
      }
      else if(messageInfo == "off"){
        digitalWrite(led, LOW);
        Serial.print("Off");
      }
  }
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    
    if (client.connect("ESP8266Client22")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("room/light");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["sensor_a0"] = random(1000);
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client

  Serial.println( client.publish("gauge", jsonBuffer));
}


// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

// For this project, you don't need to change anything in the loop function. Basically it ensures that the NodeMCU is connected to MQTT broker
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  publishMessage();
   delay(1000);
  if(!client.loop())
    client.connect("ESP8266Client22");

  }
