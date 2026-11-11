// Thermistor thermometer.
// A thermistor in a Wheatstone bridge is read on two analog pins. The bridge
// voltage gives the thermistor resistance, the Steinhart-Hart equation gives
// temperature, and a moving average smooths the result. Data goes to the
// serial monitor and to temp.txt on the SD card.
//
// Course starting template: Signal Processing for Medical Devices (Columbia).

#include <SD.h>
#include <SPI.h>
#include <Wire.h>

// ---- Bridge and sampling parameters ----
const float R1 = 10000;  // ohms
const float R2 = 10000;
const float R3 = 10000;
const float IN_VOLTAGE = 3.3;  // bridge supply (V)
const float ADC_MAX = 1023.0;
const float SAMPLE_RATE_HZ = 10;

// ---- Steinhart-Hart coefficients ----
const float SH_A = 2.59e-3;
const float SH_B = 4.09e-10;
const float SH_C = 9.66e-07;

// ---- Moving average ----
const int BUFFER_SIZE = 100;
float tempBuffer[BUFFER_SIZE];
float sumTemp = 0;
int bufferIndex = 0;

File tempRecord;

void setup() {
  Serial.begin(9600);

  tempRecord = SD.open("temp.txt", FILE_WRITE);
  tempRecord.println("V1\tV2\tVdiff\tResistance\tTemp (C)\tAvgTemp (C)");

  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
}

void loop() {
  // Read both bridge nodes (ADC counts)
  int v1 = analogRead(A2);
  int v2 = analogRead(A1);

  // Voltage across the bridge
  float vDiff = (v1 - v2) * IN_VOLTAGE / ADC_MAX;

  // Thermistor resistance from the bridge voltage
  float resistance =
      ((R2 * IN_VOLTAGE - (R1 + R2) * vDiff) / (R1 * IN_VOLTAGE + (R1 + R2) * vDiff)) * R3;

  // Temperature (Steinhart-Hart), kelvin -> celsius
  float logR = log(resistance);
  float tempC = 1.0 / (SH_A + SH_B * logR + SH_C * logR * logR * logR) - 273.15;

  // Moving average over the last BUFFER_SIZE readings
  sumTemp -= tempBuffer[bufferIndex];  // remove oldest reading
  tempBuffer[bufferIndex] = tempC;     // store newest
  sumTemp += tempC;
  float avgTemp = sumTemp / BUFFER_SIZE;
  bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;

  // Output: serial monitor and SD card (tab-separated)
  Serial.print(v1);
  Serial.print("\t");
  Serial.print(v2);
  Serial.print("\t");
  Serial.print(vDiff);
  Serial.print("\t");
  Serial.print(resistance);
  Serial.print("\t");
  Serial.print(tempC);
  Serial.print("\t");
  Serial.println(avgTemp);

  tempRecord.print(v1);
  tempRecord.print("\t");
  tempRecord.print(v2);
  tempRecord.print("\t");
  tempRecord.print(vDiff);
  tempRecord.print("\t");
  tempRecord.print(resistance);
  tempRecord.print("\t");
  tempRecord.print(tempC);
  tempRecord.print("\t");
  tempRecord.println(avgTemp);

  delay(1000 / SAMPLE_RATE_HZ);  // sampling rate
  tempRecord.flush();
}
