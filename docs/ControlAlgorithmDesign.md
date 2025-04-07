# Control Algorithm Design

The Control Algorithm is a looped process which handles the control of the DC Servo Motors. 

## Requirements
**For each motor:**
1. Power DC Motor during muscle contraction
2. Read DC Motor's current using current sensor input
3. Stop DC Motor during overdraw
4. Control 3 motor states:
- Turn
- Hold
- Release

## Motor States

The follow section outlines the 3 motor states which describe the motor.

#### Turn State

During the ***turn*** state, the motor should be in constant power and rotation. The motor will wind the finger up to simulate gripping

```c++
myServo.write(rotation speed between 90 and 180)
```

#### Hold State

During the ***hold*** state, the motor should stop rotating, but not unwind. The motor will simply hold its position

Because there are natural current spikes which fall above our current threshold, we need to implement a check that the current is "overdrawing" consecutively in time

```c++
myServo.write(90)
```

#### Release State

During the ***release*** state, the motor should stop rotating and unwind. The motor should simulate releasing its grip

```c++
myServo.write(rotation speed between 0 and 90)
```

## Public Methods
- ***void ControlMotors(float filteredSignal, float sensorReadings[])***
    - Function:
        - Full control algorithm for motors
    - Arguments:
        - filteredSignal (float): Filtered EMG data
        - sensorReadings (floats): Current sensor readings

## Private Methods
- ***void _UpdateState(motor currMotor, float sensorReading)***
  - Function:
    - Updates the state of a motor and sets its values accordingle
  - Arguments:
    - currMotor (motor): motor struct of the current motor being iterated
    - sensorReading (float): current-sensor reading of this specific motor
    
- ***int _getStallIndex(float sensorReadings[], bool overdrawn[], float currentThreshold)***
  - Function:
    - Iterate through sensors and motors to find which one is stalled
  - Arguments:
    - sensorReadings (floats): array of current-sensor values
    - overdrawn (bools): array of booleans that signify if a motor is overdrawn
    - currentThreshold (float): the minimum current that the sensorReadings must be for the motor to not be stalled
  - Returns:
    - An int value between 0 and the number of motors - 1 which symbolizes the index of the stalled motor

- ***float _getCurrentThreshold(bool overdrawn[])***
    - Function:
        - Calculates current threshold value based on number of stalled motors
    - Arguments:
        - overdrawn (bools): array of booleans that signify if a motor is overdrawn
    - Returns:
        - float: threshold value

## Testing

To test the Control Motor algorithm, we will copy the algorithm show below, fill in the setup function proprerly, and use a random number generator to simulate an input signal for the EMG signal. We will use the ```Serial.print()``` method to print the random signal so we can ensure the motor is acting as intended. We will add print statements in the ```ControlMotor()``` method to print the state and the angle being written to ensure the function acts properly. We will also use print statements to log the current sensor readings to ensure the current levels fall below threshold (which also allows us to calibrate the threshold level). Throughout testing, we will sporadically use our hands to manually stop the motor, forcing overcurrent. We will ensure the proper current senses overdraw and turns to the hold state. The ```loop``` method will look something like this:

```c++

float filteredSignal = float(rand() % 10) / 100;   // Random number between 0 and .09

float currentReadings[5];

for (int i = 0; i < 5; i++) {
    currentReadings[i] = analogRead(CURRENT_PINS[i]);
}

ControlMotors(filteredSignal, currentReadings);

```

## Algorithm

```c++
/* Constants declared in header */
const int CURRENT_PINS[5] = {A1, A2, A3, A4, A5};
const int MOTOR_PINS[5] = {9, 10, 11, 12, 13};
// const float CURRENT_THRESHOLD = 450;   // Example current threshold level in range (0 : 1023)
const float SIGNAL_THRESHOLD = 6.1;   // Example voltage threshold level in range (0 : 1023)
const motor MOTORS[5];
const float RELEASE_STEP = 20.0; // constant for how much the totalRotation will decrement each clock cycle during release

struct motor {
  Servo servo;
  bool overdrawn = false;
  float totalRotation = 0.0;
  MotorState state = RELEASE;
  float sensorReadings = 0.0;
};

enum MotorState {
  TURN,
  RELEASE,
  HOLD
};

.
.
.

void setup() {

    .
    .
    .

    // Iterate through each motor
    for (int i = 0; i < 5; i++) {
        MOTORS[i].servo.attach(MOTOR_PINS[i]); // Attach motors to their output pins
    }

}

.
.
.

void loop() {

    .
    .
    .

    /* Read EMG signal and filter it */

    /* Read current sensor pins */

    ControlMotors(filteredSignal);

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
        currMotor = MOTORS[i];

        if (currMotor.sensorReadings < currentThreshold) {
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
      currMotor.rotation += rotation;    
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

// int _getStallIndex(float sensorReadings[], float currentThreshold) {
//   /*
//     - Function:
//       - Iterate through sensors and motors to find which one is stalled
//     - Arguments:
//       - sensorReadings (floats): array of current-sensor values
//       - overdrawn (bools): array of booleans that signify if a motor is overdrawn
//       - currentThreshold (float): the minimum current that the sensorReadings must be for the motor to not be stalled
//     - Returns:
//       - An int value between 0 and the number of motors - 1 which symbolizes the index of the stalled motor
//   */
  
//   int stallIndex = -1;    // Initialize stallIndex out of range
//   float maxCurrent = -9999;

//   for (int i = 0; i < 5; i++) {
//     if (sensorReadings[i] > maxCurrent && sensorReadings[i] < currentThreshold && !MOTORS[i].overdrawn) {  // Find highest current who is under threshold and isn't already stalled
//       maxCurrent = sensorReadings[i]; // Update max current
//       stallIndex = i; // Find index of stalled motor
//     }
//   }

//   return stallIndex;
// }
```
