#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Create RTC, LCD, and DHT objects
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 4);  // I2C address, columns, rows

#define DHTPIN 2
#define BUZZER_PIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running! Setting the time...");
    rtc.adjust(DateTime(1970, 1, 1, 0, 0, 0));
  }

  lcd.begin(0, 4);
  lcd.backlight();
  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);

  // Welcome beep
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(40);
  }
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(true);  // true = Fahrenheit
  float heatIndex = dht.computeHeatIndex(temperature, humidity);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.setCursor(0, 1);
    lcd.print("Humid:");
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.setCursor(0, 2);
    lcd.print("Heat Index:");
  } else {
    // Humidity
    lcd.setCursor(0, 1);
    lcd.print("Humid: ");
    lcd.print(humidity, 1);
    lcd.print("%");

    // Temperature
    lcd.setCursor(0, 0);
    lcd.print("Temp:           ");
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature, 2);
    lcd.print((char)223);  // Degree symbol
    lcd.print("F");

    // Heat Index
    lcd.setCursor(0, 2);
    lcd.print("Heat Index:        ");
    lcd.setCursor(0, 2);
    lcd.print("Heat Index: ");
    lcd.print(heatIndex, 2);
    lcd.print((char)223);
    lcd.print("F");
  }

  DateTime now = rtc.now();

  // Time display
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

  // Date
  lcd.setCursor(8, 3);
  lcd.print(now.month());
  lcd.print('/');
  lcd.print(now.day());
  lcd.print('/');
  lcd.print(now.year() % 100);

  // Day of week
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

  delay(1000);
}
