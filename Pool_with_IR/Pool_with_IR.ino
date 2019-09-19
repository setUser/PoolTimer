#include <TimeLib.h>
#include <EEPROM.h>
#include <IRremote.h>
#include <Servo.h>
IRrecv irrecv(2);
decode_results results;
Servo servo;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  setTime(EEPROM.read(30), EEPROM.read(31), EEPROM.read(32), EEPROM.read(33), EEPROM.read(34), 2000+EEPROM.read(35)); //Damaged EEPROM = 1,2,3,4,5,6
  filter(EEPROM.read(0));
  irrecv.enableIRIn();
  Serial.begin(9600);
  Serial.println("************************\nPool Timer Commands:\nTime = "+String(hour())+":"+String(minute())+":"+String(second())+"\n"+"Date = "+String(day())+"/"+String(month())+"/"+String(year())+"\n************************\n"+
  "getTime\nsetTime=00:00:00\ngetDate\nsetDate=00/00/0000\ngetCycle1\nsetCycle1=00:00to00:00\ngetCycle2\nsetCycle2=00:00to00:00\nsetStopwatch=minutes\ngetStatus\nsetStatus=ON/OFF\n************************\n");
}
void filter(bool state) {
  servo.attach(3);
  servo.write(state ? 180 : 130);
  delay(2000);
  servo.detach();
  EEPROM.write(0, state);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  while (second() == 0 || second() == 30) { //every minute = 525,600 times in a year
    EEPROM.write(30, hour());
    delay(150);
    EEPROM.write(31, minute());
    delay(150);
    EEPROM.write(32, second());
    delay(150);
    EEPROM.write(33, day());
    delay(150);
    EEPROM.write(34, month());
    delay(150);
    EEPROM.write(35, year()-2000);
    delay(150);
  }
  if (Serial.available()) {
    String string = Serial.readString();
    if (string.substring(0, 7) == "getTime") {
      Serial.println("Current Time is "+String(hour())+":"+String(minute())+":"+String(second()));
    }
    else if (string.substring(0, 8) == "setTime=") {
      setTime(string.substring(8, 10).toInt(), string.substring(11, 13).toInt(), string.substring(14, 16).toInt(), day(), month(), year());
      EEPROM.write(30, hour());
      delay(150);
      EEPROM.write(31, minute());
      delay(150);
      EEPROM.write(32, second());
      delay(150);
      Serial.println("Time Set!!!\nCurrent Time is "+String(hour())+":"+String(minute())+":"+String(second()));
    }
    else if (string.substring(0, 7) == "getDate") {
      Serial.println("Current Date is "+String(day())+"/"+String(month())+"/"+String(year()));
    }
    else if (string.substring(0, 8) == "setDate=") {
      setTime(hour(), minute(), second(), string.substring(8, 10).toInt(), string.substring(11, 13).toInt(), string.substring(14, 18).toInt());
      EEPROM.write(33, day());
      delay(150);
      EEPROM.write(34, month());
      delay(150);
      EEPROM.write(35, year()-2000);
      delay(150);
      Serial.println("Date Set!!!\nCurrent Date is "+String(day())+"/"+String(month())+"/"+String(year()));
    }
    else if (string.substring(0, 9) == "getCycle1") {
      Serial.println("Cycle 1\nstarts at "+String(EEPROM.read(7))+":"+String(EEPROM.read(8))+" and ends at "+String(EEPROM.read(9))+":"+String(EEPROM.read(10)));
    }
    else if (string.substring(0, 10) == "setCycle1=") {
      EEPROM.write(7, string.substring(10, 12).toInt());
      EEPROM.write(8, string.substring(13, 15).toInt());
      EEPROM.write(9, string.substring(17, 19).toInt());
      EEPROM.write(10, string.substring(20, 22).toInt());
      Serial.println("Cycle 1 Set!!!\nstarts at "+String(EEPROM.read(7))+":"+String(EEPROM.read(8))+" and ends at "+String(EEPROM.read(9))+":"+String(EEPROM.read(10)));
    }
    else if (string.substring(0, 9) == "getCycle2") {
      Serial.println("Cycle 2\nstarts at "+String(EEPROM.read(11))+":"+String(EEPROM.read(12))+" and ends at "+String(EEPROM.read(13))+":"+String(EEPROM.read(14)));
    }
    else if (string.substring(0, 10) == "setCycle2=") {
      EEPROM.write(11, string.substring(10, 12).toInt());
      EEPROM.write(12, string.substring(13, 15).toInt());
      EEPROM.write(13, string.substring(17, 19).toInt());
      EEPROM.write(14, string.substring(20, 22).toInt());
      Serial.println("Cycle 2 Set!!!\nstarts at "+String(EEPROM.read(11))+":"+String(EEPROM.read(12))+" and ends at "+String(EEPROM.read(13))+":"+String(EEPROM.read(14)));
    }
    else if (string.substring(0, 13) == "setStopwatch=") {
      handleStopwatch(string.substring(13, 15).toInt());
      Serial.println("Stopwatch SET by TERMINAL");
    }
    else if (string.substring(0, 9) == "getStatus") {
      Serial.println(EEPROM.read(0) ? "Pool Filter is ON" : "Pool Filter is OFF");
    }
    else if (string.substring(0, 10) == "setStatus=") {
      if (string.substring(10, 12) == "ON") {
        filter(true);
        Serial.println("Status Set!!!\nPool Filter is ON by TERMINAL");
      }
      if (string.substring(10, 13) == "OFF") {
        filter(false);
        Serial.println("Status Set!!!\nPool Filter is OFF by TERMINAL");
      }
    }
  }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  if (hour() == EEPROM.read(7) and minute() == EEPROM.read(8) or hour() == EEPROM.read(11) and minute() == EEPROM.read(12)) {
    if (!EEPROM.read(0)) {
      filter(true);
      Serial.println("Current Time is "+String(hour())+":"+String(minute())+":"+String(second())+"\nPool Filter is ON by CYCLE");
    }
  }
  if (hour() == EEPROM.read(9) and minute() == EEPROM.read(10) or hour() == EEPROM.read(13) and minute() == EEPROM.read(14)) {
    if (EEPROM.read(0)) {
      filter(false);
      Serial.println("Current Time is "+String(hour())+":"+String(minute())+":"+String(second())+"\nPool Filter is OFF by CYCLE");
    }
  }
  if (EEPROM.read(17)) {
    if (hour() == EEPROM.read(15) and minute() == EEPROM.read(16)) {
      EEPROM.write(17, false);
      if (EEPROM.read(0)) {
        filter(false);
        Serial.println("Pool Filter is OFF by STOPWATCH");
      }
    }
  }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  if (irrecv.decode(&results)) {
    Serial.println("IR CODE: "+String(results.value, HEX));
    handleControl(results.value);
    irrecv.resume();
  }
}
void handleControl(unsigned long value) {
  delay(100);
  switch (value) {
    case 0x1818D02F:
      filter(!EEPROM.read(0));
      Serial.println(EEPROM.read(0) ? "Pool Filter is ON by IR CODE" : "Pool Filter is OFF by IR CODE");
      break;
    case 0x181800FF:
      handleStopwatch(10);
      Serial.println("Stopwatch SET by IR CODE");
      break;
    case 0x1818807F:
      handleStopwatch(20);
      Serial.println("Stopwatch SET by IR CODE");
      break;
    case 0x181840BF:
      handleStopwatch(30);
      Serial.println("Stopwatch SET by IR CODE");
      break;
    case 0x1818C03F:
      handleStopwatch(40);
      Serial.println("Stopwatch SET by IR CODE");
      break;
    case 0x181820DF:
      handleStopwatch(50);
      Serial.println("Stopwatch SET by IR CODE");
      break;
    case 0x1818A05F:
      handleStopwatch(60);
      Serial.println("Stopwatch SET by IR CODE");
      break;
    case 0x1818609F:
      handleStopwatch(70);
      Serial.println("Stopwatch SET by IR CODE");
      break;
    case 0x1818E01F:
      handleStopwatch(80);
      Serial.println("Stopwatch SET by IR CODE");
      break;
    case 0x181810EF:
      handleStopwatch(90);
      Serial.println("Stopwatch SET by IR CODE");
      break;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void handleStopwatch(int stopwatch) {
  if (!EEPROM.read(0)) {
        filter(true);
        Serial.println("Pool Filter is ON by STOPWATCH");
    }
    int h = hour();
    int m = minute()+stopwatch;
    while (m >= 60) {
      h = h+1;
      m = m-60;
    }
    EEPROM.write(15, h);
    delay(150);
    EEPROM.write(16, m);
    delay(150);
    EEPROM.write(17, true);
    delay(150);
    Serial.println("Stopwatch for "+String(stopwatch)+" minutes");
}
