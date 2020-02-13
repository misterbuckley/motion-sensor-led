// https://collectiveidea.com/blog/archives/2017/04/05/arduino-programming-in-vim

// the minimum distance you must place your hand to sense input in cm
// any closer than this sets the value to LOW
const float MINIMUM_DISTANCE_FROM_SENSOR = 5.0;

// the range over which the distance matters
// with a range of 10.0, each centimeter = 10% brightness
// with a range of 20.0, each centimeter = 5% brightness
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

  if (currentState == LED_OFF) {
    if (getDistance() < 10) {
      turnLedOn();
    }

  } else if (currentState == LED_ON) {
    if (currentTime - timeLedWasTurnedOn < INTERVAL_LED_SHOULD_STAY_LIT_FOR) return;

    turnLedOff();

  } else if (currentState == GET_R) {
    float newRvalue = askForColorInput();

    setLED(newRvalue, LOW, LOW);

    ledRvalue = newRvalue;

  } else if (currentState == GET_G) {
    float newGvalue = askForColorInput();

    setLED(LOW, newGvalue, LOW);

    ledGvalue = newGvalue;

  } else if (currentState == GET_B) {
    float newBvalue = askForColorInput();

    setLED(LOW, LOW, newBvalue);

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
  if (r == LOW || r == HIGH) {
    digitalWrite(ledRpin, r);
  } else {
    analogWrite(ledRpin, r);
  }

  if (g == LOW || g == HIGH) {
    digitalWrite(ledGpin, g);
  } else {
    analogWrite(ledGpin, g);
  }

  if (b == LOW || b == HIGH) {
    digitalWrite(ledBpin, b);
  } else {
    analogWrite(ledBpin, b);
  }
}

void turnLedOn () {
  setLED(ledRvalue, ledGvalue, ledBvalue);

  timeLedWasTurnedOn = millis();

  currentState = LED_ON;
}

void turnLedOff () {
  setLED(LOW, LOW, LOW);

  currentState = LED_OFF;
}

float askForColorInput () {
  //  5.0 = minimum distance from sensor
  //  20.0 = range of distance that affects sensor

  float inputValue = (float)((float)((getDistance() - 5.0) / 20.0) * 256.0) - 1.0;

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
