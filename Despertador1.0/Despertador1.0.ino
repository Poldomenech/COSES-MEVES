#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>              //llibreria pantalla
#include <Wire.h>                           //llibreria I2C

LiquidCrystal_I2C lcd(0x27,20,4);           //Valor en hexadecimal per I2C + tamany horitzontalment + tamany verticalment  --> Es troba el valor amb llibreria arduino 

int hores=0;
int minuts=0;
int segons=0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(16,4);
  lcd.clear();
  lcd.backlight();
  Imer1.initialize();
}

void loop() {
 lcd.setCursor(0,0);
lcd.print("HOLA");
delay(500);
lcd.setCursor(5,0);
lcd.print
}
