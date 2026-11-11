"""1D CNN for multi-label ECG diagnosis on the PTB-XL dataset.

Predicts five diagnostic superclasses (NORM, MI, CD, STTC, HYP) from 12-lead
ECG at 100 Hz, then reports per-class sensitivity and specificity.

Dataset: PTB-XL (PhysioNet). Download it separately and pass its folder with
--data-dir. The record-loading and label-aggregation steps follow the example
code published with the dataset.

Usage:
    python ecg_classifier.py --data-dir /path/to/ptb-xl/
"""
import argparse
import ast
import os
import time

import numpy as np
import pandas as pd
import tensorflow as tf
import wfdb
from sklearn.metrics import multilabel_confusion_matrix
from tensorflow import keras
from tensorflow.keras.layers import Conv1D, Dense, Flatten, MaxPooling1D

SAMPLING_RATE = 100
# NORM = normal ECG, MI = myocardial infarction, CD = conduction disturbance,
# STTC = ST/T change, HYP = hypertrophy
CLASSES = ["NORM", "MI", "CD", "STTC", "HYP"]
# strat_fold is a 1-10 split label assigned by the PTB-XL authors.
# Folds <= TRAIN_FOLD_MAX are used for training, the rest for testing.
TRAIN_FOLD_MAX = 5
EPOCHS = 10
# A class is also predicted when its probability exceeds this threshold,
# in addition to the single most likely class per ECG.
THRESHOLD = 0.6


def load_raw_data(df, sampling_rate, path):
    """Read the waveform files listed in df and return an array of signals."""
    column = df.filename_lr if sampling_rate == 100 else df.filename_hr
    data = [wfdb.rdsamp(os.path.join(path, f)) for f in column]
    return np.array([signal for signal, _meta in data])


def load_signals(df, path):
    """Load signals, using a cached .npy next to the dataset when available."""
    cache = os.path.join(path, "X_cache.npy")
    if os.path.exists(cache):
        print("Loaded signals from cache")
        return np.load(cache)
    X = load_raw_data(df, SAMPLING_RATE, path)
    np.save(cache, X)
    print("Loaded signals from raw files, cache saved")
    return X


def load_labels(path):
    """Return the metadata table and a binary label matrix (one column per class)."""
    Y = pd.read_csv(os.path.join(path, "ptbxl_database.csv"), index_col="ecg_id")
    Y.scp_codes = Y.scp_codes.apply(ast.literal_eval)

    agg_df = pd.read_csv(os.path.join(path, "scp_statements.csv"), index_col=0)
    agg_df = agg_df[agg_df.diagnostic == 1]

    def aggregate_diagnostic(codes):
        found = [agg_df.loc[k].diagnostic_class for k in codes if k in agg_df.index]
        return list(set(found))

    Y["diagnostic_superclass"] = Y.scp_codes.apply(aggregate_diagnostic)

    Z = pd.DataFrame(0, index=Y.index, columns=CLASSES, dtype="int")
    for i in Z.index:
        for k in Y.loc[i].diagnostic_superclass:
            Z.loc[i, k] = 1
    return Y, Z


def print_dataset_stats(Y, Z):
    """Cases per class, plus age, sex and weight breakdown for each class."""
    print(Z.sum())
    for c in CLASSES:
        # PTB-XL stores ages over 89 as 300, so exclude those from the mean
        mask = (Z[c] == 1) & (Y.age != 300)
        print(c, "age mean/std:", Y.age[mask].mean(), Y.age[mask].std())
    for c in CLASSES:
        print(c)
        print(Y.sex[Z[c] == 1].value_counts())
    for c in CLASSES:
        mask = (Z[c] == 1) & (Y.weight.notna())
        w = Y.weight[mask]
        print(c, "weight mean/std/n:", w.mean(), w.std(), len(w))


def build_model(n_classes):
    """Two Conv1D + MaxPool blocks, then a sigmoid output (multi-label)."""
    model = keras.Sequential([
        Conv1D(32, 5, activation="relu"),
        MaxPooling1D(2),
        Conv1D(64, 3, activation="relu"),
        MaxPooling1D(2),
        Flatten(),  # 2D feature map -> 1D vector for the dense layer
        Dense(n_classes, activation="sigmoid"),
    ])
    model.compile(
        optimizer="adam",
        loss="binary_crossentropy",
        metrics=["binary_accuracy", "Precision", "Recall"],
    )
    return model


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--data-dir", default=os.environ.get("PTBXL_PATH", "data/ptb-xl"),
                        help="PTB-XL folder (or set PTBXL_PATH)")
    args = parser.parse_args()

    Y, Z = load_labels(args.data_dir)
    X = load_signals(Y, args.data_dir)
    print_dataset_stats(Y, Z)

    X = np.nan_to_num(X)  # NaN -> 0

    train = (Y.strat_fold <= TRAIN_FOLD_MAX).values
    X_train, y_train = X[train], Z[train]
    X_test, y_test = X[~train], Z[~train]
    print("X_train shape:", X_train.shape)
    print("X_test shape:", X_test.shape)

    train_ds = tf.data.Dataset.from_tensor_slices((X_train, y_train)).batch(1)
    test_ds = tf.data.Dataset.from_tensor_slices((X_test, y_test)).batch(1)

    model = build_model(Z.shape[-1])
    start = time.time()
    model.fit(train_ds, epochs=EPOCHS)
    print(f"Training time: {time.time() - start:.1f} s")
    print(model.evaluate(test_ds, return_dict=True))

    # Predicted labels = most likely class OR any class above the threshold
    probs = model.predict(X_test)
    top_class = np.zeros(probs.shape, dtype=int)
    top_class[np.arange(len(probs)), np.argmax(probs, axis=1)] = 1
    predicted = top_class | (probs > THRESHOLD).astype(int)

    mlcm = multilabel_confusion_matrix(y_test, predicted)
    print(mlcm)
    for i, c in enumerate(CLASSES):
        tn, fp, fn, tp = mlcm[i].ravel()
        print(c, "sensitivity:", tp / (tp + fn), "specificity:", tn / (tn + fp))


if __name__ == "__main__":
    main()
