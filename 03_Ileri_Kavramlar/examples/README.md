# İleri Kavramlar - Örnek Kodlar

Bu klasör, inline embedding vs external storage konularını derinlemesine analiz eden gelişmiş örnekleri içerir.

## 🚀 Gelişmiş Örnekler

### 1. `performance_benchmark.cpp`
**Amaç:** Inline vs .rodata yaklaşımları arasındaki performans farkını ölçer.

**Özellikler:**
- Gerçek zaman performans ölçümü
- Instruction count simulation
- Cache locality etkilerini gösterir
- Compile-time analysis

**Çalıştırma:**
```bash
cd 03_Ileri_Kavramlar/examples

# Farklı optimizasyon seviyelerinde compile et
g++ -O0 -o benchmark_O0 performance_benchmark.cpp
g++ -O2 -o benchmark_O2 performance_benchmark.cpp
g++ -O3 -o benchmark_O3 performance_benchmark.cpp

# Performance karşılaştırması
time ./benchmark_O0
time ./benchmark_O2  
time ./benchmark_O3

# Cache miss analizi (Linux)
perf stat -e cache-misses,instructions ./benchmark_O2
```

### 2. `memory_layout_analysis.cpp`
**Amaç:** String literalleri ve sabitlerinin bellekteki yerleşimini detaylı analiz eder.

**Özellikler:**
- Memory section detection (.text, .rodata, .data, .bss, stack, heap)
- String deduplication verification
- SSO (Small String Optimization) analysis
- Cache locality examination

**Çalıştırma:**
```bash
g++ -O1 -o memory_layout memory_layout_analysis.cpp
./memory_layout

# Memory mapping görüntüleme (Linux)
cat /proc/$(pgrep memory_layout)/maps

# Section analysis
objdump -h memory_layout
readelf -S memory_layout
```

### 3. `optimization_analysis.cpp`
**Amaç:** Farklı optimizasyon seviyelerinin string handling üzerindeki etkilerini analiz eder.

**Özellikler:**
- Multi-level optimization comparison (-O0 to -O3)
- Template-based compile-time optimization
- Function inlining detection
- Binary size analysis

**Çalıştırma:**
```bash
# Tüm optimizasyon seviyelerinde build
g++ -O0 -g -o opt_O0 optimization_analysis.cpp
g++ -O1 -g -o opt_O1 optimization_analysis.cpp
g++ -O2 -g -o opt_O2 optimization_analysis.cpp
g++ -O3 -g -o opt_O3 optimization_analysis.cpp

# Binary boyut karşılaştırması
ls -la opt_O* | awk '{print $9, $5}' | column -t

# Assembly code comparison
objdump -d opt_O0 > asm_O0.txt
objdump -d opt_O2 > asm_O2.txt
diff -u asm_O0.txt asm_O2.txt | head -100
```

## 🔬 Analiz Araçları ve Teknikler

### Binary Analysis Tools

```bash
# Section boyutları
size [binary_name]

# .rodata içeriği  
objdump -s -j .rodata [binary_name]

# String literals
strings [binary_name] | sort | uniq -c | sort -nr

# Symbol analysis
nm [binary_name] | grep -E "(rodata|text)"

# Assembly disassembly
objdump -d [binary_name] | grep -A 10 -B 5 [function_name]
```

### Performance Profiling

```bash
# Basic timing
time ./program

# Detailed performance counters (Linux)
perf stat -e instructions,cache-misses,cache-references ./program

# Memory analysis
valgrind --tool=massif ./program

# Cache simulation
valgrind --tool=cachegrind ./program
```

### Advanced Analysis

```bash
# Link-time optimization
g++ -flto -O2 file1.cpp file2.cpp -o optimized_program

# Profile-guided optimization
g++ -fprofile-generate program.cpp -o program
./program < input.txt
g++ -fprofile-use program.cpp -o program_pgo

# Cross-platform analysis
# Intel VTune (Intel)
vtune -collect hotspots ./program

# AMD CodeXL (AMD)  
CodeXLGpuProfiler ./program
```

## 📊 Beklenen Sonuçlar ve Analizler

### Performance Benchmark Sonuçları

| Optimizasyon | Execution Time | Instruction Count | Cache Misses |
|--------------|----------------|-------------------|--------------|
| **O0** | Baseline (100%) | Highest | High |
| **O1** | ~60-80% | Medium | Medium |
| **O2** | ~30-50% | Low | Low |
| **O3** | ~25-40% | Lowest | Lowest |

### Memory Layout Gözlemleri

```
Typical 64-bit Linux Layout:
0x400000 - 0x4XXXXX: .text (code)
0x6XXXXX - 0x6XXXXX: .rodata (constants)  
0x8XXXXX - 0x8XXXXX: .data (initialized globals)
0x8XXXXX - 0x8XXXXX: .bss (uninitialized globals)
0x7XXXXXXX - 0x7XXXXXXX: heap (grows up)
0x7FFFFFFX - 0x7FFFFFFX: stack (grows down)
```

### String Optimization Patterns

```cpp
// Pattern 1: Small strings (≤ 8 chars) - Often inlined
"OK"     → mov eax, 0x4B4F    ; Direct immediate

// Pattern 2: Medium strings - .rodata reference  
"Hello"  → lea rdi, [.LC0]    ; Address loading

// Pattern 3: Large strings - Always .rodata
"Long message..." → lea rdi, [.LC1]  ; Address + cache-friendly layout
```

## 🎯 Öğrenme Hedefleri Doğrulama

Bu örnekleri çalıştırarak şunları doğrulayın:

### ✅ Performance Impact
- [ ] .rodata approach significantly faster than inline simulation
- [ ] Higher optimization levels show dramatic improvements
- [ ] Cache misses reduced with .rodata approach

### ✅ Memory Layout Understanding
- [ ] String literals appear in .rodata section
- [ ] Identical strings share the same memory address
- [ ] std::string objects allocate separate heap memory
- [ ] Memory sections follow expected ELF layout

### ✅ Compiler Optimization Intelligence
- [ ] Small constants get inlined at -O2 and above
- [ ] Function calls eliminated through inlining
- [ ] Template specializations optimized at compile-time
- [ ] Binary size decreases with optimization

## 🚀 Challenges ve Experiments

### Challenge 1: Cross-Module Optimization
```bash
# Test string deduplication across modules
echo 'const char* get_msg() { return "test"; }' > module1.cpp
echo 'const char* get_same() { return "test"; }' > module2.cpp
echo 'void f1(); void f2(); int main() { f1(); f2(); }' > main.cpp

# Without LTO
g++ -c module1.cpp module2.cpp main.cpp
g++ *.o -o test_no_lto

# With LTO  
g++ -flto -O2 module1.cpp module2.cpp main.cpp -o test_lto

# Compare
strings test_no_lto | grep -c "test"
strings test_lto | grep -c "test"
```

### Challenge 2: Platform Comparison
```bash
# x86-64 vs ARM comparison
g++ -S -O2 test.cpp -o test_x64.s                    # x86-64
arm-linux-gnueabihf-g++ -S -O2 test.cpp -o test_arm.s  # ARM

# Analyze immediate value handling differences
grep -E "(mov|ldr)" test_x64.s | head -10
grep -E "(mov|ldr)" test_arm.s | head -10
```

### Challenge 3: Memory-Constrained Optimization  
```bash
# Simulate embedded system constraints
g++ -Os -ffunction-sections -fdata-sections test.cpp -Wl,--gc-sections -o embedded
strip embedded

# Analyze size optimizations
size embedded
objdump -h embedded | grep -E "(text|rodata|data|bss)"
```

Bu gelişmiş örnekler, C++ derleyici optimizasyonlarının gerçek dünya etkilerini anlamanıza yardımcı olacak ve performans odaklı kod yazma becerilerinizi geliştirecektir.