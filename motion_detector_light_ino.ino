unsigned long previousMillis = 0;
const long detectMotionInterval = 100;
const long programInterval = 100;

// TODO:
// - replace currentState with enum

boolean ledIsOn = false;
int lightDuration = 3 * 60 * 1000; // 3 minutes

int trigPin = 7;
int echoPin = 8;

int btnPin = 2;
int currentState = 0;

// TODO: save these values to the chip for if the thing loses power
int ledRpin = 9;
float ledRvalue = 255;
int ledGpin = 10;
float ledGvalue = 255;
int ledBpin = 11;
float ledBvalue = 255;

void setup () {
  pinMode(ledRpin, OUTPUT);
  pinMode(ledGpin, OUTPUT);
  pinMode(ledBpin, OUTPUT);

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  
  pinMode(btnPin, INPUT_PULLUP);
  
  previousMillis = millis();
  Serial.begin(9600);
}

boolean btnCurrentState = HIGH;
boolean btnPreviousState = HIGH;

void loop () {
  
  // only run loop() once every programInterval milliseconds
  long currentMillis = millis();
  if (currentMillis - previousMillis < programInterval) return;
  previousMillis = currentMillis;
  
  // 1. If button is pressed, change state

  btnPreviousState = btnCurrentState;
  btnCurrentState = digitalRead(btnPin);
  
  if (btnPreviousState == LOW && btnCurrentState == HIGH) {
    currentState += 1;
    
    if (currentState == 1) { // 1 is the output state, so we skip it when getting input
      currentState += 1;
    }
    
    if (currentState > 4) {
      currentState = 0;
    }
  }
    
//    currentState += 1;
//    
//    if (currentState > 4) {
//      currentState = 0;
//    }
    
    //Serial.println(o + currentState);
  
  
  // TODO: after getting r g  and b values, 
  if (currentState == 0) {
    
      digitalWrite(ledRpin, LOW);
      digitalWrite(ledGpin, LOW);
      digitalWrite(ledBpin, LOW);
    
      // check the distance sensor, if there is movement, then turn on LED
      if (getDistance() < 10) {
        currentState = 1;
      }
      
  } else if (currentState == 1) {
      // light is on
      // TODO only turn it on for a certain time
      setLED();
      
  } else if (currentState == 2) {
      // get R value while showing it
      
      // in this next line
      //  5.0 = minimum distance from sensor
      //  20.0 = range of distance that affects sensor
      float newRvalue = (float)((float)((getDistance() - 5.0) / 20.0) * 256.0) - 1.0;
      
      if (newRvalue > 255) newRvalue = 255;
      if (newRvalue < 0) newRvalue = 0;
      
      ledRvalue = newRvalue;
      
      analogWrite(ledRpin, newRvalue);
      digitalWrite(ledGpin, LOW);
      digitalWrite(ledBpin, LOW);
      
  } else if (currentState == 3) {
      // get G value while showing it
      
      // in this next line
      //  5.0 = minimum distance from sensor
      //  20.0 = range of distance that affects sensor
      float newGvalue = (float)((float)((getDistance() - 5.0) / 20.0) * 256.0) - 1.0;
      
      if (newGvalue > 255) newGvalue = 255;
      if (newGvalue < 0) newGvalue = 0;
      
      ledGvalue = newGvalue;
      
      digitalWrite(ledRpin, LOW);
      analogWrite(ledGpin, newGvalue);
      digitalWrite(ledBpin, LOW);
      
  } else if (currentState == 4) {
      // get B value while showing it

      // in this next line
      //  5.0 = minimum distance from sensor
      //  20.0 = range of distance that affects sensor
      float newBvalue = (float)((float)((getDistance() - 5.0) / 20.0) * 256.0) - 1.0;
      
      if (newBvalue > 255) newBvalue = 255;
      if (newBvalue < 0) newBvalue = 0;      
      
      ledBvalue = newBvalue;
      
      digitalWrite(ledRpin, LOW);
      digitalWrite(ledGpin, LOW);
      analogWrite(ledBpin, newBvalue);
  }
}

void setLED () {
  analogWrite(ledRpin, ledRvalue);
  analogWrite(ledGpin, ledGvalue);
  analogWrite(ledBpin, ledBvalue);
}

long getDistance () {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration / 58.2; // distance in cm
  
  return distance;
}
