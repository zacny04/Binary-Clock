/*
setHour() and setMinute() require changes in Rtc library, they have to be added .
*/
#include <RtcDateTime.h>
#include <RtcDS1307.h>
#include <RtcUtility.h>

#define A 6
#define B 5
#define C 4
#define D 3
#define E 2
#define H 7
#define M 8

RtcDS1307 Rtc;

byte ledArray[6][4][3] {
  {{A, B, 0}, {B, A, 0}},
  {{C, A, 0}, {A, C, 0}, {B, C, 0}, {C, B, 0}},
  {{D, A, 0}, {A, D, 0}, {D, C, 0}},
  {{A, E, 0}, {E, D, 0}, {D, E, 0}, {C, D, 0}},
  {{E, A, 0}, {B, D, 0}, {D, B, 0}},
  {{C, E, 0}, {E, C, 0}, {B, E, 0}, {E, B, 0}}
};
byte s = 0, m = 0, h = 0;
int hLastState = 0, mLastState=0;
long unsigned time = millis();
/************************************/
void toBin(int a, int j) {
  int i = 0;
  if (a == 0) {
    for (i; i < 4; i++)
      ledArray[j][i][3] = 0;
  } else {
    while (a > 0) {
      ledArray[j][i++][3] = a % 2;
      a /= 2;
    }
  }
}

void setup() {
  for(int i=E;i<=A;i++)
    pinMode(i, INPUT);
  Serial.begin(9600);
  Serial.println("start rtc");
  Rtc.Begin();
  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC nieaktywne, startuje");
    Rtc.SetIsRunning(true);
  }
  //date & time object
  RtcDateTime boot = Rtc.GetDateTime();
  time = millis();
  s = boot.Second();
  m = boot.Minute();
  h = boot.Hour();
  String startTime = "H: " + (String)h + " M: " 
                   + (String)m + " s: " + (String)s;
  Serial.println(startTime);
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();

  int numbers[6] = { h/10, h%10, m/10,
                 m%10, s/10, s%10 };
  for (int i = 0; i < 6; i++) {
    toBin(numbers[i], i);
  }

  while (time + 1000 > millis()) {
    
    int hButtonState = digitalRead(H);
    if(hButtonState != hLastState) {
      if(hButtonState==HIGH) {
        now.setHour(now.Hour() + 1);
        Rtc.SetDateTime(now);
        h = now.Hour();
        Serial.println("Hours++ "+(String)h);
      }
      hLastState = hButtonState;
      delay(5);
      break;
    }

    int mButtonState = digitalRead(M);
    if(mButtonState != mLastState) {
      if(mButtonState==HIGH) {
        now.setMinute(now.Minute() + 1);
        now.setSecond(0);
        m = now.Minute();
        s = now.Second();
        Rtc.SetDateTime(now);
        Serial.println("Minute++ "+(String)m);
      }
      mLastState = mButtonState;
      delay(5);
      break;
    }
    
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 4; j++) {
        if (ledArray[i][j][3]==1) {
          pinMode(ledArray[i][j][0], OUTPUT);
          pinMode(ledArray[i][j][1], OUTPUT);
          digitalWrite(ledArray[i][j][0], HIGH);
          //led workaround
          if (i % 2 == 0)
            delay(0.9);
          pinMode(ledArray[i][j][0], INPUT);
          pinMode(ledArray[i][j][1], INPUT);
        }
      }
    
  }
  
  if(time + 1000 <= millis()) {
    time+=1000;
    now = Rtc.GetDateTime();
    s = now.Second();
    m = now.Minute();
    h = now.Hour();
  }
  if(h == 24) h=0;
}
