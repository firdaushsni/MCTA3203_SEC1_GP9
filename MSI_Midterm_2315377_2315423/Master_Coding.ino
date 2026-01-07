#include <SoftwareSerial.h>        // Library to create serial communication on digital pins

#define POT_PIN A0                // Analog pin A0 connected to potentiometer
#define LDR_PIN A5                // Analog pin A5 connected to LDR (light sensor)

// Create a software serial port to communicate with SLAVE Arduino
// Pin 11 = RX (receives data), Pin 10 = TX (sends data)
SoftwareSerial slaveSerial(11, 10); 

void setup() {
  Serial.begin(9600);             // Start USB serial communication (PC ↔ Arduino, used by Python)
  slaveSerial.begin(9600);        // Start software serial communication (Master ↔ Slave)
}

void loop() {

  // ===== READ SENSORS =====

  int potValue = analogRead(POT_PIN);     
  // Read potentiometer value (0–1023) from analog pin A0

  int servoAngle = map(potValue, 0, 1023, 0, 180);  
  // Convert potentiometer value into servo angle (0–180 degrees)

  int ldrValue = analogRead(LDR_PIN);     
  // Read LDR (light sensor) value (0–1023) from analog pin A5


  // ===== SEND TO SLAVE =====

  slaveSerial.print("A=");        
  // Send the identifier "A=" to indicate servo angle data

  slaveSerial.print(servoAngle);  
  // Send the calculated servo angle value to the slave

  slaveSerial.print(",L=");       
  // Send delimiter ",L=" to indicate LDR data follows

  slaveSerial.println(ldrValue);  
  // Send LDR value and newline to mark end of message


  // ===== SEND TO PYTHON (CSV FORMAT) =====

  Serial.print(servoAngle);       
  // Send servo angle to PC (Python) without newline

  Serial.print(",");              
  // Send comma to separate values (CSV format)

  Serial.println(ldrValue);       
  // Send LDR value followed by newline for Python parsing


  // ===== READ COMMAND FROM PC =====

  if (Serial.available()) {       
  // Check if a character has been sent from PC (keyboard / Python)

    char cmd = Serial.read();     
    // Read one character command from the serial buffer

    if (cmd == 'a') {             
    // If the user presses 'a' on the keyboard

      slaveSerial.println("#a");  
      // Send command "#a" to slave (LED ON command)

      Serial.println("CMD,LED_ON");
      // Send command feedback to Python (command log)
    }

    if (cmd == 'b') {             
    // If the user presses 'b' on the keyboard

      slaveSerial.println("#b");  
      // Send command "#b" to slave (LED OFF command)

      Serial.println("CMD,LED_OFF");
      // Send command feedback to Python (command log)
    }
  }

  delay(100);                     
  // Pause for 100 milliseconds to stabilize serial communication
}
