#include <Servo.h>  // Include Servo library

using namespace std;

/* Pin Connections */
const int EMG_PIN = A0;
const int CURRENT_PINS[5] = {A1, A2, A3, A4, A5};
const int MOTOR_PINS[5] = {3, 5, 6, 9, 11};

/* Motors */
const Servo MOTORS[5];

/* Thresholds */
const int CURRENT_THRESHOLD = 538.5;   // Current threshold level 
const float SIGNAL_THRESHOLD = 6.15;   // Voltage threshold level 

/* Conversions */
// const float sensorVoltageOffset = 2.5;  // For ACS712, it has a 2.5V offset for 0A current
// const float sensorSensitivity = 0.066;  // ACS712 30A model (0.066V per Ampere)

/* Global variables */  
bool isOverdrawn[5] = {false, false, false, false, false};  // array of bools representing if that motor has overdrawn current
float totalRotation[5] = {0.0, 0.0, 0.0, 0.0, 0.0}; // array of total angle rotated by each motor
const float RELEASE_STEP = 20.0; // constant for how much the totalRotation will decrement each clock cycle during release

/* Testing Variable */
bool DEBUG = true;

/* Sampling Frequency */
const int CLOCK_PERIOD = 500;   // 500 us (2kHz sampling frequency)

/* Pair Definition */
struct Pair {
    bool success;
    float data;

    Pair(bool s, float d) : success(s), data(d) {}
};

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

    float sensorReadings[5];

    for (int i = 0; i < 5; i++)  {
        // sensorReadings[i] = (ReadInput(CURRENT_PINS[i]) - sensorVoltageOffset) / sensorSensitivity; // Read current sensor pins in Amperes
        sensorReadings[i] = ReadInput(CURRENT_PINS[i]); // Read current sensor pins 
        if (DEBUG) {
            Serial.print("Sensor Reading for");
            Serial.print(i + 1);
            Serial.print(" = ");
            Serial.print(sensorReadings[i]);
            Serial.println(" A");
        }
    }

    ControlMotors(filteredSignal, sensorReadings);  // Control motors using filtered signal and current sensor readings

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

                if (DEBUG) {
                    Serial.print("Motor ");
                    Serial.print(i + 1);
                    Serial.println(" in hold state.");
                }

                if (isOverdrawn[i]) {   // If it is consecutively overdrawn
                    // Set to hold state
                    MOTORS[i].write(90);
                    continue;  // This should break out of line 133 loop, but remain in for-loop
                }

                isOverdrawn[i] = true;  // Record that this motor has overdrawn current

                // Continue rotating
                float rotation = constrain(map(filteredSignal, SIGNAL_THRESHOLD, 205, 90, 180), 90, 180);    // Map signal to a rotation speed
                MOTORS[i].write(rotation);
                totalRotation[i] += rotation;    
            }

            else {
                // Set to turn state
                if (DEBUG) {
                    Serial.print("Motor ");
                    Serial.print(i + 1);
                    Serial.println(" in turn state.");
                }

                isOverdrawn[i] = false;
                float rotation = constrain(map(filteredSignal, SIGNAL_THRESHOLD, 205, 90, 180), 90, 180);    // Map signal to a rotation speed
                MOTORS[i].write(rotation);
                totalRotation[i] += rotation;    
            }
        }
    }
    else {
        // Release state
        for (int i = 0; i < 5; i++) {
            isOverdrawn[i] = false;

            // Set to release state
            if (DEBUG) {
                Serial.print("Motor ");
                Serial.print(i + 1);
                Serial.println(" in release state.");
            }

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