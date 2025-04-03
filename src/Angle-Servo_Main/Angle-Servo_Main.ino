#include <Servo.h>  // Include Servo library

using namespace std;

/* Pin Connections */
const int EMG_PIN = A0;
const int CURRENT_PINS[5] = {A1, A2, A3, A4, A5};
const int MOTOR_PINS[5] = {3, 5, 6, 9, 11};

/* Motors */
const Servo MOTORS[5];

/* Thresholds */
const int CURRENT_THRESHOLD = 2;   // Example current threshold level in Amps
const float SIGNAL_THRESHOLD = 0.05;   // Example voltage threshold level in Volts

/* Conversions */
const float sensorVoltageOffset = 2.5;  // For ACS712, it has a 2.5V offset for 0A current
const float sensorSensitivity = 0.066;  // ACS712 30A model (0.066V per Ampere)

// Pair declaration
struct Pair{
    bool success;
    float data;
};

void setup() {
    Serial.begin(9600);  // Start serial communication

    // Iterate through each motor
    for (int i = 0; i < 5; i++) 
        MOTORS[i].attach(MOTOR_PINS[i]); // Attach motors to their output pins
    
}

void loop() {
    float rawSignal = ReadInput(EMG_PIN);   // Read raw data from MyoWare EMG Sensor
    Serial.print("Raw Signal = ");
    Serial.print(rawSignal);
    Serial.println(" V");

    float filteredSignal = Filter(rawSignal);   // Filter raw signal
    Serial.print("Filtered Signal = ");
    Serial.print(filteredSignal);
    Serial.println(" V");

    float sensorReadings[5];

    for (int i = 0; i < 5; i++)  {
        sensorReadings[i] = (ReadInput(CURRENT_PINS[i]) - sensorVoltageOffset) / sensorSensitivity; // Read current sensor pins in Amperes
        Serial.print("Sensor Reading for");
        Serial.print(i + 1);
        Serial.print(" = ");
        Serial.print(sensorReadings[i]);
        Serial.println(" A");
    }

    ControlMotors(filteredSignal, sensorReadings);  // Control motors using filtered signal and current sensor readings

    delay(100); // Delay 100 ms
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
        Serial.println("Error reading input");
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
   
    Pair input;

    float value = (analogRead(pinNumber) / 1023.0) * 5.0; // Input value in Volts

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

void ControlMotors(float filteredSignal, float sensorReadings[]) {
    /*
    - Function:
        - Full control algorithm for motors
    - Arguments:
        - filteredSignal (float): Filtered EMG data
        - sensorReadings (ints): Current sensor readings
    */

    // Check that the EMG signal is powering the motors
    if (filteredSignal > SIGNAL_THRESHOLD) {
        int currentAngle[5];

        // Iterate through each current sensor pin
        for (int i = 0; i < 5; i++) {
            currentAngle[i] = MOTORS[i].read();

            if (abs(sensorReadings[i]) > CURRENT_THRESHOLD) {    // If overdrawing current
                // Set to hold state
                Serial.print("Motor ");
                Serial.print(i + 1);
                Serial.println(" in hold state.");
                MOTORS[i].write(currentAngle[i]);
            }

            else {
                // Set to turn state
                Serial.print("Motor ");
                Serial.print(i + 1);
                Serial.println(" in turn state.");
                // float rotation = map(filteredSignal, SIGNAL_THRESHOLD, 0.5, 90, 180);    // Map signal to a rotation speed
		    				    		                            // ^ This is an arbitrary value for now (max contraction voltage)
                MOTORS[i].write(currentAngle[i] + 1);    
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
