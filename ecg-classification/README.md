# ECG Classification (PTB-XL)

A 1D convolutional neural network that predicts five diagnostic superclasses from 12-lead ECG recordings:

`NORM` (normal), `MI` (myocardial infarction), `CD` (conduction disturbance), `STTC` (ST/T change), `HYP` (hypertrophy)

A recording can carry more than one label, so the output layer is sigmoid with binary cross-entropy loss. A class is predicted if it is the most likely one for that ECG or if its probability is above 0.6.

## Model

Conv1D(32, 5) > MaxPool > Conv1D(64, 3) > MaxPool > Flatten > Dense(5, sigmoid). Input is 12 leads x 1000 samples (10 seconds at 100 Hz). Trained with Adam for 10 epochs.

## Data

Uses [PTB-XL](https://physionet.org/content/ptb-xl/) from PhysioNet (21,799 ECGs). The data is not included here. Download it, then point the script at the folder. The loading and label aggregation follow the example code published with the dataset.

| NORM | MI | CD | STTC | HYP |
|---|---|---|---|---|
| 9,514 | 5,469 | 4,898 | 5,235 | 2,649 |

Split: records with `strat_fold` 1 to 5 are used for training and 6 to 10 for testing.

## Run

```
pip install -r requirements.txt
python ecg_classifier.py --data-dir /path/to/ptb-xl/
```

The first run reads every raw record and saves `X_cache.npy` in the data folder so later runs start faster. The script prints dataset statistics (cases, age, sex and weight per class), training metrics, a confusion matrix per class, and sensitivity and specificity for each class.

## Results

The script as written (no metadata, 50/50 split, batch size 1) reached 0.8237 accuracy and trained in 327 s. This accuracy is Keras binary accuracy, averaged over all five labels for every ECG, so it looks better than the per-class numbers below.

| | NORM | MI | CD | STTC | HYP |
|---|---|---|---|---|---|
| Sensitivity | 0.879 | 0.452 | 0.526 | 0.598 | 0.221 |
| Specificity | 0.729 | 0.936 | 0.908 | 0.862 | 0.977 |

The model finds normal ECGs well but misses many disease cases, especially HYP, the rarest class. For a screening or decision-support tool, sensitivity matters most, since a missed cardiac condition can be fatal. These numbers are far below a bar like sensitivity of 0.95 or higher.

Experiments, with patient weight added as an extra input channel:

| Change | Training time | Accuracy |
|---|---|---|
| 10% train / 90% test | 70 s | 0.7608 |
| 50% / 50% | 284 s | 0.7895 |
| 90% / 10% | 568 s | 0.7939 |
| 50% / 50%, 32 epochs | 875 s | 0.7835 |
| 50% / 50%, 64 filters in both layers | 419 s | 0.7883 |
| 50% / 50%, batch size 32 | 106 s | 0.7835 |

- More training data helped the most (10% to 50% gave +2.9 points), then flattened out (50% to 90% gave +0.4).
- More epochs, more filters and a bigger batch did not improve accuracy. Batch size 32 cut training time from 284 s to 106 s at nearly the same accuracy, so it is the best speed/accuracy tradeoff.
- Adding weight hurt, dropping accuracy from 0.8237 to 0.7895. Only 9,421 of 21,799 patients had a recorded weight, and the missing values were filled with 0, which likely created a false link between weight 0 and certain diagnoses.

## Notes

- Training uses batch size 1, which is slow. See the batch size result above.
- Sensitivity and specificity are computed from the combined "most likely class or above threshold" predictions.
