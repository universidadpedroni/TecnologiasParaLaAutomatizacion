#include <Arduino.h>
#include <config.h>
#include <blink.h>
// oLED Display
#include <SPI.h>
#include <Wire.h>
// Display
#include "diplaySelection.h"
#include <Adafruit_GFX.h>
#if USE_BIG_DISPLAY
  #include <Adafruit_SH110X.h>
#else
  #include <Adafruit_SSD1306.h>
#endif
// Sensor DHT11
#include "dhtSelection.h"
#include <DHT.h>
// Pulsador
#include <checkButton.h>
// Encoder
#include "ESP32Encoder.h"
// Wifi
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
// Logo UTN
#include "logo.h"

const char* ssid = "MyWiFi_S9";
const char* password = "HelpUsObiJuan";
#define CONNECT_TIME 10000

String ledState;  // Stores LED state

AsyncWebServer server(80);
blink parpadeo(LED_BUILTIN);

DHT dht(DHT_PIN, DHT_SENSOR);
//DHT dht(DHT_PIN, DHT11);
ESP32Encoder myEnc;

#if USE_BIG_DISPLAY
  Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);//, OLED_RESET);
#else
  Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire);
#endif


/* Los archivos de la página web se encuentran en la carpeta /data. Hay que subirlos manualmente */

String procesamientoFrontEnd(const String& var){
  
  if(var == "STATE"){
    digitalRead(PIN_LED_G) == HIGH? ledState = "ON" : ledState = "OFF";
    Serial.printf("Estado del LED: %s\n", ledState.c_str());  
    return ledState;
  }
  return String();
 
}


void spiffsInit(){
  if(!SPIFFS.begin(true)){
    Serial.println("Error al inicializar SPIFFs. El sistema se detendra");
    while(1){
      delay(100);
    }
  }
  else{
    Serial.println("SPIFFS inicializado con exito");
    Serial.println("Listado de archivos disponibles:");
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while(file){
      Serial.printf("Archivo: %s\n", file.name());
      file.close();
      file = root.openNextFile();
    }
    root.close();
    
  }

}

void spiffsCheckFiles(char * nombreArchivo){
  SPIFFS.exists(nombreArchivo)? Serial.printf("Archivo %s encontrado\n", nombreArchivo) : Serial.printf("Archivo %s no encontrado\n", nombreArchivo);
  
}

void wifiInit(){
  unsigned long tiempoDeInicio = millis();
  Serial.printf("Intentando conectar a la red Wifi %s...", ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED && (tiempoDeInicio + CONNECT_TIME > millis())){
    Serial.print(".");
    delay(1000);
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.print("\nIP del servidor:");
    Serial.println(WiFi.localIP());
  }
  else{
    Serial.println("No se pudo conectar al Wifi. Llamen a los Avengers");
  }
  
}

void serverInitBootstrap(){
  Serial.print("Inicializando servidor usando Boostrap Framework");
  //Ruta a index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS,"/indexWithBoostrap.html",String(), false, procesamientoFrontEnd);
  });
  server.on("/src/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/bootstrap.bundle.min.js", "text/javascript");
  });
 
  server.on("/src/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/jquery-3.6.3.min.js", "text/javascript");
  });
 
  server.on("/src/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/src/bootstrap.min.css", "text/css");
  });

  // Ruta para encender el LED
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(PIN_LED_G, HIGH);
    request->send(SPIFFS, "/indexWithBoostrap.html", String(), false,procesamientoFrontEnd);
  });

  // Ruta para apagar el LED
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(PIN_LED_G, LOW);
    request->send(SPIFFS, "/indexWithBoostrap.html", String(), false,procesamientoFrontEnd);
  });

  server.begin();



}

void serverInit(){
  Serial.print("Inicializando servidor...");
  //Ruta a index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS,"/index.html",String(), false, procesamientoFrontEnd);
  });

  //Ruta a style.css
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/style.css","text/css");
  });

  // Ruta para encender el LED
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(PIN_LED_G, HIGH);
    request->send(SPIFFS, "/index.html", String(), false,procesamientoFrontEnd);
  });

  // Ruta para apagar el LED
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(PIN_LED_G, LOW);
    request->send(SPIFFS, "/index.html", String(), false,procesamientoFrontEnd);
  });

  server.begin();
  Serial.println("listo");
}


void dhtInit(){
  dht.begin();
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  Serial.printf("Temperatura: %d°C\tHumedad: %d%\n",
                (int)temp,
                (int)hum);
}

void displayInit(){
#if USE_BIG_DISPLAY
  display.begin(I2C_ADDRESS, true); // Address 0x3C default
#else
  display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS);
#endif
  //display.begin()
  display.clearDisplay();
  display.setTextSize(1);
#if USE_BIG_DISPLAY
  display.setTextColor(SH110X_WHITE);
#else
  display.setTextColor(SSD1306_WHITE);
#endif  
  display.setCursor(0,0);
  display.println(F("Hola Mundo!"));
  display.println(F("Datos de Compilacion:"));
  display.printf("Fecha %s\n",__DATE__);
  display.printf("Hora: %s\n",__TIME__);
  display.display();
}

void displayLogo(){
  display.clearDisplay();
  USE_BIG_DISPLAY == true? display.drawBitmap(0, 0, logo_UTN_128x64, 126, 126, 1): display.drawBitmap(0, 0, logo_UTN_128x32, 126, 126, 1); 
  
  display.display();
  delay(2000);

}

void displayWiFi(){
  display.clearDisplay();
  display.setCursor(0,0);
  if(WiFi.status() == WL_CONNECTED){
    display.println(F("Conectado al Wifi"));
    display.println(F("IP:"));
    display.println(WiFi.localIP());

  }
  else{
    display.println(F("No se pudo conectar a WiFi"));
  }
  display.display();

}

void myEncInit(){
  myEnc.attachSingleEdge(PIN_ENC_A, PIN_ENC_B);
  myEnc.setCount(0);
}

void displayUpdateAndShow(unsigned long interval, int cuenta, float valorADC)
{
	static unsigned long previousMillis = 0;        // will store last time LED was updated
	//const long interval = 1000;           // interval at which to blink (milliseconds)
	unsigned long currentMillis = millis();
	static bool estadoPin=false;
	
	if(currentMillis - previousMillis > interval) 
	{
		previousMillis = currentMillis;
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Temperatura: %.1fC\nHumedad: %.1f\nADC: %.2f\nEncoder: %d\n",
                    dht.readTemperature(),
                    dht.readHumidity(),
                    valorADC,
                    cuenta); 
    display.display();
	}
    
}

void setup() {
  delay(1000);
  Serial.begin(BAUDRATE);
  Serial.println(F("Hola Mundo!"));
  Serial.println(F("Hardware Test"));
  Serial.println(F("Iniciando Led"));
  parpadeo.init();
  parpadeo.off();
  Serial.println(F("Iniciando Entradas Digitales"));
  pinMode(PIN_ENC_PUSH,INPUT_PULLUP);
  pinMode(PIN_LED_G, OUTPUT);
  digitalWrite(PIN_LED_G, HIGH);
  Serial.println(F("Iniciando Display"));
  displayInit();
  displayLogo();
  DHT_SENSOR == DHT22? Serial.println(F("Iniciando DHT22")): Serial.println(F("Iniciando DHT11"));
  dhtInit();
  Serial.println(F("Configurando ADC..."));
  analogReadResolution(12);
  Serial.println(F("Inicializando Encoder"));
  myEncInit();
  Serial.println(F("Inicializando SPIFFS"));
  spiffsInit();
  Serial.println(F("Inicializando WiFi"));
  wifiInit();
  displayWiFi();
  delay(2000);
  if (!digitalRead(PIN_ENC_PUSH)){
    Serial.println("Usando servidor con Boostrap Framework");
    serverInitBootstrap();
  }
  else{
    Serial.println("Usando servidor sin Boostrap");
    serverInit();

  }


  Serial.println(F("Setup Terminado"));
  Serial.println(F("PINOUT: https://www.mischianti.org/2021/02/17/doit-esp32-dev-kit-v1-high-resolution-pinout-and-specs/"));
  delay(5000);
  digitalWrite(PIN_LED_G, LOW);
}

void loop() {
  int cuenta =myEnc.getCount();
  float valorADC = (float) map(analogRead(PIN_ADC),0 ,4095, 4095, 0) * 3.3 / 4095.0;
  parpadeo.update(BLINK_OK);
  int pulsador = checkButton(PIN_ENC_PUSH);
  if (pulsador !=0) Serial.printf("Valor del pulsador: %d\n", pulsador);
  if( pulsador !=0) digitalWrite(PIN_LED_G, !digitalRead(PIN_LED_G));
  displayUpdateAndShow(100, cuenta, valorADC);
   
}