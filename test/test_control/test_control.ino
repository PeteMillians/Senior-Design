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

  int currentAngles[5];

  // Check that the EMG signal is powering the motors
  if (filteredSignal > SIGNAL_THRESHOLD) {

    // Iterate through each current sensor pin
    for (int i = 0; i < 5; i++) {

      // Retrieve current angle of motors
      currentAngles[i] = MOTORS[i].read();

      if (sensorReadings[i] > CURRENT_THRESHOLD) {    // If overdrawing current
        
        // Set to hold state
        Serial.print("Hold state. Angle = ");
        Serial.println(map(currentAngles[i], 0, 1023, 0, 360));
        MOTORS[i].write(map(currentAngles[i], 0, 1023, 0, 360));
      }

      else {
        // Set to turn state
        Serial.print("Turn state. Angle = ");
        Serial.println(map((currentAngles[i] + filteredSignal), 0, 1023, 0, 360));
        MOTORS[i].write(map((currentAngles[i] + filteredSignal), 0, 1023, 0, 360));
      }
    }
  }
  else {
    Serial.println("No muscle contraction");

    // Iterate through each current sensor pin
    for (int i = 0; i < 5; i++) { 
      // Set to release state
      MOTORS[i].write(0);
    
    }
  }
}
