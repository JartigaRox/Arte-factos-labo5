/*
  Laboratorio 5 - Programación de Artefactos
  Parte 2: Prueba del sensor ultrasónico HC-SR04 (solo Monitor Serial)

  Conexiones:
    HC-SR04 VCC  -> 5V
    HC-SR04 GND  -> GND
    HC-SR04 TRIG -> GPIO18
    HC-SR04 ECHO -> GPIO19

  Monitor Serial a 115200 baudios.
*/

#define TRIG_PIN 18
#define ECHO_PIN 19

void setup() {

  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {

  // Pulso de disparo: LOW 2 µs -> HIGH 10 µs -> LOW
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Tiempo (µs) que ECHO permanece en HIGH; timeout de 30 ms (~5 m)
  long duracion = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duracion == 0) {

    Serial.println("No se detectó eco");

  } else {

    // d(cm) = (0.0343 * t(µs)) / 2   -> ida y vuelta, se divide entre 2
    float distancia = (duracion * 0.0343) / 2;

    Serial.print("Duración: ");
    Serial.print(duracion);
    Serial.println(" us");
    Serial.print("Distancia: ");
    Serial.print(distancia, 1);
    Serial.println(" cm");
  }

  delay(500);
}
