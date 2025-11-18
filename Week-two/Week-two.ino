#include<LiquidCrystal.h>

// Digital pins 10 to 4 are occupied by LCD display, 10 if for turning on backlight
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Motor driver pins - pins 3 & 11 are PWM pins
int enA = 3;
int enB = 11;
int in1 = A1;
int in2 = A2;
int in3 = A3;
int in4 = A4;

// Encoder pins
int rightEncoder = 2;
int leftEncoder = 1;

int rightSteps = 0;
int leftSteps = 0;
struct State {
  bool right = false;
  bool left = false;
} prevState, currentState;

void setup() {
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(rightEncoder, INPUT);
  pinMode(leftEncoder, INPUT);

  analogWrite(enA, 255);
  analogWrite(enB, 255);

  moveForward();

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Right: ");
  lcd.setCursor(7, 0);
  lcd.print(rightSteps);

  lcd.setCursor(0, 1);
  lcd.print("Left: ");
  lcd.setCursor(6, 1);
  lcd.print(leftSteps);

  currentState.right = digitalRead(rightEncoder);
  currentState.left = digitalRead(leftEncoder);
  prevState = currentState;
}

void loop() {
  currentState.right = digitalRead(rightEncoder);
  currentState.left = digitalRead(leftEncoder);
  if (currentState.right && !prevState.right) {
    rightSteps++;
    lcd.setCursor(7, 0);
    lcd.print(rightSteps);
  }
  if (currentState.left && !prevState.left) {
    leftSteps++;
    lcd.setCursor(6, 1);
    lcd.print(leftSteps);
  }

  prevState = currentState;
}

void moveForward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}
