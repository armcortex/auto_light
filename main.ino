#include <Servo.h>
#include <Wire.h>
#include <SonarSRF02.h>
#include <Timer.h>

    // "sketch": "driver/Arduino-SRF/examples/SRF02/SRF02.ino"


// Define

// SRF02 Parameters
#define SONAR_ADDRESS   (0xE0 >> 1)
#define SONAR_RANGE_MIN (22)
#define SONAR_RANGE_MAX (60)
#define SONAR_FILTER    (0.7)

SonarSRF02 Sonar(SONAR_ADDRESS);
char sonar_unit = 'c';                    // 'i' for inches, 'c' for centimeters, 'm' for micro-seconds
int sonar_thres = 0;
int sonar_dis = 0;

// ADC
#define POT_PIN (A3)
#define ADC_FILTER (0.7)

unsigned int pot = 0;

// Servo
#define SERVO_PIN     (5)
#define SERVO_ON      (180)
#define SERVO_OFF     (90)

Servo servo;
unsigned int servo_angle = 0;
bool servo_state = false;

// Timer
Timer t;


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  servo.attach(SERVO_PIN);
  Sonar.begin();

  t.every(10, print);
  t.every(10, readADC);
  t.every(10, readSonar);
  t.every(3000, controlServo);
}

void loop() {
  t.update();



  // pot = (ADC_FILTER * pot) + ((1-ADC_FILTER) * analogRead(POT_PIN));
  // sonar_dis = (SONAR_FILTER * sonar_dis) + ((1-SONAR_FILTER) * Sonar.readRange(sonar_unit));

  // sonar_thres = map(pot, 0, 1024, SONAR_RANGE_MIN, SONAR_RANGE_MAX);

  // if (sonar_dis > sonar_thres)
  // {
  //   servo.write(SERVO_ON);
  //   servo_state = true;
  // }
  // else
  // {
  //   servo.write(SERVO_OFF);
  //   servo_state = false;
  // }
  
  // Serial.print("Threshold: ");
  // Serial.print(sonar_thres);
  // Serial.print(" Sonar: ");
  // Serial.print(Sonar.readRange(sonar_unit));
  // Serial.print(" cm ");
  // Serial.print("Servo: ");
  // Serial.print(servo_state);
  // Serial.println();

  // delay(10);


  // // servo_angle = map(pot, 0, 1024, 0, 180);


  // // servo.write(servo_angle);

  // // // digitalWrite(LED_BUILTIN, LOW); 
  // // Serial.print("pot: ");
  // // Serial.print(pot);
  // // Serial.print(" angle: ");
  // // Serial.println(servo_angle);
  // // delay(10);                       
}

unsigned int filter(unsigned int oldValue, unsigned int newValue, float n)
{
  return (unsigned int)(n * (float)oldValue) + ((1.0 - n) * (float)newValue);
}

void readADC()
{
  pot = filter(pot, analogRead(POT_PIN), ADC_FILTER);

  // pot = (ADC_FILTER * pot) + ((1-ADC_FILTER) * analogRead(POT_PIN));
  sonar_thres = map(pot, 0, 1024, SONAR_RANGE_MIN, SONAR_RANGE_MAX);
}

void readSonar()
{
  // sonar_dis = (SONAR_FILTER * sonar_dis) + ((1-SONAR_FILTER) * Sonar.readRange(sonar_unit));
  // sonar_dis = Sonar.readRange(sonar_unit);

  sonar_dis = filter(sonar_dis, Sonar.readRange(sonar_unit), SONAR_FILTER);
}

void controlServo()
{
  if (sonar_dis < sonar_thres)
  {
    servo.write(SERVO_ON);
    servo_state = true;
  }
  else
  {
    servo.write(SERVO_OFF);
    servo_state = false;
  }
}

void print()
{
  Serial.print("Threshold: ");
  Serial.print(sonar_thres);
  Serial.print(" Sonar: ");
  Serial.print(sonar_dis);
  Serial.print(" cm ");
  Serial.print("Servo: ");
  Serial.print(servo_state);
  Serial.println();
}
