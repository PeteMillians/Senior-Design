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
***void ControlMotors(float filteredSignal, int sensorReadings[])***
- Function:
    - Full control algorithm for motors
- Arguments:
    - filteredSignal (float): Filtered EMG data
    - sensorReadings (ints): Current sensor readings

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

void ControlMotors(float filteredSignal, int sensorReadings[]) {
    '''
    - Function:
        - Full control algorithm for motors
    - Arguments:
        - filteredSignal (float): Filtered EMG data
        - sensorReadings (ints): Current sensor readings
    '''

    int currentAngles[5];

    // Check that the EMG signal is powering the motors
    if (filteredSignal > SIGNAL_THRESHOLD) {

        // Iterate through each current sensor pin
        for (int i = 0; i < 5; i++) {

            // Retrieve current angle of motors
            currentAngles[i] = MOTORS[i].read()

            if (sensorReadings[i] > CURRENT_THRESHOLD) {    // If overdrawing current
                
                // Set to hold state
                MOTORS[i].write(currentAngles[i]);
            }

            else {
                // Set to turn state
                MOTORS[i].write(currentAngles[i] + filteredSignal);
            }
        }
    }
    else {

        // Iterate through each current sensor pin
        for (int i = 0; i < 5; i++) {

            if (sensorReadings[i] > CURRENT_THRESHOLD) {    // If overdrawing current
                
                // Set to release state
                MOTORS[i].write(0);
            }
        }
    }

}
```
