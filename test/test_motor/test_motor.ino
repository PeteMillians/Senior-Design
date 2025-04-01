#include <Servo.h> // Include the Servo library

Servo myservo; // Create a servo object

void setup() {
  myservo.attach(9); // Attach the servo to digital pin 9
}

void loop() {
  myservo.write(0);  // Rotate the servo to 0 degrees
  delay(1000);       // Wait for 1 second
  myservo.write(180); // Rotate the servo to 180 degrees
  delay(1000);       // Wait for 1 second
}