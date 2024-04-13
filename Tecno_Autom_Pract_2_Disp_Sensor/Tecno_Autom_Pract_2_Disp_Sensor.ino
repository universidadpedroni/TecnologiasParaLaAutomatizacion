#include "DHT.h"
const unsigned long DHT_UPDATE_INTERVAL = 2000;
const int DHT_PIN = 33;
DHT dhtSensor(DHT_PIN, DHT22);

void dhtUpdate(){
    static unsigned long previousMillis = millis();
    if(millis() - previousMillis > DHT_UPDATE_INTERVAL){
        previousMillis = millis();
        Serial.printf("Temperatura: %.2f°C, Humedad; %.2f%%\n",
                    dhtSensor.readTemperature(),
                    dhtSensor.readHumidity());

    }
}

void setup(){
    Serial.begin(115200);
    Serial.printf("Fecha y hora de compilación: %s, %s\n",
                __TIME__,
                __DATE__);
    dhtSensor.begin(); 
    

}

void loop(){
    dhtUpdate();
}