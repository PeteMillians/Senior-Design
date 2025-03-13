## Digital Filter Design
The following outlines the design of the Digital Filter inside the Arduino R3

### Public Methods
- **`main()`**
  - Runs the main loop, continually reading inputs, applying the digital filter, and outputting the control signal.

- **`ReadInput(int pinNumber) -> int`**
  - Public method for reading a specific input pin. Calls the private method to ensure safe operation.
  - Handles errors gracefully by calling the private method.

- **`Filter(int data) -> int`**
  - Public method for filtering the digital data. Calls the private filtering method.
  - Ensures that the data is properly filtered, even if errors occur.

- **`ReturnOutput(int data, int pinNumber)`**
  - Public method for returning the output digital control signal to the specified pin.
  - Ensures safe handling of the output by calling the private method.

### Private Methods
- **`_TryReadInput(int pinNumber) -> [Bool, int]`**
  - Attempts to read the analog value from a specified input pin.
  - Returns a value from 0 to 1023 (or similar, based on ADC range), and a boolean representing the success or failure of the read operation.
  - Will use **A0** for the EMG input from the **MyoWare EMG sensor** and **A1-A5** for current sensor readings.
  - **Error Handling**: If read fails, return `false` and implement retries or fallbacks (e.g., default to previous values).

- **`_TryFilter(int data) -> [Bool, int]`**
  - Attempts to filter the given data using a specific voltage threshold.
  - Returns a boolean indicating if filtering was successful, and the filtered value.
  - This method will call the `_TryConvolve()` method for convolution-based filtering.
  - **Error Handling**: If filtering fails (e.g., invalid data), return `false` and prevent system failure.

- **`_TryConvolve(int data) -> [Bool, int]`**
  - Applies a convolution filter to the data to process the signal.
  - Returns `true` if successful, along with the processed signal value.
  - **Potential Filter Types**: Low-pass, moving average, or custom filters based on application needs.
  - **Error Handling**: If the convolution fails, return `false` and log error or retry.

- **`_TryReturnOutput(int data, int pinNumber) -> Bool`**
  - Tries to output the filtered data to the specified pin, translating it into a usable control signal.
  - Returns `true` if output is successful, otherwise `false`.
  - **Error Handling**: If output fails, attempt to reset or retry. Ensure control signal integrity.

- **`_DigitalToControl(int data) -> int`**
  - Converts the filtered digital signal into a control signal with one of three actions:
    1. **Turn**
    2. **Hold**
    3. **Release**
  - Determines voltage bins based on predefined calibration settings. 
  - **Calibration**: The control signal is calibrated during testing, and voltage bins are determined to match specific motor actions (e.g., motor speed or position).
  - Calibration is a critical step to ensure correct behavior and should be tested with real hardware.

---

## Error Handling
Each of the `_Try` methods returns a **boolean status** to indicate success or failure. When a failure occurs, the system should attempt to recover (e.g., retry the read, apply a fallback value) and log the failure through **Serial Monitor** for debugging.

- For example, if `_TryReadInput()` fails, the system can retry a few times, or use a **default value** for safety, while notifying the user.
- Similarly, `_TryFilter()` and `_TryReturnOutput()` should ensure that motor control signals are either corrected or halted in case of failure.
