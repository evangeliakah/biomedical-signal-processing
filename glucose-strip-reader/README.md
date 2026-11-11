# Glucose Strip Reader

Arduino sketch that reads the output voltage of a glucose test strip and converts it to glucose concentration.

## How it works

1. Read the strip on analog pin A2 and convert the ADC count to volts (3.3 V reference, 10-bit ADC).
2. Convert voltage to concentration by inverting a linear calibration fit from the lab: `V = 0.0625 * C + 1.0925`.
3. Print voltage and concentration, tab-separated, over serial at 115200 baud (about every 10 ms).

The slope and intercept come from the lab calibration data. Update them if you recalibrate.

## Hardware

- Arduino-compatible board with a 3.3 V ADC reference, [fill in board]
- Glucose test strip and readout circuit, [fill in]

## Run

Open `readStrip/readStrip.ino` in the Arduino IDE, upload, and open the serial monitor at 115200 baud.

## Results

_Add your calibration plot (voltage vs. concentration) here._
