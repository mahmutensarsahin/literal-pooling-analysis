#!/usr/bin/env python3
"""
Gerçek Performans Sonuçları Grafikleri (Veri Odaklı)
- metrics/results.csv dosyasından okunur (scripts/collect_metrics.sh üretir)
- perf_test.out dosyasından toplam işlem sayısı (ops) türetilir (varsa)

Üretilen dosyalar (assets klasöründe oluşturulup generate_visuals.sh ile plots/ altına taşınır):
- real_performance_results.png
- detailed_performance_analysis.png
"""

import csv
from pathlib import Path
import matplotlib.pyplot as plt

REPO_ROOT = Path(__file__).resolve().parents[1]
METRICS_DIR = REPO_ROOT / 'metrics'
CSV_PATH = METRICS_DIR / 'results.csv'
# perf_test.out farklı konumlarda olabilir
PERF_OUT_CANDIDATES = [
    METRICS_DIR / 'perf_test.out',
    METRICS_DIR / 'performance_test' / 'perf_test.out',
]
ASSETS_DIR = REPO_ROOT / 'assets'
PLOTS_DIR = ASSETS_DIR / 'plots'
TEST_DIAGRAMS_DIR = PLOTS_DIR / 'test_diagrams'


def read_metrics(csv_path: Path):
    rows = []
    if not csv_path.exists():
        raise FileNotFoundError(f"Metrics CSV not found: {csv_path}")
    with csv_path.open(newline='') as f:
        reader = csv.DictReader(f)
        for r in reader:
            try:
                r['value'] = float(r['value'])
            except Exception:
                pass
            rows.append(r)
    return rows


def mget(rows, module, file, binary, metric):
    for r in rows:
        if r['module'] == module and r['file'] == file and r['binary'] == binary and r['metric'] == metric:
            return r
    return None


def parse_total_ops():
    # Varsayılan: 1_000_000 iterasyon * 50 string = 50M ops
    default_total_ops = 50_000_000
    for p in PERF_OUT_CANDIDATES:
        if not p.exists():
            continue
        try:
            iters = None
            strings = None
            for line in p.read_text().splitlines():
                if 'Iteration' in line or 'İterasyon' in line:
                    digits = ''.join(ch for ch in line if ch.isdigit())
                    if digits:
                        iters = int(digits)
                if 'String count' in line or 'String sayısı' in line:
                    digits = ''.join(ch for ch in line if ch.isdigit())
                    if digits:
                        strings = int(digits)
            if iters and strings:
                return iters * strings
        except Exception:
            pass
    return default_total_ops


def create_real_performance_comparison(rows):
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))

    # 1) String karşılaştırma süreleri (µs) — CSV'den
    strcmp = mget(rows, 'Uygulamalar', 'performance_test.cpp', 'perf_test', 'strcmp_duration')
    pointer = mget(rows, 'Uygulamalar', 'performance_test.cpp', 'perf_test', 'pointer_duration')
    times = []
    labels = []
    if strcmp:
        labels.append('strcmp()')
        times.append(float(strcmp['value']))
    if pointer:
        labels.append('pointer')
        times.append(float(pointer['value']))
    if times:
        bars = ax1.bar(labels, times, color=['#F44336', '#4CAF50'][:len(times)])
        ax1.set_title('String Karşılaştırma Süresi (µs)', fontweight='bold')
        for b, v in zip(bars, times):
            ax1.text(b.get_x()+b.get_width()/2, v*1.02, f"{int(v)} µs", ha='center', va='bottom', fontweight='bold')
        if len(times) == 2 and times[1] > 0:
            ax1.text(0.5, max(times)*0.85, f"{times[0]/times[1]:.2f}x daha hızlı",
                     ha='center', fontsize=13, color='green', fontweight='bold')
    else:
        ax1.text(0.5, 0.5, 'Veri bulunamadı', ha='center', va='center')
        ax1.axis('off')

    # 2) Ops/sec (CSV + perf_test.out)
    total_ops = parse_total_ops()
    if strcmp and pointer:
        ops = [total_ops / (float(strcmp['value']) / 1_000_000.0),
               total_ops / (float(pointer['value']) / 1_000_000.0)]
        bars = ax2.bar(['strcmp()', 'pointer'], [v/1e6 for v in ops], color=['#9C27B0', '#03A9F4'])
        ax2.set_title('Throughput (M ops/sec)', fontweight='bold')
        for b, v in zip(bars, ops):
            ax2.text(b.get_x()+b.get_width()/2, (v/1e6)*1.02, f"{v/1e6:.2f}M", ha='center', va='bottom', fontweight='bold')
    else:
        ax2.text(0.5, 0.5, 'Ops/sec için veri yok', ha='center', va='center')
        ax2.axis('off')

    # 3) Bellek ayak izi — .rodata vs Buffer RAM (varsayımsal 50*256)
    ro = mget(rows, 'Uygulamalar', 'performance_test.cpp', 'perf_test', 'rodata_size')
    ro_b = float(ro['value']) if ro else 0.0
    buffer_ram = 50 * 256  # perf_test senaryosuna göre
    bars = ax3.bar(['.rodata', 'Buffer RAM'], [ro_b, buffer_ram], color=['#2196F3', '#FFC107'])
    ax3.set_title('Bellek Ayak İzi (Bytes)', fontweight='bold')
    for b, v in zip(bars, [ro_b, buffer_ram]):
        ax3.text(b.get_x()+b.get_width()/2, v*1.02, f"{int(v)} B", ha='center', va='bottom', fontweight='bold')

    # 4) Bellek dağılımı pie — .rodata vs Buffer RAM
    sizes = [ro_b, buffer_ram]
    labels = ['.rodata', 'Buffer RAM']
    colors = ['#2196F3', '#FFC107']
    explode = (0, 0.08)
    ax4.pie(sizes, explode=explode, labels=labels, colors=colors, autopct='%1.1f%%',
            shadow=True, startangle=90)
    ax4.set_title('Bellek Dağılımı', fontweight='bold')

    plt.tight_layout()
    plt.suptitle('C/C++ Sabit Havuzu — Gerçek Sonuçlar (CSV)', fontsize=18, fontweight='bold', y=0.98)
    TEST_DIAGRAMS_DIR.mkdir(parents=True, exist_ok=True)
    out = TEST_DIAGRAMS_DIR / 'real_performance_results.png'
    plt.savefig(out, dpi=300, bbox_inches='tight')
    print(f"✅ Kaydedildi: {out}")


def create_detailed_analysis(rows):
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))

    # 1) perf_test: copy vs pool (µs) — log ölçek
    copy_d = mget(rows, 'Uygulamalar', 'performance_test.cpp', 'perf_test', 'copy_duration')
    pool_d = mget(rows, 'Uygulamalar', 'performance_test.cpp', 'perf_test', 'pool_duration')
    vals = []
    labels = []
    if copy_d:
        labels.append('Copy')
        vals.append(float(copy_d['value']))
    if pool_d:
        labels.append('Constant Pool')
        vals.append(float(pool_d['value']))
    if vals:
        ax1.set_yscale('log')
        bars = ax1.bar(labels, vals, color=['#EF5350', '#66BB6A'][:len(vals)])
        ax1.set_title('Processing Duration (log µs)', fontweight='bold')
        for b, v in zip(bars, vals):
            ax1.text(b.get_x()+b.get_width()/2, v*1.02, f"{int(v)} µs", ha='center', va='bottom', fontweight='bold')
    else:
        ax1.text(0.5, 0.5, 'Veri yok', ha='center', va='center')
        ax1.axis('off')

    # 2) optimization_analysis: ns/op log ölçek
    od = mget(rows, 'Ileri', 'optimization_analysis.cpp', 'optimization_analysis', 'avg_direct')
    ofn = mget(rows, 'Ileri', 'optimization_analysis.cpp', 'optimization_analysis', 'avg_function')
    otm = mget(rows, 'Ileri', 'optimization_analysis.cpp', 'optimization_analysis', 'avg_template')
    labs, nsvals = [], []
    for name, rec in [('direct', od), ('function', ofn), ('template', otm)]:
        if rec:
            labs.append(name)
            nsvals.append(max(float(rec['value']), 1e-9))
    if nsvals:
        ax2.set_yscale('log')
        bars = ax2.bar(labs, nsvals, color=['#1E88E5', '#43A047', '#FB8C00'][:len(nsvals)])
        ax2.set_title('Optimization Analysis (ns/op, log)', fontweight='bold')
        for b, v in zip(bars, nsvals):
            ax2.text(b.get_x()+b.get_width()/2, v*1.1, f"{v:.3g}", ha='center', va='bottom', fontweight='bold')
    else:
        ax2.text(0.5, 0.5, 'Optimizasyon verisi yok', ha='center', va='center')
        ax2.axis('off')

    # 3) web_server: total time (ms)
    wt_inf = mget(rows, 'Uygulamalar', 'web_server.cpp', 'web_server', 'inefficient_total_time')
    wt_opt = mget(rows, 'Uygulamalar', 'web_server.cpp', 'web_server', 'optimized_total_time')
    wt_pool = mget(rows, 'Uygulamalar', 'web_server.cpp', 'web_server', 'pooled_total_time')
    labs, msvals = [], []
    for name, rec in [('Inefficient', wt_inf), ('Optimized', wt_opt), ('Pooled', wt_pool)]:
        if rec:
            labs.append(name)
            msvals.append(float(rec['value']))
    if msvals:
        bars = ax3.bar(labs, msvals, color=['#E53935', '#8E24AA', '#039BE5'][:len(msvals)])
        ax3.set_title('Web Server Total Time (ms)', fontweight='bold')
        for b, v in zip(bars, msvals):
            ax3.text(b.get_x()+b.get_width()/2, v*1.05, f"{v:.0f} ms", ha='center', va='bottom', fontweight='bold')
    else:
        ax3.text(0.5, 0.5, 'Web server verisi yok', ha='center', va='center')
        ax3.axis('off')

    # 4) Literal pooling YES/NO
    pool_ok = mget(rows, 'Temel', 'duplicate_test.cpp', 'duplicate_test', 'literal_pooling_worked')
    if pool_ok:
        try:
            ok = int(pool_ok['value']) != 0
        except Exception:
            ok = str(pool_ok['value']).lower() in ('true', 'yes', '1')
        txt = 'Literal Pooling: YES' if ok else 'Literal Pooling: NO'
        color = '#2E7D32' if ok else '#C62828'
        ax4.text(0.5, 0.5, txt, ha='center', va='center', fontsize=22, fontweight='bold', color=color)
        ax4.axis('off')
    else:
        ax4.text(0.5, 0.5, 'Literal pooling bilgisi yok', ha='center', va='center')
        ax4.axis('off')

    plt.tight_layout()
    plt.suptitle('Detaylı Performans Analizi (CSV)', fontsize=16, fontweight='bold', y=0.98)
    TEST_DIAGRAMS_DIR.mkdir(parents=True, exist_ok=True)
    out = TEST_DIAGRAMS_DIR / 'detailed_performance_analysis.png'
    plt.savefig(out, dpi=300, bbox_inches='tight')
    print(f"✅ Kaydedildi: {out}")


if __name__ == "__main__":
    print("🎯 Gerçek test sonuçlarına dayalı grafikler oluşturuluyor (CSV)...")
    rows = read_metrics(CSV_PATH)
    create_real_performance_comparison(rows)
    create_detailed_analysis(rows)
    print("\n✅ Tüm grafikler veri odaklı olarak üretildi.")