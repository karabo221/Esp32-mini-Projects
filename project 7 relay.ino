// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

// Set to true to define Relay as Normally Open (NO)
#define RELAY_NO true

// Set number of relays
#define NUM_RELAYS 2

// Assign each GPIO to a relay
int relayGPIOs[NUM_RELAYS] = {26, 27};

// Replace with your network credentials
const char* ssid = "123";
const char* password = "123";

const char* PARAM_INPUT_1 = "relay";
const char* PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


String buildPage() {
  String html = "";
  html += "<!DOCTYPE HTML><html lang='en'><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Relay Control Panel</title>";
  html += "<link href='https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Exo+2:wght@300;600;800&display=swap' rel='stylesheet'>";
  html += "<style>";
  html += "*,*::before,*::after{box-sizing:border-box;margin:0;padding:0}";
  html += "body{background:#0d0f14;color:#c8d0e0;font-family:'Exo 2',sans-serif;";
  html += "min-height:100vh;display:flex;flex-direction:column;align-items:center;padding:40px 20px}";
  html += "header{text-align:center;margin-bottom:48px}";
  html += ".logo-row{display:flex;align-items:center;justify-content:center;gap:12px;margin-bottom:8px}";
  html += ".logo-box{width:36px;height:36px;border:2px solid #00e5ff;border-radius:8px;";
  html += "display:flex;align-items:center;justify-content:center;";
  html += "box-shadow:0 0 18px rgba(0,229,255,0.35);background:rgba(0,229,255,0.08)}";
  html += "h1{font-family:'Exo 2',sans-serif;font-weight:800;font-size:clamp(1.4rem,4vw,2rem);";
  html += "letter-spacing:0.06em;text-transform:uppercase;color:#fff}";
  html += ".subtitle{font-family:'Share Tech Mono',monospace;font-size:0.7rem;color:#4a5470;letter-spacing:0.2em;text-transform:uppercase}";
  html += ".status{display:inline-flex;align-items:center;gap:8px;margin-top:14px;";
  html += "font-family:'Share Tech Mono',monospace;font-size:0.7rem;color:#00ff9d;letter-spacing:0.12em;";
  html += "background:rgba(0,255,157,0.07);border:1px solid rgba(0,255,157,0.22);";
  html += "border-radius:20px;padding:5px 14px}";
  html += ".dot{width:7px;height:7px;border-radius:50%;background:#00ff9d;";
  html += "box-shadow:0 0 8px #00ff9d;animation:pulse 2s ease-in-out infinite}";
  html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(270px,1fr));";
  html += "gap:20px;width:100%;max-width:680px}";
  html += ".card{background:#151820;border:1px solid #252a36;border-radius:16px;padding:26px;transition:border-color .3s,box-shadow .3s}";
  html += ".card.on{border-color:rgba(0,255,157,0.3);box-shadow:0 0 28px rgba(0,255,157,0.07)}";
  html += ".card-top{display:flex;justify-content:space-between;align-items:flex-start;margin-bottom:22px}";
  html += ".ch{font-family:'Share Tech Mono',monospace;font-size:0.65rem;color:#4a5470;letter-spacing:0.15em;margin-bottom:4px}";
  html += ".rname{font-size:1.05rem;font-weight:600;color:#fff}";
  html += ".badge{font-family:'Share Tech Mono',monospace;font-size:0.65rem;color:#00e5ff;";
  html += "background:rgba(0,229,255,0.1);border:1px solid rgba(0,229,255,0.22);";
  html += "border-radius:6px;padding:3px 9px;letter-spacing:0.08em;white-space:nowrap}";
  html += ".toggle-row{display:flex;align-items:center;justify-content:space-between}";
  html += ".slabel{font-family:'Share Tech Mono',monospace;font-size:0.78rem;font-weight:600;";
  html += "letter-spacing:0.1em;text-transform:uppercase;transition:color .3s}";
  html += ".slabel.on{color:#00ff9d}.slabel.off{color:#ff4560}";
  html += ".sw{position:relative;display:inline-block;width:64px;height:32px}";
  html += ".sw input{opacity:0;width:0;height:0;position:absolute}";
  html += ".track{display:block;width:64px;height:32px;background:rgba(255,69,96,0.15);";
  html += "border:1px solid rgba(255,69,96,0.4);border-radius:32px;cursor:pointer;";
  html += "transition:background .3s,border-color .3s,box-shadow .3s;position:relative}";
  html += ".track::after{content:'';position:absolute;top:4px;left:4px;width:22px;height:22px;";
  html += "border-radius:50%;background:#ff4560;box-shadow:0 0 10px rgba(255,69,96,0.5);";
  html += "transition:transform .3s,background .3s,box-shadow .3s}";
  html += ".sw input:checked + .track{background:rgba(0,255,157,0.12);";
  html += "border-color:rgba(0,255,157,0.5);box-shadow:0 0 16px rgba(0,255,157,0.18)}";
  html += ".sw input:checked + .track::after{transform:translateX(32px);";
  html += "background:#00ff9d;box-shadow:0 0 14px rgba(0,255,157,0.65)}";
  html += ".divider{height:1px;background:#252a36;margin:20px 0}";
  html += ".cfoot{font-family:'Share Tech Mono',monospace;font-size:0.62rem;color:#4a5470;letter-spacing:0.1em}";
  html += "footer{margin-top:48px;font-family:'Share Tech Mono',monospace;font-size:0.62rem;color:#4a5470;";
  html += "letter-spacing:0.12em;text-align:center}";
  html += "@keyframes pulse{0%,100%{opacity:1}50%{opacity:0.35}}";
  html += "</style></head><body>";

  // Header
  html += "<header>";
  html += "<div class='logo-row'>";
  html += "<div class='logo-box'>";
  html += "<svg width='20' height='20' viewBox='0 0 20 20' fill='none'>";
  html += "<path d='M10 2v4M10 14v4M2 10h4M14 10h4' stroke='#00e5ff' stroke-width='2' stroke-linecap='round'/>";
  html += "<circle cx='10' cy='10' r='3' fill='#00e5ff'/>";
  html += "</svg></div>";
  html += "<h1>Relay Control</h1></div>";
  html += "<div class='subtitle'>ESP32 // GPIO Controller</div>";
  html += "<div class='status'><div class='dot'></div>SYSTEM ONLINE</div>";
  html += "</header>";

  // Cards
  html += "<div class='grid'>";
  for (int i = 1; i <= NUM_RELAYS; i++) {
    bool isOn;
    if (RELAY_NO) {
      isOn = !digitalRead(relayGPIOs[i - 1]);  // NO relay: HIGH=off, LOW=on
    } else {
      isOn = digitalRead(relayGPIOs[i - 1]);
    }

    String cardClass  = isOn ? "card on" : "card";
    String stateClass = isOn ? "slabel on" : "slabel off";
    String stateText  = isOn ? "ENERGIZED" : "OFF";
    String chk        = isOn ? " checked" : "";
    String gpioStr    = String(relayGPIOs[i - 1]);
    String channel    = "CH-0" + String(i);

    html += "<div class='" + cardClass + "' id='card" + String(i) + "'>";
    html += "<div class='card-top'>";
    html += "<div><div class='ch'>" + channel + "</div>";
    html += "<div class='rname'>Relay " + String(i) + "</div></div>";
    html += "<span class='badge'>GPIO " + gpioStr + "</span>";
    html += "</div>";
    html += "<div class='toggle-row'>";
    html += "<span class='" + stateClass + "' id='lbl" + String(i) + "'>" + stateText + "</span>";
    html += "<label class='sw'>";
    html += "<input type='checkbox'" + chk + " onchange='toggle(" + String(i) + ",this)'>";
    html += "<span class='track'></span>";
    html += "</label>";
    html += "</div>";
    html += "<div class='divider'></div>";
    html += "<div class='cfoot'>NORMALLY ";
    html += RELAY_NO ? "OPEN" : "CLOSED";
    html += " &bull; DIGITAL OUTPUT</div>";
    html += "</div>";
  }
  html += "</div>";

  // Footer
  html += "<footer>ESP32 &mdash; ";
  html += WiFi.localIP().toString();
  html += " &mdash; AsyncWebServer</footer>";

  // JavaScript
  html += "<script>";
  html += "function toggle(relay,el){";
  html += "var s=el.checked?1:0;";
  html += "var card=document.getElementById('card'+relay);";
  html += "var lbl=document.getElementById('lbl'+relay);";
  html += "if(s){card.className='card on';lbl.textContent='ENERGIZED';lbl.className='slabel on';}";
  html += "else{card.className='card';lbl.textContent='OFF';lbl.className='slabel off';}";
  html += "fetch('/update?relay='+relay+'&state='+s);";
  html += "}";
  html += "</script>";
  html += "</body></html>";
  return html;
}

void setup() {
  Serial.begin(115200);

  // Set all relay pins to OFF on boot
  for (int i = 1; i <= NUM_RELAYS; i++) {
    pinMode(relayGPIOs[i - 1], OUTPUT);
    if (RELAY_NO) {
      digitalWrite(relayGPIOs[i - 1], HIGH);  // NO: HIGH = relay off
    } else {
      digitalWrite(relayGPIOs[i - 1], LOW);   // NC: LOW = relay off
    }
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  // Route: serve main page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", buildPage());
  });

  // Route: handle relay toggle GET requests
  // e.g. /update?relay=1&state=1
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      int relayNum  = request->getParam(PARAM_INPUT_1)->value().toInt();
      int stateVal  = request->getParam(PARAM_INPUT_2)->value().toInt();
      int gpioIndex = relayNum - 1;

      if (gpioIndex >= 0 && gpioIndex < NUM_RELAYS) {
        if (RELAY_NO) {
          digitalWrite(relayGPIOs[gpioIndex], !stateVal);  // invert for NO
        } else {
          digitalWrite(relayGPIOs[gpioIndex], stateVal);
        }
        Serial.printf("Relay %d set to %s\n", relayNum, stateVal ? "ON" : "OFF");
      }
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("Server started.");
}

void loop() {}

