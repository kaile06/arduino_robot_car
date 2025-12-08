#include <LiquidCrystal.h>

// --- PIN DEFINITIONS ---

// LCD Pins (Standard Keypad Shield: 8, 9, 4, 5, 6, 7)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// L298N Motor Driver Pins
const int ENA = 3;    // Left Motor Speed (PWM)
const int IN1 = A1;   // Left Motor Direction 1
const int IN2 = A2;   // Left Motor Direction 2

const int ENB = 11;   // Right Motor Speed (PWM)
const int IN3 = A3;   // Right Motor Direction 1
const int IN4 = A4;   // Right Motor Direction 2

// Line Sensor Pins
const int LEFT_SENSOR_PIN = 2;   
const int RIGHT_SENSOR_PIN = A5; 

// --- ENCODER PINS (UPDATED) ---
// MOVED TO 12 and 13 (Much better than 0/1)
// These are on Port B, so we don't need to unplug them to upload!
const int ENCODER_LEFT_PIN = 12;
const int ENCODER_RIGHT_PIN = 13;

// --- CONFIGURATION ---

const int SPEED_FORWARD = 90;   
const int SPEED_TURN    = 180;  

// Line Logic
const int DETECT_LINE = LOW;      
const int DETECT_SURFACE = HIGH;  

// Encoder Math
const int SLOTS_ON_DISC = 20;     
const float WHEEL_DIAMETER = 6.5; 
const float CM_PER_PULSE = (3.14159 * WHEEL_DIAMETER) / SLOTS_ON_DISC;

// Measurement Variables
volatile unsigned long leftPulseCount = 0; 
volatile unsigned long rightPulseCount = 0;
volatile byte lastPortB = 0; // Changed from PortD to PortB

unsigned long startTime = 0;
unsigned long lastLCDUpdate = 0;

void setup() {
  // 1. Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Line Follower");
  lcd.setCursor(0, 1);
  lcd.print("Final Config"); 
  delay(1000); 

  // 2. Initialize Motor Pins
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  // 3. Initialize Sensor Pins
  pinMode(LEFT_SENSOR_PIN, INPUT);
  pinMode(RIGHT_SENSOR_PIN, INPUT);

  // 4. Initialize Encoder Pins
  // Pin 13 has a built-in LED, so INPUT_PULLUP is good to ensure clean signals
  pinMode(ENCODER_LEFT_PIN, INPUT_PULLUP);
  pinMode(ENCODER_RIGHT_PIN, INPUT_PULLUP);

  // --- PIN CHANGE INTERRUPT SETUP (Port B) ---
  // Port B handles Pins 8 to 13.
  // We enable interrupts for Port B, but mask ONLY Pin 12 and 13.
  
  cli();                // Disable interrupts
  PCICR |= 0b00000001;  // Enable PCIE0 (Port B Interrupts)
  PCMSK0 |= 0b00110000; // Enable Mask for Pin 12 (Bit 4) and Pin 13 (Bit 5)
  sei();                // Re-enable interrupts
  
  lastPortB = PINB; // Initialize state

  lcd.clear();
  startTime = millis();
}

void loop() {
  // --- LINE FOLLOWER LOGIC ---
  int leftValue = digitalRead(LEFT_SENSOR_PIN);
  int rightValue = digitalRead(RIGHT_SENSOR_PIN);

  // CASE 1: Forward
  if (leftValue == DETECT_SURFACE && rightValue == DETECT_SURFACE) {
    moveForward();
  }
  // CASE 2: Turn Left
  else if (leftValue == DETECT_LINE && rightValue == DETECT_SURFACE) {
    turnLeft();
  }
  // CASE 3: Turn Right
  else if (leftValue == DETECT_SURFACE && rightValue == DETECT_LINE) {
    turnRight();
  }
  // CASE 4: Stop
  else if (leftValue == DETECT_LINE && rightValue == DETECT_LINE) {
    stopMotors();
  }

  // --- STATS DISPLAY (Updates every 500ms) ---
  if (millis() - lastLCDUpdate > 500) {
    updateStatsLCD();
    lastLCDUpdate = millis();
  }
}

// --- INTERRUPT SERVICE ROUTINE (Port B) ---
// This runs whenever Pin 12 or 13 changes state
ISR(PCINT0_vect) {
  byte currentPortB = PINB;          // Read Pins 8-13
  byte change = currentPortB ^ lastPortB; // XOR to find changes
  
  // Check Pin 12 (Bit 4) - Left Encoder
  if (change & 0b00010000) {
    if (currentPortB & 0b00010000) leftPulseCount++;
  }
  
  // Check Pin 13 (Bit 5) - Right Encoder
  if (change & 0b00100000) {
    if (currentPortB & 0b00100000) rightPulseCount++;
  }

  lastPortB = currentPortB;
}

// --- HELPER FUNCTIONS ---

void updateStatsLCD() {
  float avgPulses = (leftPulseCount + rightPulseCount) / 2.0;
  float distanceCm = avgPulses * CM_PER_PULSE;
  float timeSec = (millis() - startTime) / 1000.0;

  lcd.setCursor(0, 0);
  lcd.print("Dst:");
  lcd.print(distanceCm, 0); 
  lcd.print("cm ");
  
  // Debug display
  int L = digitalRead(LEFT_SENSOR_PIN);
  int R = digitalRead(RIGHT_SENSOR_PIN);
  if(L==DETECT_SURFACE && R==DETECT_SURFACE) lcd.print("FWD");
  else if(L==DETECT_LINE) lcd.print("LFT");
  else if(R==DETECT_LINE) lcd.print("RGT");
  else lcd.print("STP");

  lcd.setCursor(0, 1);
  lcd.print("Tm:");
  lcd.print(timeSec, 0);
  lcd.print("s ");
  lcd.print(leftPulseCount);
  lcd.print(":");
  lcd.print(rightPulseCount);
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, SPEED_FORWARD);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 95); 
}

void turnLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW); 
  analogWrite(ENA, SPEED_TURN);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, SPEED_TURN);
}

void turnRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH); 
  analogWrite(ENA, SPEED_TURN);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, SPEED_TURN);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}