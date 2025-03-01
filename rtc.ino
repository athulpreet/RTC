/*   STM32F103C8 Blue Pill with DS3231 RTC Module
  Displays date YYYY/MM/DD and time HH:MM:SS
  Uses DS3231 RTC module over I2C interface
*/

#include <Wire.h>
#include <RTClib.h>

// Initialize RTC
RTC_DS3231 rtc;

char dateTimeStr[64]; // for sprintf

void setup() {
  // Initialize Serial (USB)
  Serial.begin(115200);
  
  // Initialize UART1 (PA9=TX, PA10=RX)
  Serial1.begin(9600);
  
  // Initialize I2C communication
  Wire.begin();
  
  // Initialize DS3231 RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find DS3231 RTC");
    Serial1.println("Couldn't find DS3231 RTC");
    while (1);
  }
  
  // Check if RTC lost power and needs to be set
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting default time");
    Serial1.println("RTC lost power, setting default time");
    
    // Set RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    
    // You can also set a specific date and time:
    // rtc.adjust(DateTime(2023, 3, 1, 12, 0, 0));
  }
  
  Serial.println("DS3231 RTC initialized");
  Serial1.println("DS3231 RTC initialized");
}

void loop() {
  // Get current date and time from DS3231
  DateTime now = rtc.now();
  
  // Format and display date and time
  sprintf(dateTimeStr, "%04d/%02d/%02d %02d:%02d:%02d", 
          now.year(), now.month(), now.day(), 
          now.hour(), now.minute(), now.second());
  
  Serial.println(dateTimeStr);
  Serial1.println(dateTimeStr);
  
  // Check for data from Serial (USB) to update time
  if (Serial.available() > 0) {
    // Format expected: YYYY,MM,DD,HH,MM,SS
    String input = Serial.readStringUntil('\n');
    
    if (input.length() >= 19) { // Basic length check
      int year = input.substring(0, 4).toInt();
      int month = input.substring(5, 7).toInt();
      int day = input.substring(8, 10).toInt();
      int hour = input.substring(11, 13).toInt();
      int minute = input.substring(14, 16).toInt();
      int second = input.substring(17, 19).toInt();
      
      if (year >= 2000 && month >= 1 && month <= 12 && day >= 1 && day <= 31 &&
          hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59 && second >= 0 && second <= 59) {
        rtc.adjust(DateTime(year, month, day, hour, minute, second));
        Serial.println("RTC time updated");
        Serial1.println("RTC time updated");
      }
    }
  }
  
  // Wait one second before next update
  delay(1000);
}
