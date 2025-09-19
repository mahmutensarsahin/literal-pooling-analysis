#!/usr/bin/env python3
"""
metrics_to_png.py
- Reads metrics/results.csv produced by scripts/collect_metrics.sh
- Generates performance analysis PNGs based on real results

Outputs:
- assets/plots/performance_from_metrics.png
"""
import csv
import os
from pathlib import Path
import matplotlib.pyplot as plt

REPO_ROOT = Path(__file__).resolve().parents[1]
METRICS_DIR = REPO_ROOT / 'metrics'
ASSETS_DIR = REPO_ROOT / 'assets'
CSV_PATH = METRICS_DIR / 'results.csv'
PERF_OUT = METRICS_DIR / 'perf_test.out'
ASSETS_DIR = REPO_ROOT / 'assets'
PLOTS_DIR = ASSETS_DIR / 'plots'
TEST_DIAGRAMS_DIR = PLOTS_DIR / 'test_diagrams'


def read_metrics(csv_path: Path):
    data = []
    if not csv_path.exists():
        raise FileNotFoundError(f"Metrics CSV not found: {csv_path}")
    with csv_path.open(newline='') as f:
        reader = csv.DictReader(f)
        for row in reader:
            # Normalize types
            try:
                row['value'] = float(row['value'])
            except Exception:
                pass
            data.append(row)
    return data


def index_metrics(rows):
    # Build dictionaries for quick access
    by_metric = {}
    for r in rows:
        key = (r['module'], r['file'], r['binary'], r['metric'])
        by_metric[key] = r
    # Convenience getters for perf_test and optimization_analysis
    def get_perf(name):
        return by_metric.get(('Uygulamalar', 'performance_test.cpp', 'perf_test', name))

    def get_opt(name):
        return by_metric.get(('Ileri', 'optimization_analysis.cpp', 'optimization_analysis', name))

    def get_temel(name):
        return by_metric.get(('Temel', 'duplicate_test.cpp', 'duplicate_test', name))

    return by_metric, get_perf, get_opt, get_temel


def parse_total_ops(perf_out: Path, default_total_ops=50_000_000):
    if not perf_out.exists():
        return default_total_ops
    try:
        iters = None
        strings = None
        with perf_out.open('r') as f:
            for line in f:
                if 'İterasyon sayısı' in line or 'Iterasyon sayısı' in line or 'Iteration' in line:
                    # Extract number
                    digits = ''.join(ch for ch in line if ch.isdigit())
                    if digits:
                        iters = int(digits)
                if 'String sayısı' in line or 'String count' in line:
                    digits = ''.join(ch for ch in line if ch.isdigit())
                    if digits:
                        strings = int(digits)
        if iters and strings:
            return iters * strings
    except Exception:
        pass
    return default_total_ops


def human(value, unit):
    if unit in ('microseconds', 'µs'):
        # Convert to ms if large
        if value >= 1000:
            return f"{value/1000:.1f} ms"
        return f"{value:.0f} µs"
    if unit == 'bytes':
        if value >= 1024*1024:
            return f"{value/1024/1024:.2f} MiB"
        if value >= 1024:
            return f"{value/1024:.1f} KiB"
        return f"{value:.0f} B"
    return f"{value} {unit}"


def plot_from_metrics(rows):
    _, get_perf, get_opt, get_temel = index_metrics(rows)

    # Gather values with safe defaults
    copy = get_perf('copy_duration')
    pool = get_perf('pool_duration')
    strcmp = get_perf('strcmp_duration')
    ptr = get_perf('pointer_duration')
    rodata = get_perf('rodata_size')

    avg_direct = get_opt('avg_direct')
    avg_function = get_opt('avg_function')
    avg_template = get_opt('avg_template')

    pooling_ok = get_temel('literal_pooling_worked')

    # Create figure: 3 rows x 2 cols (adds optimization + pooling panels)
    fig, axes = plt.subplots(3, 2, figsize=(16, 18))
    (ax1, ax2), (ax3, ax4), (ax5, ax6) = axes[0], axes[1], axes[2]

    # 1) Copy vs Pool duration
    labels = []
    durations = []
    units = []
    if copy: labels.append('Copy'); durations.append(copy['value']); units.append(copy['unit'])
    if pool: labels.append('Constant Pool'); durations.append(pool['value']); units.append(pool['unit'])
    if labels:
        bars = ax1.bar(labels, durations, color=['#F44336', '#4CAF50'][:len(labels)])
        ax1.set_title('Processing Duration (perf_test)', fontweight='bold')
        ax1.set_ylabel('microseconds')
        for b, val, u in zip(bars, durations, units):
            ax1.text(b.get_x()+b.get_width()/2, b.get_height()*1.02, human(val, u), ha='center', va='bottom', fontsize=10, fontweight='bold')
        if len(durations) == 2 and durations[1] > 0:
            speedup = durations[0] / durations[1]
            ax1.text(0.5, max(durations)*0.85, f"{speedup:.2f}x faster", ha='center', fontsize=13, color='green', fontweight='bold')
    else:
        ax1.text(0.5,0.5,'No perf_test timings found', ha='center', va='center')
        ax1.axis('off')

    # 2) strcmp vs pointer
    labels = []
    times = []
    units = []
    if strcmp: labels.append('strcmp'); times.append(strcmp['value']); units.append(strcmp['unit'])
    if ptr: labels.append('pointer'); times.append(ptr['value']); units.append(ptr['unit'])
    if labels:
        bars = ax2.bar(labels, times, color=['#FF6B6B', '#4ECDC4'][:len(labels)])
        ax2.set_title('String Comparison Duration', fontweight='bold')
        ax2.set_ylabel('microseconds')
        for b, val, u in zip(bars, times, units):
            ax2.text(b.get_x()+b.get_width()/2, b.get_height()*1.02, human(val, u), ha='center', va='bottom', fontsize=10, fontweight='bold')
        if len(times) == 2 and times[1] > 0:
            speed = times[0]/times[1]
            ax2.text(0.5, max(times)*0.85, f"{speed:.2f}x faster", ha='center', fontsize=13, color='green', fontweight='bold')
    else:
        ax2.text(0.5,0.5,'No comparison timings found', ha='center', va='center')
        ax2.axis('off')

    # 3) Ops per second (derived)
    total_ops = parse_total_ops(PERF_OUT)
    if strcmp and ptr and strcmp['value'] and ptr['value']:
        strcmp_ops_per_sec = total_ops / (strcmp['value'] / 1_000_000.0)
        pointer_ops_per_sec = total_ops / (ptr['value'] / 1_000_000.0)
        bars = ax3.bar(['strcmp', 'pointer'], [strcmp_ops_per_sec/1e6, pointer_ops_per_sec/1e6], color=['#9C27B0', '#03A9F4'])
        ax3.set_title('Throughput (Millions ops/sec)', fontweight='bold')
        for b, val in zip(bars, [strcmp_ops_per_sec, pointer_ops_per_sec]):
            ax3.text(b.get_x()+b.get_width()/2, (val/1e6)*1.02, f"{val/1e6:.2f}M", ha='center', va='bottom', fontsize=10, fontweight='bold')
    else:
        ax3.text(0.5,0.5,'Ops/sec unavailable', ha='center', va='center')
        ax3.axis('off')

    # 4) Memory footprint
    if rodata:
        ro = rodata['value']
        # Approximate buffer approach: 50 strings * 256 buffer (from perf_test) = 12800 bytes
        buffer_ram = 12800
        labels = ['.rodata\n(strings/data)', 'Buffer RAM (hypothetical)']
        vals = [ro, buffer_ram]
        bars = ax4.bar(labels, vals, color=['#2196F3', '#FFC107'])
        ax4.set_title('Memory Footprint', fontweight='bold')
        ax4.set_ylabel('bytes')
        for b, v in zip(bars, vals):
            ax4.text(b.get_x()+b.get_width()/2, v*1.02, human(v, 'bytes'), ha='center', va='bottom', fontsize=10, fontweight='bold')
    else:
        ax4.text(0.5,0.5,'No .rodata size found', ha='center', va='center')
        ax4.axis('off')

    # 5) Optimization micro-benchmark (ns/op)
    if any([avg_direct, avg_function, avg_template]):
        labels, vals = [], []
        colors = []
        if avg_direct:
            labels.append('direct')
            vals.append(float(avg_direct['value']))
            colors.append('#1E88E5')
        if avg_function:
            labels.append('function')
            vals.append(float(avg_function['value']))
            colors.append('#43A047')
        if avg_template:
            labels.append('template')
            vals.append(float(avg_template['value']))
            colors.append('#FB8C00')
        # Avoid degenerate all-zeros by adding tiny epsilon for visualization
        eps = 1e-9
        plot_vals = [v if v > 0 else eps for v in vals]
        bars = ax5.bar(labels, plot_vals, color=colors)
        ax5.set_title('Optimization Analysis (ns/op)', fontweight='bold')
        ax5.set_ylabel('ns')
        # Annotate exact values (with scientific if tiny)
        for b, v in zip(bars, vals):
            label = f"{v:.3g} ns" if v < 0.01 else f"{v:.2f} ns"
            ax5.text(b.get_x()+b.get_width()/2, (max(plot_vals)*0.05 + (v if v > 0 else eps)), label,
                     ha='center', va='bottom', fontsize=10, fontweight='bold')
        # Use log scale if range spans orders of magnitude or zeros
        ax5.set_yscale('log')
    else:
        ax5.text(0.5, 0.5, 'No optimization metrics found', ha='center', va='center')
        ax5.axis('off')

    # 6) Literal pooling indicator
    ax6.set_title('Literal Pooling Deduplication', fontweight='bold')
    if pooling_ok is not None:
        try:
            ok = int(pooling_ok['value']) != 0
        except Exception:
            ok = str(pooling_ok['value']).lower() in ('true', 'yes', '1')
        txt = 'Pooling: YES' if ok else 'Pooling: NO'
        color = '#2E7D32' if ok else '#C62828'
        ax6.text(0.5, 0.5, txt, ha='center', va='center', fontsize=24, fontweight='bold', color=color)
        ax6.axis('off')
    else:
        ax6.text(0.5, 0.5, 'No pooling info found', ha='center', va='center')
        ax6.axis('off')

    plt.tight_layout(rect=[0, 0, 1, 0.97])
    plt.suptitle('Performance Analysis from Real Metrics', fontsize=18, fontweight='bold', y=0.995)
    # Ensure output directory exists
    os.makedirs(TEST_DIAGRAMS_DIR, exist_ok=True)
    OUTPUT_PNG = TEST_DIAGRAMS_DIR / 'performance_from_metrics.png'
    plt.savefig(OUTPUT_PNG, dpi=300, bbox_inches='tight')
    print(f"✅ Saved: {OUTPUT_PNG}")


if __name__ == '__main__':
    rows = read_metrics(CSV_PATH)
    plot_from_metrics(rows)
