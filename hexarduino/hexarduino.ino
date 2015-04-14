#include <SoftwareSerial.h>
#include <Ultrasonic.h>

const int ch1 = 7;
const int ch2 = 6;
const int ch3 = 5;
const int ch4 = 4;
const int ch5 = 3;
const int ch6 = 2;
const int led = 13;

const short VERTICAL_SERVOS[] = {0, 2, 4, 16, 18, 20};
const short HORIZONTAL_SERVOS[] = {1, 3, 5, 17, 19, 21};

const short REAR_RIGHT_LEG[] = {0, 1, 8};
const short MIDDLE_RIGHT_LEG[] = {2, 3, 9};
const short FRONT_RIGHT_LEG[] = {4, 5, 10};

const short REAR_LEFT_LEG[] = {16, 17, 24};
const short MIDDLE_LEFT_LEG[] = {18, 19, 25};
const short FRONT_LEFT_LEG[] = {20, 21, 26};

int mediumLegPosition = 0;
boolean moving = false;

short values[] = {1500, 1500, 1500, 1500, 1500, 1500};
short oldValues[] = {0, 0, 0, 0, 0, 0};

SoftwareSerial engines(8,9);
Ultrasonic distance(10, 11);

void setup() {
  pinMode(led, OUTPUT);

  pinMode(ch1, INPUT);
  pinMode(ch2, INPUT);
  pinMode(ch3, INPUT);
  pinMode(ch4, INPUT);
  pinMode(ch5, INPUT);
  pinMode(ch6, INPUT);
  Serial.begin(115200);
  engines.begin(115200);
  returnLegsToInitialPosition();

  setupGaitValues();
}

void loop() {
  //Get readings from RC-receiver channels
  values[0] = pulseIn(ch1, HIGH)-1500;
  values[1] = pulseIn(ch2, HIGH)-1500;
  //values[2] = pulseIn(ch3, HIGH)-1500;
  //values[3] = pulseIn(ch4, HIGH)-1500;
  //values[4] = pulseIn(ch5, HIGH)-1500;
  //values[5] = pulseIn(ch6, HIGH)-1300;
  
  //Moving speed is read from channel 1 (right stick, top-bottom direction)
  int movingSpeed = int(values[1] / 3.20);  //Normalize the value
  //Serial.print("--movingSpeed=");
  //Serial.println(movingSpeed,DEC);
  if (movingSpeed <10 && movingSpeed > -10) movingSpeed = 0;  //And remove noise

  //Turning speed is read from channel 0 (right stick, left-right direction)
  int turningSpeed = int(values[0] / 3.20);  //Normalize
  if (turningSpeed <10 && turningSpeed > -10) turningSpeed = 0;  //Remove noise
  //Serial.print("--turningSpeed=");
  //Serial.println(turningSpeed,DEC);
  
  //Lynxmotion SSC-32 already has some gaits pre-configured, and 
  // gives you API that looks like commanding a tank (by applying 
  // speed to motors connected to it's left and right tracks).
  int xl = movingSpeed;  //Left track speed
  int xr = movingSpeed;  //Right track speed
  //If you want your tank to move forward, you give both tracks 
  //the same speed. If you want to turn, slow down track on the inner 
  //side of the turn.
  if (movingSpeed < 10 && movingSpeed > -10 && turningSpeed < 10 && turningSpeed > -10) {
    if (moving) {
      Serial.println("XSTOP");
      engines.println("XSTOP");
      delay(30);
    }
    returnLegsToInitialPosition();
    moving = false;
  } else {
    moving = true;
    oldValues[0]=0;oldValues[1]=0;oldValues[2]=0;oldValues[3]=0;oldValues[4]=0;oldValues[5]=0;
    if (movingSpeed<0) {
      xr = xr + turningSpeed;
      xl = xl - turningSpeed;
    } else {
      xr = xr - turningSpeed;
      xl = xl + turningSpeed;
    }
  
    if (xl < 0) xl = max(xl, -100);
    if (xl > 0) xl = min(xl, 100);
    if (xr < 0) xr = max(xr, -100);
    if (xr > 0) xr = min(xr, 100);

    digitalWrite(led, HIGH);
    Serial.print("XL");
    engines.print("XL");
    Serial.print(xl, DEC);
    engines.print(xl, DEC);
    Serial.print(" XR");
    engines.print(" XR");
    Serial.print(xr, DEC); 
    engines.print(xr, DEC); 
    Serial.println(" XS200");
    engines.println(" XS200");
    delay(250);
  }

  digitalWrite(led, LOW);
}

void setupGaitValues() {
  int newMediumPosition = 0;
  while (newMediumPosition<900 || newMediumPosition>1900) {
    newMediumPosition = pulseIn(ch6, HIGH);
  }

  if (abs(newMediumPosition-mediumLegPosition)<50) return;
  mediumLegPosition = newMediumPosition;
  int low = mediumLegPosition-400;
  int high = mediumLegPosition+400;
  String gaitSetup = "LH";
  gaitSetup += low;
  gaitSetup += " LM";
  gaitSetup += mediumLegPosition;
  gaitSetup += " LL";
  gaitSetup += high;
  gaitSetup += " RH";
  gaitSetup += high;
  gaitSetup += " RM";
  gaitSetup += mediumLegPosition;
  gaitSetup += " RL";
  gaitSetup += low;

  //Good values should be around following: 
  //Serial.println("LH1000 LM1400 LL1800 RH1800 RM1400 RL1000 VS3000");

  gaitSetup += " VS3000";
  Serial.println(gaitSetup);
  engines.println(gaitSetup);
  delay(50);
  Serial.println("LF1700 LR1300 RF1300 RR1700 HT300");
  engines.println("LF1700 LR1300 RF1300 RR1700 HT300");
  delay(50);
}

void returnLegsToInitialPosition() {

  values[2] = pulseIn(ch3, HIGH)-1150;  //Elevation
  int diff = oldValues[2] - values[2];

  if (diff <10 && diff > -10) return;
  
  oldValues[2] = values[2];
  Serial.print("ch3=");
  Serial.print(values[2], DEC);
  Serial.print("; ch4=");
  Serial.println(values[3], DEC);
  
  digitalWrite(led, HIGH);
  Serial.print("#1P1500#3P1500#5P1500");
  engines.print("#1P1500#3P1500#5P1500");
  Serial.println("#17P1500#19P1500#21P1500");
  engines.println("#17P1500#19P1500#21P1500");

  int leftShoulder = 1490-values[2];
  int rightShoulder = 1490+values[2];
  
  short servoNums[] = {0,2,4};
  moveServos(3, servoNums, leftShoulder);

  short servoNums4[] = {16, 18, 20};
  moveServos(3, servoNums4, rightShoulder);

  int rightKnees = 1480 + (values[2]/2);
  int leftKnees  = 1480 - (values[2]/2);
  
  short servoNums1[] = {8, 9, 10};
  moveServos(3, servoNums1, leftKnees);
  
  short servoNums2[] = {24, 25, 26};
  moveServos(3, servoNums2, rightKnees);
  digitalWrite(led, LOW);
}

void moveServos(int count, short servos[], int angle) {
  for (int index = 0; index<count; index++) {
    Serial.print("#");
    engines.print("#");
    Serial.print(servos[index], DEC);
    engines.print(servos[index], DEC);
    Serial.print("P");
    engines.print("P");
    Serial.print(angle, DEC);
    engines.print(angle, DEC);
  }
  Serial.println();
  engines.println();
}
