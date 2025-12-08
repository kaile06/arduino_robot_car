#include <LiquidCrystal.h>
#include <Wire.h>

// --- HARDWARE CONFIGURATION ---

// LCD pins: RS=D8, EN=D9, D4=D4, D5=D5, D6=D6, D7=D7
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Motor Pins
// Right Motor
const int ENA = 3; 
const int IN1 = A1; 
const int IN2 = A2; 

// Left Motor
const int ENB = 11; 
const int IN3 = 12; 
const int IN4 = A3; 

// MPU6050 I2C Address
const int MPU_ADDR = 0x68;

// --- VARIABLES ---
float yaw = 0.0;           // Accumulated rotation angle (Z-axis)
float pitch = 0.0;         // Ramp/Incline angle (Y-axis)
float roll = 0.0;          // Side-to-side tilt (X-axis)
unsigned long lastTime = 0;
float gyroZ_offset = 0.0;  // Calibration offset

// State Machine Stages
enum State {
  CALIBRATING,
  WAITING_FOR_RAMP, // Driving forward on flat ground
  CLIMBING,         // Driving up the incline
  EXITING_RAMP,     // Driving forward to clear the slope
  WAITING,          // 4 second pause
  ROTATING,         // Doing the 360
  DRIVING_OFF_RAMP, // Driving off after rotation
  FINISHED          // Done
};

State currentState = CALIBRATING;
unsigned long stateStartTime = 0;
int motorSpeed = 245; 

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // LCD Init
  lcd.begin(16, 2);
  lcd.print("Init MPU6050...");
  
  // Motor Init
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopCar();

  // Wake up MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // Wake up
  Wire.endTransmission(true);

  delay(1000);
  
  // Calibration
  calibrateGyro();
  currentState = WAITING_FOR_RAMP;
  lcd.clear();
  lcd.print("Go: Flat Ground");
}

void loop() {
  // 1. Read Sensors
  readMPU();

  // 2. State Machine Logic
  switch (currentState) {
    
    case WAITING_FOR_RAMP:
      moveForward();
      if (abs(pitch) > 10) { 
        currentState = CLIMBING;
        updateLCD("Climbing Ramp");
      }
      break;

    case CLIMBING:
      moveForward();
      if (abs(pitch) < 5) {
        currentState = EXITING_RAMP;
        stateStartTime = millis();
        updateLCD("Exiting Ramp");
      }
      break;

    case EXITING_RAMP:
      moveForward();
      if (millis() - stateStartTime >= 1500) {
        stopCar();
        currentState = WAITING;
        stateStartTime = millis();
        lcd.clear();
      }
      break;

    case WAITING:
      stopCar();
      
      // --- DISPLAY MEASURED ANGLES ---
      lcd.setCursor(0, 0);
      lcd.print("angle=");
      lcd.print(pitch, 1);   // pitch angle with 1 decimal
      lcd.print("deg   ");   // clear trailing chars
      
      lcd.setCursor(0, 1);
      lcd.print("roll=");
      lcd.print(roll, 1);    // roll angle with 1 decimal
      lcd.print("deg   ");

      if (millis() - stateStartTime >= 4000) {
        currentState = ROTATING;
        yaw = 0; // Reset yaw
        updateLCD("Rotating 360");
      }
      break;

    case ROTATING:
      turnRight(); 
      
      lcd.setCursor(0, 1);
      lcd.print("Yaw: ");
      lcd.print((int)yaw);
      lcd.print("   "); 
      
      if (abs(yaw) >= 360) {
        stopCar();
        currentState = DRIVING_OFF_RAMP;
        stateStartTime = millis();
        updateLCD("Driving Off...");
      }
      break;

    case DRIVING_OFF_RAMP:
      moveForward();
      if (millis() - stateStartTime >= 2000) {
        stopCar();
        currentState = FINISHED;
        updateLCD("Done!");
      }
      break;

    case FINISHED:
      stopCar();
      break;
  }
  
  delay(10); 
}

// --- MPU6050 HELPER FUNCTIONS ---

void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  int16_t AcX = Wire.read() << 8 | Wire.read();
  int16_t AcY = Wire.read() << 8 | Wire.read();
  int16_t AcZ = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); // skip temp
  int16_t GyX = Wire.read() << 8 | Wire.read();
  int16_t GyY = Wire.read() << 8 | Wire.read();
  int16_t GyZ = Wire.read() << 8 | Wire.read();

  float accX = AcX / 16384.0; 
  float accY = AcY / 16384.0;
  float accZ = AcZ / 16384.0;
  
  pitch = atan2(-accX, sqrt(accY*accY + accZ*accZ)) * 180.0 / PI;
  roll  = atan2(accY, accZ) * 180.0 / PI;

  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  float gyroZ = GyZ / 131.0; 
  gyroZ -= gyroZ_offset; 
  if (abs(gyroZ) < 1.0) gyroZ = 0;
  yaw += gyroZ * dt; 
}

void calibrateGyro() {
  lcd.clear();
  lcd.print("Calibrating...");
  lcd.setCursor(0,1);
  lcd.print("DONT MOVE CAR");
  
  long sumZ = 0;
  for (int i = 0; i < 2000; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x47); 
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 2, true);
    int16_t GyZ = Wire.read() << 8 | Wire.read();
    sumZ += GyZ;
    delay(2);
  }
  gyroZ_offset = (sumZ / 2000.0) / 131.0;
  
  lcd.clear();
  lcd.print("Ready!");
  delay(1000);
  lastTime = millis();
}

void updateLCD(String text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Status:");
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

void turnRight() {
  // Pivot turn: right motor backward, left motor forward
  digitalWrite(IN1, HIGH); 
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 180);
  analogWrite(ENB, 180);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
