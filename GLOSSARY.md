# 📚 Terim Sözlüğü (Glossary)

**Constant Pool**
- Sabit değerlerin (literal'lar) saklandığı bellek alanı
- Derleyici tarafından otomatik olarak oluşturulur
- Aynı değerlerin tekrar kullanımını sağlar

**Constant Pool (Sabit Havuzu)**
- Derleme sırasında derleyicinin RAM'inde oluşturduğu geçici tablo
- Tüm sabit değerlerin toplandığı alan
- Duplicate kontrolü burada yapılırde, C++ literal pooling analizi projesinde kullanılan teknik terimler ve kavramlar açıklanmaktadır.

## 🔤 A

**Address / Pointer (Adres / İşaretçi)**
- Bellekte bir veri lokasyonunu gösteren değer
- Örnek: `const char* p = "hello";`
- Memory layout analizinde kritik kavram

**Assembly Language**
- Makine koduna en yakın programlama dili
- İnsan tarafından okunabilir makine talimatları
- Derleyici çıktısını analiz etmek için kullanılır

## 🔤 B

**Binary Analysis**
- Derlenmiş executable dosyaların incelenmesi
- `objdump`, `nm`, `readelf` gibi araçlarla yapılır
- Memory layout ve optimization'ları görmek için kullanılır

**Benchmark**
- Performans ölçümü için yapılan testler
- Farklı yaklaşımların karşılaştırılması
- Execution time, memory usage gibi metriklerin toplanması

## 🔤 C

**Cache Locality**
- CPU cache'inde yakın adreslerdeki verilerin birlikte saklanması
- Memory erişim performansını artırır
- Literal pooling sayesinde iyileşir

**Constant Pool**
- Sabit değerlerin (literal'lar) saklandığı bellek alanı
- Derleyici tarafından otomatik olarak oluşturulur
- Aynı değerlerin tekrar kullanımını sağlar

**Compiler Optimization**
- Derleyicinin kodu daha verimli hale getirmesi
- `-O1`, `-O2`, `-O3` gibi seviyeler
- Memory usage ve execution speed iyileştirmeleri

## 🔤 D

**Deduplication**
- Tekrarlayan verilerin birleştirilmesi
- String literal'larda aynı metinlerin tek kopyası
- Memory tasarrufu sağlar

**Duplicate Kontrolü**
- Aynı sabitin veya literalin birden fazla kez saklanmasını önleme işlemi
- Derleyici sadece bir kopya oluşturur
- Memory optimization'ın temel prensibi

**Disassembly**
- Machine code'un assembly'e çevrilmesi
- `objdump -d` komutuyla yapılır
- Derleyici optimizasyonlarını görmek için kullanılır

## 🔤 E

**Executable**
- Derlenmiş ve çalıştırılabilir dosya
- `.exe`, binary dosyalar
- Source code'dan compiler ile üretilir

## 🔤 G

**GCC (GNU Compiler Collection)**
- Açık kaynak C/C++ derleyicisi
- `-O2`, `-Os` gibi optimization flag'leri
- Cross-platform destek

## 🔤 H

**Heap**
- Dinamik bellek tahsisi yapılan çalışma zamanı alanı
- Örnek: `new std::string("hello")`
- Runtime'da büyüyüp küçülebilen bellek segmenti

## 🔤 I

**Immediate Value (İmmediat değer)**
- Talimat içinde direkt gömülmüş sabit sayı veya değer
- Örnek: `MOV R0, #5`
- Assembly kodda doğrudan kullanılan sabit

**Inline (gömme)**
- Bir değeri veya kodu, ayrı bir hafıza alanı veya fonksiyon çağrısı yerine direkt komut içine yazma işlemi
- Örnek: `MOV R0, #5`
- Performance optimization tekniği

**Instruction (Talimat / Komut)**
- CPU'nun çalıştıracağı makine kodu birimi
- Örnek: `MOV`, `LDR`, `CALL`
- Assembly language'in temel birimi

**Instruction Cache**
- CPU'nun hızlı erişim için tuttuğu komut önbelleği
- Çok büyük kod segmenti cache'i zorlar
- Performance için kritik öneme sahip

## 🔤 L

**Literal**
- Kodda doğrudan yazılan sabit değer
- `"Hello World"` (string literal)
- `42` (integer literal)

**Literal**
- Kaynak kodda doğrudan yazılmış sabit değer
- Örnek: `"hello"` veya `5`
- Compile-time'da bilinen değerler

**Literal Pooling**
- Aynı literal değerlerin tek kopyasının saklanması
- Memory optimization tekniği
- Cache performance artışı sağlar

## 🔤 M

**Memory Layout**
- Program belleğindeki segmentlerin düzeni
- `.text`, `.data`, `.rodata`, `.bss` segmentleri
- Virtual memory organization

**Memory Profiling**
- Bellek kullanım analizi
- Heap, stack, static memory ölçümleri
- Memory leak detection

**Mutable / Immutable**
- Değiştirilebilir (mutable) veya değiştirilemez (immutable) veri
- Literaller immutable, `std::string` mutable
- Type safety için önemli kavram

## 🔤 O

**objdump**
- Binary dosya analiz aracı
- Assembly code görüntüleme
- Symbol table inceleme

**Optimization Level**
- Derleyici optimizasyon seviyesi
- `-O0`: Optimizasyon yok
- `-O1`: Temel optimizasyon
- `-O2`: Gelişmiş optimizasyon
- `-O3`: Agresif optimizasyon
- `-Os`: Size optimization

## 🔤 P

**Performance Benchmark**
- Sistemin performans ölçümü
- Execution time, throughput, memory usage
- Different approaches comparison

## 🔤 R

**readelf**
- ELF format binary analiz aracı
- Segment ve section bilgileri
- Symbol table görüntüleme

**.rodata Section**
- Read-only data segment
- String literal'ların saklandığı alan
- Constant değerlerin bulunduğu bellek bölümü

**.rodata (Read-Only Data Segment)**
- Programın salt okunur veri segmenti
- String literalleri ve bazı global/const sabitler burada tutulur
- Executable'ın sabit veri kısmı

## 🔤 S

**SSO (Small String Optimization)**
- Küçük stringleri, heap yerine string objesinin içinde saklama optimizasyonu
- Memory allocation overhead'ini azaltır
- Modern C++ string implementation'larında yaygın

**Stack**
- Fonksiyon çağrıları ve lokal değişkenler için ayrılan çalışma zamanı bellek alanı
- LIFO (Last In, First Out) prensibi ile çalışır
- Heap'ten daha hızlı erişim

**String Literal**
- Kod içinde çift tırnak içinde yazılan metinler
- `"Hello World"`, `"Error message"`
- .rodata section'da saklanır

**Symbol Table**
- Binary dosyalardaki sembollerin listesi
- Function names, variable names
- `nm` komutuyla görüntülenir

## 🔤 T

**Text Section**
- Executable code'un saklandığı segment
- Machine instructions
- Read-only memory area

## 🔤 V

**Virtual Memory**
- İşletim sisteminin memory management sistemi
- Process'lerin kendi address space'i
- Physical memory'den soyutlama

---

## 🛠️ Kullanılan Araçlar

| Araç | Açıklama | Kullanım Amacı |
|------|----------|----------------|
| `objdump` | Binary disassembler | Assembly code analizi |
| `nm` | Symbol table viewer | Symbol information |
| `readelf` | ELF file analyzer | Segment/section details |
| `strings` | String extractor | Literal string'leri bulma |
| `size` | Binary size analyzer | Section size comparison |
| `gcc` | GNU C/C++ Compiler | Code compilation |
| `clang` | LLVM C/C++ Compiler | Alternative compilation |

---

## 📊 Performance Metrikleri

**Execution Time**
- Programın çalışma süresi
- Microsecond (μs) cinsinden ölçüm
- Benchmark testlerinde kullanılır

**Memory Usage**
- Program tarafından kullanılan bellek miktarı
- RSS (Resident Set Size), heap, stack
- Bytes cinsinden ölçüm

**Binary Size**
- Derlenmiş dosyanın boyutu
- .text, .data, .rodata section'larının toplamı
- Optimization'ın etkisini gösterir

---

## 🎯 Optimization Teknikleri

**Dead Code Elimination**
- Kullanılmayan kodun çıkarılması
- Compiler optimization
- Binary size reduction

**Constant Folding**
- Compile-time'da sabit hesaplamaların yapılması
- `2 + 3` → `5` conversion
- Runtime performance improvement

**Function Inlining**
- Küçük fonksiyonların çağrı yerine yazılması
- Function call overhead elimination
- Code size vs performance trade-off

---

*Bu terim sözlüğü proje boyunca karşılaşılan kavramları açıklamak için hazırlanmıştır. Yeni terimler eklenebilir.*