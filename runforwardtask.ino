#include <LiquidCrystal.h>

// LCD pins: RS=D8, EN=D9, D4=D4, D5=D5, D6=D6, D7=D7 (used by shield)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// L298N motor driver pins
int ENA = 3;   // PWM for left motor
int IN1 = 2;   // Direction 1
int IN2 = A1;  // Direction 2
int IN3 = A2;  // Direction 3
int IN4 = A3;  // Direction 4
int ENB = A4;  // PWM or ON/OFF for right motor

void setup() {
  // LCD setup
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Robot Car Ready");
  delay(2000);

  // Motor pins setup
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Start moving forward
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moving Forward");
  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  
  analogWrite(ENA, 200);  // Left motor speed (0–255)
  analogWrite(ENB, 200);  // Right motor speed (0–255)
  
  delay(10000);  // Move for 10 seconds

  // Stop
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Stopped");
}

void loop() {
  // Nothing here — runs once
}