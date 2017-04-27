bool upperLevel[40][80];
bool lowerLevel[80][40];
bool rampLevel[12][40];

void setup() {
  // put your setup code here, to run once:
  for(int i = 0; i < 40; i++){
    for(int j = 0; j < 80; j++){
      upperLevel[i][j] = 0;
    }
  }
  for(int i = 0; i < 80; i++){
    for(int j = 0; j < 40; j++){
      lowerLevel[i][j] = 0;
    }
  } 
  for(int i = 0; i < 12; i++){
    for(int j = 0; j < 40; j++){
      rampLevel[i][j] = 0;
    }
  } 
}

void loop() {
  // put your main code here, to run repeatedly:

}
