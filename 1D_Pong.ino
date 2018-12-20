#include <Adafruit_NeoPixel.h>
#include <math.h>

#define LED_DIN A2
#define NUMPIXELS 30
#define BALL_DELAY_REDUCTION 0.75
#define MIN_BALL_DELAY 5

#define PLAYER_1 4
#define PLAYER_2 5

#define TO_PLAYER_1 255
#define TO_PLAYER_2 1

#define LIGHT_MODE LOW
#define PONG_MODE HIGH

volatile boolean mode = LIGHT_MODE;
volatile byte ballLocation = 0;
volatile byte ballDirection = 1;
volatile float ballDelay = 250;

volatile byte lightColour = 1;
volatile byte lightBrightness = 1;

volatile boolean currentButtonState[2] = { HIGH, HIGH };
volatile boolean previousButtonState[2] = { HIGH, HIGH };

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LED_DIN,  NEO_GRB + NEO_KHZ800);

void setup() {
  mode = LOW;
  initializeGameState();
  
  pinMode(PLAYER_1, INPUT);
  digitalWrite(PLAYER_1, HIGH);

  pinMode(PLAYER_2, INPUT);
  digitalWrite(PLAYER_2, HIGH);

  Serial.begin(9600);
  strip.begin();
}

void loop() {
  if (mode == HIGH) {
    runGame();
  }
  setMode();
}

void initializeGameState() {
  ballLocation = NUMPIXELS / 2;
  ballDirection = 1;
  ballDelay = 250;
}

void setMode() {
  currentButtonState[0] = digitalRead(PLAYER_1);
  currentButtonState[1] = digitalRead(PLAYER_2);
  
  if (currentButtonState[0] == LOW && currentButtonState[1] == LOW) {
    initializeGameState();
  }
  
  if ((currentButtonState[0] == LOW || currentButtonState[1] == LOW) && 
      (currentButtonState[0] != previousButtonState[0] ||
       currentButtonState[1] != previousButtonState[1])) {
    if (currentButtonState[0] == LOW && currentButtonState[1] == LOW) {
      mode = !mode;
    }
    if (mode == LIGHT_MODE) {
      if (digitalRead(PLAYER_1) == LOW) {
        lightBrightness = max(lightBrightness << 1, 1);
        lightStrip();
      }
    
      if (digitalRead(PLAYER_2) == LOW) {
        lightColour = max(lightColour << 1, 1);
        lightStrip();
      }
    }
  }

  previousButtonState[0] = currentButtonState[0];
  previousButtonState[1] = currentButtonState[1];
}

void runGame() {
  if (ballLocation < NUMPIXELS && ballLocation >= 0) {
    clearStrip();
    updateGameState();
    renderGameState();
    delay(ballDelay);
  }
}

void clearStrip() {
  createPongBoard();
  strip.show();
}

void renderGameState() {
  renderBall(255, 255, 255);
  strip.show();
}

void renderBall(uint8_t r, uint8_t g, uint8_t b) {
  setPixelColor(ballLocation, r, g, b);
}

void updateGameState() {
  setBallDelay();
  setBallDirection();
  setBallLocation();
}

void setBallDirection() {
  if (ballDirection == TO_PLAYER_2 && ballLocation > NUMPIXELS - 4 && digitalRead(PLAYER_2) == LOW) {
    ballDirection = TO_PLAYER_1;
  }

  if (ballDirection == TO_PLAYER_1 && ballLocation < 3 && digitalRead(PLAYER_1) == LOW) {
    ballDirection = TO_PLAYER_2;
  }
}

void setBallLocation() {
  ballLocation += ballDirection;
}

void setBallDelay() {
  if ((ballDirection == TO_PLAYER_2 && ballLocation > NUMPIXELS - 4 && digitalRead(PLAYER_2) == LOW) ||
      (ballDirection == TO_PLAYER_1 && ballLocation < 3 && digitalRead(PLAYER_1) == LOW)) {
    ballDelay = max(ballDelay * BALL_DELAY_REDUCTION, MIN_BALL_DELAY);
  }
}

void createPongBoard() {
  for(int i=0;i<NUMPIXELS;i++){  
    if ((i >= 1 && i <= 2) || (i <= NUMPIXELS - 2 && i >= NUMPIXELS - 3)) {
      setPixelColor(i, 1, 1, 0);
    } else if (i == 0 || i == NUMPIXELS - 1) {
      setPixelColor(i, 0, 1, 0); 
    } else {
      setPixelColor(i, 1, 0, 0);
    }
  }
}

void lightStrip() {
  for(int i=0;i<NUMPIXELS;i++){  
    switch (lightColour) {
      case 1: strip.setPixelColor(i, 0, 0, lightBrightness); break;
      case 2: strip.setPixelColor(i, 0, lightBrightness, 0); break;
      case 4: strip.setPixelColor(i, 0, lightBrightness, lightBrightness); break;
      case 8: strip.setPixelColor(i, lightBrightness, 0, 0); break;
      case 16: strip.setPixelColor(i, lightBrightness, 0, lightBrightness); break;
      case 32: strip.setPixelColor(i, lightBrightness, lightBrightness, 0); break;
      case 64: strip.setPixelColor(i, lightBrightness, lightBrightness, lightBrightness); break;
      default: lightColour = 1; break; 
    }
  }
  strip.show();
}

void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
   strip.setPixelColor(n, r, g, b);
}
