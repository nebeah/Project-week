#include <Arduino.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // Digital pins 10 to 4 are occupied by LCD display, pin 10 is for turning on backlight

namespace Pins {
    namespace MotorDriver {
        constexpr int enA = 3; // right side motors
        constexpr int in1 = 1;
        constexpr int in2 = 2;

        constexpr int enB = 11; // left side motors
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
        constexpr int center = A5;
    }
}

constexpr int defaultSpeed = 100;
constexpr int maxSpeed = defaultSpeed * 2;
constexpr int minSpeed = defaultSpeed * 0.6;

// The analog threshold above which the IR sensors can be considered to be ON
constexpr int IRthreshold = 100;

struct Encoder {
    int steps = 0;
    bool prevState = false;
    float distance = 0;
} leftEncoder, rightEncoder;
constexpr float wheelCircumference = 0.207; // in metres
constexpr int slits = 20;

void updateDistance() {
    bool leftState = digitalRead(Pins::Encoders::left);
    bool rightState = digitalRead(Pins::Encoders::right);

    if (leftState && !leftEncoder.prevState) leftEncoder.steps++;
    if (rightState && !rightEncoder.prevState) rightEncoder.steps++;

    float avgSteps = (leftEncoder.steps + rightEncoder.steps) / 2;
    float rotations = avgSteps / slits;
    float distanceTravelled = rotations * wheelCircumference;

    lcd.setCursor(0, 0);
    lcd.print("Dist: ");
    lcd.print(distanceTravelled);
    lcd.print("m");

    leftEncoder.prevState = leftState;
    rightEncoder.prevState = rightState;
}

void setSpeed(int rightSpeed, int leftSpeed) {
    analogWrite(Pins::MotorDriver::enA, rightSpeed);
    analogWrite(Pins::MotorDriver::enB, leftSpeed);
}

void turnRight() {
    digitalWrite(Pins::MotorDriver::in1, LOW); // right motor go bacwards
    digitalWrite(Pins::MotorDriver::in2, HIGH);

    digitalWrite(Pins::MotorDriver::in3, HIGH); //left motor go backwards
    digitalWrite(Pins::MotorDriver::in4, LOW);

    setSpeed(defaultSpeed, maxSpeed);
}

void turnLeft() {
    digitalWrite(Pins::MotorDriver::in1, HIGH); // right motor go forwards
    digitalWrite(Pins::MotorDriver::in2, LOW);

    digitalWrite(Pins::MotorDriver::in3, LOW); //left motor go backwards
    digitalWrite(Pins::MotorDriver::in4, HIGH);

    setSpeed(maxSpeed, defaultSpeed);
}

void moveForward() {
    setSpeed(defaultSpeed, defaultSpeed);
}

bool rotating = false;
enum Turning {
    left,
    right
};

Turning turning = left;

void followLine() {
    // Check if the the sensors see a line
    bool rightIR = analogRead(Pins::IR::right) < IRthreshold;
    bool leftIR = analogRead(Pins::IR::left) < IRthreshold;

    if (turning == left) {
        if (rightIR) {
            turning = right;
            turnRight();
        }
    } else if (turning == right) {
        if (leftIR) {
            turning = left;
            turnLeft();
        }
    }
}

void setup() {
    Serial.begin(9600);

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

    digitalWrite(Pins::MotorDriver::in1, HIGH); // right motor go forwards
    digitalWrite(Pins::MotorDriver::in2, LOW);

    digitalWrite(Pins::MotorDriver::in3, HIGH); //left motor go forwards
    digitalWrite(Pins::MotorDriver::in4, LOW);
}

void loop() {
    updateDistance();
    followLine();
}