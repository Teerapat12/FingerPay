/* Locked Relays

   An Arduino Uno, Keypad, Relays and some LEDs for fun

   Using a password to enable the relays, then selective
      toggle the relays by key.

   Needed libraries
   http://arduino.cc/playground/uploads/Code/Keypad.zip
   http://arduino.cc/playground/uploads/Code/Password.zip
*/
#include <Keypad.h>
#include <Password.h>
int relay1 = 2;
int relay2 = 3;
int relay3 = 4;
int relay4 = 5;
int locked = 1;
int passinput = 0;
int lockedled = 14;
int unlockedled = 15;
long ledflashvar = 0;
long ledflashtime = 300;
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {{'1', '2', '3', 'A'}, {'4', '5', '6', 'B'}, {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {13, 12, 11, 10};
byte colPins[COLS] = {9, 8, 7, 6};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Password password = Password("0000");   // รหัสของการปลดล็อค
void setup() {
  //  กำหนดค่าเริ่มต้น และ PIN ต่างๆ
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, 255);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, 255);
  pinMode(relay3, OUTPUT);
  digitalWrite(relay3, 255);
  pinMode(relay4, OUTPUT);
  digitalWrite(relay4, 255);
  pinMode(lockedled, OUTPUT);
  digitalWrite(lockedled, 255);
  pinMode(unlockedled, OUTPUT);
  digitalWrite(unlockedled, 0);
}
void loop() {
  char key = keypad.getKey();  // รับค่าจาก keypad


  // กรณีที่ Lock อยู่  LED สีแดงจะติด  และหากอยู่ระหว่างการใส่รหัส LED สีแดงจะกระพริบ
  if (locked) {
    if (passinput) {
      unsigned long ledcurrentvar = millis();
      if (ledcurrentvar - ledflashvar > ledflashtime) {
        ledflashvar = ledcurrentvar;
        digitalWrite(lockedled, !digitalRead(lockedled));
      }
    }

    // ถ้าไม่ได้อยู่ระหว่างการใส่รหัส
    else {
      digitalWrite(lockedled, 255);   // LED สีแดงจะติด
    }
    digitalWrite(unlockedled, 0);     // LED สีเขียวดับ
  }


  if (key != NO_KEY) {
    Serial.println(key);   // แสดงค่าที่กดบน Serial monitor
    password.append(key);  // บันทึกค่ารหัสที่กดและเปรียบเทียบกับรหัสที่ถูกต้อง
    passinput = 1;         // อยู่ระหว่างการใส่รหัส

    // ถ้ากด '*' ให้เริ่มใส่รหัสใหม่
    if (key == '*') {
      password.reset();     // เริ่มต้นใส่รหัสใหม่ตั้งแต่ตัวแรก
      passinput = 0;        // ไม่อยู่ระหว่างการกดรหัส
      locked = 1;
      digitalWrite(relay1, HIGH);   // ตั้งค่า Relay ให้อยู่ในสถานะ Hight
      digitalWrite(relay2, HIGH);
      digitalWrite(relay3, HIGH);
      digitalWrite(relay4, HIGH);
    }

    if (password.evaluate()) {      //  ถ้าใส่ Password ถูกต้องให้ปลดล็อค
      locked = !locked;
      password.reset();             // เริ่มต้นใส่รหัสใหม่ตั้งแต่ตัวแรก
      passinput = 0;
    }

    //  ถ้าปลดล๊อคแล้วให้ไฟสีเขียวติด  สีแดงดับ  และสามารถควบคุมการเปิดปิดของ Relay ได้
    if (!locked) {
      passinput = 0;
      digitalWrite(lockedled, 0);
      digitalWrite(unlockedled, 255);

      switch (key) {                //  และเลือก Relay ที่ต้องการควบคุม
        case 'A':
          digitalWrite(relay1, !digitalRead(relay1));
          break;
        case 'B':
          digitalWrite(relay2, !digitalRead(relay2));
          break;
        case 'C':
          digitalWrite(relay3, !digitalRead(relay3));
          break;
        case 'D':
          digitalWrite(relay4, !digitalRead(relay4));
          break;
      }


      password.reset();              // เริ่มต้นใส่รหัสใหม่ตั้งแต่ตัวแรก
    }
  }
}
