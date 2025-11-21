#include <Arduino.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

namespace Pins {
    namespace MotorDriver {
        constexpr int enA = 3; // Right side motors 
        constexpr int in1 = 1;
        constexpr int in2 = 2;

        constexpr int enB = 11; // Left side motors
        constexpr int in3 = 12;
        constexpr int in4 = 13;
    }

    namespace Encoders {
        constexpr int right = A3;
        constexpr int left = A4;
    }
    
    namespace IR {
        constexpr int right = A1;
        constexpr int left = A2;
    }
}

constexpr int forwardSpeed = 85; 
constexpr int turnSpeedLeft = 140; 
constexpr int turnSpeedRight = 160; 

// IR sensor thresholds for analogRead
constexpr int leftThreshold = 300;
constexpr int rightThreshold = 600;

unsigned long startTime;
unsigned long finalTime = 0;
bool finished = false; 

struct Encoder {
    int steps = 0;
    bool prevState = false;
} leftEncoder, rightEncoder;

constexpr float wheelCircumference = 0.207; // in meters
constexpr int slits = 20;


void updateEncoders() {
    if (finished) return;

    bool leftState = digitalRead(Pins::Encoders::left);
    bool rightState = digitalRead(Pins::Encoders::right);

    if (leftState && !leftEncoder.prevState) leftEncoder.steps++;
    if (rightState && !rightEncoder.prevState) rightEncoder.steps++;

    leftEncoder.prevState = leftState;
    rightEncoder.prevState = rightState;
}

void updateLCD() {
    // Calculate distance
    float avgSteps = (leftEncoder.steps + rightEncoder.steps) / 2.0;
    float distanceTravelled = (avgSteps / slits) * wheelCircumference; // number of rotations mulitplied by circumference

    // Calculate time
    unsigned long timer = finished ? finalTime : (millis() - startTime);
    float timeInSeconds = timer / 1000.0;

    // Row 0: Distance
    lcd.setCursor(0, 0);
    lcd.print("Dist: ");
    lcd.print(distanceTravelled, 2); // 2 decimal places
    lcd.print("m ");

    // Row 1: Time
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(timeInSeconds, 2); // 2 decimal places
    lcd.print("s   "); // Extra spaces to clear any old artifacts
}

void setSpeed(int rightSpeed, int leftSpeed) {
    analogWrite(Pins::MotorDriver::enA, rightSpeed);
    analogWrite(Pins::MotorDriver::enB, leftSpeed);
}

void turnRight() {
    digitalWrite(Pins::MotorDriver::in1, LOW); // right motors turns backwards
    digitalWrite(Pins::MotorDriver::in2, HIGH);
    digitalWrite(Pins::MotorDriver::in3, HIGH); // left motors turn forwards
    digitalWrite(Pins::MotorDriver::in4, LOW);
    setSpeed(turnSpeedRight, turnSpeedRight);
}

void turnLeft() {
    digitalWrite(Pins::MotorDriver::in1, HIGH); // right motors turn forward
    digitalWrite(Pins::MotorDriver::in2, LOW);
    digitalWrite(Pins::MotorDriver::in3, LOW); // left motors turn backwards
    digitalWrite(Pins::MotorDriver::in4, HIGH);
    setSpeed(turnSpeedLeft, turnSpeedLeft);
}

void moveForward() {
    digitalWrite(Pins::MotorDriver::in1, HIGH); // right motors turn forwards
    digitalWrite(Pins::MotorDriver::in2, LOW);
    digitalWrite(Pins::MotorDriver::in3, HIGH); // left motors turn forwards
    digitalWrite(Pins::MotorDriver::in4, LOW);
    setSpeed(forwardSpeed, forwardSpeed);
}

void stopMotors() {
    setSpeed(0, 0);
    digitalWrite(Pins::MotorDriver::in1, LOW);
    digitalWrite(Pins::MotorDriver::in2, LOW);
    digitalWrite(Pins::MotorDriver::in3, LOW);
    digitalWrite(Pins::MotorDriver::in4, LOW);
}

void followLine() {
    if (finished) {
        stopMotors();
        updateLCD(); 
        return;
    }

    // check if the sensors see a line
    bool rightOnLine = (analogRead(Pins::IR::right) < rightThreshold);
    bool leftOnLine = (analogRead(Pins::IR::left) < leftThreshold);

    if (rightOnLine && leftOnLine) {
        finished = true;
        finalTime = millis() - startTime;
        stopMotors();
        updateLCD();
        return;
    }
    else if (rightOnLine) { turnRight(); }
    else if (leftOnLine) { turnLeft(); }
    else { moveForward(); }
}

void setup() {
    pinMode(Pins::MotorDriver::enA, OUTPUT);
    pinMode(Pins::MotorDriver::enB, OUTPUT);
    pinMode(Pins::MotorDriver::in1, OUTPUT);
    pinMode(Pins::MotorDriver::in2, OUTPUT);
    pinMode(Pins::MotorDriver::in3, OUTPUT);
    pinMode(Pins::MotorDriver::in4, OUTPUT);

    pinMode(Pins::Encoders::right, INPUT);
    pinMode(Pins::Encoders::left, INPUT);
    pinMode(Pins::IR::right, INPUT);
    pinMode(Pins::IR::left, INPUT);

    lcd.begin(16, 2);
    lcd.print("Ready...");
    delay(1000);
    lcd.clear();
    
    startTime = millis();
}

unsigned long lastLCDUpdate = 0;

void loop() {
    updateEncoders();
    followLine();

    if (millis() - lastLCDUpdate > 250) {
        updateLCD();
        lastLCDUpdate = millis();
    }
}