# 🎯 C/C++ Derleyici Sabit Havuzu ve .rodata Bölümü

> **Kapsamlı Eğitim Materyali** - C/C++ derleyicilerinin sabit havuzu (constant pool) ve .rodata bölümü optimizasyonlarını derinlemesine incelemek için hazırlanmış profesyonel ders serisi.

[![Language](https://img.shields.io/badge/Language-C%2FC%2B%2B-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B)
[![Level](https://img.shields.io/badge/Level-Intermediate%20to%20Advanced-orange.svg)](#)
[![Platform](https://img.shields.io/badge/Platform-Cross--Platform-green.svg)](#)

---

## 📚 Kurs Genel Bakışı

Bu eğitim materyali, C/C++ derleyicilerinin **sabit havuzu (constant pool)** ve **.rodata (read-only data)** bölümü optimizasyonlarını kapsamlı bir şekilde ele almaktadır. Modern derleyicilerin string literalleri, sayısal sabitler ve diğer değişmez değerleri nasıl verimli bir şekilde yönettiğini öğreneceksiniz.

### 🎓 Öğreneceğiniz Ana Konular

- ✅ C/C++ bellek düzeni ve segment yapısı
- ✅ Derleyici sabit havuzu optimizasyonları  
- ✅ .rodata bölümü ve salt okunur veriler
- ✅ String literal deduplication (tekrar eliminasyonu)
- ✅ Performans optimizasyonları ve bellek tasarrufu
- ✅ Assembly seviyesinde analiz teknikleri
- ✅ Embedded systems, oyun geliştirme, web server optimizasyonları

### 👥 Hedef Kitle

- **Intermediate-Advanced C/C++ geliştiriciler**
- **Sistem programcıları**
- **Embedded systems geliştiricileri** 
- **Performance tuning uzmanları**
- **Compiler içeriği hakkında bilgi sahibi olmak isteyenler**

---

## 🗂️ Kurs İçeriği ve Navigasyon

### [📖 1. Giriş Bölümü](01_Giris/README.md)
**Temel kavramlar ve kurs hazırlığı**

- 🎯 Öğrenme hedefleri ve önkoşullar
- 💡 Neden sabit havuzu optimizasyonu önemli?
- 🔧 Geliştirme ortamı kurulumu (GCC, Clang, analiz araçları)
- 📋 Kurs yapısı ve öğrenme yol haritası

### [⚡ 2. Temel Kavramlar](02_Temel_Kavramlar/README.md)
**Sabit havuzu fundamentalleri**

- 🧠 Derleyici sabit havuzu nedir?
- 📊 .rodata bölümü ve bellek düzeni
- 🔄 String literal deduplication
- 💾 Local vs Global sabitler
- **🛠️ [Praktik Örnekler](02_Temel_Kavramlar/examples/)**
  - `duplicate_test.cpp` - Tekrar eliminasyonu analizi
  - `size_comparison.cpp` - Bellek kullanımı karşılaştırması
  - `assembly_analysis.cpp` - Assembly seviyesinde inceleme

### [🚀 3. İleri Kavramlar](03_Ileri_Kavramlar/README.md)
**Optimizasyon stratejileri ve derinlemesine analiz**

- ⚙️ Compiler optimization flags (-O1, -O2, -O3)
- 🔍 Inline vs .rodata yerleştirme kararları
- 📈 Cache performance ve memory layout
- 🧪 Template ve constexpr optimizasyonları
- **🔬 [İleri Örnekler](03_Ileri_Kavramlar/examples/)**
  - `performance_benchmark.cpp` - Performans ölçümleri
  - `memory_layout_analysis.cpp` - Bellek düzeni analizi
  - `optimization_analysis.cpp` - Derleyici optimizasyon karşılaştırması

### [🌍 4. Gerçek Dünya Uygulamaları](04_Uygulamalar/README.md)
**Pratik senaryolar ve case study'ler**

- 🎮 **Game Development:** 60 FPS constraints altında optimizasyon
- 🤖 **Embedded Systems:** Nintendo DS, Arduino memory constraints
- 🌐 **Web Servers:** High-throughput uygulamalarında bellek yönetimi
- 📱 **Mobile Development:** Memory-constrained environment'larda optimizasyon
- **💼 [Uygulama Örnekleri](04_Uygulamalar/src/)**
  - `embedded_systems.cpp` - Mikrocontroller optimizasyonları
  - `game_performance.cpp` - Oyun performans optimizasyonu
  - `web_server.cpp` - Sunucu uygulaması örneği

### [📋 5. Özet ve İpuçları](05_Ozet_ve_Ipuclari.md)
**Best practices ve sorun giderme kılavuzu**

- 🎯 Kritik önemli noktalar
- ✅ Do's and Don'ts
- 🔧 Troubleshooting rehberi
- 📊 Performance metrikleri
- 🧪 Test framework önerileri

---

## 🎨 Görsel Materyaller

### 📊 Veri Odaklı Görseller
Bu depo, yalnızca çalıştırılan örneklerden toplanan gerçek metriklere dayalı PNG grafiklerini üretir. Statik/elle çizilmiş diyagramlar veya ASCII/Graphviz çıktıları bu akışın parçası değildir.

- performance_from_metrics.png
- real_performance_results.png
- detailed_performance_analysis.png

Çıktı klasörü: `assets/plots/test_diagrams/`

**Görselleri üretmek için:**
```bash
cd assets/
./generate_visuals.sh
```

---

## 🚀 Hızlı Başlangıç

### 1️⃣ Ön Koşullar
```bash
# GCC/Clang compiler kurulumu
# macOS
brew install gcc llvm

# Ubuntu/Debian
sudo apt-get install build-essential clang

# Arch Linux
sudo pacman -S gcc clang
```

### 2️⃣ Analiz Araçları
```bash
# Binary analysis tools
objdump --version
nm --version
readelf --version

# Performance profiling (optional)
# macOS: Instruments (Xcode ile gelir)
# Linux: perf, valgrind
sudo apt-get install linux-perf valgrind  # Ubuntu
```

### 3️⃣ İlk Örneği Çalıştır
```bash
# Temel örnek derleme
cd 02_Temel_Kavramlar/examples/
g++ -O2 -c duplicate_test.cpp
objdump -d duplicate_test.o
strings duplicate_test.o
```

### 4️⃣ Görsel Materyalleri Oluştur
```bash
# Python matplotlib grafikleri için
pip install matplotlib

# Tüm görselleri oluştur
cd assets/
./generate_visuals.sh
```

---

## 📈 Öğrenme Yol Haritası

```
1. [📖 Giriş] ────────→ Temel kavramlara hazırlık (30 dk)
                              │
                              ▼
2. [⚡ Temel Kavramlar] ────→ Hands-on örnekler (2 saat)
                              │
                              ▼
3. [🚀 İleri Kavramlar] ────→ Optimizasyon teknikleri (2 saat)
                              │
                              ▼
4. [🌍 Uygulamalar] ────────→ Gerçek senaryolar (1.5 saat)
                              │
                              ▼
5. [📋 Özet] ──────────────→ Mastery validation (30 dk)

💡 Toplam süre: ~6 saat (kendi hızınızda)
```

---

## 🔧 Kullanılan Teknolojiler

| Kategori | Araçlar |
|----------|---------|
| **Compilers** | GCC 11+, Clang 13+, MSVC 2019+ |
| **Analysis Tools** | objdump, nm, readelf, strings |
| **Profiling** | perf, Instruments, Intel VTune |
| **Debugging** | gdb, lldb, Valgrind |
| **Visualization** | Matplotlib |

---

## 🎯 Beklenen Çıktılar

Bu kursu tamamladıktan sonra şunları yapabileceksiniz:

### ✅ Teknik Yetkinlikler
- C/C++ derleyici internal'larını anlama
- Binary analizinde yetkinlik 
- Memory layout optimizasyonu
- Performance profiling becerisi
- Assembly okuma yeteneği

### ✅ Pratik Uygulamalar  
- Embedded systems için memory optimization
- Game development performance tuning
- Web server efficiency improvements
- Mobile app memory management

### ✅ Araç Kullanımı
- `objdump`, `nm`, `readelf` ile binary analysis
- Performance profiling tools (perf, valgrind)
- Compiler flag optimizasyonu
- Memory layout visualization

---

## 📚 Ek Kaynaklar

### 📖 Önerilen Okumalar
- **Modern C++ Design** - Andrei Alexandrescu
- **Optimizing Software in C++** - Agner Fog  
- **Computer Systems: A Programmer's Perspective** - Bryant & O'Hallaron

### 🔗 Yararlı Linkler
- [GCC Optimization Options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
- [Clang Optimization Guide](https://clang.llvm.org/docs/)
- [Intel Architecture Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)

### 🛠️ Online Tools
- [Compiler Explorer (godbolt.org)](https://godbolt.org/) - Assembly analiz
- [Quick-bench.com](https://quick-bench.com/) - C++ performance benchmarking

---

## 📞 Destek ve Katkı

### 🤝 Katkıda Bulunma
Bu eğitim materyali açık kaynaklıdır. Katkılarınızı bekliyoruz:

1. **Bug Reports** - Hataları bildirin
2. **Content Improvements** - İçerik geliştirme önerileri
3. **Additional Examples** - Yeni örnek kodlar
4. **Translation** - Çeviri katkıları

### 💬 İletişim
- **Issues:** GitHub issue tracker kullanın
- **Discussions:** Teknik tartışmalar için

---

## 📄 Lisans

Bu eğitim materyali MIT lisansı altında sunulmaktadır. Detaylar için `LICENSE` dosyasını inceleyiniz.

---

<div align="center">

### 🎓 Başarılar ve İyi Öğrenmeler! 

**"Good programmers know what to write. Great ones know what to rewrite."** - Eric S. Raymond

---

**[⬆️ Başa Dön](#-cc-derleyici-sabit-havuzu-ve-rodata-bölümü)** | **[🚀 Kursa Başla](01_Giris/README.md)**

</div>

## 📊 Ölçülebilir Çıktı Üreten Örnekler

Aşağıdaki dosyalar doğrudan tabloya dökülebilir metrikler üretir. Hepsini tek seferde derleyip çalıştırmak ve sonuçları CSV/Markdown olarak almak için `scripts/collect_metrics.sh` çalıştırabilirsiniz (çıktılar `metrics/` klasörüne yazılır).

- 04_Uygulamalar/src/performance_test.cpp
  - Üretilen metrikler: kopyalama ve constant pool süreleri (µs), strcmp vs pointer karşılaştırma süreleri, tahmini .rodata boyutu
- 03_Ileri_Kavramlar/examples/optimization_analysis.cpp
  - Üretilen metrikler: Direct literal, function call, template call başına ortalama ns
- 02_Temel_Kavramlar/examples/duplicate_test.cpp
  - Üretilen metrikler: Literal pooling adres eşitliği (başarılı/başarısız)

Toplu toplama: `scripts/collect_metrics.sh` çıktıları `metrics/results.csv` ve `metrics/README.md` olarak üretir.