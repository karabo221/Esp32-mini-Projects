#include <WiFi.h>

const char* ssid ="Mareka";
const char* password = "Sam@5303" ;

WiFiServer server(80); 

String redString = "0";
String greenString ="0";
String blueString ="0"; // Fixed capitalization
int pos1, pos2, pos3, pos4;

String header; 
String currentLine = ""; // Added this missing variable

const int redPin = 13; 
const int greenPin = 12; 
const int bluePin = 14; 

const int freq = 5000; 
const int resolution = 8; 

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000; 

void setup() {
  Serial.begin(115200); 

  //  Attach frequency and resolution directly to the pin
  ledcAttach(redPin, freq, resolution);
  ledcAttach(greenPin, freq, resolution);
  ledcAttach(bluePin, freq, resolution);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); 

  while(WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
  }

  Serial.println("\nWiFi connected."); 
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Fixed syntax

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    header = "";
    currentLine = ""; 

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        header += c;
        
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Send HTTP headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Web Page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name='viewport' content='width=device-width, initial-scale=1'>");
            client.println("<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css'>");
            client.println("<script src='https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js'></script>");
            client.println("</head><body><div class='container'><h1>ESP Color Picker</h1>");
            client.println("<a class='btn btn-primary' href='#' id='change_color'>Change Color</a>");
            client.println("<input class='jscolor {onFineChange:\"update(this)\"}' id='rgb'>");
            client.println("<script>function update(picker) {");
            client.println("document.getElementById('change_color').href='?r'+Math.round(picker.rgb[0])+'g'+Math.round(picker.rgb[1])+'b'+Math.round(picker.rgb[2])+'&';");
            client.println("}</script></div></body></html>");
            client.println();

            // Parse RGB values from URL
            if(header.indexOf("GET /?r") >= 0) {
              pos1 = header.indexOf('r');
              pos2 = header.indexOf('g');
              pos3 = header.indexOf('b');
              pos4 = header.indexOf('&');
              redString = header.substring(pos1+1, pos2);
              greenString = header.substring(pos2+1, pos3);
              blueString = header.substring(pos3+1, pos4);

              // Use the pin numbers for ledcWrite in v3.0
              ledcWrite(redPin, redString.toInt());
              ledcWrite(greenPin, greenString.toInt());
              ledcWrite(bluePin, blueString.toInt());
            }
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
  }
}
