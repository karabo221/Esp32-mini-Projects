/*
  =============================================
  Simon Says Memory Game - ESP32 + OLED
  =============================================
  Button 1 (GPIO 18) <-> LED 1 (GPIO 25)  RED
  Button 2 (GPIO 19) <-> LED 2 (GPIO 26)  YELLOW   ← moved from 22 (now SCL)
  Button 3 (GPIO 23) <-> LED 3 (GPIO 27)  GREEN

  OLED SSD1306 128x64 I2C:
    VCC → 3.3V
    GND → GND
    SDA → GPIO 21
    SCL → GPIO 22

  Libraries needed (Arduino Library Manager):
    - Adafruit SSD1306
    - Adafruit GFX Library
  =============================================
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- OLED ----
#define SCREEN_W   128
#define SCREEN_H    64
#define OLED_RESET  -1   // no reset pin
#define OLED_ADDR  0x3C  // most SSD1306 modules use 0x3C; try 0x3D if blank
Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, OLED_RESET);

// ---- Pins ----
const int LED_PINS[] = {25, 26, 27};
const int BTN_PINS[] = {18, 19, 23};  // Button 2 moved to GPIO 19
const int NUM        = 3;

// ---- Sequence ----
const int MAX_LEVELS = 100;
int  sequence[MAX_LEVELS];
int  currentLevel = 1;
int  playerStep   = 0;

// ---- Scores ----
int highScore = 0;   // persists for the session

// ---- Control flags ----
bool showingSeq      = false;
bool waitingForInput = false;

// ---- Debounce ----
bool          lastBtnState[NUM]   = {HIGH, HIGH, HIGH};
bool          stableBtnState[NUM] = {HIGH, HIGH, HIGH};
unsigned long lastChangeTime[NUM] = {0, 0, 0};
const unsigned long DEBOUNCE_MS   = 50;

// -----------------------------------------------
// OLED helpers
// -----------------------------------------------

// Main in-game screen: level + high score
void drawGameScreen() {
  display.clearDisplay();

  // Title bar
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(28, 0);
  display.print("SIMON SAYS");

  // Divider
  display.drawFastHLine(0, 10, SCREEN_W, SSD1306_WHITE);

  // Current level — big
  display.setTextSize(3);
  display.setCursor(10, 20);
  display.print("LVL ");
  display.print(currentLevel);

  // High score — small, bottom
  display.setTextSize(1);
  display.setCursor(4, 54);
  display.print("BEST: ");
  display.print(highScore);

  display.display();
}

// "Watch!" screen while showing the LED sequence
void drawWatchScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 10);
  display.print("WATCH!");
  display.setTextSize(1);
  display.setCursor(28, 36);
  display.print("Level ");
  display.print(currentLevel);
  display.setCursor(4, 54);
  display.print("BEST: ");
  display.print(highScore);
  display.display();
}

// "Your turn!" screen waiting for input
void drawYourTurnScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, 10);
  display.print("GO! ");
  display.print(playerStep + 1);
  display.print("/");
  display.print(currentLevel);
  display.setTextSize(1);
  display.setCursor(4, 54);
  display.print("BEST: ");
  display.print(highScore);
  display.display();
}

// "Wrong!" game-over screen with final score
void drawFailScreen(int score) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 4);
  display.print("WRONG!");
  display.setTextSize(1);
  display.setCursor(16, 28);
  display.print("Score: ");
  display.print(score);
  if (score >= highScore) {
    display.setCursor(10, 42);
    display.print("** NEW BEST! **");
  } else {
    display.setCursor(4, 42);
    display.print("Best: ");
    display.print(highScore);
  }
  display.setCursor(14, 56);
  display.print("Restarting...");
  display.display();
}

// "Level up!" screen
void drawLevelUpScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, 8);
  display.print("LEVEL UP!");
  display.setTextSize(1);
  display.setCursor(22, 34);
  display.print("Now level ");
  display.print(currentLevel);
  display.setCursor(4, 54);
  display.print("BEST: ");
  display.print(highScore);
  display.display();
}

// -----------------------------------------------
// LED / game helpers
// -----------------------------------------------

void allOff() {
  for (int i = 0; i < NUM; i++) digitalWrite(LED_PINS[i], LOW);
}

void allOn() {
  for (int i = 0; i < NUM; i++) digitalWrite(LED_PINS[i], HIGH);
}

void failFlash(int times = 6) {
  for (int t = 0; t < times; t++) {
    allOn();  delay(80);
    allOff(); delay(80);
  }
}

void levelUpFlash(int rounds = 2) {
  for (int r = 0; r < rounds; r++) {
    for (int i = 0; i < NUM; i++) {
      allOff();
      digitalWrite(LED_PINS[i], HIGH); delay(180);
      digitalWrite(LED_PINS[i], LOW);  delay(60);
    }
  }
  allOff();
  delay(300);
}

void showSequence() {
  showingSeq = true;
  drawWatchScreen();
  allOff();
  delay(600);

  int onTime  = max(200, 500 - (currentLevel / 5) * 50);
  int offTime = max(100, 250 - (currentLevel / 5) * 25);

  for (int step = 0; step < currentLevel; step++) {
    int idx = sequence[step];
    digitalWrite(LED_PINS[idx], HIGH); delay(onTime);
    digitalWrite(LED_PINS[idx], LOW);  delay(offTime);
  }

  allOff();
  delay(400);
  showingSeq      = false;
  waitingForInput = true;
  playerStep      = 0;

  drawYourTurnScreen();
  Serial.printf("=== Level %d -- repeat the sequence! ===\n", currentLevel);
}

void nextLevel() {
  if (currentLevel <= MAX_LEVELS) {
    sequence[currentLevel - 1] = random(0, NUM);
  }
  showSequence();
}

void resetGame(int failedAtLevel) {
  // Update high score
  int score = failedAtLevel - 1;   // levels successfully completed
  if (score > highScore) highScore = score;

  Serial.printf("--- WRONG! Score: %d | Best: %d ---\n", score, highScore);

  drawFailScreen(score);
  failFlash(6);
  delay(1500);   // let player read the fail screen

  currentLevel    = 1;
  playerStep      = 0;
  waitingForInput = false;

  sequence[0] = random(0, NUM);
  showSequence();
}

int readButtonPress() {
  for (int i = 0; i < NUM; i++) {
    bool reading = digitalRead(BTN_PINS[i]);

    if (reading != lastBtnState[i]) {
      lastChangeTime[i] = millis();
      lastBtnState[i]   = reading;
    }

    if ((millis() - lastChangeTime[i]) > DEBOUNCE_MS) {
      if (reading == LOW && stableBtnState[i] == HIGH) {
        stableBtnState[i] = LOW;
        return i;
      }
      if (reading == HIGH) {
        stableBtnState[i] = HIGH;
      }
    }
  }
  return -1;
}

// -----------------------------------------------
// Setup / Loop
// -----------------------------------------------

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));

  for (int i = 0; i < NUM; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    pinMode(BTN_PINS[i], INPUT_PULLUP);
  }

  // Init OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 not found — check wiring and OLED_ADDR");
    while (true);   // halt; fix wiring before continuing
  }
  display.clearDisplay();
  display.display();

  // Startup sweep
  for (int i = 0; i < NUM; i++) { digitalWrite(LED_PINS[i], HIGH); delay(150); }
  delay(300);
  for (int i = 0; i < NUM; i++) { digitalWrite(LED_PINS[i], LOW);  delay(150); }
  delay(400);

  Serial.println("===== SIMON SAYS - ESP32 =====");

  sequence[0] = random(0, NUM);
  showSequence();
}

void loop() {
  if (showingSeq || !waitingForInput) return;

  int btn = readButtonPress();
  if (btn == -1) return;

  int expected = sequence[playerStep];

  Serial.printf("Step %d/%d -- pressed %d, expected %d %s\n",
                playerStep + 1, currentLevel,
                btn + 1, expected + 1,
                btn == expected ? "OK" : "WRONG");

  // Brief LED feedback
  digitalWrite(LED_PINS[btn], HIGH);
  delay(200);
  digitalWrite(LED_PINS[btn], LOW);

  if (btn != expected) {
    resetGame(currentLevel);
    return;
  }

  playerStep++;
  drawYourTurnScreen();   // update step counter on display

  if (playerStep == currentLevel) {
    waitingForInput = false;
    Serial.printf(">>> Level %d cleared! <<<\n", currentLevel);
    delay(300);
    currentLevel++;
    drawLevelUpScreen();
    levelUpFlash(2);
    delay(500);
    nextLevel();
  }
}
