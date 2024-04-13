#include <Arduino.h>
#define RELAY_PIN 2  // Pin del relé
#define POTE_PIN 32  // Pin del potenciometro
#define PULSADOR_PIN 19 // Pin del pulsador del encoder

//const unsigned long TIEMPO_DE_EJECUCION = 100000;
const unsigned long INTERVALO_DE_EJECUCION = 1000;
const int THRESHOLD = 500;


void relayPote(){
  static unsigned long previousMillis = 0;
  if(millis() - previousMillis > INTERVALO_DE_EJECUCION){
    previousMillis = millis();
    //analogRead(POTE_PIN) > THRESHOLD ? digitalWrite(RELAY_PIN, HIGH) : digitalWrite(RELAY_PIN, LOW);
    digitalWrite(RELAY_PIN, analogRead(POTE_PIN) > THRESHOLD);
    Serial.printf("Lectura: %d, estado %s\n", analogRead(POTE_PIN), digitalRead(RELAY_PIN) == HIGH? "Encendido" : "Apagado");
  }
}
  


void relayLoop(){
  static unsigned long millisInicial = millis();
  if(millis() - millisInicial > INTERVALO_DE_EJECUCION){
    millisInicial = millis();
    digitalWrite(RELAY_PIN, !digitalRead(RELAY_PIN));
  }


}

void setup(){
  // Inicialización Puerto Serie
  Serial.begin(115200);
  Serial.printf("Fecha y hora de compilación: %s, %s\n",
  __DATE__, __TIME__);
  // Inicialización Pines
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  //pinMode(PULSADOR_PIN, INPUT_PULLUP);
  //millisInicial = millis();
}

void loop(){
  //relayLoop();
  relayPote();
  /*digitalWrite(RELAY_PIN, HIGH);
  delay(5000);
  digitalWrite(RELAY_PIN, LOW);
  delay(5000);
  Serial.printf("Encendido: %d miliseg\n", millis());
  */
}

