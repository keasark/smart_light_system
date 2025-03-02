#include <Adafruit_LiquidCrystal.h>

// Define Pins and Constant Variables
const int pirPin = 2; // PIR sensor pin
const int trigPin = 9; // Ultrasonic trigger pin
const int echoPin = 10; // Ultrasonic echo pin
const int potPin = A1; // Potentiometer analog pin for adjusting distance threshold
const int lightSensorPin = A0; // Light sensor analog pin
const int ledPin = 3; // LED pin for light control (PWM capable)
const int Distance_Threshold = 178; // Initial distance threshold based on half the average side length of a room in Houston, TX.

// Global variables
int distanceThreshold = Distance_Threshold;

// Initialize the LCD object
Adafruit_LiquidCrystal lcd(0x20); // Change to your LCD's I2C address

// Set up LCD and Pin Mode
void setup() {
    // Initialize LCD
    lcd.begin(16, 2);
    lcd.setBacklight(1);
    lcd.print("Lighting System");
  
  // Initialize pins
    pinMode(pirPin, INPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(ledPin, OUTPUT);

    delay(2000);
    lcd.clear();
}

// Logic Design
void loop() {
    // Update the distance threshold adjusted by user
    distanceThreshold = getDistanceThreshold();
    
    // Detect the presence of an object by using PIR and Ultrasonic sensors
    // Using PIR sensor 
    bool pirDetected = digitalRead(pirPin) == HIGH;
    // Using Ultrasonic sensor
    bool ultrasonicDetected = getDistance() < distanceThreshold;
    
    // Get Ambient Light Level
    int lightLevel = analogRead(lightSensorPin);
    
    // Control the Lighting System
    controlLighting(pirDetected, ultrasonicDetected, lightLevel);
    
    // Debug the distance threshold info 
    debugInfoLCD(distanceThreshold);
    
    delay(1000); // Update every second
}


// ULTRASONIC SENSOR 
// Get updated distance threshold from the Potentiometer 
int getDistanceThreshold() {
  // Read potentiometer to adjust distance threshold 
    int potValue = analogRead(potPin);
  // map the pot value to a range of 30 to 200
    return map(potValue, 0, 1023, 30, 200); // Adjustable range for distance threshold
}

// Calculate the distance of an object based on the ultrasonic waves
long getDistance() {
    // Emit pulse from ultrasonic sensor
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure echo time to calculate the object's distance
    long duration = pulseIn(echoPin, HIGH);
    // one-way distance = duration * ultrasonic speed / 2
    long distance = duration * 0.034 / 2;
    return distance;
}

// Control the lighting system
void controlLighting(bool pirDetected, bool ultrasonicDetected, int lightLevel) {
    // Detect an object by PIR and Ultrasonic Sensors
    if (pirDetected && ultrasonicDetected) {
        // In case of Sufficient Ambient Light
        if (lightLevel >= 300) { 
            // Dimming as light level increases
            int brightness = map(lightLevel, 300, 1023, 128, 0); 
            analogWrite(ledPin, brightness);
            // Display presence and brightness level on LCD
            displayLightStatus("Light On: Dimmed", lightLevel);
      } else {
            // In case of Low Ambient Light
            analogWrite(ledPin, 255);
            displayLightStatus("Light On: Full", lightLevel);
      }
  } else {
      // Turn off light if no detection
        analogWrite(ledPin, 0);
        displayLightStatus("Light Off", lightLevel);
  }
}

void displayLightStatus(const char* status, int lightLevel)
{
    lcd. clear();
    lcd.setCursor(0, 0);
    lcd.print(status);
    lcd.setCursor(0, 1);
    lcd.print("Light Leve: ");
    lcd.print(lightLevel);
}

void debugInfoLCD(int distanceThreshold){
    // Debug info on LCD for threshold
    lcd.setCursor(0, 1);
    lcd.print("Dist Thresh: ");
    lcd.print(distanceThreshold);
}
