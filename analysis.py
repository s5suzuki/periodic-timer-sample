'''
File: analysis.py
Project: Timer-test
Created Date: 02/08/2022
Author: Shun Suzuki
-----
Last Modified: 03/08/2022
Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
-----
Copyright (c) 2022 Shun Suzuki. All rights reserved.

'''

import os
import numpy as np
import csv
import pandas as pd
import sys
import matplotlib.pyplot as plt

if __name__ == '__main__':
    timer_names = []
    intervals = []
    path = sys.argv[1]
    with open(path) as f:
        reader = csv.reader(f)
        for line in reader:
            name = line[0].strip()
            interval = line[1].strip()
            if name not in timer_names:
                timer_names.append(name)
            if interval not in intervals:
                intervals.append(interval)

    data = {}
    n = 0
    for timer in timer_names:
        df = pd.DataFrame(columns=intervals)
        with open(path) as f:
            reader = csv.reader(f)
            for line in reader:
                name = line[0].strip()
                if name != timer:
                    continue
                interval = line[1].strip()
                start = int(line[2])
                df[interval] = list(map(lambda v: int(v) - start, line[2:]))
        data[timer] = df
        n = len(df.index)

    cmap = plt.get_cmap("tab10")
    markers = ['s', 'o', 'p', "v", "^", "<", ">"]
    markersize = [10, 9, 8, 7, 6, 5, 4]

    dirname = os.path.splitext(os.path.basename(path))[0]
    os.makedirs(dirname, exist_ok=True)
    for intv in intervals:
        intv = int(intv)
        unit = 'ms' if intv >= 1000 * 1000 else 'us'
        intv_div = 1000 * 1000 if intv >= 1000 * 1000 else 1000

        fig = plt.figure(figsize=(6, 6), dpi=300)
        ax = fig.add_subplot(111)
        for i, (key, value) in enumerate(data.items()):
            ax.plot(np.arange(n), value[str(intv)] / intv_div, color=cmap(i))
            ax.plot(np.arange(n)[::100], value[str(intv)][::100] / intv_div, color=cmap(i),
                    linewidth=0, marker=markers[i], label=key, markersize=markersize[i])

        ax.set_title(f'Interval: {intv/intv_div} [{unit}]')
        ax.set_xlabel('Iteration')
        ax.set_ylabel(f'Time [{unit}]')

        ax.set_xlim(0, n - 1)
        ax.set_ylim(0, intv / intv_div * (n - 1))

        ax.legend()

        plt.tight_layout()
        plt.savefig(f'{dirname}/{intv/intv_div}{unit}.svg')
