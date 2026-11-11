// Glucose test-strip reader.
// Reads the strip's output voltage on A2 and converts it to glucose
// concentration with a linear calibration: V = SLOPE * C + INTERCEPT.

const int STRIP_PIN = A2;
const float ADC_REF_V = 3.3;   // ADC reference voltage (V)
const float ADC_MAX = 1023.0;  // 10-bit ADC full scale

// Linear regression from calibration data
const float SLOPE = 0.0625;      // V per % concentration
const float INTERCEPT = 1.0925;  // V

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");
  pinMode(STRIP_PIN, INPUT);
}

void loop() {
  float voltage = analogRead(STRIP_PIN) * ADC_REF_V / ADC_MAX;
  float concentration = (voltage - INTERCEPT) / SLOPE;  // invert the calibration

  // Tab-separated: voltage (V), concentration (%)
  Serial.print(voltage);
  Serial.print("\t");
  Serial.println(concentration);

  delay(10);
}
