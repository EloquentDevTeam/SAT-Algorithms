#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import sys

args = sys.argv
plt.rcParams['text.usetex'] = True

# Load the CSV data
df = pd.read_csv(args[1])

# Calculate rolling average of memory usage
df['memory_avg'] = df['memory (kb)'].rolling(window=10, min_periods=1).mean()

# Set up the main figure and axis
fig, ax1 = plt.subplots(figsize=(12, 6))

# Plot time
ax1.set_xlabel('Teste')
ax1.set_ylabel('Timp ($\\mu s$)', color='tab:blue')
line1, = ax1.plot(df['test'], df['time (μs)'], color='tab:blue', label='Timp ($\\mu s$)')
ax1.set_ylim(0, 6e8)
ax1.tick_params(axis='y', labelcolor='tab:blue')

# Plot memory
ax2 = ax1.twinx()
ax2.set_ylabel('Memorie (kb)', color='tab:green')
line2, = ax2.plot(df['test'], df['memory (kb)'], color='tab:green', label='Memorie (kb)')
line3, = ax2.plot(df['test'], df['memory_avg'], color='tab:olive', linestyle='--', label='Memorie (medie)')
ax2.tick_params(axis='y', labelcolor='tab:green')

# Plot clauses
ax3 = ax1.twinx()
ax3.spines['right'].set_position(('outward', 60))
ax3.set_ylabel('Clauses', color='tab:red')
line4, = ax3.plot(df['test'], df['clauses'], color='tab:red', label='Clauze')
ax3.tick_params(axis='y', labelcolor='tab:red')

# Make legend
handles = [line1, line2, line3, line4]
labels = [line.get_label() for line in handles]
fig.subplots_adjust(bottom=0.2)
plt.legend(handles=handles, labels=labels, loc='upper center', bbox_to_anchor=(0.5, -0.15), ncol=2)

# Finalize plot
plt.title(args[3])
plt.savefig(args[2], dpi=300, bbox_inches='tight')
