#include <Servo.h>              // Include Servo library to control a servo motor

#define SERVO_PIN 7             // Digital pin 7 is used to control the servo signal
#define LED_PIN 4               // Digital pin 4 is connected to the LED
#define LDR_THRESHOLD 400       // Threshold value to decide when LED should turn ON based on LDR reading

Servo myServo;                  // Create a Servo object to control the servo motor

bool pcLedState = false;        // Stores LED state commanded manually from master (#a / #b)
bool ldrLedState = false;       // Stores LED state based on LDR sensor value

void setup() {
  Serial.begin(9600);           // Initialize serial communication at 9600 baud (Python reads from here)
  pinMode(LED_PIN, OUTPUT);     // Configure LED pin as output
  myServo.attach(SERVO_PIN);    // Attach the servo object to the defined servo pin

  Serial.println("READY");      // Send startup message (ignored by Python, useful for debugging)
}

void loop() {

  // Check if serial data is available from master Arduino
  if (Serial.available()) {

    // Read incoming serial message until newline character
    String msg = Serial.readStringUntil('\n');

    // Remove extra spaces or carriage return characters
    msg.trim();

    // ===== MANUAL LED COMMANDS FROM MASTER =====

    // If master sends "#a", force LED ON manually
    if (msg == "#a") {
      pcLedState = true;        // Set manual LED state to ON
      updateLED();              // Update LED using combined logic
      return;                   // Exit loop to prevent further processing
    }

    // If master sends "#b", force LED OFF manually
    if (msg == "#b") {
      pcLedState = false;       // Set manual LED state to OFF
      updateLED();              // Update LED using combined logic
      return;                   // Exit loop to prevent further processing
    }

    // ===== SENSOR DATA FROM MASTER =====
    // Expected message format: A=<angle>,L=<ldr_value>
    if (msg.startsWith("A=")) {

      // Find the index position of servo angle data
      int aIndex = msg.indexOf("A=");

      // Find the index position of LDR data
      int lIndex = msg.indexOf(",L=");

      // Ensure message format is valid
      if (lIndex > 0) {

        // Extract servo angle value from message
        int angle = msg.substring(aIndex + 2, lIndex).toInt();

        // Extract LDR sensor value from message
        int ldr = msg.substring(lIndex + 3).toInt();

        // Move servo to received angle (restricted between 0 and 180 degrees)
        myServo.write(constrain(angle, 0, 180));

        // Turn LED ON if LDR value exceeds threshold (darkness / object detected)
        ldrLedState = (ldr > LDR_THRESHOLD);

        // Update LED based on manual OR sensor condition
        updateLED();

        // ===== SEND CLEAN DATA TO PYTHON =====
        // Send servo angle value in CSV format
        Serial.print(angle);

        // Send comma separator
        Serial.print(",");

        // Send LDR value and newline for Python parsing
        Serial.println(ldr);
      }
    }
  }
}

// ===== LED UPDATE FUNCTION =====
// LED turns ON if:
// 1. Manual command requests ON OR
// 2. LDR sensor detects darkness / object
void updateLED() {
  digitalWrite(LED_PIN, (pcLedState || ldrLedState) ? HIGH : LOW);
}
