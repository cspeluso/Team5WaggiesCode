#include <HX711.h>

#include <HX711.h>

#define DOUT  6
#define CLK  5
const int tare_pin =  A1;
const int toggle  = A0;
HX711 scale;
float calibration_factor = 1119; // this calibration factor must be adjusted according to your load cell //489 for 5kg //1119 for 1kg
float units = 0;
float thresh_low = 72;
float thresh_high = 82;
int toggle_treat = 1;
int tare_buttonState = LOW;
int tog_buttonState = LOW;
int oldval;
//***************************VARIABLES FOR LCD
#include <TFT_HX8357.h>
#include <User_Setup.h>

#include <TFT_HX8357.h> // Hardware-specific library

TFT_HX8357 tft = TFT_HX8357();       // Invoke custom library
unsigned long targetTime = 0;
//byte red = 50;
//byte green = 0;
//byte blue = 0;
//byte state = 0;
//unsigned int colour = red << 11; // Colour order is RGB 5+6+5 bits each
float curweight = 0;
bool stateChng = false;
void setup() {
  //*************************for scale
  scale.begin(DOUT, CLK);
  Serial.begin(9600);
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  pinMode(tare_pin, INPUT);
  pinMode(toggle, INPUT);
  //***************************start-up display of the LCD
   tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  drawTreats(toggle_treat);
  TreatTypeDisp(toggle_treat);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
 
  
}

void loop() {
  Serial.println(tog_buttonState);
  tare_buttonState = analogRead(tare_pin);
  tog_buttonState = analogRead(toggle);
  scale.set_scale(calibration_factor);
  
  
  //TOGGLE TREAT FEATURE
  if(tog_buttonState > 0){
     tft.fillRect(5, 10, 200, 255, TFT_WHITE);
     toggle_treat++;
     if(toggle_treat == 5){
      toggle_treat = 1;
     }
     TreatTypeDisp(toggle_treat);
     drawTreats(toggle_treat);
  }
 //SCALE UNITS 
  if(abs(curweight-units) > 1){
    stateChng = true;
  }
  else{
    stateChng = false;
  }
 
 units = -scale.get_units(), 5;
 if (units < 0)
{
   units = 0.00;
}
//TARE FEATURE
  if(tare_buttonState > 0){
    Serial.print("TARING...");
    Serial.println();
    printWeight(units, true);
    scale.tare();
  }
  printWeight(units, false); //units displayed on lcd
 Serial.print("Weight: ");
 Serial.print(units);
 //Serial.print(" grams,   "); 
 // Serial.print(" calibration_factor: ");
 // Serial.print(calibration_factor);
//  if(stateChng){
 // tft.fillRect(200, 20, 200, 300, TFT_WHITE);
 // threshOut(units, toggle_treat);
 // }
  threshOut(units, toggle_treat);
delay(200);

}//LOOP END BRACKET

void drawTreats(int type_treat) //shows the display 
{
  int color = TFT_BLACK;
  bool bits = false;
  if(type_treat == 1){//chicken
    color = TFT_RED;
  }
  else if(type_treat == 2){//sweet potato
    //color = 0x00c414;
    color = 0x059600;
  }
  else if(type_treat == 3){//PB
    color = 0xffea00; //is certainly not the right color but its meant to be yellow??
  }
  else if(type_treat == 4){//ALL BITS
    bits = true;
  }
  if(!bits){
   //tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.fillEllipse(40, 50, 20, 20, color);
    tft.fillEllipse(70,50, 20, 20, color);
    tft.fillRect(40, 50, 30, 100, color);
    tft.fillEllipse(40, 150, 20, 20, color);
    tft.fillEllipse(70, 150, 20, 20, color);
  }
  else{
    //for(int i=0; i<=20;i+=1.5){
     // tft.drawEllipse(50+i, 75+i, 30, 30, 0xd16c00);
   // }
   // tft.fillEllipse(70, 95, 30, 30, 0xffeba3);

 
   tft.fillRoundRect(30.5, 68.5, 50, 50, 20, TFT_BLACK);

   tft.fillRoundRect(59.8, 113, 50, 50, 20, TFT_BLACK);
 
   tft.fillRoundRect(9.3, 117, 49.8, 49.8, 20, TFT_BLACK);
  }
}
void TreatTypeDisp(int type_treat) //puts treat type on bottom + numerical weighout
{
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  if(type_treat==1){ //chicken
     tft.drawString("Chicken", 10, 230, 4); 
     //will print weight under
  }
  else if(type_treat==2){ //sweet potato
     tft.drawString("Sweet Potato", 10, 230, 4); 
  }
  else if(type_treat==3){ //PB
     tft.drawString("Peanut Butter", 10, 230, 4);  
  }
  else if(type_treat==4){ //ALL BITS
     tft.drawString("Bits", 10, 230, 4); 
  }
}
void threshOut(float units, int type_treat){
  if(type_treat==1){//chicken
    thresh_low = 226.796;
    thresh_high = 235.301;
  }
  if(type_treat==2){//sweet potato
    thresh_low = 226.796;
    thresh_high = 235.301;
  }
  if(type_treat==3){//pb
    thresh_low = 238.136;
    thresh_high = 246.641;
  }
  if(type_treat==4){//bits
    thresh_low = 226.796; //IN GRAMS
    thresh_high = 229.6311;
  }
  if(units < thresh_low){
   printThresh(-1);
}
  else if(units > thresh_high){
    printThresh(1);
}
  else {
    printThresh(0);
  }

}
void printThresh(int val) //if val is -1 low, 0 good, 1 high
{
  if(oldval != val){
    if(val == 1){
    tft.fillRect(200, 20, 200, 300, TFT_WHITE);
    //tft.drawString("TOO HIGH", 300, 30, 2);
    tft.fillRect(240, 155, 150, 30, TFT_RED);
    }
    if(val == 0){
      tft.fillRect(200, 20, 200, 300, TFT_WHITE);
     // tft.drawString("PERFECT!", 300, 30, 2);
      tft.fillCircle(315, 170, 75, 0x059600);
      tft.fillCircle(315, 170, 60, TFT_WHITE);
      tft.fillRect(240, 90, 160, 95, TFT_WHITE);
      tft.fillCircle(275, 130, 8,  0x059600);
      tft.fillCircle(355, 130, 8,  0x059600);
     
    }
    if(val == -1){
      tft.fillRect(200, 20, 200, 300, TFT_WHITE);
     // tft.drawString("TOO LOW!", 300, 30, 2);
      tft.fillRect(300, 100, 30, 150, TFT_RED);
      tft.fillRect(240, 155, 150, 30,  TFT_RED);
    }
    oldval = val;
  }
}
void printWeight(float units, bool taring){
  int x;
  if(taring){
    x = tft.drawString("Taring...        ", 10, 290, 4);
  }
  else{
  float ounceunits = units*0.035274;
  x = tft.drawFloat(ounceunits, 2, 10, 290, 4);
  tft.drawString("              ", 10+x, 290, 4);
  tft.drawString(" oz. ", 10+x, 290, 4);
   
  }
  
 
}
//void lcddisptreat
//void treat type disp
//void buttons
//void scalereadingthresh
