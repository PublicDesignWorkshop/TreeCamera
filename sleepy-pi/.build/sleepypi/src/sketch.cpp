#include <Arduino.h>
#include "SleepyPi2.h"
#include <Time.h>
#include <LowPower.h>
#include <PCF8523.h>
#include <Wire.h>
void alarm_isr();
void setup();
void loop();
void printTimeNow();
bool getTime(const char *str);
bool getDate(const char *str);
void print2digits(int number);
void PrintRTCRegisters(void);
#line 1 "src/sketch.ino"
// 
// Simple example showing how to set the RTC alarm pin to wake up the Arduino.
// Like an alarm clock the alarm is triggered at a particular time and 
// then the Arduino wakes up.
// 
// Note: this example doesn't wake up the RPi. For that add:
//
//     SleepyPi.enablePiPower(true);  
//
// after arduino wakeup. 
// 
// To test on the RPi without power cycling and using the Arduino IDE
// to view the debug messages, fit the Power Jumper or enable
// self-power
// 
// 

// **** INCLUDES *****
//#include "SleepyPi2.h"
//#include <Time.h>
//#include <LowPower.h>
//#include <PCF8523.h>
//#include <Wire.h>

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
}; 

const int LED_PIN = 13;

// Globals
// ++++++++++++++++++++ Change me ++++++++++++++++++
// .. Setup the Periodic Timer
// .. use either eTB_SECOND or eTB_MINUTE or eTB_HOUR
eTIMER_TIMEBASE  PeriodicTimer_Timebase     = eTB_HOUR;   // e.g. Timebase set to seconds
// 168 hours = 1 week
uint8_t          PeriodicTimer_Value        = 6;           // Timer Interval in units of Timebase e.g 10 seconds
// ++++++++++++++++++++ End Change me ++++++++++++++++++

tmElements_t tm;


void alarm_isr()
{

}


void setup()
{
  // Configure "Standard" LED pin
  pinMode(LED_PIN, OUTPUT);		
  digitalWrite(LED_PIN,LOW);

  pinMode(9, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Starting...");
  delay(50);
  
  SleepyPi.rtcInit(true);

  // Default the clock to the time this was compiled.
  // Comment out if the clock is set by other means
  // ...get the date and time the compiler was run
  // if (getDate(__DATE__) && getTime(__TIME__)) {
  //     SleepyPi.setTime(DateTime(F(__DATE__), F(__TIME__)));
  // }  

  printTimeNow();

}

void loop() {
    bool piRunning;
    SleepyPi.rtcClearInterrupts();

    // Allow wake up alarm to trigger interrupt on falling edge.
    attachInterrupt(0, alarm_isr, FALLING);    // Alarm pin
    SleepyPi.setTimer1(PeriodicTimer_Timebase, PeriodicTimer_Value);
    delay(500);
    SleepyPi.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
    
    detachInterrupt(0);
  SleepyPi.ackTimer1();

    digitalWrite(LED_PIN,HIGH);    // Switch on LED
    delay(1000);
    digitalWrite(LED_PIN,LOW);    // Switch off LED
    SleepyPi.enablePiPower(true);

    // expect the RPi to let us know when it's ready to shut down
    // we want this to happen in this loop iteration because
    // otherwise it won't get checked until the SleepyPi wakes up again
    // which could be a long time from now
    // Expect the RPi to set GPIO 27 / SleepyPi pin 9 LOW because
    // the default GPIO pin state appears to be high

    piRunning = SleepyPi.checkPiStatus(false);
    while (digitalRead(9)) {
      Serial.print("Pin 9: ");
      Serial.println(digitalRead(9));
      piRunning = SleepyPi.checkPiStatus(false);
    }
    // Serial.println("Here's where we'd shut down..");
    SleepyPi.piShutdown();
}

void printTimeNow()
{
    // Read the time
    DateTime now = SleepyPi.readTime();
    
    // Print out the time
    Serial.print("Ok, Time = ");
    print2digits(now.hour());
    Serial.write(':');
    print2digits(now.minute());
    Serial.write(':');
    print2digits(now.second());
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(now.day());
    Serial.write('/');
    Serial.print(now.month()); 
    Serial.write('/');
    Serial.print(now.year(), DEC);
    Serial.println();

    return;
}
bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void PrintRTCRegisters(void)
{
  
      // Debug
      uint8_t reg_value;
      reg_value = SleepyPi.rtcReadReg(PCF8523_CONTROL_1);
      Serial.print("Control 1: 0x");
      Serial.println(reg_value,HEX);
      reg_value = SleepyPi.rtcReadReg(PCF8523_CONTROL_2);
      Serial.print("Control 2: 0x");
      Serial.println(reg_value, HEX);      
      reg_value = SleepyPi.rtcReadReg(PCF8523_CONTROL_3);
      Serial.print("Control 3: 0x");
      Serial.println(reg_value,HEX); 
      
      reg_value = SleepyPi.rtcReadReg(PCF8523_SECONDS);
      Serial.print("Seconds: ");
      Serial.println(reg_value,HEX);
      reg_value = SleepyPi.rtcReadReg(PCF8523_MINUTES);
      Serial.print("Minutes: ");
      Serial.println(reg_value,HEX);  
      reg_value = SleepyPi.rtcReadReg(PCF8523_HOURS);
      Serial.print("Hours: ");
      Serial.println(reg_value,HEX);  
      reg_value = SleepyPi.rtcReadReg(PCF8523_DAYS);
      Serial.print("Days: ");
      Serial.println(reg_value,HEX);   
      reg_value = SleepyPi.rtcReadReg(PCF8523_WEEKDAYS);
      Serial.print("Week Days: ");
      Serial.println(reg_value,HEX);    
      reg_value = SleepyPi.rtcReadReg(PCF8523_MONTHS);
      Serial.print("Months: ");
      Serial.println(reg_value,HEX);  
      reg_value = SleepyPi.rtcReadReg(PCF8523_YEARS);
      Serial.print("Years: ");
      Serial.println(reg_value,HEX); 
      
      reg_value = SleepyPi.rtcReadReg(PCF8523_MINUTE_ALARM);
      Serial.print("Minute Alarm: ");
      Serial.println(reg_value,HEX);      
      reg_value = SleepyPi.rtcReadReg(PCF8523_HOUR_ALARM);
      Serial.print("Hour Alarm: ");
      Serial.println(reg_value,HEX);  
      reg_value = SleepyPi.rtcReadReg(PCF8523_DAY_ALARM);
      Serial.print("Day Alarm: ");
      Serial.println(reg_value,HEX);      
      reg_value = SleepyPi.rtcReadReg(PCF8523_WEEKDAY_ALARM);
      Serial.print("Weekday Alarm: ");
      Serial.println(reg_value,HEX); 
      
      reg_value = SleepyPi.rtcReadReg(PCF8523_OFFSET);
      Serial.print("Offset: 0x");
      Serial.println(reg_value,HEX); 
      reg_value = SleepyPi.rtcReadReg(PCF8523_TMR_CLKOUT_CTRL);
      Serial.print("TMR_CLKOUT_CTRL: 0x");
      Serial.println(reg_value,HEX);  
      reg_value = SleepyPi.rtcReadReg(PCF8523_TMR_A_FREQ_CTRL);
      Serial.print("TMR_A_FREQ_CTRL: 0x");
      Serial.println(reg_value,HEX); 
      reg_value = SleepyPi.rtcReadReg(PCF8523_TMR_A_REG);
      Serial.print("TMR_A_REG: 0x");
      Serial.println(reg_value,HEX);     
      reg_value = SleepyPi.rtcReadReg(PCF8523_TMR_B_FREQ_CTRL);
      Serial.print("TMR_B_FREQ_CTRL: 0x");
      Serial.println(reg_value,HEX);    
       reg_value = SleepyPi.rtcReadReg(PCF8523_TMR_B_REG);
      Serial.print("TMR_B_REG: 0x");
      Serial.println(reg_value,HEX);     
 
}

