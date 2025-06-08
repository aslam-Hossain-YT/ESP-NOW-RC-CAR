// Define joystick pins
#define JOYSTICK_X 35  // Analog pin for X-axis
#define JOYSTICK_Y 34  // Analog pin for Y-axis
#define JOYSTICK_SW 25 // Digital pin for button

void setup() {
  Serial.begin(115200);

  // Configure joystick pins
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(JOYSTICK_SW, INPUT_PULLUP); // Internal pull-up resistor for button
}

void loop() {
  int xValue = analogRead(JOYSTICK_X); // Range: 0-4095
  int yValue = analogRead(JOYSTICK_Y); // Range: 0-4095
  bool buttonPressed = digitalRead(JOYSTICK_SW) == LOW;

  Serial.print("X: ");
  Serial.print(xValue);
  Serial.print(" | Y: ");
  Serial.println(yValue);

  delay(50); // Optional delay for readability
}

