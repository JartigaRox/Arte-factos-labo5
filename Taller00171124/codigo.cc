#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ======================================================
// CONFIGURACIÓN WIFI
// ======================================================

const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";


// ======================================================
// CONFIGURACIÓN MQTT
// ======================================================


const char* MQTT_SERVER = "";
const int MQTT_PORT = 1883;

// Topics utilizados por el dashboard React.
const char* TOPIC_TEMPERATURA = "clima/esp32/temperatura";
const char* TOPIC_HUMEDAD = "clima/esp32/humedad";
const char* TOPIC_LUZ = "clima/esp32/luz";


// ======================================================
// CONFIGURACIÓN DE SENSORES
// ======================================================

// (Estudiante)
// Definir los pines y objetos necesarios para los sensores.
#define DHT_pin 4
#define DHTTYPE DHT11
#define LDR_pin 32
DHT dht(DHT_pin,DHTTYPE);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Enviar una lectura cada 2 segundos.
const unsigned long INTERVALO_ENVIO = 2000;

unsigned long ultimoEnvio = 0;

void conectarWiFi() {

  Serial.print("Conectando a WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");

  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());
}

void conectarMQTT() {

  while (!mqttClient.connected()) {

    Serial.print("Conectando a Mosquitto...");

    // Crear un Client ID diferente para este ESP32.
    String clientId = "ESP32-";
    clientId += String(
      (uint32_t)(ESP.getEfuseMac() & 0xFFFFFFFF),
      HEX
    );

    if (mqttClient.connect(clientId.c_str())) {

      Serial.println(" conectado");

    } else {

      Serial.print(" error, rc=");
      Serial.print(mqttClient.state());
      Serial.println(". Reintentando en 2 segundos...");

      delay(2000);
    }
  }
}


// ======================================================
// LEER Y PUBLICAR SENSORES
// ======================================================

void publicarSensores() {


  
    //(Estudiante)
      //1. Obtener la temperatura.
      float temperatura = dht.readTemperature();

      //2. Obtener la humedad.
      float humedad = dht.readHumidity();
      //3. Obtener la lectura del sensor de iluminación.
      int LecturaLDR = analogRead(LDR_pin);
      int Luz = map(LecturaLDR, 0, 4095, 0, 100); 
      Luz = constrain(Luz, 0, 100);
      //4. Preparar los valores para poder publicarlos mediante MQTT.
      char tempStr[10];
      char hmdStr[10];
      char luzStr[10];
      dtostrf(temperatura,1,2,tempStr);
      dtostrf(humedad,1,2,hmdStr);
      sprintf(luzStr,"%d",Luz);
      //5. Publicar cada dato en su topic correspondiente:
      mqttClient.publish(TOPIC_TEMPERATURA, tempStr, true);
      mqttClient.publish(TOPIC_HUMEDAD, hmdStr, true);
      mqttClient.publish(TOPIC_LUZ, luzStr, true);
         //TOPIC_TEMPERATURA
         //TOPIC_HUMEDAD
         //TOPIC_LUZ

      //6. Mostrar las mediciones en el Monitor Serial.
      Serial.print("Temperatura: ");
      Serial.println(temperatura);
      Serial.print("Humedad: ");
      Serial.print(humedad);
      Serial.println(" %");
      Serial.print("LDR ");
      Serial.println(LecturaLDR);
      Serial.print("Luz ");
      Serial.print(Luz);
      Serial.println(" %");
}

void setup() {

  Serial.begin(115200);

  // (Estudiante)
  // Inicializar los sensores y configurar los pines
  // necesarios
  dht.begin();
  pinMode(LDR_pin, INPUT);
  analogReadResolution(12);

  conectarWiFi();

  mqttClient.setServer(
    MQTT_SERVER,
    MQTT_PORT
  );
}

void loop() {

  // Si se pierde WiFi, volver a conectarse.
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }

  // Si se pierde MQTT, volver a conectarse.
  if (!mqttClient.connected()) {
    conectarMQTT();
  }

  mqttClient.loop();


  // Publicar sensores cada 2 segundos.
  if (millis() - ultimoEnvio >= INTERVALO_ENVIO) {

    ultimoEnvio = millis();

    publicarSensores();
  }
}