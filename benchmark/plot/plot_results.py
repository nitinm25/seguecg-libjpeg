#!/usr/bin/env python3
import re
import numpy as np
import matplotlib.pyplot as plt

label_map = {
    'basic_nosimd': 'Basic\n (Function calls)',
    'rlbox_noop': 'RLBox Noop',
    'rlbox_wasm2c': 'RLBox wasm2c',
    'sa_wasm2c_guardpage': 'SA wasm2c\n(guardpage)',
    'sa_wasm2c_boundscheck': 'SA wasm2c\n(boundscheck)',
    'sa_wasm2c_watch': 'SA wasm2c\n(watch)',
    'ipc_socket': 'IPC\n(Domain Sockets)'
}

benchmarks_to_show = [
    'basic_nosimd',
    'ipc_socket',
    'rlbox_wasm2c',
    'sa_wasm2c_guardpage',
    'sa_wasm2c_watch',
    'sa_wasm2c_boundscheck',
]

# Parse the result file
data = {}
current_key = None

with open('result.txt', 'r') as f:
    for line in f:
        line = line.strip()
        if line.startswith('[') and line.endswith(':'):
            current_key = line[1:-2]  # Remove [ and ]:
            data[current_key] = []
        elif line.startswith('JPEG recoding time:'):
            time = int(line.split(':')[1].strip())
            data[current_key].append(time)

# Calculate mean and std for each benchmark
raw_data = {}
for key in data.keys():
    times = np.array(data[key]) / 1e6  # Convert nanoseconds to milliseconds
    raw_data[key] = {
        'mean': np.mean(times),
        'std': np.std(times)
    }

# Build ordered lists based on benchmarks_to_show
labels = []
means = []
stds = []

for benchmark in benchmarks_to_show:
    if benchmark in raw_data:
        labels.append(label_map.get(benchmark, benchmark))
        means.append(raw_data[benchmark]['mean'])
        stds.append(raw_data[benchmark]['std'])
    else:
        print(f"Warning: Benchmark '{benchmark}' not found in result.txt")

# Create bar plot
fig, ax = plt.subplots(figsize=(9, 4))
x = np.arange(len(labels))
# bars = ax.bar(x, means, yerr=stds, capsize=5, alpha=0.8, color='steelblue', 
#                error_kw={'elinewidth': 1, 'capthick': 1})
# bars = ax.bar(x, means, capsize=5, alpha=0.8, color='steelblue')
colors = ['#d1615d' if benchmark == 'ipc_socket' else '#5778a4' for benchmark in benchmarks_to_show if benchmark in raw_data]
bars = ax.bar(x, means, capsize=5, color=colors)

# Customize plot
ax.set_ylabel('Time (ms)', fontsize=12)
ax.set_title('JPEG Recoding')
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.grid(axis='y', alpha=0.3, linestyle='--')
ax.set_ylim(0, 110)

# Add value labels on bars
for i, (bar, mean, std) in enumerate(zip(bars, means, stds)):
    height = bar.get_height()
    ax.text(bar.get_x() + bar.get_width()/2., height + std,
            f'{mean:.1f}±{std:.1f}',
            ha='center', va='bottom', fontsize=9)

plt.tight_layout()
plt.savefig('benchmark_results.png', dpi=300, bbox_inches='tight')
