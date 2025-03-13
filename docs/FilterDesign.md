## Digital Filter Design
The following outlines the design of the Digital Filter inside the Arduino R3

### Public Methods
- `main()`
- Runs main loop, continually reading inputs, applying the digital filter, and outputting the control signal 

- `ReadInput(int pinNumber) -> int`
- Public method for reading a specific input pin, calls the private method
    - Calls the private method, preventing the algorithm from breaking if an error occurs

- `Filter(int data) -> int`
- Public method for filtering the digital data, calls the private method
    - Calls the private method, preventing the algorithm from breaking if an error occurs

- `ReturnOutput(int data, int pinNumber)`
- Public method for returning the output digital control signal
    - Calls the private method, preventing the algorithm from breaking if an error occurs

### Private Methods
- `_TryReadInput(int pinNumber) -> [Bool, int]`
- Tries to read the analog value incoming into a specified pin 
- Returns a value from 0 to 1023 and a boolean representing whether the read was successful
    - Internal ADC
- We will use pin A0 for EMG input from the MyoWare EMG sensor
- We will use pins A1 - A5 for current sensor readings

- `_TryFilter(int data) -> [Bool, int]`
- Tries to filter the given data using a specific voltage threshold
- Returns a boolean specifying if the filter worked without error, and an int representing the filtered digital amplitude
- Will call the _TryConvolve() method

- `_TryConvolve(int data) -> [Bool, int]`
- Called by the _TryFilter() method 
- Convolves the signal with a set function, filtering the data
- Returns a boolean representing whether the method worked, and an int of the convolved data

- `_TryReturnOutput(int data, int pinNumber) -> Bool`
- Tries to return the filtered data out of the specified output pin
- Translates digital signal into a control signal
- Returns a boolean representing whether the data was output properly

- `_DigitalToControl(int data) -> int`
- Translates the digital data signal into a control signal as 1 of 3 types:
    1) Turn
    2) Hold
    3) Release
- Determines Voltage bins which determine the control type
    - Determined during testing
    - Calibration 