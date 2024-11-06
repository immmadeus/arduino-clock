#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Create RTC, LCD, and DHT objects
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 4);  // Change the I2C address if needed
#define DHTPIN 2                     // Pin connected to the DHT11 sensor
#define BUZZER_PIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);  // Start serial communication
  Wire.begin();        // Initialize the I2C bus

  // Initialize the RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;  // Stop the program
  }

  // Check if the RTC is running, and set the time if it isn't
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running! Setting the time...");
    rtc.adjust(DateTime(1970, 1, 1, 0, 0, 0));  // Set time
  }

  // Initialize the LCD
  lcd.begin(0, 4);
  lcd.backlight();
  dht.begin();
  pinMode(BUZZER_PIN, OUTPUT);  // Set the buzzer pin as an output

  //alert beep
  if (dht.readTemperature(true) >= 104) {
    lcd.noBacklight();
    delay(1000);
    lcd.backlight();
    for (int i = 0; i < 5; i++) {
      for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        delay(40);
      }
      delay(1000);
    }
    lcd.setCursor(0, 0);
    lcd.println("Warning: Temperature over 104F!");
    delay(5000);
    lcd.clear();
  } else
    for (int i = 0; i < 2; i++)  //welcome beep
    {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
      delay(40);
    }
}

void loop() {
  // Read humidity and temperature from DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(true);  // Use true for Fahrenheit
  float heatIndex = dht.computeHeatIndex(temperature, humidity);

  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.setCursor(0, 1);
    lcd.print("Humid:");
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.setCursor(0, 2);
    lcd.print("Heat Index:");
  } else {
    // Display humidity
    lcd.setCursor(0, 1);
    lcd.print("Humid: ");
    lcd.print(humidity, 1);
    lcd.print("%");

    // Display temperature
    lcd.setCursor(0, 0);
    lcd.print("Temp:           ");  // Clear the entire line before writing

    lcd.setCursor(0, 0);  // Reset cursor to the start of the line
    lcd.print("Temp: ");
    lcd.print(temperature, 2);
    lcd.print((char)223);  // Degree symbol
    lcd.print("F");


    // Display Heat Index
    lcd.setCursor(0, 2);
    lcd.print("Heat Index:        ");  // Clear the entire line before writing

    lcd.setCursor(0, 2);  // Reset cursor to the start of the line
    lcd.print("Heat Index: ");
    lcd.print(heatIndex, 2);
    lcd.print((char)223);  // Degree symbol
    lcd.print("F");
  }
  // Get current time and date
  DateTime now = rtc.now();

  // Display the time
  lcd.setCursor(0, 3);
  int hour = now.hour();
  String ampm = "AM";

  if (hour >= 12) {
    ampm = "PM";
    if (hour > 12) hour -= 12;
  } else if (hour == 0) {
    hour = 12;
  }

  lcd.print(hour);
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(ampm);

  // Display the date
  lcd.setCursor(8, 3);
  if (now.month() < 10) lcd.print('0');
  lcd.print(now.month());
  lcd.print('/');
  if (now.day() < 10) lcd.print('0');
  lcd.print(now.day());
  lcd.print('/');
  lcd.print(now.year() % 100);

  // Display the day of the week
  lcd.setCursor(17, 3);
  switch (now.dayOfTheWeek()) {
    case 0: lcd.print("Sun"); break;
    case 1: lcd.print("Mon"); break;
    case 2: lcd.print("Tue"); break;
    case 3: lcd.print("Wed"); break;
    case 4: lcd.print("Thu"); break;
    case 5: lcd.print("Fri"); break;
    case 6: lcd.print("Sat"); break;
    default: lcd.print("N/A"); break;
  }
  delay(1000);  // Update every second
}
