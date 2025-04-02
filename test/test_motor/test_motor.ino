#include <Servo.h>

const int currentPins[] = {A1, A2, A3, A4, A5}; // Pins for current sensors
const int servoPins[] = {3, 5, 6, 9, 10};  // Pins for 5 servos

// Servo objects
Servo servos[5];

// Thresholds and limits
const int currentThreshold = 512;  // Example threshold for current sensor
const int filterThreshold = 500;   // Example threshold for EMG signal to apply filter

void setup() {
    Serial.begin(9600);  // Start serial communication

    // Attach the 5 servos to their respective pins
    for (int i = 0; i < 5; i++) {
        servos[i].attach(servoPins[i]);
    }
}

void loop() {
    int motorCurrentData[5];  // Array for current sensor data   

    for (int i = 0; i < 5; i++) {
      motorCurrentData[i] = analogRead(currentPins[i]);
    }

    for (int i = 0; i < 5; i++) {
      // If the current sensor reading is below the threshold, move the servo
      if (motorCurrentData[i] <= currentThreshold) {
        servos[i].write(motorCurrentData[i]);  // Control servo with filtered EMG signal
      } 
      else {
        servos[i].write(90);  // Stop the servo if overcurrent is detected
        Serial.print("Overcurrent detected on Servo ");
        Serial.println(i + 1);
      }
    }

    delay(100);  // Delay to stabilize the readings
}
