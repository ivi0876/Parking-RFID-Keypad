/*

Author: Ivan Sanchez
Date: 02/04/2023

description: this project still in process, simulates a smart parking with card reader + keypad 4x4 to save your parking spot.
*/

#include <SPI.h> 
#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 9
#define SERVO_PIN 8
#define ROWS 4
#define COLS 4

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
Servo myservo;
MFRC522 mfrc522(SS_PIN, RST_PIN);

char keymap[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, A6, A7};
Keypad keypad = Keypad(makeKeymap(keymap), rowPins, colPins, ROWS, COLS);
String authorizedCards[4] = {"e3851091", "3350d7a7", "33 50 D7 A7", "33 50 D7 A7"};

bool barrierClosed = true;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  myservo.attach(SERVO_PIN);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
}

void loop() {
  lcd.clear();
  lcd.print("Scan your card:");
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String cardID = "";
    //Serial.println("escaneada");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardID += String(mfrc522.uid.uidByte[i], HEX);
    }
    for (int i = 0; i < (sizeof(authorizedCards)/sizeof(authorizedCards[0])); i++) {
      //Serial.println(cardID);
      if (cardID == authorizedCards[i] ) {
        
        lcd.clear();
        lcd.print("Enter your spot:");
        String spotNumber = "";
        char key = keypad.waitForKey();
        while (key != '#') {
          spotNumber += key;
          lcd.setCursor(0, 1);
          lcd.print(spotNumber);
          key = keypad.waitForKey();
        }
        lcd.clear();
        lcd.print("Spot reserved");
        delay(2000);
        openBarrier();
        delay(5000);
        closeBarrier();
        return;
      } 
    }
    lcd.clear();
        //Serial.println("entro en no autorizado");
        lcd.print("no autorizado");
        delay(2000);
  }
}

void openBarrier() {
  if (barrierClosed) {
    for (int angle = 0; angle <= 90; angle += 1) {
      myservo.write(angle);
      delay(15);
    }
    barrierClosed = false;
  }
}

void closeBarrier() {
  if (!barrierClosed) {
    for (int angle = 90; angle >= 0; angle -= 1) {
      myservo.write(angle);
      delay(15);
    }
    barrierClosed = true;
  }
}