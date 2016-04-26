#include <RtcDateTime.h>
#include <RtcDS1307.h>
//#include <RtcTemperature.h>
#include <RtcUtility.h>

#define A 6
#define B 5
#define C 4
#define D 3
#define E 2
#define H 7
#define M 8

/* Create an rtc ds1307 related object */
RtcDS1307 Rtc;
long unsigned time = millis();
byte Array[6][4][2] {
  {{A, B}, {B, A}},
  {{C, A}, {A, C}, {B, C}, {C, B}},
  {{D, A}, {A, D}, {D, C}},
  {{A, E}, {E, D}, {D, E}, {C, D}},
  {{E, A}, {B, D}, {D, B}},
  {{C, E}, {E, C}, {B, E}, {E, B}}
};
byte arrayBinary[6][4];
byte s = 0, m = 0, h = 0;
int hLastState = 0, mLastState=0;
/************************************/
void toBin(int a, int j) {
  int i = 0;
  if (a == 0) {
    for (i; i < 4; i++)
      arrayBinary[j][i] = 0;
  } else {
    while (a > 0) {
      arrayBinary[j][i++] = a % 2;
      a /= 2;
    }
  }
}

void setup() {
  pinMode(A, INPUT);
  pinMode(B, INPUT);
  pinMode(C, INPUT);
  pinMode(D, INPUT);
  pinMode(E, INPUT);
  Serial.begin(9600);
  Serial.println("start rtc");
  Rtc.Begin();
  //obiekt z data i godzina
  RtcDateTime boot = Rtc.GetDateTime();
  time = millis();
  s = boot.Second();
  m = boot.Minute();
  h = boot.Hour();
  String temp = "H: " + (String)h + " M: " + (String)m + " s: " + (String)s;
  Serial.println(temp);
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();

  int pom[6];
  pom[0] = h / 10;
  pom[1] = h % 10;
  pom[2] = m / 10;
  pom[3] = m % 10;
  pom[4] = s / 10;
  pom[5] = s % 10;
  for (int i = 0; i < 6; i++) {
    toBin(pom[i], i);
  }

  while (time + 1000 > millis()) {
    
    int hButtonState = digitalRead(H);
    if(hButtonState != hLastState) {
      if(hButtonState==HIGH) {
        now.setHour(now.Hour() + 1);
      Rtc.SetDateTime(now);
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
      Rtc.SetDateTime(now);
      Serial.println("Minute++ "+(String)m);
      }
      mLastState = mButtonState;
      delay(5);
      break;
    }
    
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 4; j++) {
        if (arrayBinary[i][j] == 1) {
          pinMode(Array[i][j][0], OUTPUT);
          pinMode(Array[i][j][1], OUTPUT);
          digitalWrite(Array[i][j][0], HIGH);
          if (i % 2 == 0)
            delay(0.9);
          pinMode(Array[i][j][0], INPUT);
          pinMode(Array[i][j][1], INPUT);
        }
      }
  }
  
  if(time + 1000 <= millis()) {
    time+=1000;
    now = Rtc.GetDateTime();
    s = now.Second();
    m = now.Minute();
    h = now.Hour();
    if(h == 24) h=0;
  }
}
