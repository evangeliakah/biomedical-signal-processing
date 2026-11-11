# Thermistor Thermometer

Arduino sketch that measures temperature with a thermistor in a Wheatstone bridge.

## How it works

1. Read the two bridge nodes on A1 and A2 and compute the voltage across the bridge.
2. Solve for the thermistor resistance from the bridge voltage (R1 = R2 = R3 = 10 kOhm, 3.3 V supply).
3. Convert resistance to temperature with the Steinhart-Hart equation.
4. Smooth with a 100-sample moving average.
5. Sample at 10 Hz. Print each reading to the serial monitor (9600 baud) and write it to `temp.txt` on the SD card.

Output columns: V1, V2, Vdiff, Resistance, Temp (C), AvgTemp (C).

## Hardware

- Arduino-compatible board with SD card module, [fill in board]
- 10 kOhm thermistor and three 10 kOhm resistors in a Wheatstone bridge

## Run

Open `Thermistor/Thermistor.ino` in the Arduino IDE, upload, and open the serial monitor at 9600 baud.

## Notes

- The moving average starts from an empty buffer, so the average reads low for the first 10 seconds.

## Results

_Add a temperature-over-time plot here._
