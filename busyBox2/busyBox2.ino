
// Headers ----------------------------------------------------------------
#include <Adafruit_TLC5947.h>
#include <TM1637Display.h>
#include <pt.h>
#include <CircularBuffer.h>
#include <Keypad.h>
#include <ezButton.h>

static struct pt pt1, pt2, pt3;

class Led {
  private:
    int map;
    int r;
    int g; 
    int b;
    int pwm;
  
  public:
  Led(int map, int r, int g, int b){
    this->map = map;
    this->r = r;
    this->g = g;
    this->b = b;

    if(r > 0){
      this->pwm = 3*map;
    }

    if(g > 0) {
      this->pwm = 3*map + 1;
    }

    if(b > 0){
      this->pwm = 3*map + 2;
    }
  }
  
  getMap(){
    return this->map;
  }
  
  getR(){
    return this->r;
  }
  
  getB(){
    return this->b;
  }
  
  getG(){
    return this->g;
  }

  getPwm(){
    return this->pwm;
  }
  
};


//PWM 24 Channel-----------------------------------------------------------
#define NUM_TLC5974 1

#define data   4
#define clock   5
#define latch   6
#define oe  -1  // set to -1 to not use the enable pin (its optional)

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5974, clock, data, latch);

//LEDS
#define MAXBRIGHT 4095

//arcade
Led AWR(0, MAXBRIGHT, 0, 0);
Led AWL(1, MAXBRIGHT, 0, 0);
Led AYR(1, 0, 0, MAXBRIGHT);
Led AYL(2, MAXBRIGHT, 0, 0);
Led AGR(0, 0, MAXBRIGHT, 0);
Led AGL(0, 0, 0, MAXBRIGHT);
Led ABR(1, 0, MAXBRIGHT, 0);
Led ABL(2, 0, MAXBRIGHT, 0);
Led ARR(2, 0, 0, MAXBRIGHT);
Led ARL(3, MAXBRIGHT, 0, 0);
const Led arcadeTest[] = {AWL, AGL, AGR, AWR, ABL, AYL, AYR, ABR, ARL, ARR};

//pilot
Led PIL(4, MAXBRIGHT, 0, 0);
Led PIM(3, 0, 0, MAXBRIGHT);
Led PIR(3, 0, MAXBRIGHT, 0);
const Led pilotTest[] = {PIL, PIM, PIR};

//toggle
Led TRT(6, 0, 0, MAXBRIGHT);
Led TRB(6, 0, MAXBRIGHT, 0);
Led TYT(6, MAXBRIGHT, 0, 0);
Led TYB(5, 0, 0, MAXBRIGHT);
Led TGT(5, 0, MAXBRIGHT, 0);
Led TGB(5, MAXBRIGHT, 0, 0);
Led TBT(4, 0, 0, MAXBRIGHT);
Led TBB(4, 0, MAXBRIGHT, 0);

Led ROW4(5, MAXBRIGHT, 0 , MAXBRIGHT);
Led ROW5a(6, MAXBRIGHT, 0 , MAXBRIGHT);
Led ROW5b(3, 0, MAXBRIGHT, MAXBRIGHT);
Led ROW5c(4, MAXBRIGHT, 0, MAXBRIGHT);

const Led toggleTest[] = {TRT, TYT, TGT, TBT, TRB, TYB, TGB, TBB};

//power
Led POR(7, MAXBRIGHT, 0, 0);
Led POB(7, 0, MAXBRIGHT, 0);
Led POG(7, 0, 0, MAXBRIGHT);
const Led powerTest[] ={POR, POB, POG};

const Led row1[] = {AWL, AGL, AGR, AWR};
const Led row2[] = {ABL, AYL, AYR, ABR};
const Led row3[] = {ARR, ARL};
const Led row4[] = {TRB, TYB, TBB, TGB};
const Led row5[] = {TRT, TYT, TBT, TGT, PIR, PIL, PIM};

const Led allLeds[] = {AWR, AWL, AGR, AGL, ABR, ABL, AYR, AYL, ARR, ARL,TRT, TYT, TGT, TBT, TRB, TYB, TGB, TBB, PIL, PIR, PIM};
const Led arcadeLeds[] = {AWR, AWL, AGR, AGL, ABR, ABL, AYR, AYL, ARR, ARL};
const Led toggleLeds[] = {TRT, TYT, TGT, TBT, TRB, TYB, TGB, TBB, PIL, PIR, PIM};
// Buttons --------------------------------------------------------------

#define B_JSU 30
#define B_JSD 31
#define B_AWL 32
#define B_AGL 33
#define B_AGR 34
#define B_AWR 35
#define B_ABL 36
#define B_AYL 37
#define B_AYR 38
#define B_ABR 39
#define B_ARL 48
#define B_ARR 41
#define B_LOC 42


bool s_awl = false;
bool s_agl = false;
bool s_agr = false;
bool s_awr = false;
bool s_abl = false; 
bool s_ayl = false;
bool s_ayr = false;
bool s_abr = false;
bool s_arl = false;
bool s_arr = false;

//const int buttons[] = {B_JSU, B_JSD, B_AWL, B_AGL, B_AGR, B_AWR, B_ABL, B_AYL, B_AYR, B_ABR, B_ARL, B_ARR, B_LOC};


//7 Segment----------------------------------------------------------------

#define CLK 2
#define DIO 3

TM1637Display display = TM1637Display(CLK, DIO);
// array that turns all segments on:
const uint8_t segData[] = {0xff, 0xff, 0xff, 0xff};
// array that turns all segments off:
const uint8_t blank[] = {0x00, 0x00, 0x00, 0x00};

const uint8_t lock[] = {
  SEG_F | SEG_E | SEG_D,           // L
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_D | SEG_E | SEG_G,                           // c
  SEG_F | SEG_E | SEG_B | SEG_G | SEG_C            // k
};

//KeyPad------------------------------------------------------------------

const byte ROWS = 4; 
const byte COLS = 3; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {25, 22, 23, 24}; 
byte colPins[COLS] = {27, 28, 26}; 

CircularBuffer<int,4> buffer;


Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//--------------------------------------------------------------------------

String defaultCode = "0000";
String dadBirthYear = "1986";
String momBirthYear = "1992";

int state = 0;



void setup() {
  
  initPWM();
  initSwitches();
  initKeypadDisplay();
  
  PT_INIT(&pt1);
  PT_INIT(&pt2);
  PT_INIT(&pt3);
  
  Serial.begin(9600);

}

void loop() {
  
  
  protoThreadGetDisplayInput(&pt2);
  protoThreadGetSwitch(&pt3);
  
  switch(state){
    case 0: 
      protothreadBlinkAllLights(&pt1);
      break;
    case 1:
      state = 0;
      break;
    case 2: 
      lightupAll();
      lightupAll();
      lightupAll();
      state = 0;
      break;
    case 3: 
      lightRowsUp();
      break;
    case 4: 
      lightRowsDown();
      break;
    case 5: 
    //Do nothing. Hold.
      break;
    case 9:
      turnOnToggles();
      isCelebration();
      break;
  }
   
}

void initSwitches(){
  
  pinMode(B_JSU, INPUT_PULLUP);
  pinMode(B_JSD, INPUT_PULLUP);
  pinMode(B_LOC, INPUT_PULLUP);
  pinMode(B_AWL, INPUT_PULLUP);
  pinMode(B_AGL, INPUT_PULLUP);
  pinMode(B_AGR, INPUT_PULLUP);
  pinMode(B_AWR, INPUT_PULLUP);
  pinMode(B_ABL, INPUT_PULLUP);
  pinMode(B_AYL, INPUT_PULLUP);
  pinMode(B_AYR, INPUT_PULLUP);
  pinMode(B_ABR, INPUT_PULLUP);
  pinMode(B_ARR, INPUT_PULLUP);
  pinMode(B_ARL, INPUT_PULLUP);
}

static int protoThreadGetSwitch(struct pt *pt){
  PT_BEGIN(pt);

    static bool sJsUp = false;
    static bool sJsDn = false;
    static bool sJsNu = false;

    int joyStickUp = digitalRead(B_JSU);
    
    if (joyStickUp == LOW) {
      if(!sJsUp) {
        state = 3;
        sJsUp = true;
      } else {
        state = 5;
      }
      sJsNu = false;
    }
    
    int joyStickDown = digitalRead(B_JSD);
    
    if (joyStickDown == LOW) {
      if(!sJsDn){
        state = 4;
        sJsDn = true;
      } else {
        state = 5;
      }
      sJsNu = false;
    }

     if (joyStickUp != LOW && joyStickDown != LOW && !sJsNu) {
      sJsDn = false;
      sJsUp = false;
      state = 0;
      sJsNu = true;
     }
    
    int locked = digitalRead(B_LOC);
    
    if (locked == LOW){
      display.setSegments(lock);
    }
  
    int b_AWL = digitalRead(B_AWL);
    if (b_AWL == LOW ){
      if(s_awl == true){
        unsetPwmLed(AWL);
        s_awl = false;
      } else{
        setPwmLed(AWL);
        s_awl = true;
      }
    }
  
    int b_AWR = digitalRead(B_AWR);
  
    if (b_AWR == LOW){
      if(s_awr == true){
        unsetPwmLed(AWR);
        s_awr = false;
      } else{
        setPwmLed(AWR);
        s_awr = true;
      }
    }
  
    int b_AGR = digitalRead(B_AGR);
  
    if (b_AGR == LOW){
       if(s_agr == true){
        unsetPwmLed(AGR);
        s_agr = false;
      } else{
        setPwmLed(AGR);
        s_agr = true;
      }
    }
  
    int b_AGL = digitalRead(B_AGL);
  
    if (b_AGL == LOW){
       if(s_agl == true){
        unsetPwmLed(AGL);
        s_agl = false;
      } else{
        setPwmLed(AGL);
        s_agl = true;
      }
    }
  
    int b_ABL = digitalRead(B_ABL);
  
    if(b_ABL == LOW){
       if(s_abl == true){
        unsetPwmLed(ABL);
        s_abl = false;
      } else{
        setPwmLed(ABL);
        s_abl = true;
      }
    }
  
    int b_AYL = digitalRead(B_AYL);
  
    if(b_AYL == LOW){
       if(s_ayl == true){
        unsetPwmLed(AYL);
        s_ayl = false;
      } else{
        setPwmLed(AYL);
        s_ayl = true;
      }
    }
  
    int b_AYR = digitalRead(B_AYR);
  
    if(b_AYR == LOW){
       if(s_ayr == true){
        unsetPwmLed(AYR);
        s_ayr = false;
      } else{
        setPwmLed(AYR);
        s_ayr = true;
      
    }}
  
    int b_ABR = digitalRead(B_ABR);
  
    if(b_ABR == LOW){
       if(s_abr == true){
        unsetPwmLed(ABR);
        s_abr = false;
      } else{
        setPwmLed(ABR);
        s_abr = true;
      }
    }
  
    int b_ARR = digitalRead(B_ARR);
  
    if(b_ARR == LOW){
       if(s_arr == true){
        unsetPwmLed(ARR);
        s_arr = false;
      } else{
        setPwmLed(ARR);
        s_arr = true;
      }
    }
  
    int b_ARL = digitalRead(B_ARL);
  
    if(b_ARL == LOW){
        if(s_arl == true){
        unsetPwmLed(ARL);
        s_arl = false;
      } else{
        setPwmLed(ARL);
        s_arl = true;
      }
    }
  
  PT_END(pt);
}


void isCelebration(){
  if(s_awl && s_agl && s_agr && s_awr && s_abl && s_ayl && s_ayr && s_abr && s_arl && s_arr){
    //Celebrate
    celebrate();
    state = 0;
    s_awl = false;
    s_agl = false;
    s_agr = false;
    s_awr = false;
    s_abl = false;
    s_ayl = false;
    s_ayr = false;
    s_abr = false;
    s_arl = false;
    s_arr = false;
  }
}

void celebrate(){
  int wait = 50;
  turnOffAllLeds();

  for(unsigned int i = 0; i < 10; i++){
      setPwmLed(AGR);
      setPwmLed(AGL);
      delay(wait);
      unsetPwmLed(AGR);
      unsetPwmLed(AGL);
      
      setPwmLed(AYR);
      setPwmLed(AYL);
      delay(wait);
      unsetPwmLed(AYR);
      unsetPwmLed(AYL);
    
      setPwmLed(ARR);
      setPwmLed(ARL);
      delay(wait);
      unsetPwmLed(ARR);
      unsetPwmLed(ARL);
    
      setPwmLed(ABR);
      setPwmLed(ABL);
      delay(wait);
      unsetPwmLed(ABR);
      unsetPwmLed(ABL);
    
      setPwmLed(AWR);
      setPwmLed(AWL);
      delay(wait);
      unsetPwmLed(AWR);
      unsetPwmLed(AWL);
  }
  
}


void lightupAll() {
  int allLedSize = sizeof(allLeds)/sizeof(allLeds[0]);
  lightUpLedPwmArray(allLeds,allLedSize);
  delay(500);
  turnOffLedPwmArray(allLeds,allLedSize);
  delay(500);
}

void lightRowsDown() {

  turnOffAllLeds();
  
  int wait = 150;
  
  int row5Size = sizeof(row5)/sizeof(row5[0]);
  
  lightUpLedPwmArray(row5, row5Size);

  delay(wait);

  turnOffLedPwmArray(row5, row5Size);
  
  delay(wait);
  
  int row4Size = sizeof(row4)/sizeof(row4[0]);

  lightUpLedPwmArray(row4, row4Size);

  delay(wait);

  turnOffLedPwmArray(row4, row4Size);

 int row3Size = sizeof(row3)/sizeof(row3[0]);

   delay(wait);

  lightUpLedPwmArray(row3, row3Size);

  delay(wait);

  turnOffLedPwmArray(row3, row3Size);

   int row2Size = sizeof(row2)/sizeof(row2[0]);

     delay(wait);

  lightUpLedPwmArray(row2, row2Size);

  delay(wait);

  turnOffLedPwmArray(row2, row2Size);

  int row1Size = sizeof(row1)/sizeof(row1[0]);

  delay(wait);

  lightUpLedPwmArray(row1, row1Size);
 
}

void lightRowsUp(){

  turnOffAllLeds();

  int wait = 150;
  
  int row1Size = sizeof(row1)/sizeof(row1[0]);
  
  lightUpLedPwmArray(row1, row1Size);

  delay(wait);

  turnOffLedPwmArray(row1, row1Size);
  
  delay(wait);
  
  int row2Size = sizeof(row2)/sizeof(row2[0]);

  lightUpLedPwmArray(row2, row2Size);

  delay(wait);

  turnOffLedPwmArray(row2, row2Size);

 int row3Size = sizeof(row3)/sizeof(row3[0]);

   delay(wait);

  lightUpLedPwmArray(row3, row3Size);

  delay(wait);

  turnOffLedPwmArray(row3, row3Size);

   int row4Size = sizeof(row4)/sizeof(row4[0]);

     delay(wait);

  lightUpLedPwmArray(row4, row4Size);

  delay(wait);

  turnOffLedPwmArray(row4, row4Size);

  int row5Size = sizeof(row5)/sizeof(row5[0]);

     delay(wait);

  lightUpLedPwmArray(row5, row5Size);



  
}

void lightUpLedPwmArray(Led leds[], int ledSize){
  for(unsigned int i=0; i < ledSize; i++){
    
    Led led = leds[i];

    setPwmLed(led);

  }
}

void turnOffLedPwmArray(Led leds[], int ledSize){
  for(unsigned int i=0; i < ledSize; i++){
    
    Led led = leds[i];

    unsetPwmLed(led);

  }
}


void turnOnToggles(){
  int ledSize = sizeof(toggleLeds)/sizeof(toggleLeds[0]);

  lightUpLedPwmArray(toggleLeds, ledSize);
}

void turnOffToggles(){
   int ledSize = sizeof(toggleLeds)/sizeof(toggleLeds[0]);

  turnOffLedPwmArray(toggleLeds, ledSize);
}

void turnOffArcadeLeds(){
  int ledSize = sizeof(arcadeLeds)/sizeof(arcadeLeds[0]);

  turnOffLedPwmArray(arcadeLeds, ledSize);
}

void turnOnArcadeLeds(){
  int ledSize = sizeof(arcadeLeds)/sizeof(arcadeLeds[0]);

  lightUpLedPwmArray(arcadeLeds, ledSize);
}

void turnOffAllLeds(){
  int allLedSize = sizeof(allLeds)/sizeof(allLeds[0]);

  turnOffLedPwmArray(allLeds, allLedSize);

}

void checkCode(){
  String tempBuff = "0000";
  for (byte i = 0; i < 4; i = i + 1) {
    tempBuff.setCharAt(i, buffer[i] + '0');
  }
  
  Serial.println(tempBuff);
  
  if(tempBuff.equals(dadBirthYear)){
    state = 1;
  }
  
  if(tempBuff.equals(momBirthYear)){
    state = 2;
  }
}



void initKeypadDisplay(){
  display.clear();
  display.setBrightness(7);
  
  clearBuffer();
}

void blinkDisplay(){
  
  for(byte i=0; i < 5; i = i + 1){
    display.clear();
    delay(300);
    displayBuffer();
    delay(300);
  }

}

void displayBuffer(){
  for (byte i = 0; i < 4; i = i + 1) {
    display.showNumberDec(buffer[i], false, 1, i);
    delay(20);
  }
  
}


void clearBuffer(){
  buffer.push(0);
  buffer.push(0);
  buffer.push(0);
  buffer.push(0);
}


void flickerLedEffect(Led leds[], int ledSize){
  
  int flick[] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144};
  
  for(unsigned int i=0; i < 12; i++){

      lightUpLedPwmArray(leds, ledSize);
      delay(flick[i]);
      turnOffLedPwmArray(leds, ledSize);
      delay(flick[i]);
 
  }
}


void testAllLeds(){
  
  int arcadeLen = sizeof(arcadeTest)/sizeof(arcadeTest[0]);

  animateLedArray(arcadeTest, arcadeLen, 100, pulseLed);
  
  int pilotLen = sizeof(pilotTest)/sizeof(pilotTest[0]);
  
  animateLedArray(pilotTest, pilotLen, 100, pulseLed);
  
  
  int toggleLen = sizeof(toggleTest)/sizeof(toggleTest[0]);
  
  animateLedArray(toggleTest, toggleLen,100, pulseLed);
  
  
  int powerLen = sizeof(powerTest)/sizeof(powerTest[0]);
  
  animateLedArray(powerTest, powerLen, 100, pulseLed);
  
}

void animateLedArray(Led leds[], int ledLength, int wait, void (*ledMethod)(Led, int)){
  
  for(unsigned int i =0; i < ledLength; i++){
    ledMethod(leds[i], wait);
    delay(wait);
  }
}

void pulseLed(Led led, int wait){
  
  int map = led.getMap();
  int r = led.getR();
  int g = led.getG();
  int b = led.getB();
  
  tlc.setLED(map, r, g, b);
  tlc.write();
  delay(wait);
  tlc.setLED(map, 0, 0, 0);
  tlc.write();
  delay(wait);
  
}

void setPwmLed(Led led){
  int map = led.getPwm();

  tlc.setPWM(map, 4095);
  tlc.write();
}

void unsetPwmLed(Led led){
  int map = led.getPwm();

  tlc.setPWM(map, 0);
  tlc.write();
}

void initPWM(){
  tlc.begin();
  if (oe >= 0) {
    pinMode(oe, OUTPUT);
    digitalWrite(oe, LOW);
  }
}



static int protothreadBlinkAllLights(struct pt *pt){
  static unsigned long lastTimeBlink = 0;
  static unsigned long count = 0;
  PT_BEGIN(pt);
  while(1){
    colorWipe(4095, 0, 0, 1000);
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 1000);

    colorWipe(0, 4095, 0, 1000);
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 1000);
  
    colorWipe(0, 0, 4095, 1000);
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 1000);

    count++;

    if(count > 3){
      turnOffAllLeds();
      count = 0;
      break;
    }
  }

  state = 9;

 
  PT_END(pt);
}

static int protoThreadGetDisplayInput(struct pt *pt){
  PT_BEGIN(pt);
  char customKey = customKeypad.getKey();
  
  if (customKey){
    int myNumber = customKey - '0';
    
    if(myNumber >= 0 && myNumber < 10){
    
      buffer.push(myNumber);
      displayBuffer();
    

    }
    
    if(customKey == '*'){
      clearBuffer();
      displayBuffer();
      
    }
    
    if(customKey == '#'){
      blinkDisplay();
      checkCode();

      display.clear();
      clearBuffer();
      displayBuffer();
    }
  }
  
  PT_END(pt);
}




// Fill the dots one after the other with a color
void colorWipe(uint16_t r, uint16_t g, uint16_t b, uint8_t wait) {
  for(uint16_t i=0; i<8*NUM_TLC5974; i++) {
      tlc.setLED(i, r, g, b);
      tlc.write();

  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  uint32_t i, j;

  for(j=0; j<4096; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< 8*NUM_TLC5974; i++) {
      Wheel(i, ((i * 4096 / (8*NUM_TLC5974)) + j) & 4095);
    }
    tlc.write();
    delay(5);
  }
}

// Input a value 0 to 4095 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(uint8_t ledn, uint16_t WheelPos) {
  if(WheelPos < 1365) {
    tlc.setLED(ledn, 3*WheelPos, 4095 - 3*WheelPos, 0);
  } else if(WheelPos < 2731) {
    WheelPos -= 1365;
    tlc.setLED(ledn, 4095 - 3*WheelPos, 0, 3*WheelPos);
  } else {
    WheelPos -= 2731;
    tlc.setLED(ledn, 0, 3*WheelPos, 4095 - 3*WheelPos);
  }
}
