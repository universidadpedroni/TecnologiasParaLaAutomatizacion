#define RELAY_PIN 4  // Pin del relé
#define POTE_PIN 32  // Pin del potenciometro
#define DHT_PIN 33  // Pin del DHT

#include <DHT.h>

const unsigned long INTERVALO_DE_EJECUCION = 1000;
const float errorTh = 1.0;       // Histéresis del controlador



DHT dhtSensor(DHT_PIN, DHT22);

void controlUpdate(){
    static unsigned long previousMillis = millis();
    float Tref = 0.0;   // Temperatura de referencia.
    float Tsensor = 0.0;    // Temperatura del sistema
    float error = 0.0;  // Diferencia entre la temperatura de referencia y la del sistema.
    if(millis() - previousMillis > INTERVALO_DE_EJECUCION){
        previousMillis = millis();
        // Adquirir  el valor de referencia
        Tref = (float)analogRead(POTE_PIN) / 100.0;     // Escala: 0° - 40°
        Tsensor = dhtSensor.readTemperature();
        error = Tref - Tsensor;
        // Ejecutar la acción de control (sin histéresis)
        //digitalWrite(RELAY_PIN, !(error >= 0));
        //error >= 0 ? digitalWrite(RELAY_PIN, LOW) : digitalWrite(RELAY_PIN, HIGH);
        // Ejecutar la acción de control (con histéresis)
        //if(abs(error) > errorDelta) error >= 0 ? digitalWrite(RELAY_PIN, LOW) : digitalWrite(RELAY_PIN, HIGH);
        if(abs(error) > errorTh) digitalWrite(RELAY_PIN, !(error >= 0));
        
        Serial.printf("T ref: %.2f°C, T sens: %.2f°C, error: %.2f°C, Relay: %s\n",
                    Tref,
                    Tsensor,
                    error,
                    digitalRead(RELAY_PIN) == true? "Encendido" : "Apagado"
                    );

    }
}

void setup(){
  // Inicialización Puerto Serie
  Serial.begin(115200);
  Serial.printf("Fecha y hora de compilación: %s, %s\n",__DATE__, __TIME__);
  // Inicialización Pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  // Inicialización del DHT
  dhtSensor.begin();
}

void loop(){
  controlUpdate();
}

