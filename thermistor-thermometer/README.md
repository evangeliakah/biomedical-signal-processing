# Thermistor Thermometer

A thermometer built from a thermistor in a Wheatstone bridge, read by a Feather M0. It converts the bridge voltage to resistance, resistance to temperature, and smooths the result with a moving average. The goal was a low-noise sensor over the human body temperature range.

![Wheatstone bridge circuit](images/thermistor_circuit.png)

## How it works

1. Read the two bridge nodes on A1 and A2 and compute the voltage across the bridge.
2. Solve for the thermistor resistance from the bridge voltage (R1 = R2 = R3 = 10 kOhm, 3.3 V supply).
3. Convert resistance to temperature with the Steinhart-Hart equation.
4. Smooth with a moving average (none, 10 points or 100 points were compared).
5. Sample at 10 Hz. Print each reading to the serial monitor (9600 baud) and write it to `temp.txt` on the SD card.

Output columns: V1, V2, Vdiff, Resistance, Temp (C), AvgTemp (C).

The resistance calculation was checked by swapping the thermistor for 5.6 kOhm and 15 kOhm resistors, then for the thermistor at room temperature (about 11 kOhm).

## Results

Measured in a water bath at several set temperatures, about one minute of data each, with standard-tolerance resistors. Values are the min to max reading in degrees C, with variance in parentheses.

| Bath | No averaging | 10-point average | 100-point average |
|---|---|---|---|
| 37 C | 34.97 to 35.96 (0.0518) | 35.23 to 35.67 (0.01367) | 35.47 to 35.55 (0.00073) |
| 40 C | 37.99 to 38.41 (0.0124) | 38.14 to 38.28 (0.00142) | 38.14 to 38.19 (0.00026) |

Averaging cuts the noise a lot: at 37 C, variance dropped about 4x with 10 points and about 70x with 100 points. I'd use the 10-point average. The 100-point average is quieter, but it responds more slowly to real temperature changes.

Replacing the standard 10 kOhm resistors with lower-tolerance precision resistors, with a 10-point average:

| Bath | Min to max (C) | Variance |
|---|---|---|
| 35 C | 34.39 to 34.52 | 0.00141 |
| 37 C | 37.14 to 37.25 | 0.00099 |
| 38 C | 38.51 to 38.61 | 0.00057 |
| 40 C | 40.93 to 41.05 | 0.00073 |

At 37 C the precision resistors cut the 10-point variance from 0.01367 to 0.00099. Standard resistors read about 1.5 C lower than the bath setting, while precision resistors were much closer. Precision resistors make sense for clinical thermometers and physiological monitors. They aren't needed for hobby or environmental sensing.

Other sources of error to control: water leaking into the thermistor wiring (seal the heat shrink), loose breadboard connections, and the bath temperature drifting (keep it covered).

## Hardware

- Feather M0 with SD logging
- Thermistor and three 10 kOhm resistors in a Wheatstone bridge

## Run

Open `Thermistor/Thermistor.ino` in the Arduino IDE, upload, and open the serial monitor at 9600 baud.

## Notes

- The moving average starts from an empty buffer, so the average reads low for the first 10 seconds.
