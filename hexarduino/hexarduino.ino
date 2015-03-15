int ch1 = 9;
int ch2 = 8;
int ch3 = 7;
int ch4 = 6;
int ch5 = 5;
int ch6 = 4;
int led = 13;
int mediumLegPosition = 0;
boolean moving = false;

short values[] = {1500, 1500, 1500, 1500, 1500, 1500};

void setup() {
  pinMode(led, OUTPUT);

  pinMode(ch1, INPUT);
  pinMode(ch2, INPUT);
  pinMode(ch3, INPUT);
  pinMode(ch4, INPUT);
  pinMode(ch5, INPUT);
  pinMode(ch6, INPUT);
  Serial.begin(115200);
  
  returnLegsToInitialPosition();

  setupGaitValues();
}

void loop() {
  //Get readings from RC-receiver channels
  values[0] = pulseIn(ch1, HIGH)-1500;
  values[1] = pulseIn(ch2, HIGH)-1500;
  values[2] = pulseIn(ch3, HIGH)-1500;
  values[3] = pulseIn(ch4, HIGH)-1500;
  values[4] = pulseIn(ch5, HIGH)-1500;
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
      delay(30);
      returnLegsToInitialPosition();
    } else {
      setupGaitValues();
      delay(30);
    }
    moving = false;
  } else {
    moving = true;
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
    Serial.print(xl, DEC);
    Serial.print(" XR");
    Serial.print(xr, DEC); 
    Serial.println(" XS200");
  }
    
  delay(250);
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
  
  Serial.println("LH1000 LM1400 LL1800 RH1800 RM1400 RL1000 VS3000");

  gaitSetup += " VS3000";
  Serial.println(gaitSetup);
  delay(50);
  Serial.println("LF1700 LR1300 RF1300 RR1700 HT300");
  delay(50);
}

void returnLegsToInitialPosition() {
  digitalWrite(led, HIGH);
  Serial.print("#8P1300#9P1300#10P1300");
  delay(30);
  Serial.print("#24P1700#25P1700#26P1700");  
  delay(30);
  Serial.print("#1P1500#3P1500#5P1500");
  delay(30);
  Serial.print("#17P1500#19P1500#21P1500");
  delay(30);
  Serial.print("#0P1300#2P1300#4P1300");
  delay(30);
  Serial.println("#16P1700#18P1700#20P1700T500");
  delay(600);
  digitalWrite(led, LOW);
}
