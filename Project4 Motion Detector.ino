
const int buzzerPin =  26;      // the number of the buzzer pin
const int motionSensor = 27;    // the middle pin in the motion detector to pin 27


unsigned long previousMillis = 0;        // will store last time buzzer was updated
unsigned long currentMillis;
const long interval = 10000;           // interval at which to blink (milliseconds)

void setup() {
   Serial.begin(115200);            // initialize serial
  // set the digital pin as output:
  pinMode(buzzerPin, OUTPUT);
  pinMode(motionSensor, INPUT);
}

void loop() 
{
  int reading = digitalRead(motionSensor);
  if (reading == HIGH)
  {digitalWrite(buzzerPin,HIGH);
   Serial.println("Motion detected!Buzzer alarm!");
   previousMillis = millis();
  }
  else{
 // currentMillis = millis();
   if (millis() - previousMillis >= interval) 
   {
    if(digitalRead(buzzerPin) == HIGH)
    {
      digitalWrite(buzzerPin, LOW);
    Serial.println("Area Clear! Buzzer off!");
    }
    
    }
  }
}
