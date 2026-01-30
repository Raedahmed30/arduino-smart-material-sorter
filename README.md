# 🔧 Arduino Material Sorter (Metal / Wet / Plastic)

A smart sorting system that detects an object using an IR sensor, classifies it as **Metal**, **Wet**, or **Plastic**, then routes it using a **Stepper Motor (28BYJ-48 + ULN2003)** and a **Servo Gate**, with status shown on an **I2C LCD**.

> شرح عربي مع مصطلحات إنجليزية – مناسب للمبتدئين والمتوسطين.

---

## ✅ فكرة المشروع | Project Idea
عند مرور قطعة أمام حساس **IR Obstacle Sensor**:
1. يتم تثبيت وجود القطعة (Debounce).
2. قراءة الحساسات:
   - **Inductive Sensor (LJ12A3)** → لاكتشاف المعدن (Metal)
   - **Rain Sensor (D0)** → لاكتشاف القطعة الرطبة (Wet)
3. تدوير المسار باستخدام **Stepper Motor** لزاوية محددة حسب النوع.
4. فتح بوابة الفرز بواسطة **Servo Motor** لمدة ثانيتين.
5. إغلاق البوابة وإرجاع المحرك لوضعه الأساسي.
6. عرض آخر نتيجة وعدّاد القطع على شاشة LCD.

---

## 🧩 المكونات | Components
- Arduino (UNO/Nano)
- IR Obstacle Sensor
- Inductive Sensor (LJ12A3-4-Z/BX or similar)
- Rain Sensor Module (D0 output)
- Stepper Motor 28BYJ-48 + ULN2003 Driver
- Servo Motor (SG90 or similar)
- I2C LCD 16x2 (Address: 0x27)
- LEDs (Metal / Wet / Plastic) + resistors
- Wires + Breadboard / PCB
- Power supply (5V مناسب للمحركات)

---

## 🔌 التوصيلات | Wiring (Pins)
### Sensors
- IR Sensor → `PIN_IR = 4`
- Inductive Sensor → `PIN_METAL = 2` (INPUT_PULLUP)
- Rain Sensor D0 → `PIN_RAIN = 3`

### LEDs
- Metal LED → `LED_METAL = 8`
- Wet LED → `LED_WET = 9`
- Plastic LED → `LED_PLAST = 0` ⚠️ *(غير مفضل، اقرأ الملاحظات)*

### Servo
- Servo Signal → `SERVO_PIN = 6`

### Stepper (ULN2003)
- IN1 → 10
- IN2 → 11
- IN3 → 12
- IN4 → 13

### LCD I2C
- SDA / SCL حسب لوحتك (UNO: A4/A5 غالبًا)

---

## ⚙️ الإعدادات المهمة | Important Settings
- `SENSE_SETTLE_MS` زمن تثبيت القراءة بعد اكتشاف الجسم
- `SERVO_OPEN_MS` مدة فتح البوابة
- `STEP_DELAY_MS` سرعة الستيبّر (أكبر = أبطأ)

زوايا التوجيه الحالية داخل الكود:
- METAL: rotate 250°
- WET: rotate 5°
- PLASTIC: rotate 125°
> عدّل الزوايا حسب تصميمك الميكانيكي.

---

## 📝 ملاحظات مهمة | Notes
✅ **لا تستخدم pin 0 للـ LED** لأنه مرتبط بالـ Serial (RX) وقد يسبب مشاكل.  
اقترح تغييره إلى `LED_PLAST = 7` أو أي pin فاضي.

✅ **الـ 28BYJ-48 يسخن** عند التشغيل المستمر، والكود يقوم بإطفاء الملفات بعد الدوران لتقليل السخونة (Good).

✅ لو حساساتك تعطي عكس (Active HIGH بدل Active LOW) عدّل:
- `ACTIVE_LOW_IR`
- `ACTIVE_LOW_METAL`
- `ACTIVE_LOW_RAIN`

---

## 📸 Demo
ضع صور المشروع هنا داخل مجلد `images/`:
- `setup.jpg` صورة التوصيل
- `demo.gif` لقطة تشغيل

---

## 🧠 Author
Content & Project idea: The Arab Inventor (المخترع العربي)

---
