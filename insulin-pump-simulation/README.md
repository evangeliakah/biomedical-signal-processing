# Insulin Pump Simulation

Arduino sketch that simulates blood glucose after a meal and tests an insulin pump controller. It does not use a real glucose sensor or pump. Everything is computed in software.

## How it works

- Three state variables (Q, G, I) are updated each step with a simple Euler integration of the model equations. G is glucose and I is insulin. The default parameters are for a diabetic patient. The non-diabetic set is in the code comments.
- A meal enters for the first 15 steps.
- A PID controller adds insulin to hold glucose near 140. Insulin cannot be removed, and the pump reservoir holds 50 units total. The integral gain is set to 0 in this run, so it acts as PD control.
- Manual bolus scenarios (at meal start, at meal end, before and after, plus basal) are listed in the code and can be uncommented to compare.
- Each step prints step, Q, G, I and total insulin delivered, tab-separated, at 115200 baud. The run stops after 400 steps.

An SD logging function (`logToSD`) is included but not called by default.

## Run

Open `GlucoseSystem/GlucoseSystem.ino` in the Arduino IDE, upload, and read the output in the serial monitor or Serial Plotter.

## Results

_Add a plot of glucose over time with and without the controller here._
