int ch1 = 9;
int ch2 = 8;
int ch3 = 7;
int ch4 = 6;
int ch5 = 5;
int ch6 = 4;
int led = 13;

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

  Serial.println("LH1000 LM1200 LL1800 RH1800 RM1200 RL1000 VS3000");
  delay(50);
  Serial.println("LF1700 LR1300 RF1300 RR1700 HT300");
  delay(50);
}

void loop() {
  //Get readings from RC-receiver channels
  values[0] = pulseIn(ch1, HIGH)-1300;
  values[1] = pulseIn(ch2, HIGH)-1300;
  values[2] = pulseIn(ch3, HIGH)-1300;
  values[3] = pulseIn(ch4, HIGH)-1300;
  values[4] = pulseIn(ch5, HIGH)-1300;
  values[5] = pulseIn(ch6, HIGH)-1300;
  
  //Moving speed is read from channel 1 (right stick, top-bottom direction)
  int movingSpeed = int(values[1] / 3.20);  //Normalize the value
  if (movingSpeed <10 && movingSpeed > -10) movingSpeed = 0;  //And remove noise

  //Turning speed is read from channel 0 (right stick, left-right direction)
  int turningSpeed = int(values[0] / 3.20);  //Normalize
  if (turningSpeed <10 && turningSpeed > -10) turningSpeed = 0;  //Remove noise
  
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
    
  delay(100);
  digitalWrite(led, LOW);
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
  Serial.println("#0P1300#2P1300#4P1300");
  delay(30);
  Serial.println("#16P1700#18P1700#20P1700T500");
  delay(2000);
  digitalWrite(led, LOW);
}
