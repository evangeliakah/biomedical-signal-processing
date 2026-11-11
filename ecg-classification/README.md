# ECG Classification (PTB-XL)

A 1D convolutional neural network that predicts five diagnostic superclasses from 12-lead ECG recordings:

`NORM` (normal), `MI` (myocardial infarction), `CD` (conduction disturbance), `STTC` (ST/T change), `HYP` (hypertrophy)

A recording can carry more than one label, so the output layer is sigmoid with binary cross-entropy loss. A class is predicted if it is the most likely one for that ECG or if its probability is above 0.6.

## Model

Conv1D(32, 5) > MaxPool > Conv1D(64, 3) > MaxPool > Flatten > Dense(5, sigmoid). Trained with Adam for 10 epochs at 100 Hz sampling.

## Data

Uses [PTB-XL](https://physionet.org/content/ptb-xl/) from PhysioNet. The data is not included here. Download it, then point the script at the folder. The loading and label aggregation follow the example code published with the dataset.

Split: records with `strat_fold` 1 to 5 are used for training and 6 to 10 for testing.

## Run

```
pip install -r requirements.txt
python ecg_classifier.py --data-dir /path/to/ptb-xl/
```

The first run reads every raw record and saves `X_cache.npy` in the data folder so later runs start faster. The script prints dataset statistics (cases, age, sex and weight per class), training metrics, a confusion matrix per class, and sensitivity and specificity for each class.

## Results

_Add your numbers and a confusion matrix or training plot here._

## Notes

- Training uses batch size 1, which is slow. A larger batch size would speed it up.
- Sensitivity and specificity are computed from the combined "most likely class or above threshold" predictions.
