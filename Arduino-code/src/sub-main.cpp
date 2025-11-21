// #include<Arduino.h>
// // Simple 3-IR-sensor line follower for Arduino using L298N motor driver
// // Author: GitHub Copilot (customized for you)
// // Notes:
// // - Supports either digital (digitalRead) or analog (analogRead + threshold) IR sensor modules.
// // - Works with typical L298N pinout: ENA/ENB are PWM pins to control motor speeds,
// //   IN1/IN2 control left motor direction, IN3/IN4 control right motor direction.
// // - Tweak pin assignments, baseSpeed, thresholds and boolean flags to match your hardware.

// // ----------------------- USER CONFIG -----------------------
// const bool sensorsAreAnalog = false;     // Set true if you're using analog IR sensors (read by analogRead)
// const bool sensorActiveLow = true;       // Set true if the sensor outputs LOW when on the black line (common for IR modules)
// const int analogThreshold = 100;         // If using analog sensors, value above/below this indicates line (depends on module)
// const int baseSpeed = 180;               // 0-255 PWM base forward speed
// const int turnSpeed = 160;               // Speed used when turning
// // -----------------------------------------------------------

// // IR sensor pins (change to match your wiring)
// const int leftSensorPin   = A2;   // if analog, can use A0/A1/A2 (use A0..A5 constants)
// const int centerSensorPin = A5;
// const int rightSensorPin  = A1;

// // L298N pin assignments (change to match your wiring)
// const int enA = 3;   // PWM - left motor speed
// const int in1 = 2;   // left motor direction
// const int in2 = 1;

// const int enB = 11;   // PWM - right motor speed
// const int in3 = 12;  // right motor direction
// const int in4 = 13;

// enum LastSeen {
//   SEEN_NONE = 0,
//   SEEN_LEFT,
//   SEEN_CENTER,
//   SEEN_RIGHT
// };

// LastSeen lastSeen = SEEN_CENTER;

// // Reads a sensor pin and returns true if it detects the line according to configuration
// bool readSensor(int pin) {
//   if (sensorsAreAnalog) {
//     int raw = analogRead(pin); // 0..1023
//     if (sensorActiveLow) {
//       // Many IR analogs produce low values on dark surfaces; adjust if needed
//       return raw < analogThreshold;
//     } else {
//       return raw > analogThreshold;
//     }
//   } else {
//     int val = digitalRead(pin); // HIGH or LOW
//     if (sensorActiveLow) {
//       // Module gives LOW when on line
//       return val == LOW;
//     } else {
//       return val == HIGH;
//     }
//   }
// }

// void setMotorLeft(int speed) {
//   if (speed > 0) {
//     digitalWrite(in1, HIGH);
//     digitalWrite(in2, LOW);
//     analogWrite(enA, constrain(speed, 0, 255));
//   } else if (speed < 0) {
//     digitalWrite(in1, LOW);
//     digitalWrite(in2, HIGH);
//     analogWrite(enA, constrain(-speed, 0, 255));
//   } else {
//     // Stop left motor (coast)
//     digitalWrite(in1, LOW);
//     digitalWrite(in2, LOW);
//     analogWrite(enA, 0);
//   }
// }

// void setMotorRight(int speed) {
//   if (speed > 0) {
//     digitalWrite(in3, HIGH);
//     digitalWrite(in4, LOW);
//     analogWrite(enB, constrain(speed, 0, 255));
//   } else if (speed < 0) {
//     digitalWrite(in3, LOW);
//     digitalWrite(in4, HIGH);
//     analogWrite(enB, constrain(-speed, 0, 255));
//   } else {
//     // Stop right motor (coast)
//     digitalWrite(in3, LOW);
//     digitalWrite(in4, LOW);
//     analogWrite(enB, 0);
//   }
// }

// // Set left and right motor speeds where speed range is -255..255
// void setMotors(int leftSpeed, int rightSpeed) {
//   setMotorLeft(leftSpeed);
//   setMotorRight(rightSpeed);
// }

// void stopMotors() {
//   setMotors(0, 0);
// }

// void setup() {
//   // Sensor pins
//   if (sensorsAreAnalog) {
//     // If using analog pins but wired to A0/A1/A2 you should set the pin constants to A0..A2.
//     // No pinMode needed for analogRead.
//   } else {
//     pinMode(leftSensorPin, INPUT);
//     pinMode(centerSensorPin, INPUT);
//     pinMode(rightSensorPin, INPUT);
//   }

//   // Motor pins
//   pinMode(enA, OUTPUT);
//   pinMode(in1, OUTPUT);
//   pinMode(in2, OUTPUT);

//   pinMode(enB, OUTPUT);
//   pinMode(in3, OUTPUT);
//   pinMode(in4, OUTPUT);

//   // Stop motors at start
//   stopMotors();
// }

// void loop() {
//   bool leftActive = readSensor(leftSensorPin);
//   bool centerActive = readSensor(centerSensorPin);
//   bool rightActive = readSensor(rightSensorPin);

//   // Update lastSeen for recovery behavior
//   if (leftActive && !centerActive && !rightActive) lastSeen = SEEN_LEFT;
//   else if (rightActive && !centerActive && !leftActive) lastSeen = SEEN_RIGHT;
//   else if (centerActive) lastSeen = SEEN_CENTER;

//   // Decision logic (simple finite-state, can be tuned)
//   if (centerActive && !leftActive && !rightActive) {
//     // Center only -> go straight
//     setMotors(baseSpeed, baseSpeed);
//   }
//   else if (leftActive && !centerActive && !rightActive) {
//     // Left only -> sharp left turn
//     // Reduce left motor, keep right motor high to pivot left
//     setMotors(baseSpeed/3, turnSpeed);
//   }
//   else if (rightActive && !centerActive && !leftActive) {
//     // Right only -> sharp right turn
//     setMotors(turnSpeed, baseSpeed/3);
//   }
//   else if (leftActive && centerActive && !rightActive) {
//     // Left + center -> slight left
//     setMotors(baseSpeed/1.5, baseSpeed);
//   }
//   else if (rightActive && centerActive && !leftActive) {
//     // Right + center -> slight right
//     setMotors(baseSpeed, baseSpeed/1.5);
//   }
//   else if (leftActive && rightActive && !centerActive) {
//     // Both outer sensors see the line: likely on an intersection or wide line -> go forward slowly
//     setMotors(baseSpeed/1.2, baseSpeed/1.2);
//   }
//   else if (leftActive && centerActive && rightActive) {
//     // All three detect: intersection or very dark spot -> go forward
//     setMotors(baseSpeed, baseSpeed);
//   }
//   else {
//     // No sensors active -> lost
//     // Recovery behavior: rotate toward last seen side
//     if (lastSeen == SEEN_LEFT) {
//       // rotate left to search
//       setMotors(-turnSpeed, turnSpeed/2);
//     } else if (lastSeen == SEEN_RIGHT) {
//       // rotate right
//       setMotors(turnSpeed/2, -turnSpeed);
//     } else {
//       // Unknown - stop and slowly spin to find line
//       setMotors(turnSpeed/2, -turnSpeed/2);
//     }
//   }

//   delay(10); // small loop delay; reduce for faster reaction
// }