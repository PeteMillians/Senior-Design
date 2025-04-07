#include <Servo.h>

enum MotorState {
  TURN,
  RELEASE,
  HOLD
};

struct motor {
  Servo servo;
  bool overdrawn = false;
  float totalRotation = 0.0;
  MotorState state = RELEASE;
  float sensorReading = 0.0;
};

/* Constants declared in header */
const int CURRENT_PINS[5] = {A1, A2, A3, A4, A5};
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

//   if(totalTime > 500) {
//     filteredSignal = 2;   // Off signal for release stae
//   }

//   Serial.print("Filtered Signal = ");
//   Serial.println(filteredSignal);

  float currentReadings[5];
  
  for (int i = 0; i < 5; i++) {
    MOTORS[i].sensorReading = float(analogRead(CURRENT_PINS[i]));
    Serial.print(MOTORS[i].sensorReading + String(" "));
  }
  Serial.println();

  ControlMotors(filteredSignal);

  delay(100);
  totalTime += 100;

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

    // Iterate through each current sensor pin
    for (int i = 0; i < sizeof(MOTORS) - 1; i++) {
      float currentThreshold = _getCurrentThreshold(); // Get current threshold of current motor states
      motor currMotor = MOTORS[i];

      if (currMotor.sensorReading < currentThreshold) {
        currMotor.state = HOLD;
      }
      else {
        currMotor.state = TURN;
      }
      _UpdateState(currMotor, filteredSignal);
    }
  }
  else {
    for (int i = 0; i < sizeof(MOTORS) - 1; i++) {
      MOTORS[i].state = RELEASE;
      _UpdateState(MOTORS[i], 0.0);
    }
  }
}

void _UpdateState(motor currMotor, float filteredSignal) {
/*
  - Function:
    - Updates the state of a motor and sets its values accordingle
  - Arguments:
    - currMotor (motor): motor struct of the current motor being iterated
    - filteredSignal (float): EMG reading from MyoWare
*/

switch(currMotor.state) {
  case (TURN):
    // Set to turn state
    currMotor.overdrawn = false;
    float rotation = constrain(map(filteredSignal, SIGNAL_THRESHOLD, 205, 90, 180), 90, 180);    // Map signal to a rotation speed
    currMotor.servo.write(rotation);
    currMotor.totalRotation += rotation;    
    break;
  case (HOLD):
    // int stallIndex = _getStallIndex(currMotor.sensorReading, currentThreshold);

    // // // If this index isn't the stalled one, keep moving
    // if (stallIndex == -1) {
    //   break;
    // }
    currMotor.overdrawn = true;  // Record that this motor has overdrawn current
    currMotor.servo.write(90);
    break;
  case (RELEASE):
    // Release state
    currMotor.overdrawn = false;
    // Check if the motor has moved at all yet
    if (currMotor.totalRotation > 0) {
      currMotor.servo.write(80);  // slowly reverse motor
      currMotor.totalRotation -= RELEASE_STEP;  // arbitrary value (requires testing)

      if (currMotor.totalRotation <= 0) { // once the motor has gotten to its return state
        currMotor.totalRotation = 0;
        currMotor.servo.write(90);  // stop movement
      }
    } 
    else {
      currMotor.servo.write(90);  // already at original position, stop
    }
    break;
}
}

float _getCurrentThreshold() {
  /*
      - Function:
          - Calculates current threshold value based on number of stalled motors
      - Arguments:
          - overdrawn: array of booleans that signify if a motor is overdrawn
      - Returns:
          - float: threshold value
  */

  int numStalled = 0;
  for (int i = 0; i < sizeof(MOTORS) - 1; i++) {
    if (MOTORS[i].overdrawn) {
      numStalled++;
    }
  }

  float threshold = 460 - (35 * numStalled);

  return threshold;
}
