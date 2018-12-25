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
unsigned int chance[] = {0, 0};
unsigned int timer[] = {0, 0};

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
  if (mode == PONG_MODE) {
    timer[1] = millis();
    if ((timer[1] - timer[0]) >= ballDelay) {
      renderPong();      
      setBallLocation();
      timer[0] = timer[1];
    }
    updateGameState();
  }
  handleButtonPresses();
}

void initializeGameState() {
  ballLocation = NUMPIXELS / 2;
  ballDirection = 1;
  chance[1] = 1;
  ballDelay = 250;
}

void handleButtonPresses() {
  currentButtonState[0] = digitalRead(PLAYER_1);
  currentButtonState[1] = digitalRead(PLAYER_2);
  
  if ((currentButtonState[0] == LOW || currentButtonState[1] == LOW) && 
      (currentButtonState[0] != previousButtonState[0] ||
       currentButtonState[1] != previousButtonState[1])) {
    if (currentButtonState[0] == LOW && currentButtonState[1] == LOW) {
      changeMode();
      initializeGameState();
    }
    
    if (mode == LIGHT_MODE) {
      handleLightMode();
    }
  }

  previousButtonState[0] = currentButtonState[0];
  previousButtonState[1] = currentButtonState[1];
}

void changeMode() {
  if ((mode == PONG_MODE && ballLocation >= NUMPIXELS) ||
      (mode == LIGHT_MODE)) {
    mode = !mode;
    if (mode == PONG_MODE) {
      delay(250);
    }
  }
}

void handleLightMode() {
  if (digitalRead(PLAYER_1) == LOW) {
    lightBrightness = max(lightBrightness << 1, 1);
    lightStrip();
  }
    
  if (digitalRead(PLAYER_2) == LOW) {
    lightColour = max(lightColour << 1, 1);
    lightStrip();
  }
}

void renderPong() {
  if (ballLocation < NUMPIXELS && ballLocation >= 0) {
    clearStrip();
  }
  renderBall(255, 255, 255);
}

void clearStrip() {
  createPongBoard();
  strip.show();
}

void renderBall(uint8_t r, uint8_t g, uint8_t b) {
  if (ballLocation == 255) {
    blinkPixelColor(0, 255, 0, 0);
  } else if (ballLocation == NUMPIXELS) {
    blinkPixelColor(NUMPIXELS - 1, 255, 0, 0);
  } else {
    setPixelColor(ballLocation, r, g, b);
  }
  strip.show();
}

void blinkPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if (strip.getPixelColor(n) == 0) {
    setPixelColor(n, r, g, b);
  } else {
    setPixelColor(n, 0, 0, 0);
  }
}

void updateGameState() {
  setBallDelay();
  setBallDirection();
}

void setBallDirection() {
  if (ballDirection == TO_PLAYER_2 && ballLocation > NUMPIXELS - 4 && digitalRead(PLAYER_2) == LOW) {
    ballDirection = TO_PLAYER_1;
  } else if (ballDirection == TO_PLAYER_1 && ballLocation < 3 && digitalRead(PLAYER_1) == LOW) {
    ballDirection = TO_PLAYER_2;
  } else if (ballLocation >= NUMPIXELS) {
    ballDirection = 0;
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
      setPixelColor(i, 128, 128, 0);
    } else if (i == 0 || i == NUMPIXELS - 1) {
      setPixelColor(i, 0, 128, 0); 
    } else {
      setPixelColor(i, 0, 0, 128);
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
