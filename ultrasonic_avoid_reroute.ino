#include <LiquidCrystal.h>

// LCD pins: RS=D8, EN=D9, D4=D4, D5=D5, D6=D6, D7=D7
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Motor driver pins
int ENA = 3; 	 // Left motor PWM
int IN1 = A1; 	 // Left motor direction
int IN2 = A2;
int ENB = 11; 	 // Right motor PWM
int IN3 = 0; 	 // Right motor direction
int IN4 = 1;

// Ultrasonic sensor pins
#define TRIG_PIN 12
#define ECHO_PIN 13

long duration;
int distance;

unsigned long lastScan = 0;
// Note: Increased scan interval for stability, but kept your original fast 25ms
const int scanInterval = 25; 
unsigned long startTime;
const int moveDuration = 10000; // Move for 10 seconds

// --- Function Prototypes for new movement functions ---
void moveForward();
void reverse();
void turnRight();
// --------------------------------------------------------

void setup() {
	lcd.begin(16, 2);
	lcd.setCursor(0, 0);
	lcd.print("Robot Car Ready");
	delay(2000);

	pinMode(ENA, OUTPUT);
	pinMode(ENB, OUTPUT);
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

	pinMode(TRIG_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Scanning...");

	measureDistance();

	if (distance == -1) {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("No Echo Received");
		lcd.setCursor(0, 1);
		lcd.print("Assuming Clear Path");
		delay(30);
	} else if (distance < 20) {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Obstacle Ahead!");
		lcd.setCursor(0, 1);
		lcd.print("Will try to turn");
		delay(1500); // Wait a bit before attempting to move
		
		// Immediately attempt to reroute if obstacle is in setup
		turnRight(); 
		
	}
	
	// Start moving forward
	moveForward();
	startTime = millis();
}

void loop() {
	unsigned long currentTime = millis();

	// Stop after 10 seconds
	if (currentTime - startTime >= moveDuration) {
		stopMotors();
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Stopped");
		while (true); // Halt further execution
	}

	// Scan every 25ms
	if (currentTime - lastScan >= scanInterval) {
		lastScan = currentTime;
		measureDistance();

		if (distance != -1 && distance < 20) {
			// **OBSTACLE AVOIDANCE LOGIC**
			
			// 1. Stop
			stopMotors(); 
			delay(100); 

			// 2. Display message
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Obstacle Avoid");
			lcd.setCursor(0, 1);
			lcd.print("Dist: ");
			lcd.print(distance);
			lcd.print(" cm");
			delay(500); // Show message briefly

			// 3. Reverse (to back away from obstacle)
			reverse();
			delay(500); // Reverse for half a second

			// 4. Turn Right (to reroute)
			stopMotors();
			delay(100); // Brief pause
			turnRight();
			delay(500); // Turn for half a second

			// 5. Resume Forward Movement
			moveForward(); 
			// **END OBSTACLE AVOIDANCE LOGIC**
		}
		
		lcd.setCursor(0, 1);
		lcd.print("Timer: ");
		lcd.print((currentTime - startTime) / 1000);
		lcd.print("s   ");
	}
}

// --- Movement Helper Functions ---

void moveForward() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Moving Forward");

	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH); // Left Wheel Forward
	digitalWrite(IN3, HIGH); // Right Wheel Forward
	digitalWrite(IN4, LOW);

	analogWrite(ENA, 150); // Set speed
	analogWrite(ENB, 150);
}

void reverse() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Reversing");

	digitalWrite(IN1, HIGH); // Left Wheel Reverse
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);  // Right Wheel Reverse
	digitalWrite(IN4, HIGH);

	analogWrite(ENA, 100); // Lower speed for reverse/turn maneuvers
	analogWrite(ENB, 100);
  delay(50);
}

void turnRight() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Turning Right");

	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH); // Left Wheel Forward
	digitalWrite(IN3, LOW);  // Right Wheel Reverse (Pivot Turn)
	digitalWrite(IN4, HIGH);

	analogWrite(ENA, 150); // Lower speed for reverse/turn maneuvers
	analogWrite(ENB, 150);
	delay(150);
}

void stopMotors() {
	analogWrite(ENA, 0);
	analogWrite(ENB, 0);
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
}

// --- Sensor Function ---

void measureDistance() {
	digitalWrite(TRIG_PIN, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIG_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG_PIN, LOW);

	duration = pulseIn(ECHO_PIN, HIGH, 20000); // 20ms timeout
	if (duration == 0) {
		distance = -1; // No echo received
	} else {
		// Speed of sound in cm/us is approx. 0.0343 / 2 (since sound travels there and back)
		distance = duration * 0.034 / 2; 
	}
}