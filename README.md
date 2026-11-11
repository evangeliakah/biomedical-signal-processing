# Biomedical Signal Processing

Four projects from Signal Processing for Medical Devices at Columbia. They cover biosignal classification, sensor calibration, and embedded data acquisition on Arduino. Course starting templates were provided by the instructor. The implementation is my own.

| Project | What it does | Tools |
|---|---|---|
| [ecg-classification](ecg-classification/) | 1D CNN that labels 12-lead ECGs as normal, MI, conduction disturbance, ST/T change, or hypertrophy | Python, TensorFlow/Keras, wfdb, scikit-learn |
| [glucose-strip-reader](glucose-strip-reader/) | Reads a glucose test strip's voltage and converts it to concentration using a linear calibration | Arduino C++ |
| [insulin-pump-simulation](insulin-pump-simulation/) | Simulates a glucose-insulin model with a PID-controlled insulin pump | Arduino C++ |
| [thermistor-thermometer](thermistor-thermometer/) | Wheatstone bridge + thermistor temperature readout with Steinhart-Hart conversion, moving average and SD logging | Arduino C++ |

Each folder has its own README with how to run it.
