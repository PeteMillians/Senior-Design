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
myServo.write(currentAngle + newAngle)
```

#### Hold State

During the ***hold*** state, the motor should stop rotating, but not unwind. The motor will simply hold its position

```c++
myServo.write(currentAngle)
```

#### Release State

During the ***release*** state, the motor should stop rotating and unwind. The motor should simulate releasing its grip

```c++
myServo.write(0)
```

## Public Methods
- ***void ControlMotors(float filteredSignal, int sensorReadings[])***
    - Function:
        - Full control algorithm for motors
    - Arguments:
        - filteredSignal (float): Filtered EMG data
        - sensorReadings (ints): Current sensor readings

## Testing

To test the Control Motor algorithm, we will copy the algorithm show below, fill in the setup function proprerly, and use a random number generator to simulate an input signal for the EMG signal. We will use the ```Serial.print()``` method to print the random signal so we can ensure the motor is acting as intended. We will add print statements in the ```ControlMotor()``` method to print the state and the angle being written to ensure the function acts properly. We will also use print statements to log the current sensor readings to ensure the current levels fall below threshold (which also allows us to calibrate the threshold level). Throughout testing, we will sporadically use our hands to manually stop the motor, forcing overcurrent. We will ensure the proper current senses overdraw and turns to the hold state. The ```loop``` method will look something like this:

```c++

int filteredSignal = rand() % 1024;   // Random number between 0 and 1023

int currentReadings[5];

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
const int CURRENT_THRESHOLD = 500;   # Example current threshold level in range (0 : 1023)
const int SIGNAL_THRESHOLD = 500;   # Example voltage threshold level in range (0 : 1023)
const Servo MOTORS[5];

.
.
.

void setup() {

    .
    .
    .

    // Iterate through each motor
    for (int i = 0; i < 5; i++) {
        MOTORS[i].attach(MOTOR_PINS[i]); // Attach motors to their output pins
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

    ControlMotors(filteredSignal, sensorReadings);

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

        // Iterate through each current sensor pin
        for (int i = 0; i < 5; i++) {

            if (sensorReadings[i] > CURRENT_THRESHOLD) {    // If overdrawing current
                // Set to hold state
                MOTORS[i].write(90);
            }

            else {
                // Set to turn state
                float rotation = map(filteredSignal, SIGNAL_THRESHOLD, 0.5, 90, 180);    // Map signal to a rotation speed
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
```
