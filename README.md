H-Bridge Motor Control:Constructed an H-bridge circuit on a breadboard, utilizing four switches to manually dictate the clockwise and anticlockwise rotation of a DC motor.

Straight-Line Navigation: The base robot car chassis was assembled using an Arduino UNO, an LCD keypad shield, an L298N motor driver module, and four DC motors. It was programmed to move in a straight line for ten seconds, tracking and displaying its progress on the LCD screen before automatically coming to a stop.

Light Sensor Prototyping: A custom light detection circuit was prototyped on a Veroboard to distinguish between light and dark surfaces. This circuit utilized an IR emitter, a photodiode, and an LM358 operational amplifier to boost the signal, functioning as the primary sensor for detecting black lines.

Autonomous Line Following: Using two of the soldered light sensor circuits and rotary encoders, the robot was programmed to autonomously follow a black line track. The car successfully navigated the six-metre track within a 60-second time limit while displaying the traveled distance on the LCD.

Timed Stop Execution: The line-following code was expanded to include a precise distance-triggered halt. The car tracked its distance, followed the line for exactly 340 cm, stopped for three seconds, and then successfully resumed following the path to completion.

Orientation Sensing and Ramp Navigation: An MPU-6050 Gyro Accelerometer sensor was integrated to detect inclines. The robot drove up a ramp, used trigonometry to calculate the incline angle (e.g., 25.6 degrees), stopped at the peak for four seconds to display this angle on the LCD, and executed a 360-degree rotation before moving down the ramp.

Obstacle Detection: An SN-HC-SR04 ultrasonic sensor was added to give the robot autonomous collision avoidance capabilities. By calculating the time of reflected pulses to determine distance, the car was programmed to stop upon detecting an obstacle, reverse, and turn left until the path was clear to continue moving forward.

Bluetooth Remote Control: An HC-05 Bluetooth Serial Module was wired to the Arduino, incorporating a voltage divider circuit to safely step down the Arduino's 5V transmission to the module's 3.3V receiver. This allowed the robot car to receive wireless commands (forward, backward, turn left, turn right) via a smartphone remote control application.

Demo video of line following with IR sensors: 

https://github.com/user-attachments/assets/05735924-5ad1-4c53-9993-8c0a3059afac

