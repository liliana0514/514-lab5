#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Include the required addons
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// WiFi credentials and Firebase project details
const char* ssid = "UW MPSK";
const char* password = "Dup{c3R/n<"; // Replace with your network password
#define DATABASE_URL "https://esp32-firebase-demo-515d2-default-rtdb.firebaseio.com/" // Replace with your database URL
#define API_KEY "AIzaSyCyQhuoAbSIB58E-DtVI1rpaqizvEPMyWc" // Replace with your API key

// Define ultrasonic sensor pins
const int trigPin = 2;
const int echoPin = 3;

// Define sound speed in cm/usec
const float soundSpeed = 0.034;

// Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Function prototypes
float measureDistance();
void connectToWiFi();
void initFirebase();
void sendDataToFirebase(float distance);

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Connect to Wi-Fi and initialize Firebase
  connectToWiFi();
  initFirebase();

  // Initial measurement and upload
  float distance = measureDistance();
  sendDataToFirebase(distance);

  // Enter deep sleep for the first time
  Serial.println("Entering deep sleep mode...");
  esp_sleep_enable_timer_wakeup(30 * 1000000); // Sleep for 30 seconds
  esp_deep_sleep_start();
}

void loop() {
  // This is not used as we're waking up from deep sleep continuously
}

float measureDistance() {
  // Trigger the measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo pin
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * soundSpeed / 2; // Calculate the distance

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void initFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Attempt to sign up and sign in to Firebase
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Signed up to Firebase");
    signupOK = true;
  } else {
    Serial.println("Failed to sign up to Firebase");
  }

  Firebase.begin(&config, &auth);
}

void sendDataToFirebase(float distance) {
  if (signupOK && Firebase.ready()) {
    if (Firebase.RTDB.pushFloat(&fbdo, "/distance", distance)) {
      Serial.println("Distance data sent to Firebase");
    } else {
      Serial.print("Failed
