#include <Arduino.h>
#include "TimerOne.h"


#define D_BG

#ifdef DBG
#define Serial_begin Serial.begin
#define Serial_println Serial.println
#define Serial_print Serial.print
#else
#define Serial_begin(x)
#define Serial_println(x) 
#define Serial_print(x)
#endif

const int PinCLK=7;                   // Used for generating interrupts using CLK signal
const int PinDT=8;                    // Used for reading DT signal
const int PinSW=9;                    // Used for the push button switch
bool bLastCLK=false;


const int PinSig=4; //rpm input
const int PinZF=2;  //direction
const int PinVR=3;  //speed PWM
const int PinEL=5;  //enable

void setup() {
    Serial_begin(9600);
    Serial_println("Spinner");

    pinMode(PinSig, INPUT);
    pinMode(PinZF, OUTPUT);
    pinMode(PinVR, OUTPUT);
    pinMode(PinEL, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(PinCLK,INPUT);
    pinMode(PinDT,INPUT);  
    pinMode(PinSW,INPUT_PULLUP);
    //attachInterrupt (1,isr,CHANGE);   // interrupt 1 is always connected to pin 3 on Arduino UNO
    bLastCLK=digitalRead(PinCLK);


    //Timer1.initialize(100);  // 10 us = 100 kHz
    //Timer1.attachInterrupt(stepperAdvance);
    Serial_println("init.done");
}

unsigned int cnt=0;
int iRPM=100;
unsigned long iStepCount=0;

/*void isr ()  {                    // Interrupt service routine is executed when a HIGH to LOW transition is detected on CLK
  bool bCLK=digitalRead(PinCLK);
  bool bDT=digitalRead(PinDT);

      Serial_print(bCLK);
      Serial_print(",");
      Serial_print(bDT);
      Serial_print("\n");
  
  bool up = bCLK == bDT;
  if(up) iRPM-=5; else iRPM+=5;
  if(iRPM<0) iRPM=0;
  if(iRPM>400) iRPM=400;
}*/

void loop() {
  cnt++;
  digitalWrite(LED_BUILTIN, ((cnt/10)&1)!=0);

  const int iMin=30;
  analogWrite(PinVR,iMin + (long(iRPM)*(255-iMin)/1000));
  digitalWrite(PinEL, iRPM>0);

  if(iRPM<1) return;
  delay(10);
  

#ifdef DBG
  //*
  unsigned int iNM=millis();
  static unsigned int iLastLog=0;
  if(iNM>iLastLog+500) {
    iLastLog=iNM;

    const long iRot=(iStepCount*GEAR_A)/(GEAR_B*SIGPR);
    //const int iMSTEPS=(long(STEPS_PERREV*32)*GEAR_B)/GEAR_A;

    Serial_print(iRPM);
    Serial_print(",");
    Serial_print(iStepCount);
    Serial_print(",");
    Serial_print(iRot);
    Serial_print("\n");
    cnt=0;
  }/**/
#endif

  //Serial.println("loop");
}

unsigned long iLastStep=0;
bool bStepOn=false;
unsigned int iClick=0;
bool bSig=false;

void stepperAdvance() {
  unsigned long iNow=micros();
  if(iNow<iLastStep) {iLastStep=iNow;}; //overflow

  bool bCLK=digitalRead(PinCLK);
  if(bLastCLK!=bCLK) {
    bLastCLK=bCLK;
    bool bDT=digitalRead(PinDT);
    if(bDT!=bCLK) {
      iRPM+=20;
      if(iRPM>1000) iRPM=1000;
    } else {
      iRPM-=20;
      if(iRPM<0) iRPM=0;
    }
 }
  
  if (!(digitalRead(PinSW))) {
    //Serial_print(iClick);
    iClick++;
    if(iClick>7000) {iStepCount=0;};
    iRPM=0;
  } else {iClick=0;}


  bool bS=digitalRead(PinSig);
  if(bS!=bSig) {
    iStepCount++;
    bSig=bS;
  };
}




