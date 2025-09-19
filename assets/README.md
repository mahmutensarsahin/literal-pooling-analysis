# 🎨 Görsel Materyaller

Bu klasör, ders boyunca toplanan gerçek metriklerden üretilen veri odaklı grafiklerin çıktısını barındırır. Statik/elle çizilen diyagramlar artık otomatik üretilmez.

## 📂 Çıktı Klasörü

- `plots/test_diagrams/`
    - performance_from_metrics.png
    - real_performance_results.png
    - detailed_performance_analysis.png

Tüm PNG’ler, `metrics/results.csv` dosyasındaki gerçek çalıştırma sonuçlarından üretilir.

## ⚙️ Nasıl Üretilir?

Oluşturma komutları bu klasördeki betiklerle otomatikleştirilmiştir. Betik, önce metrikleri toplar sonra grafikleri üretir.

```bash
cd assets
chmod +x ./generate_visuals.sh
./generate_visuals.sh
```

Notlar:
- Betik çalıştığında `metrics/` klasörünü yeniden oluşturur ve `metrics/results.csv` dosyasını üretir.
- Grafikler yalnızca bu CSV’deki verilere göre çizilir; sabit/değer gömülü görseller yoktur.

## ✅ Gereksinimler

Python tarafı için yalnızca Matplotlib yeterlidir:
```bash
pip install matplotlib
```

Ek araç gerektirmez (Graphviz vb. gerekli değildir).

## 🔗 İlgili Belgeler

- ../01_Giris/README.md
- ../02_Temel_Kavramlar/README.md
- ../03_Ileri_Kavramlar/README.md
- ../04_Uygulamalar/README.md

