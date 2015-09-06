#include <Button.h>

#include <Encoder.h>

#include <AccelStepper.h>


#include <LiquidCrystal.h>

float blade_kerf = 0.125;
float finger_width = 0.25;
float tolerance = 0.01;

const int STEP_DELAY = 175;
const int BUTTON = 5;
const int LIMIT = 6;
const int STEP = 7;
const int DIR = 8;
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
Button button(BUTTON, true, true, 100);
Encoder knob(2, 3);
class Jig
{
  public:
    Jig(int step, int dir);
    void move(float inches);
    void moveBack(float inches);
    float position();
    void moveTo(float inches);
    void zero();
  private:
    float _pos;
    int _step;
    int _dir;
    float steps_per_inch = 800 * 16;
};
Jig::Jig(int step, int dir)
{
  pinMode(step, OUTPUT);
  pinMode(dir, OUTPUT);
  digitalWrite(step, LOW);
  digitalWrite(dir, LOW);
  _step = step;
  _dir = dir;
}
void Jig::moveTo(float inches) {
  float p = _pos;
  if (inches > p) {
    move(inches - p);
  } else if (inches < p) {
    moveBack(p - inches);
  }
}
void Jig::move(float inches) {
  for (int i = 0; i < steps_per_inch * inches; i++) {
    digitalWrite(DIR, LOW);
    digitalWrite(STEP, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(STEP, LOW);
    delayMicroseconds(STEP_DELAY);
  }
}
void Jig::moveBack(float inches) {
  for (int i = 0; i < steps_per_inch * inches; i++) {
    if (digitalRead(LIMIT) == LOW) {
     delay(20);
     if (digitalRead(LIMIT) == LOW) {
       return;
     }
   }
    digitalWrite(DIR, HIGH);
    digitalWrite(STEP, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(STEP, LOW);
    delayMicroseconds(STEP_DELAY);
  }
}
void Jig::zero() {
 while(true) {
   if (digitalRead(LIMIT) == LOW) {
     delay(20);
     if (digitalRead(LIMIT) == LOW) {
       return;
     }
   }
   button.read();
    if (button.wasPressed()) {
      return;
    }
    digitalWrite(DIR, HIGH);
    digitalWrite(STEP, HIGH);
    delayMicroseconds(STEP_DELAY);
    digitalWrite(STEP, LOW);
    delayMicroseconds(STEP_DELAY);
  }
}
float Jig::position() {
  return _pos;
}

Jig jig(STEP, DIR);


int drawStartMenu() {
  lcd.clear();
  int option = 0;
  knob.write(0);
  lcd.setCursor(0, 0);
  lcd.print("Start");
  lcd.print(" <-");
  lcd.setCursor(0, 1);
  lcd.print("Zero");
  int pos = knob.read();
  while (true) {
    button.read();
    int pos2 = knob.read();
    if (pos != pos2) {
      if (abs(pos % 10) < 5) {
        lcd.setCursor(5, 1);
        lcd.print("   ");
        lcd.setCursor(5, 0);
        lcd.print(" <-");
        option = 0;
      } else {
        lcd.setCursor(5, 0);
        lcd.print("   ");
        lcd.setCursor(5, 1);
        lcd.print(" <-");
        option = 1;
      }
      pos = pos2;
    }
    if (button.isPressed()) {
      return option;
    }


  }
}
int drawCutMenu() {
  lcd.clear();
  int option = 0;
  knob.write(0);
  lcd.setCursor(0, 0);
  lcd.print("Advance");
  lcd.print(" <-");
  lcd.setCursor(0, 1);
  lcd.print("Exit");
  int pos = knob.read();
  while (true) {
    button.read();
    int pos2 = knob.read();
    if (pos != pos2) {
      if (abs(pos % 10) < 5) {
        lcd.setCursor(5, 1);
        lcd.print("   ");
        lcd.setCursor(7, 0);
        lcd.print(" <-");
        option = 0;
      } else {
        lcd.setCursor(7, 0);
        lcd.print("   ");
        lcd.setCursor(5, 1);
        lcd.print(" <-");
        option = 1;
      }
      pos = pos2;
    }
    if (button.isPressed()) {
      return option;
    }
  }
}
int drawCutScreen() {
  int finger_num = 1;
  lcd.clear();
  lcd.setCursor(0, 0);
  float cuts = finger_width / (blade_kerf);
  int int_cuts = (int) ceil(cuts);
  float move_dist = finger_width / int_cuts;
  while (true) {
    lcd.clear();
    lcd.print("Moving...");
    lcd.print(finger_width, 3);//+blade_kerf, 3);
    jig.move(finger_width);// + blade_kerf);
    lcd.clear();
    int option = drawCutMenu();
    if (option == 1) {
      return 0;
    }
    for (int i = 0; i < int_cuts; i++) {
      lcd.clear();
      lcd.print("Moving...");
      lcd.print(move_dist, 3);
      jig.move(move_dist);
      lcd.clear();
       int option = drawCutMenu();
    if (option == 1) {
      return 0;
    }
    }
  }
}
int drawToleranceScreen() {
  int pos = tolerance * 1000;
  knob.write(pos);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set tolerance:");
  lcd.setCursor(0, 1);
  lcd.print(tolerance, 3);
  while (true) {
    button.read();
    if (button.wasPressed()) {
      return 0;
    }
    int pos2 = knob.read();
    if (pos != pos2 && pos2 > 0) {
      pos = knob.read();
      lcd.setCursor(0, 1);
      tolerance = pos / 1000.0;
      lcd.print(tolerance, 3);
    }
  }
}
int drawBladeScreen() {
  int pos = blade_kerf * 1000;
  knob.write(pos);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set blade kerf:");
  lcd.setCursor(0, 1);
  lcd.print(blade_kerf, 3);
  while (true) {
    button.read();
    if (button.wasPressed()) {
      return 0;
    }
    int pos2 = knob.read();
    if (pos != pos2 && pos2 > 0) {
      pos = knob.read();
      lcd.setCursor(0, 1);
      blade_kerf = pos / 1000.0;
      lcd.print(blade_kerf, 3);
    }
  }
}
int drawFingerScreen() {
  int pos = finger_width * 1000;
  knob.write(pos);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set finger size:");
  lcd.setCursor(0, 1);
  lcd.print(finger_width, 3);
  while (true) {
    button.read();
    if (button.wasPressed()) {
      return 0;
    }
    int pos2 = knob.read();
    if (pos != pos2 && pos2 > 0) {
      pos = knob.read();
      lcd.setCursor(0, 1);
      finger_width = pos / 1000.0;
      lcd.print(finger_width, 3);
    }
  }
}
int drawZeroScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moving jig to");
  lcd.setCursor(0, 1);
  lcd.print("zero position...");
  jig.zero();
  //knob.write(0);
  //int pos = 0;
  //lcd.clear();
  
//  while (true) {
//    button.read();
//    if (button.wasPressed()) {
//      return 0;
//    }
//    int pos2 = knob.read();
//    if (pos != pos2) {
//      lcd.setCursor(0, 0);
//      lcd.print(pos);
//      lcd.print(" ");
//      int diff = abs(pos - pos2);
//      lcd.print(diff);
//      if (pos >= pos2) {
//        jig.move(0.01 * diff);
//      } else {
//        jig.moveBack(0.01 * diff);
//      }
//    }
//    pos = pos2;
//  }

}
void setup() {
  //Serial.begin(115200);
  pinMode(BUTTON, INPUT);
  digitalWrite(BUTTON, HIGH);
  pinMode(LIMIT, INPUT);
  digitalWrite(LIMIT, HIGH);
  lcd.begin(16, 2);
  lcd.print("BoxJoint v0.1");
  delay(1000);
  jig.move(0.25);
  jig.moveBack(0.25);

}

void loop() {
  int option = drawStartMenu();
  switch (option) {
    case 0:
      option = drawFingerScreen();
      option = drawBladeScreen();
      option = drawToleranceScreen();
      option = drawCutScreen();
      break;
    case 1:
      option = drawZeroScreen();
      break;
  }
  delay(500);
}
