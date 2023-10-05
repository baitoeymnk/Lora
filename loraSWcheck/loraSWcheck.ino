#define LED_PIN 16
#define BUTTON_PIN 4

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    Serial.println(digitalRead(BUTTON_PIN));
    digitalWrite(LED_PIN, LOW);
    Serial.println("off");

    
  }
  else if(digitalRead(BUTTON_PIN) == LOW)  {
    digitalWrite(LED_PIN,HIGH);
    Serial.println("on");
    
  }
}
