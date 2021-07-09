#include <SoftwareSerial.h>   //using software serial so as not to conflict with serial download
#include <SPI.h>
#include <SD.h>

SoftwareSerial mySerial(2, 3); // RX, TX

File myFile;
int waktu = 0;
const int ledRed = 9;
bool parsing = false;
char filename[] = "00000000.CSV";

void setup() {

  mySerial.begin(9600); //Setup software serial
  Serial.begin(9600);    //Setup serial monitor
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  pinMode(ledRed, OUTPUT);
}

void loop()  {
  while (mySerial.available()) {
    String inBarcode = mySerial.readString();
    int index = inBarcode.indexOf('#');
    String BarCodeValue = inBarcode.substring(0, index - 1);
    String DateValue = inBarcode.substring(index + 1, inBarcode.length() - 1);

    myFile = SD.open("logger.csv", FILE_WRITE);

    if (myFile) {
      myFile.print(BarCodeValue);
      Serial.println("SAVED TO SD CARD");
      digitalWrite(ledRed, HIGH);
      delay(250);
      digitalWrite(ledRed, LOW);
      delay(250);
      digitalWrite(ledRed, HIGH);
      delay(250);
      digitalWrite(ledRed, LOW);
      delay(250);
      myFile.close();
    }
    //    Serial.println(inBarcode);
    Serial.println(BarCodeValue);
    //    Serial.println(DateValue);
    delay(50);
  }
}
