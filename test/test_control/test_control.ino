#include <Servo.h>

/* Constants declared in header */
const int CURRENT_PINS[5] = {A1, A2, A3, A4, A5};
const int MOTOR_PINS[5] = {3, 5, 6, 9, 10};
const int CURRENT_THRESHOLD = 2;   // Example current threshold level in range (0 : 1023)
const int SIGNAL_THRESHOLD = 0.03;   // Example voltage threshold level in range (0 : 1023)
const Servo MOTORS[5];
float totalRotation[5] = {0.0, 0.0, 0.0, 0.0, 0.0}; // array of total angle rotated by each motor
const float RELEASE_STEP = 10.0; // constant for how much the totalRotation will decrement each clock cycle during release

void setup() {

  Serial.begin(9600);  // Start serial communication

  // Iterate through each motor
  for (int i = 0; i < 5; i++) {
    MOTORS[i].attach(MOTOR_PINS[i]); // Attach motors to their output pins
  }

}

void loop() {

  float filteredSignal = float(rand() % 10) / 100;   // Random number between 0 and 0.9

  Serial.print("Filtered Signal = ");
  Serial.println(filteredSignal);

  float currentReadings[5];
  
  for (int i = 0; i < 5; i++) {
    currentReadings[i] = float(analogRead(CURRENT_PINS[i]));
    }

    ControlMotors(filteredSignal, currentReadings);

}

void ControlMotors(float filteredSignal, float sensorReadings[]) {
    /*
    - Function:
        - Full control algorithm for motors
    - Arguments:
        - filteredSignal (float): Filtered EMG data
        - sensorReadings (floats): Current sensor readings
    */


    // Check that the EMG signal is powering the motors
    if (filteredSignal > SIGNAL_THRESHOLD) {

        // Iterate through each current sensor pin
        for (int i = 0; i < 5; i++) {

            if (sensorReadings[i] > CURRENT_THRESHOLD) {    // If overdrawing current

                if (isOverdrawn[i]) {   // If it is consecutively overdrawn
                    // Set to hold state
                    MOTORS[i].write(90);
                    continue;  // This should break out of line 133 loop, but remain in for-loop
                }

                isOverdrawn[i] = true;  // Record that this motor has overdrawn current

                // Continue rotating
                float rotation = map(filteredSignal, SIGNAL_THRESHOLD, 0.5, 90, 180);    // Map signal to a rotation speed
                MOTORS[i].write(rotation);
                totalRotation[i] += rotation;    
            }

            else {
                // Set to turn state
                isOverdrawn[i] = false;
                float rotation = map(filteredSignal, SIGNAL_THRESHOLD, 0.5, 90, 180);    // Map signal to a rotation speed
                MOTORS[i].write(rotation);
                totalRotation[i] += rotation;    
            }
        }
    }
    else {
        // Release state
        for (int i = 0; i < 5; i++) {
            isOverdrawn[i] = false;

            // Check if the motor has moved at all yet
            if (totalRotation[i] > 0) {
                MOTORS[i].write(80);  // slowly reverse motor
                totalRotation[i] -= RELEASE_STEP;  // arbitrary value (requires testing)

                if (totalRotation[i] <= 0) { // once the motor has gotten to its return state
                    totalRotation[i] = 0;
                    MOTORS[i].write(90);  // stop movement
                }
            } 
            else {
                MOTORS[i].write(90);  // already at original position, stop
            }
        }
    }

}
