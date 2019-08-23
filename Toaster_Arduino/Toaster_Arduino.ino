#include <ESP8266WiFi.h>
#include <MQTT.h>

const char ssid[] = "ALSW";
const char pass[] = "25264897";

int HornoToaster = 12;
int SensorPuerta = 13;
int estadoHorno = 0;
int estadoPuerta = 0;
long InicioHorneo = 0;
long TiempoHornear = 1;

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

  client.subscribe("/toaster/Horno/mensaje");
  client.subscribe("/toaster/Horno/tiempo");

  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if (topic == "/toaster/Horno/mensaje") {
    if (payload == "1") {
      digitalWrite(HornoToaster , 1);
      Serial.println("Encender horno");
      estadoHorno = 1;
      InicioHorneo = millis();
    }
    else {
      Serial.println("Apagar horno");
      digitalWrite(HornoToaster , 0);
      estadoHorno = 0;
    }
  }
  else if (topic == "/toaster/Horno/tiempo") {
    if (estadoHorno == 0) {
      client.publish("/toaster/Horno/status", "Apagado");
    }
    else {
      long TiempoElaborado = millis() - InicioHorneo;
      long Segundos = TiempoElaborado / 1000;
      long Minutos = Segundos / 60;
      Segundos = Segundos % 60;
      String MensajeTiempo = String(int(Minutos));
      MensajeTiempo = MensajeTiempo + ":";
      if (Segundos < 10) {
        MensajeTiempo = MensajeTiempo + "0";
      }
      MensajeTiempo = MensajeTiempo + String(int(Segundos));
      client.publish("/toaster/Horno/status", MensajeTiempo);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

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

  if (digitalRead(SensorPuerta)) {
    estadoPuerta = false;
  }
  else {
    estadoPuerta = true;
  }


  if (millis() - lastMillis > 1000) {
    String mensaje = String(estadoPuerta);
    client.publish("/toaster/puerta/estado", mensaje);
    mensaje = String(estadoHorno);
    client.publish("/toaster/Horno/estado", mensaje);
    lastMillis = millis();
  }


  if (estadoHorno == 1) {
    long TiempoElaborado = millis() - InicioHorneo;
    long Segundos = TiempoElaborado / 1000;
    long Minutos = Segundos / 60;
    Serial.println(Segundos);
    //Serial.print("  ");
    //Serial.print(Minutos);
    if (Minutos >= TiempoHornear) {
      Serial.println("Se apago el horno");
      estadoHorno = 0;
      digitalWrite(HornoToaster, 0);
      client.publish("/toaster/Horno/estado", "3");
    }

  }

}
