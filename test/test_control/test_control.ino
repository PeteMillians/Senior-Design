#include <Servo.h>

/* Constants declared in header */
const int CURRENT_PINS[5] = {A1, A2, A3, A4, A5};
const int MOTOR_PINS[5] = {3, 5, 6, 9, 11};
const float CURRENT_THRESHOLD = 450;   // Example current threshold 
const float SIGNAL_THRESHOLD = 6.1;   // Example voltage threshold
const Servo MOTORS[5];
float totalRotation[5] = {0.0, 0.0, 0.0, 0.0, 0.0}; // array of total angle rotated by each motor
bool isOverdrawn[5] = {false, false, false, false, false};
const float RELEASE_STEP = 20; // constant for how much the totalRotation will decrement each clock cycle during release

int totalTime = 0;

void setup() {

  Serial.begin(9600);  // Start serial communication

  // Iterate through each motor
  for (int i = 0; i < 5; i++) {
    MOTORS[i].attach(MOTOR_PINS[i]); // Attach motors to their output pins
  }

}

void loop() {

  float filteredSignal = 150;   // On signal for turn state

//   if(totalTime > 500) {
//     filteredSignal = 2;   // Off signal for release stae
//   }

//   Serial.print("Filtered Signal = ");
//   Serial.println(filteredSignal);

  float currentReadings[5];
  
  for (int i = 0; i < 5; i++) {
    currentReadings[i] = float(analogRead(CURRENT_PINS[i]));
    Serial.print(currentReadings[i] + String(" "));
    }
    Serial.println();

  ControlMotors(filteredSignal, currentReadings);

  delay(100);
  totalTime += 100;

}

void ControlMotors(float filteredSignal, float sensorReadings[]) {
    /*
    - Function:
        - Full control algorithm for motors
    - Arguments:
        - filteredSignal (float): Filtered EMG data
        - sensorReadings (floats): Current sensor readings
    */

    int stallIndex = -1;    // Initialize stallIndex out of range

    // Check that the EMG signal is powering the motors
    if (filteredSignal > SIGNAL_THRESHOLD) {

        // Iterate through each current sensor pin
        for (int i = 0; i < 5; i++) {

            if (sensorReadings[i] < CURRENT_THRESHOLD) {    // If overdrawing current

                if (isOverdrawn[i] && i == stallIndex) {   // If it is consecutively overdrawn
                    // Set to hold state
                    MOTORS[i].write(90);
                    continue;  // This should break out of line 133 loop, but remain in for-loop
                }

                stallIndex = sensorReadings.index(min(sensorReadings)); // Index of stalled motor

                isOverdrawn[i] = true;  // Record that this motor has overdrawn current

                // Continue rotating
                // Serial.println("Hold State");
                float rotation = constrain(map(filteredSignal, SIGNAL_THRESHOLD, 205, 90, 180), 90, 180);    // Map signal to a rotation speed
                // Serial.print("Rotation Speed = ");
                // Serial.println(rotation);
                MOTORS[i].write(rotation);
                totalRotation[i] += rotation;    
            }

            else {
                // Set to turn state
                // Serial.println("Turn State");
                isOverdrawn[i] = false;
                float rotation = map(filteredSignal, SIGNAL_THRESHOLD, 205, 90, 180);    // Map signal to a rotation speed
                // Serial.print("Rotation Speed = ");
                // Serial.println(rotation);
                MOTORS[i].write(rotation);
                totalRotation[i] += rotation;    
            }
        }
    }
    else {
        // Release state
        for (int i = 0; i < 5; i++) {
            isOverdrawn[i] = false;
            Serial.println(totalRotation[i]);

            // Check if the motor has moved at all yet
            if (totalRotation[i] > 0) {
                MOTORS[i].write(80);  // reverse motor
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
