
int i = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(100);                      
  digitalWrite(LED_BUILTIN, LOW);    
  delay(100);                       
  Serial.println(i);
  i++;
}