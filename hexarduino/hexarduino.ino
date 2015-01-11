int ch1 = 4;
int ch2 = 5;
int ch3 = 6;
int ch4 = 7;
int ch5 = 8;
int ch6 = 9;

short values[] = {1500, 1500, 1500, 1500, 1500, 1500};

void setup() {
  pinMode(ch1, INPUT);
  pinMode(ch2, INPUT);
  pinMode(ch3, INPUT);
  pinMode(ch4, INPUT);
  pinMode(ch5, INPUT);
  pinMode(ch6, INPUT);
  Serial.begin(115200);
}

void loop() {
  
  values[0] = pulseIn(ch1, HIGH);
  values[1] = pulseIn(ch2, HIGH);
  values[2] = pulseIn(ch3, HIGH);
  values[3] = pulseIn(ch4, HIGH);
  values[4] = pulseIn(ch5, HIGH);
  values[5] = pulseIn(ch6, HIGH);
  
  for (int idx = 0; idx<6; idx++) {
    Serial.print("#");
    Serial.print(idx);
    Serial.print("P");
    Serial.print(values[idx], DEC);
  }
  Serial.println("T20");
  delay(30);
}
