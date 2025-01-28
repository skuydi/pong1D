/*
 * Like all Arduino code - copied from somewhere else :)
 * So don't claim it as your own
 * Based on the code of  FlyingAngel - 18.4.2020
 * ---------------------------------
Modified version - 23/07 by skuydi
*/

#define FASTLED_INTERNAL       // Disable version number message in FastLED library (looks like an error)
#include <FastLED.h>

// *********************************
// Definition personal Variables
// *********************************

// Neopixel
#define NUM_LEDS 106            //106 - 142  // Number of LEDs
#define DATA_PIN 3              // Data-Pin
int maxBright       =       30; // max brightness
byte playerBtnPin[] =   {5, 8}; // Pins for buttons
byte playerLedPin[] =   {6, 9}; // Pins for LEDs in buttons
int gameSpeedMin    =       25; // min game-speed
int gameSpeedMax    =        5; // max game-speed
int gameSpeedStep   =        2; // fasten up when change direction
int ballSpeedMax    =        8; // max ball-speed  - plus haut moins vite
int ballBoost0      =       15; // superboost on last position - plus haut plus vite
int ballBoost1      =       10; // boost on forelast position - plus haut plus vite
int ballBoost2      =        5; // boost on for-forelast position - plus haut plus vite
byte playerColor[]  =  {0, 96}; // red & green
int winRounds       =       10; // x winning rounds for winning game
int endZoneSize     =       10; // size endzone
int endZoneColor    =       30; // color endzone 160=blue

// *********************************
// Definition System-Variablen
// *********************************
boolean activeGame = false;                 // true when game is active
unsigned long previousMoveMillis;           // time of last ball move
unsigned long previousButtonMillis;         // time of last button-press
int playerButtonPressed[2];                 // ball-position where button was pressed; „-1“ button not pressed
int previousButtonPos = -1;                 // position of last button-press
byte previousButtonColor;                   // color of field for last Button-press
int playerScore[2];                         // actual Score
byte playerStart;                           // who starts game
int gameSpeed;                              // actual game-speed
int ballDir = 1;                            // direction, ball is moving (+/- 1)
int ballPos;                                // actual ball-position
int ballSpeed;                              // actual ball-speed (higher = slower)

CRGB leds[NUM_LEDS];                        // Define the array of LEDs
byte previousButtonBright = maxBright / 2;  // bright of marked last position when button pressen
byte scoreDimBright       = maxBright / 4;  // bright of dimmed score

// *********************************
// Setup
// *********************************
void setup() {
  // Serial.begin(9600);
  randomSeed(analogRead(0));              // better Random
  // FastLed definition
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // PIN defination
  pinMode(playerBtnPin[0], INPUT_PULLUP); // PINs for buttons
  pinMode(playerBtnPin[1], INPUT_PULLUP);
  pinMode(playerLedPin[0], OUTPUT);
  pinMode(playerLedPin[1], OUTPUT);       // PINs for LEDs
  playerStart = random(2);                // random starting player
}

// *********************************
// Loop
// *********************************
void loop(){
  menu();
}

// Menu (set brightness and start game)
void menu() {
  digitalWrite(playerLedPin[0], HIGH);  // activate both LEDs
  digitalWrite(playerLedPin[1], HIGH);
  GeneratePlayField(maxBright);         // show play-field
  FastLED.show();

  // set brightness (button player 0)
  if (buttonBounce(0, 5) == true) {
    maxBright += 50;
    if (maxBright > 255) {
      maxBright = 55;
    }
    GeneratePlayField(maxBright);
    FastLED.show();

    while (buttonBounce(0, 5) == true) {}   // wait for button release
  }

  // start game (button player 1)
  if (buttonBounce(1, 5) == true) {
    digitalWrite(playerLedPin[0], LOW);     // deactivate both LEDs
    digitalWrite(playerLedPin[1], LOW);
    activeGame = true;
    while (buttonBounce(1, 5) == true) {}   // wait for button release
    game();
  }
}

// function to debounce button (true == pressed, false == not pressed)
  boolean buttonBounce(byte button, byte bounceTime) { // bounce the button
  boolean result = false;
  if (digitalRead(playerBtnPin[button]) == LOW) {
    delay (bounceTime);
    if (digitalRead(playerBtnPin[button]) == LOW) {
      result = true;
    }
  }
  return result;
}

void game() {
  while (activeGame) {
    gameSpeed = gameSpeedMin;           // set starting game speed
    ballSpeed = gameSpeed;              // set starting ball speed
    memset(playerButtonPressed, -1, sizeof(playerButtonPressed)); // clear keypress
    GeneratePlayField(scoreDimBright);  // show gamefield with dimmed score
    FastLED.show();
    InitializePlayers();                // set the player-settings -> wait for keypress to start game
    GameLoop();                         // main loop: move ball -> ball left gamefield? -> check keypress -> check if keypress in endzone -> change direction
    CheckScore();                       // check who made score and show it
    CheckWinner();                      // check if we have a winner
  }
}

void InitializePlayers() {
  if (playerStart == 0) {                             // initialize for player 0
    ballDir = 1;                                      // set ball direction
    ballPos = 0;                                      // set startposition of ball
    digitalWrite(playerLedPin[0], HIGH);              // activate LED
    while (digitalRead(playerBtnPin[0]) == HIGH) {}   // wait for button
    digitalWrite(playerLedPin[0], LOW);               // deactivate LED
  }
  else {  // initialize for player 1
    ballDir = -1;                                     // set ball direction
    ballPos = NUM_LEDS - 1;                           // set startposition of ball
    digitalWrite(playerLedPin[1], HIGH);              // activate LED
    while (digitalRead(playerBtnPin[1]) == HIGH) {}   // wait for button
    digitalWrite(playerLedPin[1], LOW);               // deactivate LED
  }
}

void GameLoop(){
  while (true) {                                      // loop, exit with break when one player made a score
    if (millis() - previousMoveMillis > ballSpeed) {  // time to move ball
      previousMoveMillis = millis();
      GeneratePlayField(scoreDimBright);
      ballPos += ballDir;
      if (ballPos < 0 || ballPos >= NUM_LEDS) {       // ball left endzone?
        break;                                        // leave loop -> one player made a score
      }
      leds[ballPos] = CHSV(0, 0, maxBright);          // generate ball (white)
      FastLED.show();
    }

    CheckButtons();     // check keypress
    // fix positions of keypress for testing
    // if (ballPos == 3) playerButtonPressed[0] = 3;
    // if (ballPos == 59) playerButtonPressed[1] = 59;
    CheckButtonPressedPosition();
  } // end of while-loop
}

// *** check if buttons pressed
void CheckButtons() {
  for (int i = 0; i < 2; i++) {
    // player pressed button?
    if (playerButtonPressed[i] == -1 && digitalRead(playerBtnPin[i]) == LOW && (ballDir + 1) / 2 == i) {
      // (ballDir + 1) / 2 == i  -->  TRUE, when:
      // ballDir == -1  AND  i = 0  -->  player 0 is active player
      // ballDir == +1  AND  i = 1  -->  player 1 is active player
      // only the button-press of the active player is stored
      playerButtonPressed[i] = ballPos;   //store position of pressed button
      previousButtonPos = ballPos;
      previousButtonColor = playerColor[i];
      previousButtonMillis = millis(); // store time when button was pressed
    }
  }
}

// *** check, if button was pressed when ball was in endzone and if so, change direction of ball
void CheckButtonPressedPosition() {
  if (ballDir == -1 && playerButtonPressed[0] <= endZoneSize - 1 && playerButtonPressed[0] != -1) {
    ChangeDirection();
  }
  if (ballDir == +1 && playerButtonPressed[1] >= NUM_LEDS - endZoneSize) {
    ChangeDirection();
  }
}

void ChangeDirection(){
  ballDir *= -1;
  gameSpeed -= gameSpeedStep;
  ballSpeed = gameSpeed;
  if (ballPos == 0 || ballPos == NUM_LEDS - 1) {                // triggered on first or last segment
    ballSpeed -= ballBoost0;                                    // Super-Boost
  }

  if (ballPos == 1 || ballPos == NUM_LEDS - 2) {                // triggered on second or forelast segment
    ballSpeed -= ballBoost1;                                    // Boost
  }

  if (ballPos == 2 || ballPos == NUM_LEDS - 3) {                // triggered on third or for-forelast segment
    ballSpeed -= ballBoost2;                                    // Boost
  }
  
  ballSpeed = max(ballSpeed, ballSpeedMax);                     // limit the maximum ballspeed
  memset(playerButtonPressed, -1, sizeof(playerButtonPressed)); // clear keypress
}

void CheckScore() {
  previousButtonPos = -1;           // clear last ball-position at button-press
  // check who made score
  if (ballPos < 0)   {              // player1 made the score
    playerScore[1] += 1;            // new score for player1
    GeneratePlayField(maxBright);   // show new score full bright
    BlinkNewScore(NUM_LEDS / 2 - 1 + playerScore[1], playerColor[1]); // blink last score
    playerStart = 1;                // define next player to start (player, who made the point)
  }
  else {                          // player0 made the score
    playerScore[0] += 1;          // new score for player0
    GeneratePlayField(maxBright); // show new score full bright
    BlinkNewScore(NUM_LEDS / 2 - playerScore[0], playerColor[0]); // blink last score
    playerStart = 0;              // define next player to start (player, who made the point)
  }

  GeneratePlayField(maxBright);   // show new score full bright
  FastLED.show();
  delay(1000);
}

void CheckWinner() {
  // check if we have a winner
  if (playerScore[0] >= winRounds || playerScore[1] >= winRounds) { // we have a winner!
    activeGame = false;
    FastLED.clear();
    Rainbow(playerScore[0] > playerScore[1]);     // TRUE if player 0 won; FALSE when player 1 won
    memset(playerScore, 0, sizeof(playerScore));  // reset all scores
    playerStart = abs(playerStart - 1);           // next game starts looser
  }
}


void GeneratePlayField(byte bright) {
  FastLED.clear();        // clear all
  GenerateEndZone();      // generate endzone
  GenerateScore(bright);  // generate actual score
  GenerateLastHit();      // generate mark of position of last button-press
}

void GenerateEndZone() {
  for (int i = 0; i < endZoneSize; i++)   {
    leds[i] = CHSV(endZoneColor, 255, maxBright);
    leds[NUM_LEDS - 1 - i] = CHSV(endZoneColor, 255, maxBright);
  }
}

void GenerateScore(int bright) {
  int i;
  // Player 0
  for (i = 0; i < playerScore[0]; i++) {
    leds[NUM_LEDS / 2 - 1 - i] = CHSV(playerColor[0], 255, bright);
  }
  // Player 1
  for (i = 0; i < playerScore[1]; i++) {
    leds[NUM_LEDS / 2 + i] = CHSV(playerColor[1], 255, bright);
  }
}

void GenerateLastHit() {
  if (previousButtonPos != -1 && previousButtonMillis + 500 > millis()) {
    leds[previousButtonPos] = CHSV(previousButtonColor, 255, previousButtonBright);
  }
}

void BlinkNewScore(int pos, byte color) {
  for (int i = 1; i <= 4; i++)  {
    leds[pos] = CHSV(color, 255, (i % 2) * maxBright);  // blink LED 2 times (1-0-1-0)
    FastLED.show();
    delay(300);
  }
}

void Rainbow(boolean won) {
  for (int k = 0; k < 3; k++) {  // 3 rounds rainbow
    for (int j = 0; j <= 255; j++) {
      for (int i = 0; i < NUM_LEDS / 2; i++) {
        if (won == true) { // player 0 won --> Rainbow left
          leds[i] = CHSV(((i * 256 / NUM_LEDS) + j) % 256, 255, maxBright);
        }
        else { // player 1 won --> Rainbow right
          leds[NUM_LEDS - i - 1] = CHSV(((i * 256 / NUM_LEDS) + j) % 256, 255, maxBright);
        }
      }
      FastLED.show();
      delay(7);
    }
  }
}
