#include <Wire.h>

#include <dht.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial GPS(2, 3); // RX, TX
TinyGPSPlus gps;
float Lat = 9.01390, Long = 7.3965;
dht DHT;
#define DHT11_PIN 6
LiquidCrystal_I2C lcd1(0x25, 16, 2);
LiquidCrystal_I2C lcd2(0x26, 16, 2);
LiquidCrystal_I2C lcd3(0x23, 16, 2);

// Set the LCD address to 0x26 FOR A0 CONNECTED
// Set the LCD address to 0x25 FOR A1 CONNECTED
// Set the LCD address to 0x24 FOR A2 CONNECTED

const int trigPin = 7;
const int echoPin = 8;
const int buzzer = 13;
const int door1 = A2;
const int door2 = A1;
const int door3 = A3;
const int motor1 = 9;
const int motor2 = 10;
float temp, humi;
float temp1, humi1;
float temp2, humi2;
float temp3, humi3;
int Gen_temp;
int vibration = 0;
int ThermistorPIN = A0;  // Analog Pin 2

int count3 = 0, count2 = 0, count1 = 0;
long duration;
int distance;
int a = 0;
double Thermistor(int RawADC) {
  long Resistance;  double Temp;
  Resistance = 10000.0 * ((1024.0 / RawADC) - 1);
  Temp = log(Resistance);
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius
  return Temp;  // Return the Temperature
}


void setup() {
  Serial.begin(9600);
  GPS.begin(9600);
  lcd1.begin();
  lcd2.begin();
  lcd3.begin();
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(door1, INPUT);
  pinMode(door2, INPUT);
  pinMode(door3, INPUT);

  digitalWrite(motor1, LOW);
  digitalWrite(motor2, LOW);
  digitalWrite(buzzer, LOW);
  digitalWrite(door1, HIGH);
  digitalWrite(door2, HIGH);
  digitalWrite(door3, HIGH);


  lcd1.backlight();
  lcd1.print("  Station 1 ");
  lcd2.backlight();
  lcd2.print("  Station 2");
  lcd3.backlight();
  lcd3.print("  Station 3 ");
  delay(1000);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  //reading temp/humi
  int chk = DHT.read11(DHT11_PIN);
  temp = DHT.temperature;
  humi = DHT.humidity;
  Gen_temp = Thermistor(analogRead(ThermistorPIN));
  temp1 = temp + 1.6; humi1 = humi + 13;
  temp2 = temp + 3.1; humi2 = humi + 12;
  temp3 = temp + 2.7; humi3 = humi + 8;

  //counting numbers og passengers
  if (!digitalRead(door1))
  {
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    count1 = count1 + 1;
  }

  if (!digitalRead(door2))
  {
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    count2 = count2 + 1;
  }

  if (!digitalRead(door3))
  {
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    count3 = count3 + 1;
  }

  //controlling the train
  if ((count1 > 5) || (count2 > 5) || (count3 > 5))
  {
    count1 = 0;
    lcd1.setCursor(0, 0);
    lcd1.print(temp1);
    lcd1.print(" *C   ");
    lcd1.print(humi1);
    lcd1.print(" %   ");
    lcd1.setCursor(0, 1);
    lcd1.print("Passenagers: ");
    lcd1.print(count1);
    digitalWrite(motor1, HIGH);
    digitalWrite(motor2, LOW);
    delay(3300);
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
    delay(3000);
    count2 = 0;
    lcd2.setCursor(0, 0);
    lcd2.print(temp2);
    lcd2.print(" *C   ");
    lcd2.print(humi2);
    lcd2.print(" %   ");
    lcd2.setCursor(0, 1);
    lcd2.print("Passenagers: ");
    lcd2.print(count2);

    digitalWrite(motor1, HIGH);
    digitalWrite(motor2, LOW);
    delay(3700);
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
    count3 = 0;
    delay(3000);
    lcd3.setCursor(0, 0);
    lcd3.print(temp3);
    lcd3.print(" *C   ");
    lcd3.print(humi3);
    lcd3.print(" %   ");
    lcd3.setCursor(0, 1);
    lcd3.print("Passenagers: ");
    lcd3.print(count3);


    digitalWrite(motor1, LOW);
    digitalWrite(motor2, HIGH);
    delay(3700);
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
    delay(3000);
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, HIGH);
    delay(3300);
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
  }

  if (Serial.available())
  {
    String pos[2] = Serial.readStringUntil('#');
    String _ID = pos[0];
    if (_ID == "yes") {
      count1 = 10;
    }
    else if (_ID == "no") {
      //sending data online
      Serial.print(temp1); //10 data
      Serial.print("#");
      Serial.print(humi1);
      Serial.print("#");
      Serial.print(count1);
      Serial.print("#");
      Serial.print(temp2);
      Serial.print("#");
      Serial.print(humi2);
      Serial.print("#");
      Serial.print(count2);
      Serial.print("#");
      Serial.print(temp3);
      Serial.print("#");
      Serial.print(humi3);
      Serial.print("#");
      Serial.print(count3);
      Serial.print("#");
      Serial.print(Gen_temp);
      Serial.println("#");
    }
    else {
      ;
    }
  }


  //getting gps coordinate
  while (GPS.available() > 0)
  {
    gps.encode(GPS.read());
    if (gps.location.isUpdated())
    {
      Lat = gps.location.lat();
      Long = gps.location.lng();
    }
    else {
      ;
    }
  }


  lcd1.setCursor(0, 0);
  lcd1.print(temp1);
  lcd1.print(" *C   ");
  lcd1.print(humi1);
  lcd1.print(" %   ");
  lcd1.setCursor(0, 1);
  lcd1.print("Passenagers: ");
  lcd1.print(count1);

  lcd2.setCursor(0, 0);
  lcd2.print(temp2);
  lcd2.print(" *C   ");
  lcd2.print(humi2);
  lcd2.print(" %   ");
  lcd2.setCursor(0, 1);
  lcd2.print("Passenagers: ");
  lcd2.print(count2);

  lcd3.setCursor(0, 0);
  lcd3.print(temp3);
  lcd3.print(" *C   ");
  lcd3.print(humi3);
  lcd3.print(" %   ");
  lcd3.setCursor(0, 1);
  lcd3.print("Passenagers: ");
  lcd3.print(count3);
  delay(500);
}
