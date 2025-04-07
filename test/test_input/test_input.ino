const int emgPin = A0;  // Analog pin connected to MyoWare sensor
float voltage;          // Variable to store voltage value

void setup() {
    Serial.begin(9600);  // Start serial communication
}

void loop() {
    int sensorValue = analogRead(emgPin);  // Read EMG sensor value
    voltage = (sensorValue / 1023.0) * 5.0; // Convert to voltage (for 5V Arduino)
    
    // Serial.print("EMG Voltage: ");
    // Serial.print(voltage);
    // Serial.println(" V");
    Serial.print(0); Serial.print(" "); Serial.println(5);  // Set Y-range from 0V to 3.7V

    Serial.println(voltage);
    
    delay(100);  // Small delay to stabilize readings
}
