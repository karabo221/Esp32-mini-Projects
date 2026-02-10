const int ledPin = 4; // 4 corresponds to GPIO4 
// setting PWM properties 
const int freq = 5000; 
const int ledChannel = 0; 
const int resolution = 8; 

void setup(){ 
  // configure LED PWM functionalities 
  ledcAttach(ledPin, freq, resolution);  // New API: ledcAttach
}

void loop(){
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++)
  {
    // change the LED brightness 
    ledcWrite(ledPin, dutyCycle);  // Use ledPin instead of ledChannel
    delay(15);
  }
  
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--)
  {
    ledcWrite(ledPin, dutyCycle);  // Use ledPin instead of ledChannel
    delay(15);
  }
}