#include <Servo.h>  // Include Servo library

using namespace std;

/* Pin Connections */
const int EMG_PIN = A0;
const int CURRENT_PINS[5] = {A1, A2, A3, A4, A5};
const int MOTOR_PINS[5] = {9, 10, 11, 12, 13};

/* Motors */
const Servo MOTORS[5];

/* Thresholds */
const int CURRENT_THRESHOLD = 2;   // Example current threshold level in Amps
const int SIGNAL_THRESHOLD = 0.03;   // Example voltage threshold level in Volts

/* Conversions */
const float sensorVoltageOffset = 2.5;  // For ACS712, it has a 2.5V offset for 0A current
const float sensorSensitivity = 0.066;  // ACS712 30A model (0.066V per Ampere)

// Pair declaration
struct Pair{
    bool success;
    int data;
};

void setup() {
    Serial.begin(9600);  // Start serial communication

    // Iterate through each motor
    for (int i = 0; i < 5; i++) {
        MOTORS[i].attach(MOTOR_PINS[i]); // Attach motors to their output pins
    }
}

void loop() {

    // Read raw data from MyoWare EMG Sensor
    int rawSignal = ReadInput(EMG_PIN);
    rawSignal = (rawSignal / 1023.0) * 5.0; 	// Convert to Volts

    // Filter raw signal
    float filteredSignal = Filter(rawSignal);

    int sensorReadings[5];

    for (int i = 0; i < 5; i++) {
        // Read current sensor pins 
        sensorReadings[i] = ReadInput(CURRENT_PINS[i]);
	sensorReadings[i] = (((sensorReadings[i] / 1023.0) * 5.0) - sensorVoltageOffset) / sensorSensitivity;	// Convert to Amperes
    }

    // Control motors using filtered signal and current sensor readings
    ControlMotors(filteredSignal, sensorReadings);

    delay(100);     // Delay 100 ms

}

int ReadInput(int pinNumber) {
    /* 
    - Function:
        - Calls the private ***_TryReadInput*** method 
        - Only returns a value if ***_TryReadInput*** was successful
    - Arguments:
        - pinNumber (int): the specific input pin that is read from
    - Returns:
        - int: digital value from pin
    */

    Pair input = _TryReadInput(pinNumber);
    if (!input.success) {
        Serial.println("Error reading input");
        return 0.0;
    }

    return int(input.data);
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
   
    Pair input;

    int value = analogRead(pinNumber);

    input.success = true;  
    input.data = float(value);

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
		Serial.println("Filter Failure");
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

	Pair tryFilter;
	
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

            if (abs(sensorReadings[i]) > CURRENT_THRESHOLD) {    // If overdrawing current
                // Set to hold state
                MOTORS[i].write(90);
            }

            else {
                // Set to turn state
                float rotation = map(filteredSignal, SIGNAL_THRESHOLD, 0.5, 90, 180);    // Map signal to a rotation speed
		    				    		     // ^ This is an arbitrary value for now (max contraction voltage)
                MOTORS[i].write(rotation);    
            }
        }
    }
    else {
        // Iterate through each current sensor pin
        for (int i = 0; i < 5; i++) {

            // Set to release state
            MOTORS[i].write(0);
        }
    }

}
