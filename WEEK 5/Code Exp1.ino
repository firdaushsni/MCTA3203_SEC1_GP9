EXAMPLE 1

// --- Mechatronic System Integration (MCTA3203) ---
// Controlling Two DC Motors using L298P Motor Driver Shield and GPIO

// Motor A pins
const int ENA = 5;    // PWM pin for Motor A speed
const int IN1 = 4;   // Direction pin 1 for Motor A
const int IN2 = 4;   // Direction pin 2 for Motor A

// Motor B pins
const int ENB = 7;   // PWM pin for Motor B speed
const int IN3 = 6;    // Direction pin 1 for Motor B
const int IN4 = 6;    // Direction pin 2 for Motor B

void setup() {
  // Set all pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);
  Serial.println("Dual Motor Control Initialized.");
}

void loop() {
  // --- Move both motors forward ---
  moveMotor(ENA, IN1, IN2, 200, true);  // Motor A forward
  moveMotor(ENB, IN3, IN4, 200, true);  // Motor B forward
  Serial.println("Both motors moving forward...");
  delay(2000);

  // --- Move both motors backward ---
  moveMotor(ENA, IN1, IN2, 200, false);
  moveMotor(ENB, IN3, IN4, 200, false);
  Serial.println("Both motors moving backward...");
  delay(2000);

  // --- Stop both motors ---
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  Serial.println("Motors stopped.");
  delay(1000);
}

// --- Function to control a motor's direction and speed ---
void moveMotor(int EN, int IN1, int IN2, int speed, bool forward) {
  if (forward) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(EN, speed);
}
