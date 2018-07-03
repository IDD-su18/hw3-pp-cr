
/*
   Arduino2Max
   Send pin values from Arduino to MAX/MSP

   Arduino2Max.pde
   ------------
   This version: .5, November 29, 2010
   ------------
   Copyleft: use as you like
   by Daniel Jolliffe
   Based on a sketch and patch by Thomas Ouellet Fredericks  tof.danslchamp.org

*/

#define NUM_INPUTS 5
#define DEBOUNCE_TIME 5
int x = 0;
int pinkyButton = 2;
int chordValue = 0;

boolean buttons[5];     // Pinky is [0] and far thumb is [6]
boolean latchingButtons[5];
boolean acquiringPresses = LOW;
boolean calculateKey = LOW;
boolean ledON;

int keyValue;

void setup() {
  // 115200 is the default Arduino Bluetooth speed
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(8, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
}


void loop() {
  acquiringPresses = checkButtonArray();
  if (acquiringPresses) {
    delay(DEBOUNCE_TIME);                           // Instead of a true software debounce this will wait a moment until the first button press has settled.
    typingChord();                      // Wait and see which keys are touched. When they are all released print the correct letter.
    sendKeyPress();                     // Using the buttons pressed during the typingChord function determine how to handle the chord.
    delay(DEBOUNCE_TIME);                           // The other half of the software "debounce"
    for (int i = 0; i < NUM_INPUTS ; i++) {   // Once a keypress has been sent the booleans should be reset.
      latchingButtons[i] = LOW;
    }
    chordValue = 0;
  }

  digitalWrite(7, ledON);

  
  if (Serial.available() > 0) {        // Check serial buffer for characters
    if (Serial.read() == 'r') {       // If an 'r' is received then read the pins
      for (int pin = 0; pin <= 5; pin++) {    // Read and send analog pins 0-5
        x = analogRead(pin);
        if (pin==5) {
          sendValue(keyValue);
        }
        else if (pin==1) {
          sendValue(digitalRead(6));
        }
        else {
          sendValue(x);  
        }
      }
      for (int pin = 2; pin <= 13; pin++) { // Read and send digital pins 2-13
        x = digitalRead(pin);
        sendValue(x);
      }
      Serial.println();                 // Send a carriage returnt to mark end of pin data.
      delay (5);                        // add a delay to prevent crashing/overloading of the serial port

    }
  }
}

void sendValue (int x) {             // function to send the pin value followed by a "space".
  Serial.print(x);
  Serial.write(32);
}

boolean checkButtonArray() {
  // Update the buttons[] array with each scan. Set the acquiringPresses bit HIGH if any switch is pressed.
  for (int i = 0; i < NUM_INPUTS; i++) {
    // idk what this is doing
    boolean buttonState = !digitalRead(pinkyButton + i);
    if (buttonState) {
      buttons[i] = HIGH;
    } else {
      buttons[i] = LOW;
    }
  }
  for (int i = 0; i < NUM_INPUTS; i++) {
    if (buttons[i]) {
      return HIGH;
    }
  }
  return LOW;
}

void typingChord() {
  while (acquiringPresses) {
    for (int i = 0; i < NUM_INPUTS; i++) {
      if (buttons[i] == HIGH) {
        latchingButtons[i] = HIGH;
      }
    }
    acquiringPresses = checkButtonArray();
  }
}

int customPower(int functionBase, int functionExponent) {
  int functionResult = 1;
  for (int i = 0; i < functionExponent; i++) {
    functionResult = functionResult * functionBase;
  }

  return functionResult;
}


int findLetter(int chordValue) {
  //Serial.println(chordValue);
  switch (chordValue) {
    case 1:
      return 'a';
    case 2: 
      return 'b';
    case 4:
      return 'c';
    case 8:
      return 'd';
    case 3:
      return 'e';
    case 5:
      return 'f';
    case 9:
      return 'g';
    case 16:
      return 'h';
    case 10:
      return 'i';
    case 15:
      ledON = !ledON;
      return 'i';
    default:
      return '\0';
      break;
  }
}

void sendKeyPress() {
  for (int i = 0; i < NUM_INPUTS; i++) {
    if (latchingButtons[i] == HIGH) {
      chordValue = chordValue + customPower(2, i);

    }
  }
  keyValue = findLetter(chordValue);
  
}
