# Temel Kavramlar - Örnek Kodlar

Bu klasör, derleyici sabit havuzu ve .rodata bölümü konularını somut örneklerle gösteren kod dosyalarını içerir.

## 🔬 Örnekler

### 1. `duplicate_test.cpp`
**Amaç:** Derleyicinin aynı string literallerini nasıl birleştirdiğini gösterir.

**Test edilenler:**
- String literal duplicate kontrolü
- Pointer address karşılaştırması
- Farklı API'lar (printf, std::cout) ile aynı literal kullanımı

**Çalıştırma:**
```bash
cd 02_Temel_Kavramlar/examples
g++ -o duplicate_test duplicate_test.cpp
./duplicate_test
```

### 2. `size_comparison.cpp`  
**Amaç:** Farklı sabit türlerinin bellek kullanımını karşılaştırır.

**Test edilenler:**
- Küçük vs büyük integer sabitler
- Global vs local sabitler
- Array sabitleri
- String literal boyutları

**Çalıştırma:**
```bash
g++ -O0 -o size_O0 size_comparison.cpp
g++ -O2 -o size_O2 size_comparison.cpp
ls -la size_*  # Boyut farkını gör
```

### 3. `assembly_analysis.cpp`
**Amaç:** Assembly kod seviyesinde sabit yönetimini inceler.

**Test edilenler:**
- Immediate values vs memory loads
- Float sabitlerinin saklanması
- Template compile-time evaluation
- std::string vs C string

**Çalıştırma:**
```bash
g++ -S -O1 assembly_analysis.cpp  # Assembly kod üret
g++ -g -O1 -o assembly_test assembly_analysis.cpp
objdump -d assembly_test > disassembly.txt
```

## 🛠️ Analiz Araçları

### Binary İnceleme Komutları

```bash
# .rodata section içeriğini göster
objdump -s -j .rodata [binary_name]

# String literallerini listele  
strings [binary_name]

# Symbol tablosunu görüntüle
nm [binary_name] | grep -i rodata

# Assembly kodunu görüntüle
objdump -d [binary_name]

# Section header'ları listele
objdump -h [binary_name]

# Memory map'i göster (Linux)
cat /proc/[pid]/maps
```

### GDB Debugging

```bash
gdb [binary_name]
(gdb) break main
(gdb) run
(gdb) info registers
(gdb) disas [function_name]
(gdb) x/10s 0x[rodata_address]  # String'leri hexdump
```

## 📊 Beklenen Sonuçlar

### Duplicate Test
- Aynı string literalleri aynı adresi göstermeli
- .rodata'da tek kopya bulunmalı
- Platform farkı gözetmeksizin çalışmalı

### Size Comparison  
- -O2 optimizasyonu daha küçük binary üretmeli
- Küçük sabitler assembly'de immediate olarak görünmeli
- Array'ler .rodata'da yer almalı

### Assembly Analysis
- x86-64: `mov $value, %reg` immediate kullanımı
- ARM: `mov reg, #value` immediate kullanımı
- String'ler için address loading: `lea .LC0(%rip), %rdi`

## 🎯 Öğrenme Hedefleri Kontrolü

Bu örnekleri çalıştırarak şunları doğrulayabilirsiniz:

- ✅ Derleyici duplicate kontrolü yapıyor mu?
- ✅ .rodata bölümü doğru boyutta mı?
- ✅ Assembly kodu beklediğiniz gibi mi?
- ✅ Optimizasyon seviyeleri fark ediyor mu?

## 🚀 İleri Seviye Deneyler

### 1. Platform Karşılaştırması
```bash
# x86-64 assembly
g++ -S -m64 test.cpp -o test_x64.s

# ARM assembly (cross-compile gerekebilir)
arm-linux-gnueabihf-g++ -S test.cpp -o test_arm.s
```

### 2. Farklı Derleyici Karşılaştırması
```bash
# GCC
g++ -S test.cpp -o test_gcc.s

# Clang  
clang++ -S test.cpp -o test_clang.s

# Farklılıkları göster
diff test_gcc.s test_clang.s
```

### 3. Linker Analizi
```bash
# Object file'ı incele
g++ -c test.cpp
objdump -t test.o  # Symbol table
objdump -r test.o  # Relocations

# Final executable
g++ test.o -o test
objdump -t test | grep rodata
```

Bu örnekler, teorik bilgileri pratiğe dökmenizi ve derleyici davranışını gözlemlemenizi sağlar.