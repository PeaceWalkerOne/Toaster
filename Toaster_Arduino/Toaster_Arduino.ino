// This example uses an Adafruit Huzzah ESP8266
// to connect to shiftr.io.
//
// You can check on your device after a successful
// connection here: https://shiftr.io/try.
//
// by Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt

#include <ESP8266WiFi.h>
#include <MQTT.h>

const char ssid[] = "ALSW";
const char pass[] = "25264897";

int HornoToaster = 12;
int SensorPuerta = 13;
int estadoHorno = 0;
int estadoPuerta = 0;

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void connect() {
  pinMode(HornoToaster, OUTPUT);
  pinMode(SensorPuerta, INPUT);

  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "PaperBot", "MAQUINADEGUERRAESLAONDA")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/hello");
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    String mensaje = String(estadoPuerta);
    client.publish("/toaster/puerta", mensaje);
    mensaje = String(estadoHorno);
    client.publish("/toaster/Horno/estado", mensaje);
    
    lastMillis = millis();

  }
}
