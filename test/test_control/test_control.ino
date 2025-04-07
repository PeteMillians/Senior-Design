#include <Servo.h>

enum MotorState {
  TURN,
  RELEASE,
  HOLD
};

struct motor {
  Servo servo;
  int overdrawn = 0;
  float totalRotation = 0.0;
  MotorState state = RELEASE;
  float sensorReading = 0.0;
};

/* Constants declared in header */
const int CURRENT_PINS[5] = {A1, A2, A3, A4, A5};
const int NUM_MOTORS = 5;
const int MOTOR_PINS[5] = {3, 5, 6, 9, 11};
const float CURRENT_THRESHOLD = 450;   // Example current threshold 
const float SIGNAL_THRESHOLD = 6.1;   // Example voltage threshold
motor MOTORS[5];
const float RELEASE_STEP = 20; // constant for how much the totalRotation will decrement each clock cycle during release

int totalTime = 0;

void setup() {

  Serial.begin(9600);  // Start serial communication

  // Iterate through each motor
  for (int i = 0; i < 5; i++) {
    MOTORS[i].servo.attach(MOTOR_PINS[i]); // Attach motors to their output pins
  }

}

void loop() {

  float filteredSignal = 150;   // On signal for turn state

  float currentReadings[5];
  
  for (int i = 0; i < 5; i++) {
    MOTORS[i].sensorReading = float(analogRead(CURRENT_PINS[i])); // Save current-sensor value for motor
    Serial.print(MOTORS[i].sensorReading + String(" "));  // Print current sensor values to output all on 1 line
  }
  Serial.println();

  ControlMotors(filteredSignal);  // Send EMG signal to motor control algorithm

  delay(2000);  // 2 sec delay for testing
}

void ControlMotors(float filteredSignal) {
  /*
  - Function:
      - Full control algorithm for motors
  - Arguments:
      - filteredSignal (float): Filtered EMG data
  */ 

  // Check that the EMG signal is powering the motors
  if (filteredSignal > SIGNAL_THRESHOLD) {
    Serial.println("EMG Signal above threshold");

    // Iterate through each current sensor pin
    for (int i = 0; i < 5; i++) {
      Serial.println("CurrentSensor from motor " + String(i + 1) + " = " + String(MOTORS[i].sensorReading) + "; CURRENT_THRESHOLD = " + String(CURRENT_THRESHOLD));
      if (MOTORS[i].sensorReading < CURRENT_THRESHOLD) { // Check if that motor's current reading is less than the current threshold
        Serial.println("Updating motor " + String(i + 1) + " to HOLD state");
        MOTORS[i].state = HOLD; // Set motorState to HOLD
      }
      else {
        Serial.println("Updating motor " + String(i + 1) + " to TURN state");
        MOTORS[i].state = TURN; // Set motorState to TURN
      }
      _UpdateState(MOTORS[i], filteredSignal);  // Update the state
    }
  }
  else {
    for (int i = 0; i < 5; i++) {
      Serial.println("Updating motor " + String(i + 1) + " to RELEASE state");
      MOTORS[i].state = RELEASE;  // Set the motor state to RELEASE 
      _UpdateState(MOTORS[i], 0.0); // Update the motor states to RELEASE
    }
  }
}

void _UpdateState(motor& currMotor, float filteredSignal) {
  /*
    - Function:
      - Updates the state of a motor and sets its values accordingle
    - Arguments:
      - currMotor (motor): motor struct of the current motor being iterated
      - filteredSignal (float): EMG reading from MyoWare
  */

  if (currMotor.overdrawn > 0 && currMotor.overdrawn < 15) { // No matter the new state, keep HOLD state for 15 clock cycles
    currMotor.state = HOLD;
  }

  switch(currMotor.state) {
    case (TURN):
      _UpdateTurnState(currMotor, filteredSignal);
      break;
    case (HOLD):
      _UpdateHoldState(currMotor, filteredSignal);
      break;
    case (RELEASE):
      _UpdateReleaseState(currMotor, filteredSignal);
      break;
  }
}

void _UpdateTurnState(motor& currMotor, float filteredSignal) {
  /*
    - Function:
      - Updates the currMotor to the TURN state
      - Writes a rotation value between 90 and 180 based on filteredSignal level
    - Arguments:
      - currMotor (motor): the current motor we are iterating through
      - filteredSignal (float): the EMG signal from the MyoWare sensor
  */

  // Set to turn state
  currMotor.overdrawn = 0;  // Reset overdrawn counter

  float rotation = constrain(map(filteredSignal, SIGNAL_THRESHOLD, 205, 90, 180), 90, 180);    // Map signal to a positive rotation

  currMotor.servo.write(rotation);  // Send rotation signal to the servo

  currMotor.totalRotation += rotation;  // Log that rotation (used for returning to original positon)
}

void _UpdateReleaseState(motor& currMotor, float filteredSignal) {
  /*
    - Function:
      - Updates the currMotor to the RELEASE state
      - Writes a rotation value of 80 to the motor while decrementing the totalRotation
    - Arguments:
      - currMotor (motor): the current motor we are iterating through
      - filteredSignal (float): the EMG signal from the MyoWare sensor
  */

  currMotor.overdrawn = 0;  // Reset overdrawn counter

  // Check if the motor has moved at all yet
  if (currMotor.totalRotation > 0) {  // If it has moved at all

    currMotor.servo.write(80);  // slowly reverse motor

    currMotor.totalRotation -= RELEASE_STEP;  // Decrement the totalRotation 

    if (currMotor.totalRotation <= 0) { // once the motor has gotten to its return state
      currMotor.totalRotation = 0;  // Reset totalRotation count
      currMotor.servo.write(90);  // stop movement
    }
  } 
  else {
    currMotor.servo.write(90);  // already at original position, stop
  }
}

void _UpdateHoldState(motor& currMotor, float filteredSignal) {
  /*
    - Function:
      - Updates the currMotor to the HOLD state
      - Writes a rotation of 90 to the servo, keeping it still
    - Arguments:
      - currMotor (motor): the current motor we are iterating through
      - filteredSignal (float): the EMG signal from the MyoWare sensor
  */

  currMotor.overdrawn++;  // Increment overdrawn current
  currMotor.servo.write(90);  // Write no movement to the motor
}
