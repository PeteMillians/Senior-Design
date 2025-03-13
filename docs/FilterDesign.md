## Digital Filter Design
The following outlines the design of the Digital Filter inside the Arduino R3

### Public Methods
- `main()`
- Runs main loop, continually reading inputs, applying the digital filter, and outputting the control signal 

- `ReadInput(int pinNumber) -> int`
- Public method for reading a specific input pin, calls the private method
    - This prevents the algorithm from causing an error if the Read does not work

- `Filter(int data) -> int`
- Public method for filtering the digital data, calls the private method

### Private Methods
- `_TryReadInnput(int pinNumber) -> [Bool, int]`
- Tries to read the analog value incoming into a specified pin 
- Returns a value from 0 to 1023 and a boolean representing whether the read was successful
    - Internal ADC
- We will use pin A0 for EMG input from the MyoWare EMG sensor
- We will use pins A1 - A5 for current sensor readings

- `_TryFilter(int data) -> [Bool, int]`
- Tries to filter the given data using a specific voltage threshold
- Returns a boolean specifying if the filter worked without error, and an int representing the filtered digital amplitude
- Will call the convolve