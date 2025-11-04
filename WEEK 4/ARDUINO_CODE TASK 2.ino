#include <Wire.h>
#include <MPU6050.h>
#include <Servo.h>

MPU6050 mpu;
Servo myServo;

const int GREEN_LED = 4;
const int RED_LED = 3;
const int SERVO_PIN = 9;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  Serial.println("System Ready");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    switch (cmd) {
      case 'A':  // Motion detected and authorized
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        myServo.write(60);
        delay(5000);
        digitalWrite(GREEN_LED, LOW);
        myServo.write(0);
        break;

      case 'D':  // No motion or unauthorized
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        myServo.write(0);
        delay(3000);
        digitalWrite(RED_LED, LOW);
        break;

      case 'M':  // MPU6050 motion data request
        int16_t ax, ay, az, gx, gy, gz;
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        Serial.print(ax); Serial.print(",");
        Serial.print(ay); Serial.print(",");
        Serial.print(az); Serial.print(",");
        Serial.print(gx); Serial.print(",");
        Serial.print(gy); Serial.print(",");
        Serial.println(gz);
        break;
    }
  }
}
