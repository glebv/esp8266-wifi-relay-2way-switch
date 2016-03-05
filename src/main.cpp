#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "PubSubClient.h"
#include "Bounce2.h"
#include "credentials.cpp"


//-- MQTT
char stateTopic[] = "outdoor/highlight";
char cmdTopic[] = "outdoor/highlight/set";
char topicValue[] = "OFF";

//-- GPIO
#define relayPin 5 //BUILTIN_LED //5
#define buttonPin 4

//-- SETTINGS
const int mqttTimeReconect = 5000;
const int buttonPressHoldTime = 500;
const int showWifiReconnectionTime = 1000;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
Bounce bouncer = Bounce();

bool showWiFiStatus = false;
long lastMsg = 0;
long lastMqttReconnect = 0;
long lastWiFiReconnect = 0;
String topicMsg;
int prevButtonState = LOW;

void setupGpio() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  pinMode(buttonPin, INPUT);
  bouncer.attach(buttonPin);
  bouncer.interval(5);
}

//--- WiFi
void setupWifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifiSsid);

  WiFi.begin(wifiSsid, wifiPassword);
}

void reconnectWiFi() {
  long now = millis();
  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWiFiReconnect > showWifiReconnectionTime) { //show reconnect status
      lastWiFiReconnect = now;
      Serial.println("Failed connect to WiFi...");
      showWiFiStatus = false;
    }
  } else {
    if(!showWiFiStatus) {
      showWiFiStatus = true;
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.print("RSSI: ");
      Serial.println(WiFi.RSSI());
    }
  }
}
//-- MQTT

void setRelayState(uint8_t state) {
  //state should be inverted in according with the circuit
  digitalWrite(relayPin, !state);
}

char messageBuff[100];
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  for(i=0; i<length; i++) {
    messageBuff[i] = payload[i];
  }
  messageBuff[i] = '\0';

  String topicPayload = String(messageBuff);

  Serial.print("Recived for topic: ");
  Serial.print(topic);
  Serial.println(" data: " + topicPayload);
  setRelayState((topicPayload == "ON") ? HIGH : LOW);
}

void setupMqtt() {
  client.setServer(mqttServerIp, 1883);
  client.setCallback(mqttCallback);
}

String macToStr(const uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void reconnectMqtt() {
  long now = millis();
  // Loop until we're reconnected
  if (!client.connected() && (now - lastMqttReconnect > mqttTimeReconect)) {
    lastMqttReconnect = now;
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("esp8266HiglightOutdoor", mqttUser, mqttPassword)) {
      Serial.println("connected");
      if(client.subscribe(cmdTopic)) {
        Serial.print("Subscribed on topic: ");
        Serial.println(cmdTopic);
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
    }
  }
}

void setup() {
  Serial.begin(115200);
  setupGpio();
  setupWifi();
  setupMqtt();
}


void loop() {
  bouncer.update();
  int buttonState = bouncer.read();
  long now = millis();

  reconnectWiFi();

  if (!client.loop()) {
    reconnectMqtt();
  }

  if (now - lastMsg > buttonPressHoldTime) { //you should press the button more than
    lastMsg = now;

    if(prevButtonState != buttonState) {
      topicMsg = "OFF";
      if(buttonState == HIGH) {
        topicMsg = "ON";
      }

      if(client.publish(stateTopic, topicMsg.c_str())) {
        Serial.print("Topic: ");
        Serial.print(stateTopic);
        Serial.print(" ");
        Serial.println(topicMsg +"; Published Ok"); //it sends ON by default on nodemcu
      } else {
        Serial.print("Topic: ");
        Serial.print(stateTopic);
        Serial.print(" ");
        Serial.println(topicMsg +"; Published failed");
      }

      setRelayState(buttonState);
      prevButtonState = buttonState;
    }
  }
}
