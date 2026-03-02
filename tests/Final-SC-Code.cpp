#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <math.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;

byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {7, 6, 5, 4};

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


char num1[16] = "";
char num2[16] = "";

byte idx1 = 0;
byte idx2 = 0;

char op = 0;

bool factorialMode = false;
bool resultDisplayed = false;
bool firstAPressed = false;

enum Mode { NORMAL, POWER, PERCENT, SQRT };
Mode currentMode = NORMAL;



void clearFirstRow() {
  lcd.setCursor(0, 0);
  for (int i = 0; i < 16; i++) {
    lcd.print(" ");
  }
  lcd.setCursor(0, 0);
}



void startupScreen() {
  lcd.clear();
  lcd.print("Smart Calculator");
  delay(1200);
  lcd.clear();
}




void showMode(const char msg[]) {
  clearFirstRow();
  lcd.print(msg);
  delay(800);
  lcd.clear();
}



void resetAll() {
  
  for (int i = 0; i < 16; i++) {
    num1[i] = '\0';
    num2[i] = '\0';
  }

  idx1 = 0;
  idx2 = 0;
  op = 0;
  factorialMode = false;
  resultDisplayed = false;
  currentMode = NORMAL;

  lcd.clear();   
}



void showReset() {
  lcd.clear();
  lcd.print("Reset");
  delay(900);
  resetAll();    
}


void showError(const char msg[]) {
  lcd.clear();
  lcd.print(msg);
  delay(1500);
  resetAll();
}

void printResult(double result) {
  char buffer[16];
  if (result >= 0 && result == floor(result) && result <= 4294967295.0) 
  {
    ultoa((unsigned long)result, buffer, 10);
  } else {
    result = result + 0.000001;
    result = round(result * 10000.0) / 10000.0;
    dtostrf(result, 0, 4, buffer);
  }

  if (strlen(buffer) > 14) {
    showError("Overflow");
    return;
  }

  lcd.clear();
  lcd.print("Result:");
  lcd.setCursor(0, 1);
  lcd.print(buffer);

  resultDisplayed = true;
}

unsigned long factorial(unsigned long n) {
  unsigned long f =1;
  for (int i = 1; i <=n; i++) {
        f = f * i;
    }
  
  return f;
}

bool checkReset(char key) {
  if (key != 'A') {
    firstAPressed = false;
    return false;
  }

  if (!firstAPressed) {
    firstAPressed = true;
    return false;
  }

  showReset();
  firstAPressed = false;
  return true;
}

void setup() {
  lcd.init();
  lcd.backlight();
  startupScreen();
}

void loop() {
  char key = keypad.getKey();

  if (!key) return;

  
  if (checkReset(key)) return;

  
  if (resultDisplayed) {
    return; 
  }

  if (key >= '0' && key <= '9') {
    if (op == 0) {
      if (idx1 < 14) {
        num1[idx1++] = key;
        num1[idx1] = '\0';
        lcd.print(key);
      }
    } else {
      if (idx2 < 14) {
        num2[idx2++] = key;
        num2[idx2] = '\0';
        lcd.print(key);
      }
    }
    return;
  }

  if (key == 'A') {
    if (currentMode == NORMAL) {
      op = 'A';
      lcd.print("+");
    }
    return;
  }

  if (key == 'B') {
    if (currentMode == NORMAL) {
      op = 'B';
      lcd.print("-");
    }
    return;
  }

  if (key == 'C' && op == 'C') {
    if (currentMode != NORMAL) return; 
    resetAll();
    currentMode = POWER;
    showMode("Power Mode");
    return;
  }

  if (key == 'C' && currentMode == POWER) {
    op = 'P';
    lcd.print("^");
    return;
  }

  if (key == 'C') {
    if (currentMode == NORMAL) {
      op = 'C';
      lcd.print("x");
    }
    return;
  }

  if (key == 'D' && op == 'D') {
    if (currentMode != NORMAL) return; 
    resetAll();
    currentMode = SQRT;
    showMode("SQRT Mode");
    return;
  }

  if (key == 'D' && currentMode == SQRT) {
    op = 'S';
    lcd.print("sqr");
    return;
  }

  if (key == '*') {
    if (op == 'C' && currentMode == NORMAL) {
      op = 0;
      currentMode = PERCENT;
      showMode("Percent Mode");
      lcd.print(num1); 
      return;
    }

    if (op == 0 && currentMode == NORMAL) {
      factorialMode = true;
      lcd.print("!");
    }
    return;
  }

  if (key == 'D' && currentMode == PERCENT) {
    op = 'R';   
    lcd.print("%");
    return;
  }
  if (key == 'D') {
    if (currentMode == NORMAL) {
      op = 'D';
      lcd.print("/");
    }
    return;
  }

  if (key == '#') {
    double a = atof(num1);
    double b = atof(num2);
    double result = 0;

   if (factorialMode) {
  unsigned long a_int = (unsigned long)a;  
  if (a_int > 12) {
    showError("Overflow");
    return;
  }
  unsigned long tempr = factorial(a_int);  
  printResult((double)tempr);              
  return;
}
    if (op == 'S') {
      result = sqrt(a);
      printResult(result);
      return;
    }

    if (op == 'P') {
      result = round(pow(a, b));
      printResult(result);
      return;
    }

    if (op == 'R') {
      result = (a * b) / 100.0;
      printResult(result);
      return;
    }

    if (op == 'A') {
      result = a + b;
    } else if (op == 'B') {
      result = a - b;
    } else if (op == 'C') {
      result = a * b;
    } else if (op == 'D') {
      if (b == 0) {
        showError("Div by 0");
        return;
      }
      result = a / b;
    }

    printResult(result);
  }
}