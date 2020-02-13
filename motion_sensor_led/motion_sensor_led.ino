// https://collectiveidea.com/blog/archives/2017/04/05/arduino-programming-in-vim

// if the sensor detects something this close or closer, LED will turn on
const float ACTIVATION_DISTANCE = 10.0;

// the minimum distance you must place your hand when getting input
// any closer than this sets the value to 0
const float MINIMUM_INPUT_DISTANCE = 5.0;

// the range over which the distance matters when taking input
// with a range of 20.0, each centimeter = 5% brightness
// if your hand >= MINIMUM_INPUT_DISTANCE + SENSOR_RANGE cm away from the sensor, brightness will be 100%
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
unsigned long timeLEDWasTurnedOn = 0;

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

  switch (currentState) {
    case LED_OFF:
      if (getDistance() < ACTIVATION_DISTANCE) {
        turnLEDOn();
      }

      break;

    case LED_ON:
      if (currentTime - timeLEDWasTurnedOn >= INTERVAL_LED_SHOULD_STAY_LIT_FOR) {
        turnLEDOff();
      }

      break;

    case GET_R:
      float newRvalue = askForColorInput();

      setLED(newRvalue, 0, 0);

      ledRvalue = newRvalue;

      break;

    case GET_G:
      float newGvalue = askForColorInput();

      setLED(0, newGvalue, 0);

      ledGvalue = newGvalue;

      break;

    case GET_B:
      float newBvalue = askForColorInput();

      setLED(0, 0, newBvalue);

      ledBvalue = newBvalue;

      break;
  }

  delay(programInterval);
}

float getDistance () {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration / 58.2; // distance in cm

  return distance;
}

void setLED (float r, float g, float b) {
  analogWrite(ledRpin, r);
  analogWrite(ledGpin, g);
  analogWrite(ledBpin, b);
}

void turnLEDOn () {
  setLED(ledRvalue, ledGvalue, ledBvalue);

  timeLEDWasTurnedOn = millis();

  currentState = LED_ON;
}

void turnLEDOff () {
  setLED(0, 0, 0);

  currentState = LED_OFF;
}

float askForColorInput () {
  // TODO figure out if i can get rid of this (float) (float) shit
  float inputValue = (float)((float)((getDistance() - MINIMUM_INPUT_DISTANCE) / SENSOR_RANGE) * 256.0) - 1.0;

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
