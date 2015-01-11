int ch1 = 4;
int ch2 = 5;
int ch3 = 6;
int ch4 = 7;
int ch5 = 8;
int ch6 = 9;

boolean moving = false;

short values[] = {1500, 1500, 1500, 1500, 1500, 1500};

void setup() {
  pinMode(ch1, INPUT);
  pinMode(ch2, INPUT);
  pinMode(ch3, INPUT);
  pinMode(ch4, INPUT);
  pinMode(ch5, INPUT);
  pinMode(ch6, INPUT);
  Serial.begin(115200);
  
  Serial.println("LH1000 LM1400 LL1800 RH1800 RM1400 RL1000 VS3000");
  delay(50);
  Serial.println("LF1700 LR1300 RF1300 RR1700 HT1500");
  delay(50);
  Serial.println("#8P1500#9P1500#10P1500#24P1500#25P1500#26P1500T2000");
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

    Serial.print("XL");
    Serial.print(xl, DEC); 
    Serial.print(" XR");
    Serial.print(xr, DEC); 
    Serial.println(" XS100");
  }
    
  delay(100);
}
