#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

// WIFI & SERVER
const char* ssid = "Redmi Note 13 Pro 5G";
const char* password = "113333555555";
const char* mqtt_server = "broker.hivemq.com";

// PIN LED/WARNA
const int redPin = 18;
const int greenPin = 17;
const int bluePin = 16;

// Kondisi LED
int colorRed = 0;
int colorGreen = 0;
int colorBlue = 0;
bool colorRGB = false;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();

  // Mengatur warna LED berdasarkan pesan yang diterima
  if (String(topic) == "lampu/subPubTopic") {
    Serial.print("Keterangan: ");
    Serial.println(message);
    if (message == "red" || message == "green" || message == "blue" || message == "allOn") {
      setColor(message);
    } else if (message == "allOff") {
      // Matikan semua LED
      colorRed = 0;
      colorGreen = 0;
      colorBlue = 0;
      analogWrite(redPin, colorRed);
      analogWrite(greenPin, colorGreen);
      analogWrite(bluePin, colorBlue);
      Serial.println("Semua warna mati.");
    } else if (message == "rgb") {
      rgbColors();
    }
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("cobaLampu")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("lampu/pubSubTopic", "hello world");
      // ... and resubscribe
      client.subscribe("lampu/subPubTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "Terkirim #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("lampu/pubSubTopic", msg);
  }

  delay(5000);
}

void setColor(String color) {
  if (color == "red") {
    colorRed = colorRed == 0 ? 255 : 0;
  } else if (color == "green") {
    colorGreen = colorGreen == 0 ? 255 : 0;
  } else if (color == "blue") {
    colorBlue = colorBlue == 0 ? 255 : 0;
  } else if (color == "allOn") {
    colorRed = 255;
    colorGreen = 255;
    colorBlue = 255;
  }

  // Set nilai untuk LED
  analogWrite(redPin, colorRed);
  analogWrite(greenPin, colorGreen);
  analogWrite(bluePin, colorBlue);
  
  Serial.print("Warna yang dinyalakan: R=");
  Serial.print(colorRed);
  Serial.print(" G=");
  Serial.print(colorGreen);
  Serial.print(" B=");
  Serial.println(colorBlue);
}

void rgbColors() {
  colorRGB = true;
  
  for (int i = 0; i < 10; i++) {
    // Red blink
    analogWrite(redPin, 255);
    delay(100);
    analogWrite(redPin, 0);

    // Green blink
    analogWrite(greenPin, 255);
    delay(100);
    analogWrite(greenPin, 0);

    // Blue blink
    analogWrite(bluePin, 255);
    delay(100);
    analogWrite(bluePin, 0);
  }
  
  colorRGB = false;
}