/*
  Switch
  
*/

void initSwitch() {

  pinMode(PIN_SWITCH, INPUT);   // Initialize the Switch pin as an input
}

void enable(){
  
  int EN = digitalRead(PIN_SWITCH);
  if(EN ==HIGH && EN_ANT == LOW){
    c = c+1;   
  }
  EN_ANT = EN;
  if (c % 2 == 0){
    ESTAT = 0;
  }
  else {
    ESTAT = 1;
  }
}