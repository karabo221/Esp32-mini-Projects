#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

// --- Configuration ---
const char* ssid = "123";
const char* password = "123";

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

AsyncWebServer server(80);

// Helper function to read DHT values
String getTemperature() {
  float t = dht.readTemperature();
  return isnan(t) ? "--" : String(t, 1);
}

String getHumidity() {
  float h = dht.readHumidity();
  return isnan(h) ? "--" : String(h, 0);
}

// Function to build the High-Tech UI
String buildPage() {
  String html = "<!DOCTYPE HTML><html lang='en'><head>";
  html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Weather Station Terminal</title>";
  html += "<link href='https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Exo+2:wght@300;600;800&display=swap' rel='stylesheet'>";
  html += "<style>";
  html += "*,*::before,*::after{box-sizing:border-box;margin:0;padding:0}";
  html += "body{background:#0d0f14;color:#c8d0e0;font-family:'Exo 2',sans-serif;min-height:100vh;display:flex;flex-direction:column;align-items:center;padding:40px 20px}";
  html += "header{text-align:center;margin-bottom:48px}";
  html += ".logo-box{width:40px;height:40px;border:2px solid #ff6b35;border-radius:8px;display:flex;align-items:center;justify-content:center;margin:0 auto 12px;box-shadow:0 0 18px rgba(255,107,53,0.3);background:rgba(255,107,53,0.08)}";
  html += "h1{font-weight:800;font-size:1.8rem;letter-spacing:0.06em;text-transform:uppercase;color:#fff}";
  html += ".subtitle{font-family:'Share Tech Mono',monospace;font-size:0.7rem;color:#4a5470;letter-spacing:0.2em;text-transform:uppercase}";
  html += ".status{display:inline-flex;align-items:center;gap:8px;margin-top:14px;font-family:'Share Tech Mono',monospace;font-size:0.7rem;color:#38bdf8;background:rgba(56,189,248,0.07);border:1px solid rgba(56,189,248,0.22);border-radius:20px;padding:5px 14px}";
  html += ".dot{width:7px;height:7px;border-radius:50%;background:#38bdf8;box-shadow:0 0 8px #38bdf8;animation:pulse 2s infinite}";
  html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(250px,1fr));gap:20px;width:100%;max-width:600px}";
  html += ".card{background:#151820;border:1px solid #252a36;border-radius:16px;padding:26px;position:relative;overflow:hidden}";
  html += ".card::before{content:'';position:absolute;top:0;left:0;width:100%;height:3px}";
  html += ".temp-card::before{background:#ff6b35;box-shadow:0 2px 10px rgba(255,107,53,0.5)}";
  html += ".hum-card::before{background:#38bdf8;box-shadow:0 2px 10px rgba(56,189,248,0.5)}";
  html += ".ch{font-family:'Share Tech Mono',monospace;font-size:0.65rem;color:#4a5470;letter-spacing:0.15em;margin-bottom:8px}";
  html += ".val-row{display:flex;align-items:baseline;gap:8px}";
  html += ".val{font-size:3.5rem;font-weight:800;color:#fff;line-height:1}";
  html += ".unit{font-family:'Share Tech Mono',monospace;font-size:1.2rem;color:#4a5470}";
  html += ".divider{height:1px;background:#252a36;margin:20px 0}";
  html += ".cfoot{font-family:'Share Tech Mono',monospace;font-size:0.62rem;color:#4a5470;letter-spacing:0.1em}";
  html += "@keyframes pulse{0%,100%{opacity:1}50%{opacity:0.3}}";
  html += "@keyframes flash{0%{opacity:0.5}100%{opacity:1}}";
  html += ".updating{animation:flash 0.5s ease}";
  html += "</style></head><body>";

  html += "<header><div class='logo-box'>🌡</div><h1>Weather Station</h1>";
  html += "<div class='subtitle'>ATMOSPHERIC DATA // SENSOR-01</div>";
  html += "<div class='status'><div class='dot'></div>DATA FEED ACTIVE</div></header>";

  html += "<div class='grid'>";
  // Temperature Card
  html += "<div class='card temp-card'><div class='ch'>INTERNAL_TEMP</div>";
  html += "<div class='val-row'><span class='val' id='temp'>" + getTemperature() + "</span><span class='unit'>°C</span></div>";
  html += "<div class='divider'></div><div class='cfoot'>THERMO_LINK STAT: NOMINAL</div></div>";
  
  // Humidity Card
  html += "<div class='card hum-card'><div class='ch'>HUMIDITY_LEVEL</div>";
  html += "<div class='val-row'><span class='val' id='hum'>" + getHumidity() + "</span><span class='unit'>%</span></div>";
  html += "<div class='divider'></div><div class='cfoot'>HYGRO_LINK STAT: NOMINAL</div></div>";
  html += "</div>";

  html += "<footer><div class='subtitle' style='margin-top:40px'>ESP32 // " + WiFi.localIP().toString() + "</div></footer>";

  html += "<script>function update(){";
  html += "fetch('/temp').then(r=>r.text()).then(t=>{document.getElementById('temp').innerText=t;});";
  html += "fetch('/hum').then(r=>r.text()).then(h=>{document.getElementById('hum').innerText=h;});";
  html += "}setInterval(update, 5000);</script>";
  
  html += "</body></html>";
  return html;
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(1000); Serial.print("."); }
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", buildPage());
  });

  server.on("/temp", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", getTemperature());
  });

  server.on("/hum", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", getHumidity());
  });

  server.begin();
}

void loop() {}