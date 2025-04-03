import serial

# Change this to match your Arduino port
PORT = "COM3"  # For Windows, e.g., "COM3"; For Mac/Linux, "/dev/ttyUSB0"
BAUD_RATE = 9600  # Match your Arduino baud rate

# Open the serial connection
ser = serial.Serial(PORT, BAUD_RATE)

# Open a file to save the data
with open("serial_output.txt", "w") as file:
    print(f"Logging data from {PORT}... Press Ctrl+C to stop.")
    try:
        while True:
            data = ser.readline().decode('utf-8').strip()  # Read and decode
            print(data)  # Print to console
            file.write(data + "\n")  # Write to file
    except KeyboardInterrupt:
        print("\nLogging stopped.")
        ser.close()  # Close serial connection
