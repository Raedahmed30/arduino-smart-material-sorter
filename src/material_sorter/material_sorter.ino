#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


// 28BYJ-48 + ULN2003 Stepper Control by Angle
const int IN1 = 10;
const int IN2 = 11;
const int IN3 = 12;
const int IN4 = 13;

const int STEPS_PER_REV = 4096;      // نصف خطوة = دورة كاملة
int STEP_DELAY_MS = 3;               // زمن بين الخطوات (أكبر = أبطأ)

const uint8_t seq[8][4] = {
  {1,0,0,0},
  {1,1,0,0},
  {0,1,0,0},
  {0,1,1,0},
  {0,0,1,0},
  {0,0,1,1},
  {0,0,0,1},
  {1,0,0,1}
};

int stepIndex = 0;

void stepMotor(int idx){
  digitalWrite(IN1, seq[idx][0]);
  digitalWrite(IN2, seq[idx][1]);
  digitalWrite(IN3, seq[idx][2]);
  digitalWrite(IN4, seq[idx][3]);
}

void rotateSteps(long steps, bool cw){
  for (long s=0; s<steps; s++){
    stepIndex = cw ? (stepIndex+1) & 7 : (stepIndex+7) & 7;
    stepMotor(stepIndex);
    delay(STEP_DELAY_MS);
  }
  // حرر الملفات لتقليل السخونة
  digitalWrite(IN1,0); digitalWrite(IN2,0);
  digitalWrite(IN3,0); digitalWrite(IN4,0);
}

// الدالة الرئيسية للتحريك بالزاوية
void rotateDegrees(float angle, bool cw){
  long steps = (long)(angle * STEPS_PER_REV / 360.0);
  rotateSteps(steps, cw);
}



// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Pins ---
const int PIN_IR     = 4;   // IR obstacle
const int PIN_METAL  = 2;   // Inductive LJ12A3
const int PIN_RAIN   = 3;   // Raindrops D0

// LEDs
const int LED_METAL  = 8;
const int LED_WET    = 9;
const int LED_PLAST  = 0;

// Servo
Servo gateServo;
const int SERVO_PIN   = 6;
const int ANGLE_OPEN  = 45; // فتح
const int ANGLE_CLOSE = 155;  // غلق

// --- Signal polarity ---
const bool ACTIVE_LOW_IR    = true;
const bool ACTIVE_LOW_METAL = true;
const bool ACTIVE_LOW_RAIN  = true;

// --- Timings ---
const unsigned long IR_DEBOUNCE_MS   = 250;
const unsigned long SENSE_SETTLE_MS  = 500;
const unsigned long SERVO_OPEN_MS    = 2000;  // وقت بقاء البوابة مفتوحة (2 ثانية)
const unsigned long AFTER_SORT_MS    = 1000;  // وقت إضافي بعد الغلق

enum State { IDLE, SENSE, CLASSIFY, DONE };
State state = IDLE;

unsigned long cntMetal = 0, cntWet = 0, cntPlast = 0;

bool isActive(int pin, bool activeLow) {
  int v = digitalRead(pin);
  return activeLow ? (v == LOW) : (v == HIGH);
}

bool stableObjectPresent() {
  unsigned long t0 = millis();
  while (millis() - t0 < IR_DEBOUNCE_MS) {
    if (!isActive(PIN_IR, ACTIVE_LOW_IR)) return false;
    delay(10);
  }
  return true;
}

void setResultLEDs(bool m, bool w, bool p){
  digitalWrite(LED_METAL,  m ? HIGH : LOW);
  digitalWrite(LED_WET,    w ? HIGH : LOW);
  digitalWrite(LED_PLAST,  p ? HIGH : LOW);
}

void lcdShow(const char* line1, const char* line2 = nullptr) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  if (line2) lcd.print(line2);
}

void lcdShowCounts() {
  char buf2[16];
  snprintf(buf2, sizeof(buf2), "M:%lu W:%lu P:%lu",
           cntMetal, cntWet, cntPlast);
  lcd.setCursor(0,1);
  lcd.print(buf2);
  delay(1000);
}

void setup() {
  pinMode(PIN_IR,    INPUT);
  pinMode(PIN_METAL, INPUT_PULLUP);
  pinMode(PIN_RAIN,  INPUT);

  pinMode(LED_METAL, OUTPUT);
  pinMode(LED_WET,   OUTPUT);
  pinMode(LED_PLAST, OUTPUT);


  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  // OFF مبدئياً
  digitalWrite(IN1,0); digitalWrite(IN2,0);
  digitalWrite(IN3,0); digitalWrite(IN4,0);


  gateServo.attach(SERVO_PIN);
  gateServo.write(ANGLE_CLOSE); // يبدأ مغلق

  Serial.begin(9600);
  Serial.println("IR-gated sorter ready");

  lcd.init();
  lcd.backlight();
  lcdShow("Sorter Ready", "Waiting...");
}

void loop() {
  switch (state) {
    case IDLE: {
      if (isActive(PIN_IR, ACTIVE_LOW_IR) && stableObjectPresent()) {
        lcdShow("Object detected", "Hold...");
        state = SENSE;
      }
    } break;

    case SENSE: {
      delay(SENSE_SETTLE_MS);
      state = CLASSIFY;
    } break;

    case CLASSIFY: {
      bool metal = isActive(PIN_METAL, ACTIVE_LOW_METAL);
      bool wet   = isActive(PIN_RAIN,  ACTIVE_LOW_RAIN);

      if (metal) {
        Serial.println("Detected: METAL");
        setResultLEDs(true,false,false);
        cntMetal++;
        lcdShow("Last: METAL");
        lcdShowCounts();
  // مثال: تحريك 90° مع عقارب الساعة
  rotateDegrees(250, true);
  delay(1000);

      // فتح البوابة 2 ثانية ثم غلق
      gateServo.write(ANGLE_OPEN);
      delay(SERVO_OPEN_MS);
      gateServo.write(ANGLE_CLOSE);
        delay(1000);
  rotateDegrees(250, false);
  delay(1000);




      } else if (wet) {
        Serial.println("Detected: WET");
        setResultLEDs(false,true,false);
        cntWet++;
        lcdShow("Last: WET");
        lcdShowCounts();
  // مثال: تحريك 90° مع عقارب الساعة
  rotateDegrees(5, true);
  delay(1000);

      // فتح البوابة 2 ثانية ثم غلق
      gateServo.write(ANGLE_OPEN);
      delay(SERVO_OPEN_MS);
      gateServo.write(ANGLE_CLOSE);
        delay(1000);
  rotateDegrees(5, false);
  delay(1000);



      } else {
        Serial.println("Detected: PLASTIC");
        setResultLEDs(false,false,true);
        cntPlast++;
        lcdShow("Last: PLASTIC");
        lcdShowCounts();
  // مثال: تحريك 90° مع عقارب الساعة
  rotateDegrees(125, true);
  delay(1000);

        // فتح البوابة 2 ثانية ثم غلق
      gateServo.write(ANGLE_OPEN);
      delay(SERVO_OPEN_MS);
      gateServo.write(ANGLE_CLOSE);
        delay(1000);
  rotateDegrees(125, false);
  delay(1000);

      }
/*
      // فتح البوابة 2 ثانية ثم غلق
      gateServo.write(ANGLE_OPEN);
      delay(SERVO_OPEN_MS);
      gateServo.write(ANGLE_CLOSE);
*/
      state = DONE;
    } break;

    case DONE: {
      delay(AFTER_SORT_MS);
      setResultLEDs(false,false,false);
      if (!isActive(PIN_IR, ACTIVE_LOW_IR)) {
        lcdShow("Waiting...", "Place an item");
        state = IDLE;
      }
    } break;
  }
}

