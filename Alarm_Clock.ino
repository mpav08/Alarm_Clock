//Want to the authors/contributors of the libraries that are currently used in this sketch.

//libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Time.h>
#include <TaskScheduler.h>
#include <ezButton.h>
#include <Preferences.h>

#define led_status 16
#define btn1 18
#define btn2 4
#define btn3 15
#define bzr 12
#define led1 17
#define led2 19

//lcd
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

unsigned int alarmHour, alarmMinute;
bool alarmSet = false;
bool showAlarm = false;
unsigned int showAlarmtime = 0;
unsigned int changeState = 0;

//
unsigned int newmonth;
unsigned int year, month, day, second, hour, minute;
// Buttons
ezButton button1(btn1);
ezButton button2(btn2);
ezButton button3(btn3);

// Preferences object
Preferences pref;

//callbacks, tasks and runner
void t1Callback();
void t2Callback();
void t3Callback();
void t4Callback();
void t5Callback();
void t6Callback();

byte Bell[] = {
  B00100,
  B01110,
  B01110,
  B01110,
  B11111,
  B00000,
  B00100,
  B00000
};
//lcd show
Task t1(1000, TASK_FOREVER, &t1Callback);

//led status turn on activate t3
Task t2(5000, TASK_FOREVER, &t2Callback);

//turn off led status disable t3
Task t3(0, TASK_FOREVER, &t3Callback);

//buzzer and led turn on, activate t5
Task t4(1000, TASK_FOREVER, &t4Callback);

//turn off led alarms
Task t5(0, TASK_FOREVER, &t5Callback);
Task t6(0,TASK_FOREVER, &t6Callback);

Scheduler runner;

//ESP32Time rtc;
ESP32Time rtc(0);

//time string for displaying time at lcd
char timeString[20];
char timeString1[20];

//callback that shows time on lcd
void t1Callback() {
  //Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));   // (String) returns time with specified format
  // formating options  http://www.cplusplus.com/reference/ctime/strftime/
  struct tm timeinfo = rtc.getTimeStruct();
  if (changeState == 0) {
    lcd.setCursor(0, 0);
    //sprintf(dateString, "%03d, %03d/", rtc.getDate());
    //lcd.print(timeString);
    lcd.print(rtc.getDate());
    lcd.setCursor(0, 1);
    sprintf(timeString, "%02d:%02d:%02d", rtc.getHour(true), rtc.getMinute(), rtc.getSecond());
    lcd.print(timeString);
    Serial.println(timeString);
  }

  if (changeState != 0){
      runner.addTask(t6);
      t6.enable();
      t6.setInterval(500);
  }


  // if (changeState == 1) {
  //   Serial.println(rtc.getMonth());
  // }
  // if (changeState == 2) {
  //   Serial.println(rtc.getDay());
  // }
  // if (changeState == 3) {
  //   Serial.println(rtc.getYear());
  // }
  // if (changeState == 4) {
  //   Serial.println(rtc.getHour(true));
  // }
  // if (changeState == 5) {
  //   Serial.println(rtc.getMinute());
  // }
  // if (changeState == 6) {
  //   Serial.println(alarmHour);
  // }
  // if (changeState == 7) {
  //   Serial.println(alarmMinute);
  // }


  if (showAlarm == true && changeState == 0) {
    showAlarmtime++;
    if (showAlarmtime <= 5) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarm Time:");
      lcd.setCursor(0, 1);
      sprintf(timeString1, "%02d:%02d", alarmHour, alarmMinute);
      lcd.print(timeString1);
    } else {
      showAlarmtime = 0;
      showAlarm = false;
    }
  }
   else if (changeState == 6 || changeState == 7){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarm Time:");
      lcd.setCursor(0, 1);
      sprintf(timeString1, "%02d:%02d", alarmHour, alarmMinute);
      lcd.print(timeString1);
   }
   else {
    lcd.clear();
    lcd.setCursor(0, 0);
    //sprintf(dateString, "%03d, %03d/", rtc.getDate());
    //lcd.print(timeString);
    lcd.print(rtc.getDate());

    lcd.setCursor(0, 1);
    sprintf(timeString, "%02d:%02d:%02d", rtc.getHour(true), rtc.getMinute(), rtc.getSecond());
    lcd.print(timeString);
    //Serial.println(timeString);
  }
  Serial.println(rtc.getMonth());
  saveSettings();
}

//led status turns on every 5 seconds for 250 msec, it will turns off thx to t3
void t2Callback() {
  //Serial.println("LED ON");
  digitalWrite(led_status, HIGH);
  runner.addTask(t3);
  t3.enable();
  t3.setInterval(250);
}

//turn off led
void t3Callback() {
  t3.disable();
  runner.deleteTask(t3);
  digitalWrite(led_status, LOW);
  //Serial.println("LED OFF");
}

//ALARM IS BUZZING
void t4Callback() {
  if (rtc.getHour(true) == alarmHour && rtc.getMinute() == alarmMinute && alarmSet == true) {
    //Serial.println("ALARM");
    digitalWrite(bzr, HIGH);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    runner.addTask(t5);
    t5.enable();
    t5.setInterval(100);
  } else {
    digitalWrite(bzr, LOW);
  }
}

//DISABLE ALARM
void t5Callback() {
  t5.disable();
  runner.deleteTask(t5);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}

void t6Callback(){
  t6.disable();
  runner.deleteTask(t6);

  //change Month
  if(changeState == 1){
    lcd.setCursor(5,0);
    lcd.print("   ");
  }

  //change Day
  else if(changeState == 2){
    lcd.setCursor(9,0);
    lcd.print("  ");
    }

  //change Year
  else if(changeState == 3){
    lcd.setCursor(12,0);
    lcd.print("    ");
  }

  //change Hour
  else if(changeState == 4){
    lcd.setCursor(0,1);
    lcd.print("  ");   
  }

  //Change Minute
  else if(changeState == 5){
    lcd.setCursor(3,1);
    lcd.print("  ");   
  }

  //Change Alarm Hour
  else if(changeState == 6){
    lcd.setCursor(0,1);
    lcd.print("  ");   
  }
  
  //Change Alarm Minute
  else if(changeState == 7){
    lcd.setCursor(3,1);
    lcd.print("  ");   
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pref.begin("alarm_clock", false);

  //set default time
  alarmHour = pref.getInt("alarmHour",0);
  alarmMinute = pref.getInt("alarmMinute", 0);
  alarmSet = pref.getBool("alarmSet", false);

  // Load time components from Preferences
  year = pref.getInt("year", 2023);
  month = pref.getInt("month", 1);
  day = pref.getInt("day", 1);
  hour = pref.getInt("hour", 0);
  minute = pref.getInt("minute", 0);
  second = pref.getInt("second", 0);
  //rtc.setTime(55, 59, 23, 13, 7, 2024);  // sec, min, hour, day, month, year

  

  rtc.setTime(second, minute, hour, day, month, year);

  //PIN MODES
  pinMode(led_status, OUTPUT);
  pinMode(bzr, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  //SET DEBOUNCE TIMES
  button1.setDebounceTime(100);
  button2.setDebounceTime(100);
  button3.setDebounceTime(100);

  //LCD STARTS
  lcd.begin(16, 2);

  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  //CREATE A CUSTOM CHAR FOR LCD
  lcd.createChar(0, Bell);
  lcd.clear();
  //initialize runners & tasks
  runner.init();
  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t4);
  //runner.addTask(t6);
  
  //DEFAULT ALARM HOUR AND MINUTE
  // alarmHour = 5;
  // alarmMinute = 0;

  //ENABLE TASKS
  t1.enable();
  t2.enable();
  t4.enable();
  //t6.enable();
}

void saveSettings() {
  pref.putInt("alarmHour", alarmHour);
  pref.putInt("alarmMinute", alarmMinute);
  pref.putBool("alarmSet", alarmSet);
  pref.putInt("year", rtc.getYear());
  pref.putInt("month", rtc.getMonth() + 1);
  pref.putInt("day", rtc.getDay());
  pref.putInt("hour", rtc.getHour(true));
  pref.putInt("minute", rtc.getMinute());
  pref.putInt("second", rtc.getSecond());
  // Ensure preferences are properly saved
}


void loop() {
  // put your main code here, to run repeatedly:
  //RUNNER EXECUTE RUNS ALL THE TASKS
  runner.execute();

  //BUTTONS WILL ENGANGE IN THE LOOP
  button1.loop();
  button2.loop();
  button3.loop();
//Default State buttons
  if (changeState == 0) {
    //SET ON/OFF THE ALARM
    if (button1.isPressed()) {
      alarmSet = !alarmSet;
    }
    //CREATE A BELL SYMBOL ON THE LCD SCREEN
    if (alarmSet == true) {
      lcd.setCursor(15, 1);
      lcd.write(byte(0));
    } else if (alarmSet == false) {
      //DELETE THE BELL SYMBOL FROM THE LCD
      lcd.setCursor(15, 1);
      lcd.print(" ");
    }

    if (button2.isPressed()) {
      showAlarm = true;
      //Serial.println("Show time");
    }
  }

//For some reason even when you change Year or Day the month decreases by one
//make sure the month increases after the new change related in rtc

//Change month
  if (changeState == 1) {
    if (button1.isPressed()) {
      newmonth = rtc.getMonth() + 1;
      rtc.setTime(rtc.getSecond(), rtc.getMinute(), rtc.getHour(true), rtc.getDay(), newmonth + 1, rtc.getYear());
      pref.putInt("month", newmonth + 1);
    }

    if (button2.isPressed()) {
      newmonth = rtc.getMonth() - 1;
      rtc.setTime(rtc.getSecond(), rtc.getMinute(), rtc.getHour(true), rtc.getDay(), newmonth + 1, rtc.getYear());
      pref.putInt("month", newmonth + 1);
    }
  }

//Change Day
  else if (changeState == 2) {
    if (button1.isPressed()) {
      rtc.setTime(rtc.getSecond(), rtc.getMinute(), rtc.getHour(true), rtc.getDay() + 1, rtc.getMonth() + 1, rtc.getYear());
      pref.putInt("day", rtc.getDay() + 1);
    }
    if (button2.isPressed()) {  
      rtc.setTime(rtc.getSecond(), rtc.getMinute(), rtc.getHour(true), rtc.getDay() - 1, rtc.getMonth() + 1, rtc.getYear());
      pref.putInt("day", rtc.getDay() - 1);
    }
    
  }

//Change Year

  else if (changeState == 3){
      if (button1.isPressed()) {
      rtc.setTime(rtc.getSecond(), rtc.getMinute(), rtc.getHour(true), rtc.getDay(), rtc.getMonth() + 1, rtc.getYear() + 1);
      pref.putInt("year", rtc.getYear() + 1);
    }
    if (button2.isPressed()) {
      rtc.setTime(rtc.getSecond(), rtc.getMinute(), rtc.getHour(true), rtc.getDay(), rtc.getMonth() + 1, rtc.getYear() - 1);
      pref.putInt("year", rtc.getYear() - 1);
    }
    
  }

  //Change Hour

  else if (changeState == 4){
      if (button1.isPressed()) {
      rtc.setTime(rtc.getSecond(), rtc.getMinute(), rtc.getHour(true) + 1, rtc.getDay(), rtc.getMonth() + 1, rtc.getYear());
      pref.putInt("hour", rtc.getHour(true) + 1);
    }
    if (button2.isPressed()) {
      rtc.setTime(rtc.getSecond(), rtc.getMinute(), rtc.getHour(true) - 1, rtc.getDay(), rtc.getMonth() + 1, rtc.getYear());
      pref.putInt("hour", rtc.getHour(true) - 1);
    }
    
  }

  //Change Minute
  else if (changeState == 5){
      if (button1.isPressed()) {
      rtc.setTime(rtc.getSecond(), rtc.getMinute() + 1, rtc.getHour(true), rtc.getDay(), rtc.getMonth() + 1, rtc.getYear());
      pref.putInt("minute", rtc.getMinute() + 1);
    }
    if (button2.isPressed()) {
      rtc.setTime(rtc.getSecond(), rtc.getMinute() - 1, rtc.getHour(true), rtc.getDay(), rtc.getMonth() + 1, rtc.getYear());
      pref.putInt("minute", rtc.getMinute() - 1);
    }
  
  }
  //Change Alarm Hour
  else if (changeState == 6){
    if (button1.isPressed()){
      alarmHour++;
      if(alarmHour > 23){
        alarmHour = 0;
      }
      pref.putInt("alarmHour", alarmHour);
    }

    if (button2.isPressed()){
      alarmHour--;
      if (alarmHour < 0){
        alarmHour = 23;
      }
      pref.putInt("alarmHour", alarmHour);
    }
  }

  else if (changeState == 7){
    if (button1.isPressed()){
        alarmMinute++;
        if(alarmMinute > 59 ){
          alarmMinute = 0;
        }
        pref.putInt("alarmMinute", alarmMinute);
      }
    
    if (button2.isPressed()){
      alarmMinute--;
      if(alarmMinute < 0){
        alarmMinute = 59;
      }
      pref.putInt("alarmMinute", alarmMinute);
    }
  }
  if (button3.isPressed()) {
    changeState++;
    if (changeState > 7) {
      changeState = 0;
    }
    //Serial.println(changeState);
  }
}
