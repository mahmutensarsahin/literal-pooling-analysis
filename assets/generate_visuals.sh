#!/bin/bash

# PNG Görselleri Üretme Scripti (Veri Odaklı, Sade)
# Sadece metrik toplanması ve PNG grafiklerin oluşturulması

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

echo "🎨 PNG görselleri üretiliyor (sade mod)..."


# Çıktı klasörü (Python çıktılarıyla uyumlu): assets/plots/test_diagrams
DIAGRAMS_DIR="./plots/test_diagrams"
mkdir -p "$DIAGRAMS_DIR"



# 1) Metrikleri topla (CSV üret)
echo "📈 Metrikler toplanıyor..."
if [[ -x "../scripts/collect_metrics.sh" ]]; then
    bash ../scripts/collect_metrics.sh || echo "⚠️  Metrik toplanırken hata: mevcut CSV varsa devam edilecek."
else
    echo "ℹ️  ../scripts/collect_metrics.sh bulunamadı; mevcut CSV ile devam ediliyor."
fi

# 2) Python yorumlayıcıyı belirle
PYBIN="../.venv/bin/python"
if [[ ! -x "$PYBIN" ]]; then
    if command -v python3 >/dev/null 2>&1; then
        PYBIN="python3"
    else
        PYBIN="python"
    fi
fi

# 3) Statik diyagram PNG'leri
# 3) Veri odaklı grafikler (CSV varsa)
if [[ -f "../metrics/results.csv" ]]; then
    "$PYBIN" metrics_to_png.py || true
    "$PYBIN" real_performance_generator.py || true
else
    echo "ℹ️  CSV yok: ../metrics/results.csv — veri odaklı grafikler atlandı."
fi

# 5) Güvenlik: Eğer çalışma dizininde PNG oluştuysa test diagrams içine taşı (geriye dönük uyum)
echo "📦 PNG dosyaları '$DIAGRAMS_DIR' klasörüne yerleştiriliyor..."
for f in real_performance_results.png detailed_performance_analysis.png performance_from_metrics.png; do
    if [[ -f "$f" ]]; then mv -f "$f" "$DIAGRAMS_DIR/"; fi
done

echo "✅ Tamamlandı. PNG'ler: $DIAGRAMS_DIR/"