1) 3 wires 
2) 1 Potentiometer 
3) Esp32 
const int potPin = 4; 
 int potValue =0; // GND Pin 

void setup() { 
Serial.begin(115200); 
delay(1000);} 
void loop() { 
// Reading potentiometer value 
potValue = analogRead(potPin); 
Serial.println(potValue); 
delay(500);} 