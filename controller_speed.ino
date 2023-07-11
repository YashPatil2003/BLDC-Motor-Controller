const int motorPinAHigh = 2;
const int motorPinALow = 3;
const int motorPinBHigh = 4;
const int motorPinBLow = 5;
const int motorPinCHigh = 6;
const int motorPinCLow = 7;
const int direct = 13;

const int hallPin1 = 18;  // Hall effect sensor pin 1
const int hallPin2 = 19;  // Hall effect sensor pin 2
const int hallPin3 = 20;  // Hall effect sensor pin 3

volatile int pulseCount = 0;
unsigned long startTime = 0;
unsigned long elapsedTime = 0;

const int pulsesPerRevolution = 24;  // Number of pulses per revolution of the motor

// Variables for PWM frequency and duty cycle
const int pwmFrequency = 10000;  // 21 kHz
int dutyCycle = 0;

// Commutation states
// Forward mode
const int commutationStateForward[8][6] = {
  {0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 1},
  {1, 0, 0, 1, 0, 0},
  {1, 0, 0, 0, 0, 1},
  {0, 1, 0, 0, 1, 0},
  {0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0},
  {0, 0, 0, 0, 0, 0}
};

// Reverse mode 
const int commutationStateReverse[8][6] = {
  {0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 0},
  {0, 1, 1, 0, 0, 0},
  {0, 1, 0, 0, 1, 0},
  {1, 0, 0, 0, 0, 1},
  {1, 0, 0, 1, 0, 0},
  {0, 0, 1, 0, 0, 1},
  {0, 0, 0, 0, 0, 0},
};

// Variables for commutation state and hall sensor states
int commutationStateIndex = 0;
int hallState1 = 0;
int hallState2 = 0;
int hallState3 = 0;

void updateCommutationState();
void controlMotor();

void setup() {
  pinMode(motorPinAHigh, OUTPUT);
  pinMode(motorPinALow, OUTPUT);
  pinMode(motorPinBHigh, OUTPUT);
  pinMode(motorPinBLow, OUTPUT);
  pinMode(motorPinCHigh, OUTPUT);
  pinMode(motorPinCLow, OUTPUT);

  TCCR1B = TCCR1B & B11111000 | B00000001;  // Set timer 1 prescaler to 1

  TCCR1A = (1 << WGM10) | (1 << WGM11);  // Fast PWM, TOP = 0xFF
  TCCR1B = (1 << WGM12) | (1 << WGM13);  // Fast PWM, TOP = 0xFF

  pinMode(hallPin1, INPUT_PULLUP);
  pinMode(hallPin2, INPUT_PULLUP);
  pinMode(hallPin3, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(hallPin1), updateCommutationState, CHANGE);
  attachInterrupt(digitalPinToInterrupt(hallPin2), updateCommutationState, CHANGE);
  attachInterrupt(digitalPinToInterrupt(hallPin3), updateCommutationState, CHANGE);

  startTime = millis();
  Serial.begin(9600);
}

void loop() {
  dutyCycle = map(analogRead(A0), 0, 1024, 0, 255);
  controlMotor();

  elapsedTime = millis() - startTime;

  if (elapsedTime >= 500) {
    float speed = (pulseCount * 60.0) / (pulsesPerRevolution * (elapsedTime / 1000.0));

    Serial.print("Speed: ");
    Serial.print(speed);
    Serial.println(" RPM");

    pulseCount = 0;
    startTime = millis();
  }
    // Delay between iterations
  delayMicroseconds(100);
}


// Update the commutation state based on the Hall sensor inputs
void updateCommutationState() {
  hallState1 = digitalRead(18);
  hallState2 = digitalRead(19);
  hallState3 = digitalRead(20);

  int hallPosition = (hallState1 << 2) | (hallState2 << 1) | hallState3;

  for (int i = 0; i < 8; i++) {
    if (hallPosition == i) {
      commutationStateIndex = i;
      break;
    }
  }

  pulseCount++; // Increment pulse count
}


void controlMotor() {
  int FR = digitalRead(direct);
  const int (*commutationStates)[6];

  if (FR) {
    commutationStates = commutationStateReverse;
  } else {
    commutationStates = commutationStateForward;
  }

  int commutationState[6];
  for (int i = 0; i < 6; i++) {
    commutationState[i] = commutationStates[commutationStateIndex][i];
  }

  analogWrite(motorPinALow, dutyCycle * commutationState[0]);
  digitalWrite(motorPinAHigh, commutationState[1]);
  delayMicroseconds(1);

  analogWrite(motorPinBLow, dutyCycle * commutationState[2]);
  digitalWrite(motorPinBHigh, commutationState[3]);
  delayMicroseconds(1);

  analogWrite(motorPinCLow, dutyCycle * commutationState[4]);
  digitalWrite(motorPinCHigh, commutationState[5]);
  delayMicroseconds(1);
}
