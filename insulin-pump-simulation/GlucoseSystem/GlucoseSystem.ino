// Closed-loop insulin pump simulation on Arduino.
// Integrates a glucose-insulin model (states Q, G, I) with a simple
// Euler step each loop, and a PID controller that adds insulin to hold
// glucose near a target after a meal.

#include <SD.h>
#include <SPI.h>

// ---- Simulation settings ----
const int END_STEP = 400;       // stop after this many steps
const int MEAL_STEPS = 15;      // meal is absorbed during the first steps
const float MEAL_SIZE = 300;    // meal input while eating
const int STEP_DELAY_MS = 100;

// ---- State variables (initial conditions) ----
// Q: meal-related state, G: glucose, I: insulin
float Q = 0;
float G = 163.0;
float I = 11.26;

// ---- PID controller ----
const float TARGET_G = 140.0;     // glucose the pump aims for
const float Kp = 0.1;
const float Ki = 0;               // integral term is off in this run
const float Kd = 10;
const float RESERVOIR_MAX = 50;   // pump reservoir holds 50 U
float sumError = 0;               // running total of error
float totalInsulin = 0;           // total insulin delivered by the pump

// ---- Model parameters ----
// Diabetic model
const float beta = 10;
const float eta = 4.641;
const float gam = 25;
const float R0 = 2.5;
const float E = 2.5e-3;
const float S = 1.14e-3;
const float kq = 0.026;
const float Imax = 0.93;
const float alpha = 1e4;
const float ki = 0.06;
// Non-diabetic alternative (swap in to compare):
//   beta = 20, eta = 4.086, gam = 40, R0 = 2.1, E = 1e-3, S = 3.06e-3,
//   kq = 0.098, Imax = 0.28, alpha = 1e4, ki = 0.01

// ---- SD logging (not called by default, see logToSD) ----
const int CHIP_SELECT = 4;

int step = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");
  while (!Serial) {
    ;  // wait for serial port to connect
  }

  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("SD not found");
    return;
  }
  Serial.println("SD found");

  File log = SD.open("gluSys.txt", FILE_WRITE);
  if (log) {
    log.println("Q \t G \t I");
    log.close();
  } else {
    Serial.println("error opening gluSys.txt");
  }
}

// Append the current Q, G, I to gluSys.txt. Call from loop() to record a run.
void logToSD() {
  File log = SD.open("gluSys.txt", FILE_WRITE);
  if (log) {
    log.print(Q);
    log.print("\t");
    log.print(G);
    log.print("\t");
    log.println(I);
    log.close();
  } else {
    Serial.println("error opening gluSys.txt");
  }
}

void loop() {
  if (step > END_STEP) {
    while (true) {
    }
  }

  // Meal input
  float mealInput = (step < MEAL_STEPS) ? MEAL_SIZE : 0;

  // Model equations
  float dQ = (-beta * Q + eta * mealInput) / (gam * gam + Q * Q);
  float dG = R0 - (E + S * I) * G + kq * Q;
  float dI = Imax * (G * G / (alpha + G * G)) - ki * I;
  Q += dQ;
  G += dG;
  I += dI;

  // Manual bolus scenarios (uncomment one to test):
  //   1: bolus at meal start        if (step == 0) { I += 15; }
  //   2: bolus at meal end          if (step == MEAL_STEPS) { I += 15; }
  //   3: bolus before meal and a corrective bolus at meal end
  //                                 if (step == 0 || step == MEAL_STEPS) { I += 15; }
  //   4: bolus at start plus basal  if (step == 0) { I += 15; }  I += 0.5;

  // PID control: extra insulin from the pump
  float error = G - TARGET_G;  // how far above target
  sumError += error;
  float pumpInsulin = Kp * error + Ki * sumError + Kd * dG;
  if (pumpInsulin < 0) {
    pumpInsulin = 0;  // pump cannot remove insulin
  }
  if (totalInsulin + pumpInsulin > RESERVOIR_MAX) {
    pumpInsulin = RESERVOIR_MAX - totalInsulin;
  }
  totalInsulin += pumpInsulin;
  I += pumpInsulin;

  // Tab-separated: step, Q, G, I, total insulin delivered
  Serial.print(step);
  Serial.print("\t");
  Serial.print(Q);
  Serial.print("\t");
  Serial.print(G);
  Serial.print("\t");
  Serial.print(I);
  Serial.print("\t");
  Serial.println(totalInsulin);

  step++;
  delay(STEP_DELAY_MS);
}
