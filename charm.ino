#include <Arduino.h>
#include <Servo.h>
#include <SoftwareSerial.h>
SoftwareSerial piSerial(10, 11);

// ── Motor Pins ────────────────────────────
#define LEFT_MOTOR_F  2
#define LEFT_MOTOR_B  3
#define RIGHT_MOTOR_F 4
#define RIGHT_MOTOR_B 5

// ── Arm Servos (PWM) ──────────────────────
Servo shoulder, elbow, wrist, claw;

// ── Motor Functions ───────────────────────
void moveForward() {
  digitalWrite(LEFT_MOTOR_F,  HIGH);
  digitalWrite(LEFT_MOTOR_B,  LOW);
  digitalWrite(RIGHT_MOTOR_F, HIGH);
  digitalWrite(RIGHT_MOTOR_B, LOW);
}

void moveBackward() {
  digitalWrite(LEFT_MOTOR_F,  LOW);
  digitalWrite(LEFT_MOTOR_B,  HIGH);
  digitalWrite(RIGHT_MOTOR_F, LOW);
  digitalWrite(RIGHT_MOTOR_B, HIGH);
}

void turnLeft() {
  digitalWrite(LEFT_MOTOR_F,  LOW);
  digitalWrite(LEFT_MOTOR_B,  HIGH);
  digitalWrite(RIGHT_MOTOR_F, HIGH);
  digitalWrite(RIGHT_MOTOR_B, LOW);
}

void turnRight() {
  digitalWrite(LEFT_MOTOR_F,  HIGH);
  digitalWrite(LEFT_MOTOR_B,  LOW);
  digitalWrite(RIGHT_MOTOR_F, LOW);
  digitalWrite(RIGHT_MOTOR_B, HIGH);
}

void stopMotors() {
  digitalWrite(LEFT_MOTOR_F,  LOW);
  digitalWrite(LEFT_MOTOR_B,  LOW);
  digitalWrite(RIGHT_MOTOR_F, LOW);
  digitalWrite(RIGHT_MOTOR_B, LOW);
}

// ── Arm Function ──────────────────────────
void pickTrash() {
  stopMotors();

  shoulder.write(45);   // arm নামাও
  delay(600);
  elbow.write(30);
  delay(600);
  claw.write(0);        // claw খোলো
  delay(500);
  claw.write(90);       // claw বন্ধ করো (grab)
  delay(800);
  shoulder.write(90);   // arm তোলো
  elbow.write(90);
  delay(600);
}

// ── Setup ─────────────────────────────────
void setup() {
  piSerial.begin(9600);   // Pi এর সাথে communication

  pinMode(LEFT_MOTOR_F,  OUTPUT);
  pinMode(LEFT_MOTOR_B,  OUTPUT);
  pinMode(RIGHT_MOTOR_F, OUTPUT);
  pinMode(RIGHT_MOTOR_B, OUTPUT);

  shoulder.attach(6);
  elbow.attach(7);
  wrist.attach(8);
  claw.attach(9);

  // Neutral position
  shoulder.write(90);
  elbow.write(90);
  wrist.write(90);
  claw.write(90);

  stopMotors();
}

// ── Loop ──────────────────────────────────
void loop() {
  if (piSerial.available()) {
    String cmd = piSerial.readStringUntil('\n');
    cmd.trim();

    if      (cmd == "FW") moveForward();
    else if (cmd == "BK") moveBackward();
    else if (cmd == "LT") turnLeft();
    else if (cmd == "RT") turnRight();
    else if (cmd == "ST") stopMotors();
    else if (cmd == "PK") pickTrash();

    // Manual servo control
    else if (cmd.startsWith("SH")) shoulder.write(cmd.substring(2).toInt());
    else if (cmd.startsWith("EL")) elbow.write(cmd.substring(2).toInt());
    else if (cmd.startsWith("WR")) wrist.write(cmd.substring(2).toInt());
    else if (cmd.startsWith("CL")) claw.write(cmd.substring(2).toInt());
  }
}
