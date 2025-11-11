

// ======== Motor A Connections ========
const int ENA = 5;    // Speed control (PWM)
const int IN1 = 4;   // Direction 1
const int IN2 = 4;   // Direction 2

// ======== Motor B Connections ========
const int ENB = 7;    // Speed control (PWM)
const int IN3 = 6;   // Direction 1
const int IN4 = 6;   // Direction 2

void setup() {
  // Set all motor pins as outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {

  // --- Move both motors forward ---
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 200);   // Motor A speed (0–255)
  analogWrite(ENB, 200);   // Motor B speed (0–255)
  delay(2000);

  // --- Move both motors backward ---
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
  delay(2000);

  // --- Apply braking (both motors stop instantly) ---
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  delay(1000);

  // --- Stop (coast stop – motors free spin) ---
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  delay(1000);

  // --- Forward again at different speeds (speed test) ---
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 128);  // Half speed
  analogWrite(ENB, 255);  // Full speed
  delay(2000);

  // Repeat
}
