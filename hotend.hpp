/*
  HotEnd
  PID Basic simulated heater Example
  Reading analog input 0 to control analog PWM output 3
  
*/

//#include <PID_v1.h> // https://github.com/br3ttb/Arduino-PID-Library
#include <PID_v1_bc.h> // https://github.com/drf5n/Arduino-PID-Library

double temperatureKelvin;           // Température Courante en Kelvin
double temperatureCelsius;          // Température Courante en Celsius
double temperature;                 // Température Courante

bool F = false;
bool Fc = false;
bool Fi = false;

//double Setpoint ;                   // will be the desired value
double Input;                       //
double Output ;                     // LED
//PID parameters
double agrKp=6,agrKi=0,agrKd=0;     // PID 
double conKp=12,conKi=0.3,conKd=0;  // PID 
//double Max = 240;                   // Température Max

//PID myPID(&Input, &Output, &Setpoint, conKp, conKi, conKd, DIRECT);
PID myPID(&temperature, &Output, &To, Kp, Ki, Kd, DIRECT);

double tempLastSample;              //
double tempLastFilament;            //
double tempLastNoFilament;          //
double tempLastStart;               //

double filamentSession = 0;         // Longueur de filament de la session
double durationSession = 0;         // Durée de la session

//thermistor
float logR2, R2;
//float R1 = 1000;                    // resistencia fija del divisor de tension 
//steinhart-hart coeficients for thermistor
// coeficientes de steinhart-Hart en pagina: 
// http://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm
float c1 = 0.8438162826e-03, c2 = 2.059601750e-04, c3 = 0.8615484887e-07;

double Thermistor(float Volts) {

  //Serial.printf("Volts : %d\n", Volts);
  R2 = R1 * (1023.0 / (float)Volts - 1.0);                                      //calculate resistance on thermistor

  logR2 = log(R2);
  temperatureKelvin = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));   // temperature in Kelvin
  temperatureCelsius = temperatureKelvin - 273.15;                              //convert Kelvin to Celsius
  temperature = temperatureCelsius;
  return temperature;
}

void start(){
  if (tempLastStart==0) {
      filamentSession = 0;
      durationSession = 0;
      status = "working";
      tempLastStart = millis();
      if (tempLastStart==0) tempLastStart = 1;
    }
}

void stop(){
    
  status = "stopped";
  tempLastStart = 0;
  //ifttt();
}

void initHotEnd() {

  //Setpoint = 220;
  //myPID.SetMode(AUTOMATIC);
  myPID.SetTunings(Kp, Ki, Kd);
  myPID.SetOutputLimits(PIN_HEATER, Max);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_FILAMENT_STOP, INPUT);
  if (status=="") start();

}

void hotendReadTempTask() {
  if (status == "stopped" && myPID.GetMode() == AUTOMATIC) {
    myPID.SetMode(MANUAL);
    Output = 0;
  }
  if (status == "working" && myPID.GetMode() != AUTOMATIC) {
    myPID.SetMode(AUTOMATIC);
  }
  if (millis() >= tempLastSample + 100) {
    Thermistor(analogRead(PIN_THERMISTOR)); //Volt to temp, update T
    //Serial.printf("Temperature : %d\n", temperatureCelsius);
    if (temperature > temperatureMax || isnan(temperature)) {
      Output = 0;
    } else {
      myPID.Compute();
    }
    if (status == "working") {
      start();
      if (temperature > 150 || temperature > To + 20 ) {
        digitalWrite(LED_BUILTIN , LOW);// target temperature ready
      } else {
        digitalWrite(LED_BUILTIN , !digitalRead(LED_BUILTIN));//reaching tarjet temp
      }
    } else {
        digitalWrite(LED_BUILTIN , HIGH);
    }

    analogWrite(PIN_HEATER, Output);
    
    Fc = digitalRead(PIN_FILAMENT_STOP);
    
    if (Fc && !F) {
      tempLastFilament = millis();
      start();
    }
    
    if (!Fc && F) {
      tempLastFilament = 0;
      tempLastNoFilament = millis();
    }

    F = Fc;
    if (filamentSensorEnable) {
      if (Fc && tempLastFilament > 0 && millis() >= tempLastFilament + 3*1000){
        Fi = true;
      }
      
      if (!Fc && Fi && tempLastNoFilament > 0 && millis() >= tempLastNoFilament + 500) { // no filament
        stop();
        tempLastNoFilament = 0;
        Fi = false;
      }
      
      if (!Fc && !Fi && tempLastStart > 0 && millis() >= tempLastStart + 5*60*1000) { // no filament for 5 min
        stop();
      }
    }

    tempLastSample = millis();
    
  }
}