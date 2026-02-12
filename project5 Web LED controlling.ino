#include <WiFi.h>


const char* ssid = "Your own SSID";
const char* password = "Wife password";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // HTTP headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Logic for GPIO control
            if (header.indexOf("GET /26/on") >= 0) {
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              output27State = "off";
              digitalWrite(output27, LOW);
            }
            
            // HTML Web Page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            // Import Font Awesome for Icons
            client.println("<link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\">");
            
            // CSS Styling
            client.println("<style>");
            client.println("html { font-family: 'Segoe UI', Arial; display: block; margin: 0px auto; text-align: center; background-color: #121212; color: white; }");
            client.println("h1 { margin: 40px 0; font-weight: 300; color: #00d4ff; }");
            client.println(".container { display: flex; flex-direction: column; align-items: center; }");
            client.println(".card { background: #1e1e1e; padding: 25px; border-radius: 15px; box-shadow: 0 4px 15px rgba(0,0,0,0.4); margin: 15px; width: 85%; max-width: 350px; }");
            client.println(".state-label { font-size: 0.9rem; color: #888; text-transform: uppercase; margin-bottom: 10px; }");
            client.println(".button { display: inline-block; padding: 15px 40px; font-size: 18px; font-weight: bold; text-decoration: none; border-radius: 50px; transition: 0.3s; }");
            client.println(".on { background-color: #00d4ff; color: #121212; }");
            client.println(".off { background-color: #333; color: #ff4b2b; border: 2px solid #ff4b2b; }");
            client.println(".button:active { transform: scale(0.95); }");
            client.println("i { margin-right: 10px; }");
            client.println("</style></head>");
            
            // Web Page Content
            client.println("<body><div class=\"container\">");
            client.println("<h1>ESP32 Dashboard</h1>");
            
            // GPIO 26 Card
            client.println("<div class=\"card\">");
            client.println("<div class=\"state-label\">Device 1 (Pin 26)</div>");
            if (output26State == "off") {
              client.println("<a href=\"/26/on\" class=\"button on\"><i class=\"fas fa-power-off\"></i>TURN ON</a>");
            } else {
              client.println("<a href=\"/26/off\" class=\"button off\"><i class=\"fas fa-lightbulb\"></i>TURN OFF</a>");
            }
            client.println("</div>");
            
            // GPIO 27 Card
            client.println("<div class=\"card\">");
            client.println("<div class=\"state-label\">Device 2 (Pin 27)</div>");
            if (output27State == "off") {
              client.println("<a href=\"/27/on\" class=\"button on\"><i class=\"fas fa-power-off\"></i>TURN ON</a>");
            } else {
              client.println("<a href=\"/27/off\" class=\"button off\"><i class=\"fas fa-fan\"></i>TURN OFF</a>");
            }
            client.println("</div>");
            
            client.println("</div></body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
  }
}
