#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Jorge";
const char* password = "12345678";
String token = "EAADGJZAYdRZBsBOZBqCfxVelv03GHcVSR8NOqONa0GajjfOPPCcJ1q2w84I44yLOqqqsRmBTkMNHdzaHaFZBlwg3PVvNMpZCKApzRWWcLmK3vlzmVc327cm4HwY31KZBn35psbadQC1jpNMF3Pa2kWC3lOkm7eOASSirVsi1vZB77QQDErcsVS6LGXk0hsRAz8PAERUg8ghX0s6ZBTDF";
String servidor = "https://graph.facebook.com/v17.0/184761381381361/messages";
String payload = "{\"messaging_product\":\"whatsapp\",\"to\":\"527121122441\",\"type\":\"text\",\"text\": {\"body\": \"Se ha abierto el Locker con el teclado\"}}";

const int numRows = 4;
const int numCols = 4;
char keypadKeys[numRows][numCols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

const int rowPins[numRows] = {17, 5, 18, 19};
const int colPins[numCols] = {15, 2, 4, 16};

String inputPassword = "";
const String correctPassword = "123";

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Se ha conectado al wifi con la ip: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Leer la contraseña desde el teclado matricial
  inputPassword = readKeypad();

  if (inputPassword.length() > 0) {
    if (inputPassword.equals(correctPassword)) {
      sendWhatsAppMessage();
    } else {
      Serial.println("Contraseña incorrecta");
    }
  }

  delay(1000);
}

String readKeypad() {
  String input = "";
  char key;

  for (int row = 0; row < numRows; row++) {
    digitalWrite(rowPins[row], LOW);

    for (int col = 0; col < numCols; col++) {
      if (digitalRead(colPins[col]) == LOW) {
        key = keypadKeys[row][col];
        input += key;
        delay(200);  // Debounce delay
      }
    }

    digitalWrite(rowPins[row], HIGH);
  }

  return input;
}

void sendWhatsAppMessage() {
  HTTPClient http;
  http.begin(servidor.c_str());
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", token);

  int httpPostCode = http.POST(payload);

  if (httpPostCode > 0) {
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
  }

  http.end();
}

