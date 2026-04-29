import re
import numpy as np
import matplotlib.pyplot as plt

RESULT_FILE = "/home/cc/seguecg-libjpeg/benchmark/plot/result-diff.txt"
OUTPUT_FILE = "/home/cc/seguecg-libjpeg/benchmark/plot/result-diff.png"

# --- Labels (key = name in result file, value = bar label) ---

LABELS = {
    "basic_nosimd":      "basic",
    "rlbox_wasm2c":      "wasm",
    "rlbox_ipc_socket":  "socket",
    "rlbox_ipc_futex":   "futex",
    "rlbox_ipc_spin":    "spin",
    "rlbox_ipc_dynamic": "dynamic",
}

# --- Colors ---

COLOR_NATIVE = "#4C72B0"  # function call baseline
COLOR_SFI    = "#DD8452"  # SFI (wasm2c)
COLOR_IPC    = "#55A868"  # IPC schemes

COLORS = {
    "basic_nosimd":      COLOR_NATIVE,
    "rlbox_wasm2c":      COLOR_SFI,
    "rlbox_ipc_socket":  COLOR_IPC,
    "rlbox_ipc_futex":   COLOR_IPC,
    "rlbox_ipc_spin":    COLOR_IPC,
    "rlbox_ipc_dynamic": COLOR_IPC,
}

# --- Bar order ---

ORDER = [
    "basic_nosimd",
    "rlbox_ipc_spin",
    "rlbox_ipc_dynamic",
    "rlbox_ipc_futex",
    "rlbox_wasm2c",
    "rlbox_ipc_socket",
]

# --- Parse ---

def parse_results(path):
    data = {}
    current = None
    with open(path) as f:
        for line in f:
            line = line.strip()
            m = re.match(r'^\[(.+)\]:$', line)
            if m:
                current = m.group(1)
                data[current] = []
            elif line.startswith("JPEG recoding time:") and current:
                ns = int(line.split(":")[1].strip())
                data[current].append(ns / 1e6)  # ns -> ms
    return data

# --- Plot ---

data = parse_results(RESULT_FILE)

means  = [np.mean(data[k]) for k in ORDER]
stds   = [np.std(data[k])  for k in ORDER]
labels = [LABELS[k]        for k in ORDER]
colors = [COLORS[k]        for k in ORDER]

x = np.arange(len(ORDER))

fig, ax = plt.subplots(figsize=(6, 3))

ax.bar(x, means, yerr=stds, capsize=4,
       color=colors, edgecolor="white", linewidth=0.5,
       error_kw=dict(elinewidth=1.2, ecolor="black", capthick=1.2))

ax.set_xticks(x)
ax.set_xticklabels(labels, rotation=0, ha="center")
ax.set_ylabel("Time (ms)")
# ax.set_title("JPEG Recoding Time — Different Cores")
ax.yaxis.grid(True, linestyle="-", alpha=0.3)
ax.set_axisbelow(True)

from matplotlib.patches import Patch
legend_elements = [
    Patch(facecolor=COLOR_NATIVE, label="Direct Functions"),
    Patch(facecolor=COLOR_SFI,    label="In-process"),
    Patch(facecolor=COLOR_IPC,    label="Cross-process"),
]
ax.legend(handles=legend_elements, loc="upper left")

plt.tight_layout()
plt.savefig(OUTPUT_FILE, dpi=150)
print(f"Saved to {OUTPUT_FILE}")
