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

volatile byte ballLocation = 0;
volatile byte ballDirection = 1;
volatile float ballDelay = 250;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LED_DIN,  NEO_GRB + NEO_KHZ800);

void setup() {
  initializeGameState();
  
  pinMode(PLAYER_1, INPUT);
  digitalWrite(PLAYER_1, HIGH);

  pinMode(PLAYER_2, INPUT);
  digitalWrite(PLAYER_2, HIGH);
  
  strip.begin();
}

void loop() {
  if (ballLocation < NUMPIXELS && ballLocation >= 0) {
    runGame();
  } else {
    fill
  }

  if (digitalRead(PLAYER_1) == LOW && digitalRead(PLAYER_2) == LOW) {
    initializeGameState();
  }
}

void initializeGameState() {
  ballLocation = NUMPIXELS / 2;
  ballDirection = 1;
  ballDelay = 250;
}

void runGame() {
  clearStrip();
  updateGameState();
  renderGameState();
  delay(ballDelay);
}

void clearStrip() {
  fillStrip();
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

void fillStrip() {
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

void setPixelColor( uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
   strip.setPixelColor(n, r, g, b);
}
