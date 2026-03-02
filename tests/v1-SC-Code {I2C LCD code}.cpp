#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <math.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;

byte rowPins[ROWS] = {5,4,3,2};
byte colPins[COLS] = {A3,A2,A1,A0};

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String num1="";
String num2="";
char op=0;
bool powerMode=false;
bool percentMode=false;

long factorial(int n){
  long f=1;
  for(int i=1;i<=n;i++){
    f*=i;
  }
  return f;
}

void setup(){
  lcd.init();
  lcd.backlight();
  lcd.print("Smart Calculator");
  delay(1500);
  lcd.clear();
}

void loop(){

  char key = keypad.getKey();

  if(key){

    // Numbers
    if(key>='0' && key<='9'){
      if(op==0){
        num1+=key;
      } else {
        num2+=key;
      }
      lcd.print(key);
    }

    // Addition
    else if(key=='A'){
      op='A';
      lcd.print("+");
    }

    // Subtraction
    else if(key=='B'){
      op='B';
      lcd.print("-");
    }

    // Multiplication
    else if(key=='C'){
      op='C';
      percentMode=false;
      lcd.print("*");
    }

    // Division or Power
    else if(key=='D'){
      if(op=='D'){
        powerMode=true;
        lcd.clear();
        lcd.print("Power Mode");
        delay(800);
        lcd.clear();
      } else {
        op='D';
        powerMode=false;
        lcd.print("/");
      }
    }

    // Percentage (press * after C)
    else if(key=='*'){
      if(op=='C'){   // if multiplication selected
        percentMode=true;
        lcd.clear();
        lcd.print("Percent Mode");
        delay(800);
        lcd.clear();
      }
      else{
        // Factorial
        long result = factorial(num1.toInt());
        lcd.clear();
        lcd.print("Result:");
        lcd.setCursor(0,1);
        lcd.print(result);
        num1=""; num2=""; op=0;
      }
    }

    // Equal
    else if(key=='#'){

      if(num1=="" || num2=="") return;

      float a=num1.toFloat();
      float b=num2.toFloat();
      float result=0;

      if(op=='A') result=a+b;
      else if(op=='B') result=a-b;
      else if(op=='C' && !percentMode) result=a*b;
      else if(op=='C' && percentMode) result=(a*b)/100.0;
      else if(op=='D' && !powerMode) result=a/b;
      else if(op=='D' && powerMode) result=pow(a,b);

      lcd.clear();
      lcd.print("Result:");
      lcd.setCursor(0,1);
      lcd.print(result);

      num1=""; num2=""; op=0;
      powerMode=false;
      percentMode=false;
    }
  }
}