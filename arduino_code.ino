
#include <Keypad.h>

// ================= KEYPAD =================

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'D','#','0','*'},
  {'C','9','8','7'},
  {'B','6','5','4'},
  {'A','3','2','1'}
};

byte rowPins[ROWS] = {27, 14, 12, 13};
byte colPins[COLS] = {32, 33, 25, 26};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


// ================= IR =================

#define IR1 34
#define IR2 35


// ================= BUZZER =================

#define BUZZER 23


// ================= LEDs =================

#define R1 4
#define Y1 16
#define G1 17

#define R2 18
#define Y2 19
#define G2 21

#define R3 22
#define Y3 5
#define G3 15

#define R4 2
#define Y4 3
#define G4 1


// ================= VARIABLES =================

int lastSensor = 0;
int violationCount = 0;

bool emergencyMode = false;
int emergencyRoad = 0;

bool failureMode = false;

unsigned long densityStart = 0;


// ================= SETUP =================

void setup()
{

pinMode(IR1, INPUT);
pinMode(IR2, INPUT);

pinMode(BUZZER, OUTPUT);

int pins[] = {R1,Y1,G1,R2,Y2,G2,R3,Y3,G3,R4,Y4,G4};

for(int i=0;i<12;i++)
pinMode(pins[i], OUTPUT);

}


// ================= ALL RED =================

void allRed()
{

digitalWrite(R1,HIGH);
digitalWrite(R2,HIGH);
digitalWrite(R3,HIGH);
digitalWrite(R4,HIGH);

digitalWrite(G1,LOW);
digitalWrite(G2,LOW);
digitalWrite(G3,LOW);
digitalWrite(G4,LOW);

digitalWrite(Y1,LOW);
digitalWrite(Y2,LOW);
digitalWrite(Y3,LOW);
digitalWrite(Y4,LOW);

}


// ================= WRONG SIDE =================

void checkWrongSide()
{

if(digitalRead(IR1)==LOW)
{

if(lastSensor==2)
{

violationCount++;

tone(BUZZER,3000);
delay(400);
noTone(BUZZER);

delay(300);

for(int i=0;i<violationCount;i++)
{
tone(BUZZER,2000);
delay(200);
noTone(BUZZER);
delay(200);
}

}

lastSensor=1;

}

if(digitalRead(IR2)==LOW)
{
lastSensor=2;
}

}


// ================= EMERGENCY =================

void emergencyControl()
{

allRed();

switch(emergencyRoad)
{

case 1:
digitalWrite(R1,LOW);
digitalWrite(G1,HIGH);
break;

case 2:
digitalWrite(R2,LOW);
digitalWrite(G2,HIGH);
break;

case 3:
digitalWrite(R3,LOW);
digitalWrite(G3,HIGH);
break;

case 4:
digitalWrite(R4,LOW);
digitalWrite(G4,HIGH);
break;

}

tone(BUZZER,2500);
delay(300);
noTone(BUZZER);

delay(5000);

emergencyMode=false;

}


// ================= FAILURE MODE =================

void failureControl()
{

while(failureMode)
{

digitalWrite(R1,LOW);
digitalWrite(R2,LOW);
digitalWrite(R3,LOW);
digitalWrite(R4,LOW);

digitalWrite(G1,LOW);
digitalWrite(G2,LOW);
digitalWrite(G3,LOW);
digitalWrite(G4,LOW);

digitalWrite(Y1,HIGH);
digitalWrite(Y2,HIGH);
digitalWrite(Y3,HIGH);
digitalWrite(Y4,HIGH);

tone(BUZZER,1500);
delay(300);
noTone(BUZZER);

delay(700);

digitalWrite(Y1,LOW);
digitalWrite(Y2,LOW);
digitalWrite(Y3,LOW);
digitalWrite(Y4,LOW);

delay(700);

keypadControl();

}

}


// ================= TIMER =================

void runTimer(int duration)
{

unsigned long start = millis();
densityStart = 0;

while(millis()-start < duration)
{

checkWrongSide();
keypadControl();

if(emergencyMode || failureMode)
return;


// ===== TRAFFIC DENSITY FEATURE =====

if(digitalRead(IR1) == LOW)
{

if(densityStart == 0)
densityStart = millis();

if(millis() - densityStart >= 2000)
{
duration += 2000; // extend green
densityStart = 0;
}

}
else
{
densityStart = 0;
}

}

}


// ================= TRAFFIC =================

void trafficSequence()
{

allRed();
digitalWrite(R1,LOW);
digitalWrite(G1,HIGH);
runTimer(3000);
digitalWrite(G1,LOW);
digitalWrite(Y1,HIGH);
delay(1000);

allRed();
digitalWrite(R2,LOW);
digitalWrite(G2,HIGH);
runTimer(3000);
digitalWrite(G2,LOW);
digitalWrite(Y2,HIGH);
delay(1000);

allRed();
digitalWrite(R3,LOW);
digitalWrite(G3,HIGH);
runTimer(3000);
digitalWrite(G3,LOW);
digitalWrite(Y3,HIGH);
delay(1000);

allRed();
digitalWrite(R4,LOW);
digitalWrite(G4,HIGH);
runTimer(3000);
digitalWrite(G4,LOW);
digitalWrite(Y4,HIGH);
delay(1000);

}


// ================= KEYPAD =================

void keypadControl()
{

char key=keypad.getKey();

if(key=='#')
violationCount=0;

if(key=='A')
{
emergencyMode=true;
emergencyRoad=1;
}

if(key=='B')
{
emergencyMode=true;
emergencyRoad=2;
}

if(key=='C')
{
emergencyMode=true;
emergencyRoad=3;
}

if(key=='D')
{
emergencyMode=true;
emergencyRoad=4;
}

if(key=='0')
failureMode=true;

if(key=='*')
failureMode=false;

}


// ================= LOOP =================

void loop()
{

if(failureMode)
failureControl();

else if(emergencyMode)
emergencyControl();

else
trafficSequence();

}
