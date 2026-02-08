const int buttonPin = 4;  // the number of the pushbutton pin
const int ledPin = 5;    // the number of the LED pin

// variable for storing the pushbutton status 
int buttonState = 0;

void setup() {
  Serial.begin(115200);  
  // initialize the pushbutton pin as an input with internal pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);
  // initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // read the state of the pushbutton value
  buttonState = digitalRead(buttonPin);
  Serial.println(buttonState);
  
  // check if the pushbutton is pressed.
  // With INPUT_PULLUP: LOW = pressed or ==0, HIGH = not pressed ==1
  if (buttonState == LOW) {
    // turn LED on
    digitalWrite(ledPin, HIGH);
    Serial.println("Button pressed - LED ON");
  } else {
    // turn LED off
    digitalWrite(ledPin, LOW);
  }
  
  delay(50);  // Small delay timer between loops 
}