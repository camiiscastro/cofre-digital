// code for digital safe box

// element pins
#define BUTTON1 12
#define BUTTON2 11
#define BUTTON3 10
#define BUTTON4 9
#define BUTTON5 8
#define RESET_BUTTON 7
#define RED_LED 6
#define GREEN_LED 5
#define BUZZER 13
#define PASSWORD_LENGTH 5

#define TIME_READING 10000

// class to deal with button debounce
class Button {
  private:
    uint8_t btn;
    uint16_t state;
  public:
    void begin(uint8_t button) {
      btn = button;
      state = 0;
      pinMode(btn, INPUT_PULLUP);
    }
    bool debounce() {
      state = (state << 1) | digitalRead(btn) | 0xfe00;
      return (state == 0xff00);
    }
};

// create instances for each button
Button btn1;
Button btn2;
Button btn3;
Button btn4;
Button btn5;

// array for safebox password
int password[PASSWORD_LENGTH] = {1, 2, 3, 4, 5};
// array for attempts
int attempt[PASSWORD_LENGTH] = {0};
// function that reads a password (5 digits)
void readAttempt(int pos);
// function that reset a read password
void resetAttempt();
// function that displays the password array
void displayAttemptArray();
// function that returns the value of the pressed button
int getPressedValue();
// function that checks the entered password
bool isValidAttempt();
// function to blink a led
void blinkLed(int pin);
// function that checks an attempt
bool checkAttempt();
// function that sets a new password to the safe box
void setNewPassword();
// function that reads the new password
void readNewPassword(int pos);
// function that resets the password array
void resetPassword();
// function that displays the new password
void displayNewPassword();
// function to beep buzzer 
void beepBuzzer(int times);

void setup() {
  btn1.begin(BUTTON1);
  btn2.begin(BUTTON2);
  btn3.begin(BUTTON3);
  btn4.begin(BUTTON4);
  btn5.begin(BUTTON5);
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  // setting up leds
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  // turning on the red led
  digitalWrite(RED_LED, HIGH);
  Serial.begin(9600);
}

void loop() {
  static bool isLocked = true;
  static int stateButton = 0;
  static int amountAttempts = 1;

  // if the safe box is open and the user clicked on reset button
  if (!isLocked && !digitalRead(RESET_BUTTON)) {
    Serial.println("\nEnter your new password:");

    setNewPassword();
    displayNewPassword();
    isLocked = true;
    stateButton = 0;
    resetAttempt();

    return;
  }

  // get pressed value
  int pressedValue = getPressedValue();
  
  // after click any button, locks the box
  if (!isLocked && pressedValue) {
    Serial.println("Safe box locked!");
    digitalWrite(GREEN_LED, LOW);
    isLocked = true;
    resetAttempt();

    return;
  }

  if (!stateButton) {
    stateButton = pressedValue;
    attempt[0] = stateButton;
  }

  // if some button has pressed
  if (stateButton) {
    // reading password and displaying it
    Serial.println("\nEnter the password:\n");
    readAttempt(1);
    stateButton = 0;
    displayAttemptArray();

    // if it's not a valid attempt
    if (!isValidAttempt()) {
      Serial.println("\nInvalid attempt! Try again");
      resetAttempt();

      return;
    }

    // if it is
    Serial.println("\nValid attempt!");
    if (checkAttempt()) {
      Serial.println("\nUnlocked!");
      digitalWrite(GREEN_LED, HIGH);
      isLocked = false;
    } else {
      Serial.println("\nWrong password, try again!");

      beepBuzzer(1);
      resetAttempt();

      if (amountAttempts > 3) {
        beepBuzzer(10);
        amountAttempts = 1;
      } else {
        amountAttempts++;
      }
    }
  }
}

int getPressedValue() {
  if (btn1.debounce()) return 1;
  if (btn2.debounce()) return 2;
  if (btn3.debounce()) return 3;
  if (btn4.debounce()) return 4;
  if (btn5.debounce()) return 5;

  return 0;
}

void readAttempt(int pos) {
  static unsigned long time = millis();

  for (;;) {
    if (pos > 5 || (millis() - time) >= TIME_READING) {
      time = millis();
      return;
    }

    if (attempt[pos] == 0) {
      attempt[pos] = getPressedValue();
    } else {
      pos++;
    }
  }
}

void resetAttempt() {
  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    attempt[i] = 0;
  }
}

void displayAttemptArray() {
  Serial.print("\nAttempt Array: ");

  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    Serial.print(attempt[i]);
  }
  Serial.println();
}

bool isValidAttempt() {
  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    if (!attempt[i]) return false;
  }

  return true;
}

void blinkLed(int pin) {
  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
  }
}

bool checkAttempt() {
  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    if (!(password[i] == attempt[i])){
      return false;
    }
  }

  return true;
}

void resetPassword() {
  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    password[i] = 0;
  }
}

void readNewPassword(int pos) {
  for (;;) {
    if (pos > 5) {
      return;
    }

    if (password[pos] == 0) {
      password[pos] = getPressedValue();
    } else {
      pos++;
    }
  }
}

void setNewPassword() {
  resetPassword();
  readNewPassword(0);
  digitalWrite(GREEN_LED, LOW);
}

void displayNewPassword() {
  Serial.print("\nYour New Password is: ");
  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    Serial.print(password[i]);
  }
  Serial.println();
}

void beepBuzzer(int times) {
  for (int i = 0; i < times; i++) {
    tone(BUZZER, 1500);
    delay(500);
  }
  noTone(BUZZER);
}

