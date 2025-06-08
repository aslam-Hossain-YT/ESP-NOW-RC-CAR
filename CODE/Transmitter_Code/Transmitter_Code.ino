#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define JOY_X_PIN 35  // ADC input for joystick X (GPIO 34)
#define JOY_Y_PIN 34  // ADC input for joystick Y (GPIO 35)

uint8_t receiverAddress[] = { 0x48, 0xE7, 0x29, 0xB2, 0xE6, 0x3C };  // Receiver MAC

typedef struct struct_message {
  int16_t x;  // -1000 to 1000 range (mapped from ADC)
  int16_t y;
} struct_message;

struct_message myData;

void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (1);
  }
  
  esp_now_register_send_cb(onSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    while (1);
  }
}

void loop() {
  int rawX = analogRead(JOY_X_PIN);
  int rawY = analogRead(JOY_Y_PIN);

  // Map ADC 0-4095 to -1000 to +1000 for easier math
  myData.x = map(rawX, 0, 4095, 1000, -1000);
  myData.y = map(rawY, 0, 4095, 1000, -1000);

  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&myData, sizeof(myData));

  if (result != ESP_OK) {
    Serial.println("Send error");
  }

  delay(50);  // send every 50 ms (~20 Hz)
}

