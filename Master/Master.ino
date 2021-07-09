#include <usbhid.h>
#include <hiduniversal.h>
#include <LiquidCrystal_I2C.h>
#include <avr/pgmspace.h>
#include <Usb.h>
#include <usbhub.h>
#include <avr/pgmspace.h>
#include <hidboot.h>
#include <RTClib.h>
#include <SoftwareSerial.h>

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
SoftwareSerial mySerial(2, 3);

USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> Keyboard(&Usb);

class KbdRptParser : public KeyboardReportParser {
    void PrintKey(uint8_t mod, uint8_t key);
  protected:
    virtual void OnKeyDown  (uint8_t mod, uint8_t key);
    virtual void OnKeyPressed(uint8_t key);
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key) {
  uint8_t c = OemToAscii(mod, key);
  char CharBarcode;
  String stringBarcode;
  DateTime now = rtc.now();
  int hari = now.day();
  int bulan = now.month();
  int tahun = now.year();
  int jam = now.hour();
  int menit = now.minute();
  int detik = now.second();
  CharBarcode = c;
  Serial.print(CharBarcode);
  //  stringBarcode = String(CharBarcode);
  //  stringBarcode += String(CharBarcode);

  if (c == 13) {
    mySerial.print(" | ");
    mySerial.print(String(hari) + "/" + String(hari) + "/" + String(bulan) + "/" + String(tahun) + " | " + String(jam) + ":" + String(menit) + ":" + String(detik));
  }
  mySerial.print(CharBarcode);

  if (c)
    OnKeyPressed(c);
}

unsigned char n_char[7];
unsigned char  i_n = 0;

unsigned char border;
void KbdRptParser::OnKeyPressed(uint8_t key) {
  static uint32_t next_time = 0;      //watchdog
  static uint8_t current_cursor = 0;  //tracks current cursor position

  if ( millis() > next_time ) {
    lcd.clear();
    current_cursor = 0;
    delay( 5 );
    //      Sending Time to Arduino Slave
    //      End
    lcd.setCursor(2, 1);
    lcd.print(" MODEL  |  SERI ");
    lcd.setCursor(0, 0);
    lcd.print(' ');
    DateTime now = rtc.now();
    int hari = now.day();
    int bulan = now.month();
    int tahun = now.year();
    int jam = now.hour();
    int menit = now.minute();
    int detik = now.second();
    lcd.print(hari);
    lcd.print("/");
    lcd.print(bulan);
    lcd.print("/");
    lcd.print(tahun);
    lcd.print(" ");
    if (jam < 10) {
      lcd.print(0);
      lcd.print(jam);
    } else {
      lcd.print(jam);
    }
    lcd.print(':');
    if (menit < 10) {
      lcd.print(0);
      lcd.print(menit);
    } else {
      lcd.print(menit);
    }
    lcd.print(':');
    if (detik < 10) {
      lcd.print(0);
      lcd.print(detik);
    } else {
      lcd.print(detik);
    }
    lcd.println();
    lcd.setCursor(1, 2);
  }
  next_time = millis() + 200;  //reset watchdog

  lcd.print((char)key);
};

KbdRptParser Prs;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  if (Usb.Init() == -1) {
    Serial.println("OSC did not start.");
  }
  Hid.SetReportParser(0, (HIDReportParser*)&Prs);
  //   set up the LCD's number of columns and rows:
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print(" MODEL  |  SERI ");
  rtc.begin();
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  Usb.Task();
}
