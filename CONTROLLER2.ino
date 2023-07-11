// Motor driver pin configuration
const int motorPinAHigh = 2;
const int motorPinALow = 3;
const int motorPinBHigh = 4;
const int motorPinBLow = 5;
const int motorPinCHigh = 6;
const int motorPinCLow = 7;
const int direct = 13;

// Analog input pin for voltage control
const int analogPin = A0;

// Variables for PWM frequency and duty cycle
const int pwmFrequency = 10000;  // 10 kHz
int dutyCycle = 0;

// Commutation states
 //Forward mode
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

 //Reverse mode 
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

// Function declarations
void updateCommutationState();
void controlMotor();

void setup() {
  // Set motor driver pins as outputs
  pinMode(motorPinAHigh, OUTPUT);
  pinMode(motorPinALow, OUTPUT);
  pinMode(motorPinBHigh, OUTPUT);
  pinMode(motorPinBLow, OUTPUT);
  pinMode(motorPinCHigh, OUTPUT);
  pinMode(motorPinCLow, OUTPUT);

  // Set PWM frequency
  TCCR1B = TCCR1B & B11111000 | B00000001;  // Set timer 1 prescaler to 1 (Arduino Mega)

  // Start timer 1 in fast PWM mode with TOP value 255
  TCCR1A = (1 << WGM10) | (1 << WGM11);  // Fast PWM, TOP = 0xFF
  TCCR1B = (1 << WGM12) | (1 << WGM13);  // Fast PWM, TOP = 0xFF

  // Attach interrupt for Hall sensor inputs
  attachInterrupt(digitalPinToInterrupt(18), updateCommutationState, CHANGE);
  attachInterrupt(digitalPinToInterrupt(19), updateCommutationState, CHANGE);
  attachInterrupt(digitalPinToInterrupt(20), updateCommutationState, CHANGE);
}

void loop() {
 
  // Read analog input and scale to match duty cycle range
 dutyCycle = map(analogRead(analogPin), 0, 1024, 0, 255);
 //dutyCycle = map(dutyCycle * dutyCycle, 0, 65536, 10, 255);

  
  // Control the motor
  controlMotor();
  
  // Delay between iterations
  delayMicroseconds(50);
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
  
}

// Control the motor based on the commutation state and duty cycle
void controlMotor() {
  int FR =digitalRead(direct);
  const int (*commutationStates)[6];
  if(FR){
    commutationStates=commutationStateReverse;
  }
  else{
    commutationStates=commutationStateForward;
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
