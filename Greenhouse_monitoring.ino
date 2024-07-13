#include <Wire.h>
#include <DHT.h>
#include <Servo.h>

// Pin definitions
#define SOIL_MOISTURE_PIN1 A0
#define SOIL_MOISTURE_PIN2 A1
#define PHOTORESISTOR_PIN A2
#define GAS_SENSOR_PIN A3
#define WATER_PUMP_RELAY_PIN 8
#define LED_PIN 10
#define SERVO_PIN 13
#define DHT_PIN 11
#define DHT_TYPE DHT11

// Threshold values
#define SOIL_MOISTURE_THRESHOLD 1000 // Adjust based on calibration
#define LIGHT_THRESHOLD 100         // Adjust based on calibration
#define GAS_THRESHOLD 20           // Adjust based on calibration

// Initialize components
DHT dht(DHT_PIN, DHT_TYPE);
Servo servo;

bool servoUp = true; // Flag to track servo position
unsigned long previousMillis = 0; // Store last time servo was moved
const long interval = 1000; // Interval for servo movement

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize sensors and actuators
  pinMode(SOIL_MOISTURE_PIN1, INPUT);
  pinMode(SOIL_MOISTURE_PIN2, INPUT);
  pinMode(PHOTORESISTOR_PIN, INPUT);
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(WATER_PUMP_RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Initialize servo
  servo.attach(SERVO_PIN);

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  // Read sensors
  int soilMoisture1 = analogRead(SOIL_MOISTURE_PIN1);
  int soilMoisture2 = analogRead(SOIL_MOISTURE_PIN2);
  int lightLevel = analogRead(PHOTORESISTOR_PIN);
  int gasLevel = analogRead(GAS_SENSOR_PIN);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Print data to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Soil Moisture 1: ");
  Serial.println(soilMoisture1);

  Serial.print("Soil Moisture 2: ");
  Serial.println(soilMoisture2);

  Serial.print("Light Level: ");
  Serial.println(lightLevel);

  Serial.print("Gas Level: ");
  Serial.println(gasLevel);

  Serial.println(); // Blank line for readability

  // Check soil moisture and water plant if needed
  if (soilMoisture1 < SOIL_MOISTURE_THRESHOLD || soilMoisture2 < SOIL_MOISTURE_THRESHOLD) {
    digitalWrite(WATER_PUMP_RELAY_PIN, HIGH); // Turn on water pump
    Serial.println("Water pump: ON");
  } else {
    digitalWrite(WATER_PUMP_RELAY_PIN, LOW);  // Turn off water pump
    Serial.println("Water pump: OFF");
  }

  // Check light level and turn on LED if needed
  if (lightLevel < LIGHT_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH); // Turn on LED
    Serial.println("LED: ON");
  } else {
    digitalWrite(LED_PIN, LOW);  // Turn off LED
    Serial.println("LED: OFF");
  }

  // Check gas level and move servo if needed
  unsigned long currentMillis = millis();
  if (gasLevel > GAS_THRESHOLD) {
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; // Save the last time servo was moved

      if (servoUp) {
        servo.write(180); // Move servo to 180 degrees
        Serial.println("Servo: 180 degrees");
        servoUp = false;
      } else {
        servo.write(0); // Move servo to 0 degrees
        Serial.println("Servo: 0 degrees");
        servoUp = true;
      }
    }
  } else {
    servo.write(0); // Reset servo to 0 degrees when gas level is normal
    Serial.println("Servo: 0 degrees");
  }

  delay(4000);
}
