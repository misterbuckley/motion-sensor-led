// https://collectiveidea.com/blog/archives/2017/04/05/arduino-programming-in-vim

// the minimum distance you must place your hand to sense input in cm
// any closer than this sets the value to 0
const float MINIMUM_DISTANCE_FROM_SENSOR = 5.0;

// the range over which the distance matters
// with a range of 10.0, each centimeter = 10% brightness
// with a range of 20.0, each centimeter = 5% brightness
// TODO maybe change this to a MAXIMUM_DISTANCE_FROM_SENSOR instead
const float SENSOR_RANGE = 20.0;

const unsigned long INTERVAL_LED_SHOULD_STAY_LIT_FOR = 2 * 1000; // 2 seconds

// TODO:
// - hold button for a second to turn off led and then ignore the distance sensor for a few seconds to let user move hand away
// - save color to chip for if the thing loses power

// TODO create header files and organize the declarations here a bit better
int trigPin = 7;
int echoPin = 8;

int btnPin = 2;
boolean btnCurrentState = HIGH;
boolean btnPreviousState = HIGH;

int ledRpin = 9;
int ledGpin = 10;
int ledBpin = 11;
float ledRvalue = 255;
float ledGvalue = 255;
float ledBvalue = 255;
unsigned long timeLedWasTurnedOn = 0;

const long programInterval = 100;
unsigned long currentTime;

enum State {
  LED_OFF,
  LED_ON,
  GET_R,
  GET_G,
  GET_B
};

State currentState = LED_OFF;

void setup () {
  pinMode(ledRpin, OUTPUT);
  pinMode(ledGpin, OUTPUT);
  pinMode(ledBpin, OUTPUT);

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  pinMode(btnPin, INPUT_PULLUP);
}

void loop () {
  currentTime = millis();

  listenForButtonPress();

  // TODO maybe move this to a handleCurrentState() function
  if (currentState == LED_OFF) {
    if (getDistance() < 10) {
      turnLedOn();
    }

  } else if (currentState == LED_ON) {
    if (currentTime - timeLedWasTurnedOn < INTERVAL_LED_SHOULD_STAY_LIT_FOR) return;

    turnLedOff();

  } else if (currentState == GET_R) {
    float newRvalue = askForColorInput();

    setLED(newRvalue, 0, 0);

    ledRvalue = newRvalue;

  } else if (currentState == GET_G) {
    float newGvalue = askForColorInput();

    setLED(0, newGvalue, 0);

    ledGvalue = newGvalue;

  } else if (currentState == GET_B) {
    float newBvalue = askForColorInput();

    setLED(0, 0, newBvalue);

    ledBvalue = newBvalue;
  }

  delay(programInterval);
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

void setLED (float r, float g, float b) {
  analogWrite(ledRpin, r);
  analogWrite(ledGpin, g);
  analogWrite(ledBpin, b);
}

void turnLedOn () {
  setLED(ledRvalue, ledGvalue, ledBvalue);

  timeLedWasTurnedOn = millis();

  currentState = LED_ON;
}

void turnLedOff () {
  setLED(0, 0, 0);

  currentState = LED_OFF;
}

float askForColorInput () {
  // TODO figure out if i can get rid of this (float) (float) shit
  // TODO also maybe break this up a bit to make the math clearer ?
  float inputValue = (float)((float)((getDistance() - MINIMUM_DISTANCE_FROM_SENSOR) / SENSOR_RANGE) * 256.0) - 1.0;

  if (inputValue > 255) inputValue = 255;
  if (inputValue < 0) inputValue = 0;

  return inputValue;
}

void listenForButtonPress () {
  btnPreviousState = btnCurrentState;
  btnCurrentState = digitalRead(btnPin);

  if (btnPreviousState == LOW && btnCurrentState == HIGH) {
    switchState();
  }
}

void switchState () {
  switch (currentState) {
    case LED_OFF:
    case LED_ON:
      currentState = GET_R;
      break;
    case GET_R:
      currentState = GET_G;
      break;
    case GET_G:
      currentState = GET_B;
      break;
    case GET_B:
      currentState = LED_OFF;
      break;
  }
}
