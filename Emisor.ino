#include <WiFi.h>

// Pines de botones
#define BUTTON1 13
#define BUTTON2 12
#define BUTTON3 14
#define BUTTON4 27

const char* ssid = "SSID";  
const char* password = "Contraseña";
const char* serverIP = "Ip del Receptor";  
const int serverPort = 80;

WiFiClient client;

void setup() {
  Serial.begin(9600);
  
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi.");
}

void loop() {
  char message = '\0';  

  if (digitalRead(BUTTON1) == LOW) {
    message = '1';
  } else if (digitalRead(BUTTON2) == LOW) {
    message = '2';
  } else if (digitalRead(BUTTON3) == LOW) {
    message = '3';
  } else if (digitalRead(BUTTON4) == LOW) {
    message = '4';
  }

  if (message != '\0') {
    Serial.print("Enviando: ");
    Serial.println(message);

    if (client.connect(serverIP, serverPort)) {
      client.write(message);  
      client.stop();
      Serial.println("Mensaje enviado correctamente.");
    } else {
      Serial.println("Error al conectar con el servidor.");
    }

    delay(500); 
  }
}
