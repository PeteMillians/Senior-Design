#include <Servo.h>

/* Constants declared in header */
const int CURRENT_PINS[5] = {A1, A2, A3, A4, A5};
const int MOTOR_PINS[5] = {3, 5, 6, 9, 10};
const int CURRENT_THRESHOLD = 500;   // Example current threshold level in range (0 : 1023)
const int SIGNAL_THRESHOLD = 500;   // Example voltage threshold level in range (0 : 1023)
const Servo MOTORS[5];

void setup() {

  Serial.begin(9600);  // Start serial communication

  // Iterate through each motor
  for (int i = 0; i < 5; i++) {
    MOTORS[i].attach(MOTOR_PINS[i]); // Attach motors to their output pins
  }

}

void loop() {

  int filteredSignal = rand() % 1024;   // Random number between 0 and 1023

  Serial.print("Filtered Signal = ");
  Serial.println(filteredSignal);

  int currentReadings[5];
  
  for (int i = 0; i < 5; i++) {
      currentReadings[i] = analogRead(CURRENT_PINS[i]);
    }
    
    ControlMotors(filteredSignal, currentReadings);
    
  }
  
  void ControlMotors(float filteredSignal, int sensorReadings[]) {
    /*
    - Function:
        - Full control algorithm for motors
    - Arguments:
        - filteredSignal (float): Filtered EMG data
        - sensorReadings (ints): Current sensor readings
    */

    // Check that the EMG signal is powering the motors
    if (filteredSignal > SIGNAL_THRESHOLD) {

        // Iterate through each current sensor pin
        for (int i = 0; i < 5; i++) {

            if (sensorReadings[i] > CURRENT_THRESHOLD) {    // If overdrawing current
                // Set to hold state
                Serial.print("Motor ");
                Serial.print(i + 1);
                Serial.println(" in hold state.");
                MOTORS[i].write(90);
            }

            else {
                float rotation = map(filterSignal, SIGNAL_THRESHOLD, 1023, 90, 180);
                // Set to turn state
                Serial.print("Motor ");
                Serial.print(i + 1);
                Serial.print(" in turn State. Rotation = ");
                Serial.println(rotation);
                MOTORS[i].write(rotation);    // Map signal to a rotation speed
            }
        }
    }
    else {
        // Iterate through each current sensor pin
        for (int i = 0; i < 5; i++) {

            // Set to release state
            Serial.print("Motor ");
            Serial.print(i + 1);
            Serial.println(" in release state.");
            MOTORS[i].write(0);
        }
    }

}
