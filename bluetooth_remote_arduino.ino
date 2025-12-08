#include <LiquidCrystal.h>

// LCD pins: RS=D8, EN=D9, D4=D4, D5=D5, D6=D6, D7=D7
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// --- MOTOR CONFIGURATION ---
// Right Motor
int ENA = 3;    // PWM Speed
int IN1 = A1;   // Direction 1
int IN2 = A2;   // Direction 2

// Left Motor
int ENB = 11;   // PWM Speed
// CHANGED: Moved from 0/1 to A3/A4 to save Serial pins for Bluetooth
int IN3 = A4;   
int IN4 = A3;   

// Speed Variable (0-255)
int motorSpeed = 255; 
char command; // Variable to store incoming Bluetooth data

void setup() {
  // 1. Initialize Serial Communication for Bluetooth
  // Connect Bluetooth TX to Arduino RX (Pin 0)
  // Connect Bluetooth RX to Arduino TX (Pin 1)
  Serial.begin(9600); 

  // 2. LCD Setup
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("BT Car Ready");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");

  // 3. Motor Pins Setup
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Ensure motors are stopped initially
  stopCar();
}

void loop() {
  // Check if data is coming from the Bluetooth module
  if (Serial.available() > 0) {
    command = Serial.read(); // Read the data
    
    // Execute command
    switch (command) {
      case 'F': // Forward
        moveForward();
        updateLCD("Forward");
        break;
        
      case 'B': // Backward
        moveBackward();
        updateLCD("Backward");
        break;
        
      case 'L': // Left
        turnLeft();
        updateLCD("Turn Left");
        break;
        
      case 'R': // Right
        turnRight();
        updateLCD("Turn Right");
        break;
        
      case 'S': // Stop
        stopCar();
        updateLCD("Stopped");
        break;

      // Optional: Speed control logic can be added here
      case '0': motorSpeed = 100; break;
      case '9': motorSpeed = 255; break;
    }
  }
}

// --- HELPER FUNCTIONS ---

void updateLCD(String text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BT Control:");
  lcd.setCursor(0, 1);
  lcd.print(text);
}

void moveForward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

void moveBackward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

void turnLeft() {
  // Right motor Forward, Left motor Backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

void turnRight() {
  // Right motor Backward, Left motor Forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}