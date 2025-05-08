#include <WiFi.h>

const char* ssid = "Greñas";
const char* password = "19992002";

// Pines del motor (verifica que coincidan con tu conexión real)
#define ENA  13   // PWM motor A
#define IN1  12   // Dirección motor A
#define IN2  14   // Dirección motor A
#define IN3  27   // Dirección motor B
#define IN4  26   // Dirección motor B
#define ENB  25   // PWM motor B
#define LED  33   // LED indicador
#define red 2     // LED rojo
#define green 4   // LED verde
#define blue 5    // LED azul

WiFiServer server(80);
char comando = '0';
unsigned long lastCommandTime = 0;
bool motorsEnabled = true; // Bandera para habilitar/deshabilitar motores

void setup() {
  Serial.begin(115200); // Aumentamos velocidad para mejor depuración

  // Configurar pines
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  // Estado inicial
  stopMotors();
  updateLEDs(WL_IDLE_STATUS);

  // Iniciar prueba automática de motores al inicio
  testMotors();
  delay(1000); // Pausa después de la prueba

  // Conectar a WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    updateLEDs(WiFi.status());
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado!");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void updateLEDs(int wifiStatus) {
  if (wifiStatus == WL_CONNECTED) {
    digitalWrite(red, LOW);
    digitalWrite(blue, LOW);
    digitalWrite(green, HIGH);
  } else {
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
    digitalWrite(blue, millis() % 1000 < 500 ? HIGH : LOW);
  }
}

void stopMotors() {
  Serial.println("Deteniendo motores");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(ENA, 0);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(ENB, 0);
  digitalWrite(LED, LOW);
}

void testMotors() {
  Serial.println("\n--- INICIANDO PRUEBA DE MOTORES ---");
  
  // Test Motor A
  Serial.println("TEST MOTOR A (pines ENA, IN1, IN2)");
  Serial.println("Dirección adelante");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 200); // Velocidad media
  delay(2000);
  
  Serial.println("Dirección atrás");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);
  delay(2000);
  
  // Test Motor B
  Serial.println("TEST MOTOR B (pines ENB, IN3, IN4)");
  Serial.println("Dirección adelante");
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 200);
  delay(2000);
  
  Serial.println("Dirección atrás");
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 200);
  delay(2000);
  
  stopMotors();
  Serial.println("--- PRUEBA DE MOTORES COMPLETADA ---\n");
}

void handleCommand(char cmd) {
  if (!motorsEnabled) {
    Serial.println("Motores deshabilitados por configuración");
    return;
  }

  switch(cmd) {
    case '1': // Adelante
      Serial.println("Comando: Adelante");
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      analogWrite(ENA, 150); // Máxima velocidad
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      analogWrite(ENB, 150);
      digitalWrite(LED, HIGH);
      break;
      
    case '2': // Atrás
      Serial.println("Comando: Atrás");
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 150);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(ENB, 150);
      digitalWrite(LED, HIGH);
      break;
      
    case '0': // Detener
      Serial.println("Comando: Detener");
      stopMotors();
      break;
      
    case 't': // Test motores (nuevo comando)
      Serial.println("Comando: Prueba de motores");
      testMotors();
      break;
      
    case 'e': // Habilitar/deshabilitar motores
      motorsEnabled = !motorsEnabled;
      Serial.print("Motores ahora están ");
      Serial.println(motorsEnabled ? "HABILITADOS" : "DESHABILITADOS");
      if (!motorsEnabled) stopMotors();
      break;
      
    default:
      Serial.print("Comando no reconocido: ");
      Serial.println(cmd);
  }
}

void loop() {
  updateLEDs(WiFi.status());
  
  // Manejo de conexiones WiFi
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Cliente conectado");
    unsigned long connectionTime = millis();

    while (client.connected() && millis() - connectionTime < 5000) {
      if (client.available()) {
        char received = client.read();
        
        if (received == '0' || received == '1' || received == '2' || received == 't' || received == 'e') {
          comando = received;
          lastCommandTime = millis();
          Serial.print("Comando recibido: ");
          Serial.println(comando);
          
          handleCommand(comando); // Procesar comando
          client.println("OK"); // Confirmación
        } else if (received != '\n' && received != '\r') {
          Serial.print("Caracter inválido ignorado: ");
          Serial.println(received);
          client.println("ERROR: Comando inválido");
        }
      }
      delay(10);
    }
    client.stop();
    Serial.println("Cliente desconectado");
  }

  // Timeout de comando (2 segundos sin comando -> detener)
  if (millis() - lastCommandTime > 2000 && comando != '0') {
    Serial.println("Timeout - deteniendo motores");
    comando = '0';
    stopMotors();
  }

  delay(10);
}