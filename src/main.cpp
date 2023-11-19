// Pinout https://lobodarobotica.com/blog/wp-content/uploads/2020/09/ESP32-Pinout.jpg

#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <addons/RTDBHelper.h>

// #define FIREBASE_HOST "bark-detector-a1277-default-rtdb.firebaseio.com"
//#define FIREBASE_AUTH ""
#define WIFI_SSID "uaifai-apolo"
#define WIFI_PASSWORD "bemvindoaocesar"

#define RED_PIN 1
#define YELLOW_PIN 21
#define GREEN_PIN 18
#define ANALOG_AUDIO_INPUT 36 // ADC1_0 corresponde ao GPIO36 no ESP32

// put function declarations here:
void sendSerialUART(const String& message) { // enviar mensagem para a porta serial
  Serial.println(message);
}

void connectToWiFi() { // conectar ao wi-fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    sendSerialUART("Conectando ao WiFi...");
  }

  sendSerialUART("Conectado ao WiFi");
}

FirebaseData firebaseData;

void sendFirebase(const String& message, int audioIntensity) {
  float audioPercentage = (audioIntensity / 4095.0) * 100;

  Firebase.set(firebaseData, "/message", message);
  Firebase.set(firebaseData, "/audioPercentage", audioPercentage);
}

void readAudioAndLightLED() { // ler o input analógico e tratar a cor do LED
  int audioIntensity = analogRead(ANALOG_AUDIO_INPUT);

  if (audioIntensity < 2048) { // acende o verde
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(RED_PIN, LOW);

    sendSerialUART("LATIDOS DE BAIXA INTENSIDADE DETECTADOS\n");
    sendSerialUART(String(audioIntensity));

    sendFirebase("LATIDOS DE BAIXA INTENSIDADE DETECTADOS", audioIntensity);

  } else if (audioIntensity >= 2048 && audioIntensity < 3072) { // acende o amarelo
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(YELLOW_PIN, HIGH);
    digitalWrite(RED_PIN, LOW);

    sendSerialUART("LATIDOS DE MÉDIA INTENSIDADE DETECTADOS\n");
    sendSerialUART(String(audioIntensity));
    sendFirebase("LATIDOS DE MÉDIA INTENSIDADE DETECTADOS", audioIntensity);

  } else { // acende o vermelho
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);

    sendSerialUART("LATIDOS DE ALTA INTENSIDADE DETECTADOS\n");
    sendSerialUART(String(audioIntensity));
    sendFirebase("LATIDOS DE ALTA INTENSIDADE DETECTADOS", audioIntensity);
  }
}

void setup() {
    connectToWiFi();
    
    Serial.begin(9600);
    FirebaseConfig firebaseConfig;
    firebaseConfig.database_url = "https://bark-detector-a1277-default-rtdb.firebaseio.com/";
    firebaseConfig.api_key = "HT5rfEAxiFmVwANpuMLBAxbRSklp7FCYbwsyKaV3";

    Firebase.begin(&firebaseConfig, nullptr);

    WiFiClient wifiClient;
    FirebaseAuth firebaseAuth;
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    
    // Configura os pinos GPIO como saída
    pinMode(RED_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);

    // Configura o pino ADC como entrada
    pinMode(ANALOG_AUDIO_INPUT, INPUT);
}

void loop() {
    // put your main code here, to run repeatedly:
    readAudioAndLightLED();
}

// put function definitions here:
