#include <Servo.h>
#include <Wire.h>
#include <Timer.h>
// #include <avr/wdt.h>

#define HCSR04                                 // SRF02 or HCSR04

#if defined(SRF02)
  #include <SonarSRF02.h>
#elif defined(HCSR04)
  #include <HCSR04.h>
#else
  #error "Unsupported sonar device model"
#endif


// Sonar Parameters
#if defined(SRF02)
  #define SONAR_ADDRESS   (0xE0 >> 1)
  #define SONAR_RANGE_MIN (22)
  #define SONAR_RANGE_MAX (150)

  SonarSRF02 Sonar(SONAR_ADDRESS);
  char sonar_unit = 'c';                    // 'i' for inches, 'c' for centimeters, 'm' for micro-seconds
#elif defined(HCSR04)
  #define ECHO_PIN        (11)
  #define TIG_PIN         (10)
  #define SONAR_RANGE_MIN (0)
  #define SONAR_RANGE_MAX (150)
  
  UltraSonicDistanceSensor Sonar(TIG_PIN, ECHO_PIN); 
#else
  #error "Unsupported sonar device model"
#endif

#define SONAR_FILTER    (0.7)

int sonar_thres = 0;
int sonar_dis = 0;

// Switch
#define SW_PIN        (3)
#define BLINK_TIME    (100)

// ADC
#define POT_PIN       (A3)
#define ADC_FILTER    (0.7)

unsigned int pot = 0;

// Servo
#define SERVO_PIN       (5)
#define SERVO_IDLE      (90)
#define SERVO_DELTA     (30)
#define SERVO_ON        (SERVO_IDLE - 25)
#define SERVO_OFF       (SERVO_IDLE + 40)
#define SERVO_POWER_PIN (4)

Servo servo;
unsigned int servo_angle = 0;
bool servo_state = false;

// Timer
#define OFF_TIME        (600000)          // 10 mins

Timer t;
char print_id = 0;
char readADC_id = 0;
char readSonar_id = 0;
char servoOn_id = 0;
char servoOff_id = 0;
char blink_id = 0;

// Other
unsigned int cnt = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SERVO_POWER_PIN, OUTPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  servo.attach(SERVO_PIN);
  servo.write(SERVO_IDLE);
  delay(5000);

  #if defined(SRF02)
    Sonar.begin();
  #endif


  print_id = t.every(50, print);
  readADC_id =  t.every(10, readADC);
  readSonar_id =  t.every(50, readSonar);
  servoOn_id = t.every(300, servoOn);
  servoOff_id = t.every(OFF_TIME, servoOff);

  blink_id = t.every(BLINK_TIME, blink);
  t.stop(blink_id);

  // wdt_enable(WDTO_8S);
}

void loop() {
  bool sw_state = digitalRead(SW_PIN);
  t.update();    

  if (sw_state) 
  {
    t.stop(blink_id);

    if (sonar_dis < sonar_thres)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      t.stop(servoOff_id);                            // re-calculate
      servoOff_id = t.every(OFF_TIME, servoOff);
    }
    else
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
    
    // if (cnt < 10000)
    //   wdt_reset();
  }
  else 
  {
      blink_id = t.every(BLINK_TIME, blink);

      // stop all timers
      t.stop(print_id);
      t.stop(readADC_id);
      t.stop(readSonar_id);
      t.stop(servoOn_id);
      t.stop(servoOff_id);

      // re-calculate
      print_id = t.every(50, print);
      readADC_id =  t.every(10, readADC);
      readSonar_id =  t.every(50, readSonar);
      servoOn_id = t.every(300, servoOn);
      servoOff_id = t.every(OFF_TIME, servoOff);
  
      // turn off
      servo.write(SERVO_OFF);
      servo_state = false;
  }
}

unsigned int filter(unsigned int oldValue, unsigned int newValue, float n)
{
  return (unsigned int)(n * (float)oldValue) + ((1.0 - n) * (float)newValue);
}

void readADC()
{
  pot = filter(pot, analogRead(POT_PIN), ADC_FILTER);

  #if defined(SRF02)
    sonar_thres = map(pot, 0, 1024, SONAR_RANGE_MIN, SONAR_RANGE_MAX);
  #elif defined(HCSR04)
    sonar_thres = map(pot, 0, 1024, SONAR_RANGE_MIN, SONAR_RANGE_MAX);
  #else
    #error "Unsupported sonar device model"
  #endif
}

void readSonar()
{
  #if defined(SRF02)
    sonar_dis = filter(sonar_dis, Sonar.readRange(sonar_unit), SONAR_FILTER);
  #elif defined(HCSR04)
    sonar_dis = filter(sonar_dis, Sonar.measureDistanceCm(), SONAR_FILTER);
  #else
    #error "Unsupported sonar device model"
  #endif
}

void servoOn()
{
  if (sonar_dis < sonar_thres)
  {
    if (servo_state == false)
    {
      digitalWrite(SERVO_POWER_PIN, HIGH);
      servo.write(SERVO_ON);
    }
    servo_state = true;
  }
}

void servoOff()
{
  if (sonar_dis >= sonar_thres)
  {
    if (servo_state == true)
    {
      digitalWrite(SERVO_POWER_PIN, LOW);
      servo.write(SERVO_OFF);
    }
    servo_state = false;
  }
}

void blink()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void print()
{
  Serial.print(cnt);
  Serial.print(". Threshold: ");
  Serial.print(sonar_thres);
  Serial.print(" Sonar: ");
  Serial.print(sonar_dis);
  Serial.print(" cm");
  Serial.print(" On: ");
  Serial.print(sonar_dis < sonar_thres);
  Serial.print(" Servo: ");
  Serial.print(servo_state);

  Serial.println();
  
  if (cnt >= 0xFFFFFFFF)
    cnt = 0;
  cnt++;
}
