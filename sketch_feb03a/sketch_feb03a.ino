#include <dht.h>  
#include <MQ2.h>  
#include "DHT.h"
#include <LiquidCrystal.h>
//inkluderar biblioteket för: luftfuktighets och tempratur sensorn, rök och gas sensorn och för LCD-skärmen.

#define dht_apin A0 //definerar analoga pinen A0 till luftfuktighets och tempratur sensorn.
dht DHT;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //deinerar utportarna för LCD-skämen

int AnalogMQ2_Input = A1;
int lpg, co;
MQ2 mq2(AnalogMQ2_Input);

//Initierar Inputs & knapp-logiken för att byta meny.
const int numOfInputs = 1;
const int inputPins[numOfInputs] = {13};
int inputState[numOfInputs];
int lastInputState[numOfInputs] = {LOW};
bool inputFlags[numOfInputs] = {LOW};
long lastDebounceTime[numOfInputs] = {0};
long debounceDelay = 5;

//LCD Menyerna.
const int numOfScreens = 2;
int currentScreen = 0;
String screens[numOfScreens][2] = {{"Temp: ","Hum: "}, {"Smoke: ","Gas: "}};
int parameters[numOfScreens];

void setup() {
  for(int i = 0; i < numOfInputs; i++) { //itierar för knapptryck.
    pinMode(inputPins[i], INPUT);
    digitalWrite(inputPins[i], HIGH); // 20k
  }
    pinMode(7, INPUT);// INPUT från rörelse sensor
    pinMode(8, OUTPUT);// OUTPUT till LED
    pinMode(6, INPUT);// INPUT från rörelse sensor
    pinMode(9, OUTPUT);// OUTPUT till LED
  
    //Serial.begin(9600);
    lcd.begin(16, 2); //startar LCD-skärmen och definerar alla positioner.
    mq2.begin(); //startar mq2-sensorn.
    
}

void loop() {
  setInputFlags(); //Återkallar funktionen "setInputFlags"
  resolveInputFlags(); //Återkallar funktionen "resolveInputFlags"
  motionPIR();  //Återkallar funktionen "motionPIR"
  DHT.read11(dht_apin); //läser DHT senorn
}

void setInputFlags() {
  for(int i = 0; i < numOfInputs; i++) {
    int reading = digitalRead(inputPins[i]);
    if (reading != lastInputState[i]) {
      lastDebounceTime[i] = millis();
    }
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != inputState[i]) {
        inputState[i] = reading;
        if (inputState[i] == HIGH) {
          inputFlags[i] = HIGH;
        }
      }
    }
    lastInputState[i] = reading;
  }
}

void resolveInputFlags() {
  for(int i = 0; i < numOfInputs; i++) {
    if(inputFlags[i] == HIGH) {
      inputAction(i);
      inputFlags[i] = LOW;
      printScreen();
    }
  }
}

void inputAction(int input) {
  if(input == 0) {
    if (currentScreen == 0) {
      currentScreen = numOfScreens-1;
    }else{
      currentScreen--;
    }
  }else if(input == 1) {
    if (currentScreen == numOfScreens-1) {
      currentScreen = 0;
    }else{
      currentScreen++;
    }
  }
}

void motionPIR(){
  int motionRED =digitalRead(7);
  int motionBLUE =digitalRead(6);
  lcd.setCursor(0, 0);
    
  if(motionRED){
    lcd.clear();
    lcd.print("Motion Room A");
    digitalWrite(8,HIGH);
    delay(5000);
   }
  else{ 
    digitalWrite(8,LOW);
   }

  if(motionBLUE){
    lcd.clear();
    lcd.print("Motion Room B");
    digitalWrite(9,HIGH);
    delay(5000);
   }
  else{
    digitalWrite(9 ,LOW);
   }
}



void printScreen() {  //funktionen som printar sensorernas värden på LCD-skärmen
  float* values= mq2.read(true);
  co = mq2.readCO();
  int smoke = analogRead(A1);
  //smoke = 3.23;
  int a = analogRead(A1);
  lpg = a/2;
  //mq2.readLPG();
  
  lcd.clear();
  lcd.print(screens[currentScreen][0]);
  lcd.setCursor(0,1);
  lcd.print(screens[currentScreen][1]);
  
   if(currentScreen == 0){
     lcd.setCursor(7,0);
     lcd.print(DHT.temperature);
     lcd.print("C");
     lcd.setCursor(7,1);
     lcd.print(DHT.humidity);
     lcd.print("%");
   }
   else if(currentScreen == 1){
     lcd.setCursor(7,0);
     lcd.print(smoke);
     lcd.setCursor(9,0);
     lcd.print("PPM");
     lcd.setCursor(5,1);
     lcd.print(lpg);
     lcd.setCursor(7,1);
     lcd.print("LPG");
   }
   
}
