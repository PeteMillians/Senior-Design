#include <Servo.h>  // Include Servo library

// Pin definitions
const int emgPin = A0;  // Pin for EMG input (MyoWare)
const int currentPins[] = {A1, A2, A3, A4, A5}; // Pins for current sensors
const int servoPins[] = {9, 10, 11, 12, 13};  // Pins for 5 servos

// Servo objects
Servo servos[5];

// Thresholds and limits
const int currentThreshold = 512;  // Example threshold for current sensor
const int filterThreshold = 500;   // Example threshold for EMG signal to apply filter

void setup() {
  Serial.begin(9600);  // Start serial communication

  // Attach the 5 servos to their respective pins
  for (int i = 0; i < 5; i++) {
    servos[i].attach(servoPins[i]);
  }
}

void loop() {
  // Step 1: Read input (EMG & current sensors)
  Pair emgResult = ReadInput(emgPin);  // Read EMG input
  int motorCurrentData[5];  // Array for current sensor data

  // Step 2: Read current sensors for each motor
  for (int i = 0; i < 5; i++) {
    motorCurrentData[i] = ReadInput(currentPins[i]).data;  // Read each current sensor
  }

  // Step 3: Filter the EMG data
  Pair filteredResult = Filter(emgResult.data);

  // Step 4: Output the filtered data to control the servos
  for (int i = 0; i < 5; i++) {
    // If the current sensor reading is below the threshold, move the servo
    if (motorCurrentData[i] <= currentThreshold) {
      servos[i].write(filteredResult.data);  // Control servo with filtered EMG signal
    } else {
      servos[i].write(90);  // Stop the servo if overcurrent is detected
      Serial.print("Overcurrent detected on Servo ");
      Serial.println(i + 1);
    }
  }

  delay(100);  // Delay to stabilize the readings
}

// --- Public Methods ---

// Reads the input value from a specific pin
Pair ReadInput(int pinNumber) {
  Pair result;

  // Try reading the input
  result = _TryReadInput(pinNumber);

  if (result.success) {
    return result;
  } else {
    Serial.println("Error reading input pin.");
    result.data = -1;  // Return an error value
    return result;
  }
}

// Filters the input data
Pair Filter(int data) {
  Pair result;

  // Example of a simple filter: Use a threshold to detect muscle contraction
  if (data > filterThreshold) {
    result.data = map(data, filterThreshold, 1023, 0, 180);  // Map to servo angle range (0 to 180)
    result.success = true;
  } else {
    result.data = 90;  // If data is too low, keep servo in neutral position
    result.success = false;
  }

  return result;
}

// --- Private Methods ---

// Tries to read input from the specified pin
Pair _TryReadInput(int pinNumber) {
  Pair result;
  
  result.data = analogRead(pinNumber);  // Read analog value (0 to 1023)
  
  if (result.data >= 0 && result.data <= 1023) {
    result.success = true;  // Successful read
  } else {
    result.success = false;  // Failed to read
  }

  return result;
}

// Tries to filter the input data
Pair _TryFilter(int data) {
  Pair result;

  // Example of a simple filter: Use a threshold to detect muscle contraction
  if (data > filterThreshold) {
    result.data = map(data, filterThreshold, 1023, 0, 180);  // Map to servo angle range (0 to 180)
    result.success = true;
  } else {
    result.data = 90;  // If data is too low, keep servo in neutral position
    result.success = false;
  }

  return result;
}
