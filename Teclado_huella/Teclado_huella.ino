#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Fingerprint.h>

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

const int rowPins[numRows] = {5, 4, 3, 2};
const int colPins[numCols] = {9, 8, 7, 6};

const int fingerprintRx = 16;
const int fingerprintTx = 17;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  Serial1.begin(57600);

  lcd.begin(16, 2);
  lcd.print("Ingrese la");
  lcd.setCursor(0, 1);
  lcd.print("contrasena:");

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Se ha conectado al wifi con la ip: ");
  Serial.println(WiFi.localIP());

  finger.begin(fingerprintRx, fingerprintTx);

  // Inicializar el lector de huellas
  if (finger.verifyPassword()) {
    Serial.println("Lector de huellas encontrado.");
  } else {
    Serial.println("No se pudo encontrar el lector de huellas, asegúrate de conectarlo correctamente.");
    while (1);
  }
}

void loop() {
  inputPassword = readKeypad();

  if (inputPassword.length() > 0) {
    lcd.clear();
    lcd.print("Contraseña:");

    if (inputPassword.equals(correctPassword) || fingerprintVerification()) {
      lcd.setCursor(0, 1);
      lcd.print("Correcta");
      delay(2000);
      sendWhatsAppMessage();
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Incorrecta");
      delay(2000);
    }

    lcd.clear();
    lcd.print("Ingrese la");
    lcd.setCursor(0, 1);
    lcd.print("contrasena:");
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

bool fingerprintVerification() {
  Serial.println("Coloca tu dedo en el lector de huellas...");

  int fingerprintID = finger.getImage();
  
  if (fingerprintID == FINGERPRINT_OK) {
    Serial.println("Imagen tomada correctamente. Levanta el dedo.");
    delay(2000);
    finger.image2Tz(1);
    int result = finger.createModel();
    
    if (result == FINGERPRINT_OK) {
      Serial.println("Huella registrada correctamente.");
      return true;
    } else {
      Serial.println("Error al registrar la huella.");
      return false;
    }
  } else if (fingerprintID == FINGERPRINT_NOFINGER) {
    Serial.println("No se detectó ningún dedo.");
    return false;
  } else {
    Serial.println("Error al tomar la imagen del dedo.");
    return false;
  }
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
