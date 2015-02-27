// library code
#include <LiquidCrystal.h>

// initialize LiquidCrystal library
// pins for the LCD Keypad Shield
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);
// custom character, only lower 5 bits count!
uint8_t custom_smile[8] = {31,17,10,10,0,10,4,0}; // smiley
uint8_t custom_heart[8] = {0,27,31,31,14,4,0,0};
uint8_t adc_key_pin = 0;
uint16_t adc_key_val[5] = {50, 200, 400, 600, 800};
// none, right, up, down, left, select
//uint16_t adc_key_in;  // current key, 0 for no key
//uint16_t adc_key_prev; // previous key, 0 for no key

uint8_t player_shape = 92;
uint8_t playerrow, playercol;
uint8_t startrow;
uint8_t shapes[8] = {32,32,32,42,165,165,2,255};
uint8_t shape_count[8];
uint8_t board[12][12];

// which key was pressed, 0 for no key
uint8_t decodeKey(uint8_t adc_pin) {
  uint8_t key = 0; // assume no key pressed
  uint16_t first, second;
  first = analogRead(adc_pin);
  delay(50);
  second = analogRead(adc_pin);
  //if (abs(second-first) < 10) {
  if ((second ^ first) < 4) { // xor two readings
    key = 0;
    while ((key < 5) && (second > adc_key_val[key])) {
      key += 1;
    }
    key += 1; // 0-4 becomes 1-5
    key = key % 6; // 6 becomes 0
  }
  //lcd.setCursor(0,0);
  //lcd.print(first);
  //lcd.print(":");
  //lcd.setCursor(0,1);
  //lcd.print(second);
  //lcd.print(":");
  return key;
}

void generateBoard() {
  int i,j,s;
  for(j=0;j<8;j++) {
    shape_count[j] = 0;
  }
  for(j=0;j<12;j++) {
    for(i=0;i<12;i++) {
      s = random(0,8);
      board[j][i] = shapes[s];
      shape_count[s] += 1;
    }
  }
}

void displayBoard(uint8_t startRow) {
  int i,j,row;
  
  for(j=0;j<2;j++) {
    row = (j + startRow) % 12;
    lcd.setCursor(0,j);
    for(i=0;i<12;i++) {
      lcd.write(board[row][i]);
    }
    if (row == playerrow) {
      lcd.setCursor(playercol,j);
      lcd.write(1);
    }
  }
}

void movePlayer(uint8_t key) {
  if (key == 2) { // up
    if (playerrow != startrow) {
      if (playerrow == 0) {
        playerrow = 11;
      } else {
        playerrow -= 1;
      }
    }
    if (startrow == 0) {
      startrow = 11;
    } else {
      startrow -=1;
    }
  }
  if (key == 3) { // down
    if (playerrow == startrow) {
      if (playerrow == 11) {
        playerrow = 0;
      } else {
        playerrow += 1;
      }
    }
    if (startrow == 11) {
      startrow = 0;
    } else {
      startrow +=1;
    }
  }
  if (key == 1) { // right
    if (playercol == 11) {
      playercol = 0;
    } else {
      playercol += 1;
    }
  }
  if (key == 4) { // left
    if (playercol == 0) {
      playercol = 11;
    } else {
      playercol -= 1;
    }
  }
}

void setup() {
  uint8_t key;
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  lcd.begin(16,2);
  lcd.createChar(1,custom_smile);
  lcd.createChar(2,custom_heart);
  lcd.setCursor(0,0);
  lcd.print("  Maze Munchers ");
  lcd.setCursor(0,1);
  lcd.print("  Press Any Key ");
  while (decodeKey(adc_key_pin) == 0) 
    {};
  while (decodeKey(adc_key_pin) > 0)
    {};
  lcd.clear();
  randomSeed((millis() + analogRead(1)) % 1024);
//  adc_key_in = 0;
//  adc_key_prev = 0;
  generateBoard();
  startrow = 0;
  playerrow = 0;
  playercol = 0;
  displayBoard(startrow);
}

void loop() {
  int i, j, key;
  key = decodeKey(adc_key_pin);
  movePlayer(key);
  if (key > 0) {
    displayBoard(startrow);
    // indicate keypress delay
    digitalWrite(13,HIGH);
    delay(100);
    digitalWrite(13,LOW);
  }
}
