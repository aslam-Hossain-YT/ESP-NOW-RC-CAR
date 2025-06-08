#include <esp_now.h>
#include <WiFi.h>

#define ENA 14  // Motor A speed (PWM) pin
#define ENB 32 // Motor B speed (PWM) pin
#define IN1 27  // Motor A direction pin 1
#define IN2 26  // Motor A direction pin 2
#define IN3 25  // Motor B direction pin 1
#define IN4 33  // Motor B direction pin 2


typedef struct struct_message {
  int16_t x;
  int16_t y;
} struct_message;

struct_message incomingData;

void onReceive(const uint8_t * mac, const uint8_t *incomingDataRaw, int len) {
  memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));
  Serial.printf("Joystick X: %d Y: %d\n", incomingData.x, incomingData.y);
  
  driveMotors(incomingData.x, incomingData.y);
}

void driveMotors(int16_t x, int16_t y) {
  // Deadzone to prevent motor jitter around zero
  int deadzone = 100;
  if (abs(x) < deadzone) x = 0;
  if (abs(y) < deadzone) y = 0;

  // Calculate motor speeds [-255..255]
  // Simple tank drive style mixing
  int16_t motorA = y + x;  
  int16_t motorB = y - x;

  // Constrain to PWM limits
  motorA = constrain(motorA, -1000, 1000);
  motorB = constrain(motorB, -1000, 1000);

  // Map from -1000..1000 to -255..255
  motorA = map(motorA, -1000, 1000, -255, 255);
  motorB = map(motorB, -1000, 1000, -255, 255);

  setMotor(ENA, IN1, IN2, motorA);
  setMotor(ENB, IN3, IN4, motorB);
}

void setMotor(int pwmPin, int dirPin1, int dirPin2, int speed) {
  if (speed > 0) {  //Forward move
    digitalWrite(dirPin1, HIGH);
    digitalWrite(dirPin2, LOW);
    analogWrite(pwmPin, speed);
  } else if (speed < 0) {  ////backward move
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, HIGH);
    analogWrite(pwmPin, -speed);
  } else {
    // Stop motor
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, LOW);
    analogWrite(pwmPin, 0);
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while(1);
  }

  esp_now_register_recv_cb(onReceive);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  Serial.println("Ready to receive joystick data");
}

void loop() {
  // Nothing to do here, all handled in callback
}

