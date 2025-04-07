#include <Servo.h>  // Include Servo library

using namespace std;

/* Struct Definitions */
struct Pair {
  bool success;
  float data;

  Pair(bool s, float d) : success(s), data(d) {}
};

struct motor {
  Servo servo;
  int overdrawn = 0;
  float totalRotation = 0.0;
  MotorState state = RELEASE;
  float sensorReading = 0.0;
};

/* Enum Definition */
enum MotorState {
  TURN,
  RELEASE,
  HOLD
};

/* Pin Connections */
const int EMG_PIN = A0;
const int CURRENT_PINS[5] = {A1, A2, A3, A4, A5};
const int MOTOR_PINS[5] = {3, 5, 6, 9, 11};

/* Motors */
const motor MOTORS[5];

/* Thresholds */
const float CURRENT_THRESHOLD = 538.5;   // Current threshold level 
const float SIGNAL_THRESHOLD = 6.15;   // Voltage threshold level 

/* Global variables */  
const float RELEASE_STEP = 20.0; // constant for how much the totalRotation will decrement each clock cycle during release
const int NUM_MOTORS = 5;

/* Testing Variable */
bool DEBUG = false;

/* Sampling Frequency */
const int CLOCK_PERIOD = 500;   // 500 us (2kHz sampling frequency)


void setup() {
    Serial.begin(9600);  // Start serial communication

    // Iterate through each motor
    for (int i = 0; i < 5; i++) 
        MOTORS[i].attach(MOTOR_PINS[i]); // Attach motors to their output pins
    
}

void loop() {
  float rawSignal = ReadInput(EMG_PIN);   // Read raw data from MyoWare EMG Sensor
  if (DEBUG) {
      Serial.print("Raw Signal = ");
      Serial.print(rawSignal);
      Serial.println(" V");
  }

  float filteredSignal = Filter(rawSignal);   // Filter raw signal
  if (DEBUG) {
      Serial.print("Filtered Signal = ");
      Serial.print(filteredSignal);
      Serial.println(" V");
  }

  for (int i = 0; i < 5; i++)  {
      MOTORS[i].sensorReadings = ReadInput(CURRENT_PINS[i]); // Read current sensor pins 
      if (DEBUG) {
          Serial.print("Sensor Reading for");
          Serial.print(i + 1);
          Serial.print(" = ");
          Serial.print(MOTORS[i].sensorReadings);
          Serial.println(" A");
      }
  }

  ControlMotors(filteredSignal);  // Control motors using filtered signal and current sensor readings

  delayMicroseconds(CLOCK_PERIOD); // 2kHz sampling frequency (500 us)
}

float ReadInput(int pinNumber) {
  /* 
  - Function:
      - Calls the private ***_TryReadInput*** method 
      - Only returns a value if ***_TryReadInput*** was successful
  - Arguments:
      - pinNumber (int): the specific input pin that is read from
  - Returns:
      - float: digital value from pin
  */

  Pair input = _TryReadInput(pinNumber);
  if (!input.success) {
      Serial.println("ERROR: Failure reading input at pin " + String(pinNumber));
      return 0.0;
  }

  return input.data;
}

Pair _TryReadInput(int pinNumber) {
  /* 
  - Function:
      - Reads the analog input value at a specified pin
  - Arguments:
      - pinNumber(int): the specific pin that is read from
  - Returns:
      - bool: whether or not the read was sucessful
      - float: the value read from the pin as a float
  */
  
  Pair input(false, 0.0);

  // float value = (analogRead(pinNumber) / 1023.0) * 5.0; // Input value in Volts
  float value = analogRead(pinNumber);

  input.success = true;  
  input.data = value;

  return input;
}

float Filter(float data) {
	/*
	- Function:
		- Public method for filtering the digital data. Calls the private filtering method.
		- Ensures that the data is properly filtered, even if errors occur.
	- Arguments:
		- data (int): the digital data passed which needs to be filtered
	- Returns:
		- float value of filtered data
	*/

	Pair filterPair = _TryFilter(data);

	if (!filterPair.success) {
		Serial.println("ERROR: Failure filtering data");
		return 0.0;
	}
	
	return filterPair.data;

}

Pair _TryFilter(float data) {
	/*
	- Function:
		- Attempts to filter the given data using a specific voltage threshold.
	- Arguments:
		- data (int): The raw EMG data 
	- Returns:
		- Returns a boolean indicating if filtering was successful, and the filtered value.
	*/

	Pair tryFilter(false, 0.0);
	
	if(data < SIGNAL_THRESHOLD) {
		tryFilter.data = 0.0;
		tryFilter.success = true;
	}
	else {
		tryFilter.data = data;
		tryFilter.success = true;
	}

	return tryFilter;

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
    for (int i = 0; i < NUM_MOTORS; i++) {
      if (MOTORS[i].sensorReading < CURRENT_THRESHOLD) { // Check if that motor's current reading is less than the current threshold
        MOTORS[i].state = HOLD; // Set motorState to HOLD
      }
      else {
        MOTORS[i].state = TURN; // Set motorState to TURN
      }
      _UpdateState(MOTORS[i], filteredSignal);  // Update the state
    }
  }
  else {
    for (int i = 0; i < NUM_MOTORS; i++) {
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
    - Arguments:
      - currMotor (motor): the current motor we are iterating through
      - filteredSignal (float): the EMG signal from the MyoWare sensor
  */

  // TODO: Need logic here to keep it stopped when stalled but not stop others

  currMotor.overdrawn++;  // Increment overdrawn current
  currMotor.servo.write(90);  // Write no movement to the motor
}