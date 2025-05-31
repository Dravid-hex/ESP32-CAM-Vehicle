//Receptor

#include <WiFi.h>

const char* ssid = "SSID";
const char* password = "Contraseña";

// Pines del motor
#define ENA  13   
#define IN1  12
#define IN2  14
#define IN3 27
#define IN4 26
#define ENB 25

// Variables WiFi
WiFiServer server(80);

void setup() {
  Serial.begin(9600);

  
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Conectar a WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("¡Conectado!");
  Serial.println(WiFi.localIP());

  // Iniciar servidor
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Cliente conectado");

    while (client.connected()) {
      if (client.available()) {
        char comando = client.read();
         // request.trim();
        Serial.print("Comando recibido: ");
        Serial.println(comando);

        if (comando == '1') {
          // Adelante
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, HIGH);
          analogWrite(ENA, 100);  
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, HIGH);
          analogWrite(ENB, 100);
        } else if (comando == '2') {
          // Atrás
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
          analogWrite(ENA, 100);  
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
          analogWrite(ENB, 100);
        } else {
          // Detener motor si se recibe otro valor
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, LOW);
          analogWrite(ENA, 0);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, LOW);
          analogWrite(ENB, 0);

        }
      }
    }

    client.stop();
    Serial.println("Cliente desconectado");
  }
}