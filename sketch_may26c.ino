#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LCD_I2C.h>

LCD_I2C lcd(0x27, 20, 2);

#define SS_PIN 10
#define RST_PIN 9
#define LED_G 4 // define green LED pin
#define LED_R 5 // define red LED
#define BUZZER 2 // buzzer pin
#define RELAY 3 // relay pin
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

void setup() 
{
  Serial.begin(9600); // Initiate a serial communication
  SPI.begin(); // Initiate SPI bus
  mfrc522.PCD_Init(); // Initiate MFRC522
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH); // Ensure the door is locked on startup
  Serial.println("Place your card on reader...");
  Serial.println();
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0); // column, row
  lcd.print(" Scan Your RFID ");
  lcd.setCursor(0, 1); // column, row
  lcd.print("   Door Locked   ");
}

void loop() 
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message: ");
  content.toUpperCase();

  if (content.substring(1) == "43 EA 82 28") // change here the UID of card/cards or tag/tags that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    delay(500);
    digitalWrite(RELAY, LOW);
    digitalWrite(LED_G, HIGH);
    lcd.setCursor(0, 1); // column, row
    lcd.print(" Door Un-Locked  ");
    tone(BUZZER, 2000);
    delay(100);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 2000);
    delay(100);
    noTone(BUZZER);
    delay(ACCESS_DELAY);
    digitalWrite(RELAY, HIGH);
    digitalWrite(LED_G, LOW);
    lcd.setCursor(0, 1); // column, row
    lcd.print("   Door Locked   ");
    tone(BUZZER, 2000);
    delay(100);
    noTone(BUZZER);
    delay(50);
  } else {
    Serial.println(" Access denied");
    lcd.setCursor(0, 1); // column, row
    lcd.print("Invalid RFID Tag ");
    digitalWrite(LED_R, HIGH);
    tone(BUZZER, 300);
    delay(DENIED_DELAY);
    digitalWrite(LED_R, LOW);
    noTone(BUZZER);
    lcd.setCursor(0, 1); // column, row
    lcd.print("   Door Locked   ");
  }
}
