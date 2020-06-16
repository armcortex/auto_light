#include <Servo.h>
#include <Wire.h>
#include <SonarSRF02.h>

// Define
#define SONAR_ADDRESS (0xE0 >> 1)
#define POT_PIN       (A3)
#define FILTER        (0.7)

// SRF02 Parameters
#define SONAR_RANGE_MIN 22
SonarSRF02 Sonar(SONAR_ADDRESS);
char unit = 'c';                    // 'i' for inches, 'c' for centimeters, 'm' for micro-seconds
unsigned int dis_thres = 0;

// ADC
unsigned int adc_value = 0;

// Servo
#define SERVO_PIN     (5)
Servo servo;
unsigned int servo_angle = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  servo.attach(SERVO_PIN);
}

void loop() {
  adc_value = (FILTER * adc_value) + ((1-FILTER) * analogRead(POT_PIN));
  
  servo_angle = map(adc_value, 0, 1024, 0, 180);
  servo.write(servo_angle);

  // digitalWrite(LED_BUILTIN, LOW); 
  Serial.print("adc: ");
  Serial.print(adc_value);
  Serial.print(" angle: ");
  Serial.println(servo_angle);
  delay(10);                       
}