# Uygulamalar - Kaynak Kodlar

Bu klasör, gerçek dünya senaryolarında string literal optimizasyonunun nasıl uygulandığını gösteren kapsamlı örnek kodları içerir.

## 🚀 Pratik Uygulamalar

### 1. `embedded_systems.cpp`
**Senaryo:** Nintendo DS benzeri gömülü sistem kısıtları altında string management.

**Özellikler:**
- 4MB RAM, 32MB ROM constraint simulation
- RAM vs ROM optimization comparison
- Multi-language support with minimal RAM usage
- Real-time game loop simulation
- Memory usage tracking and analysis

**Çalıştırma:**
```bash
cd 04_Uygulamalar/src

# Standard embedded compilation
g++ -O2 -o embedded_sim embedded_systems.cpp -pthread

# Size-optimized (embedded style)
g++ -Os -ffunction-sections -fdata-sections -o embedded_opt embedded_systems.cpp -Wl,--gc-sections -pthread

# Run simulation
./embedded_sim

# Analyze binary size and sections
ls -la embedded_*
objdump -h embedded_opt | grep -E "(rodata|text|data|bss)"
```

**Öne Çıkan Öğrenimler:**
- ROM/Flash abundance vs RAM scarcity in embedded systems
- PROGMEM-style optimization techniques
- Localization without RAM overhead
- Real-time performance constraints

### 2. `game_performance.cpp`
**Senaryo:** Real-time game development performance optimization.

**Özellikler:**
- 60 FPS game loop simulation
- Cache locality analysis for game strings
- Multi-language game UI system
- Performance comparison: inefficient vs optimized
- Cache miss simulation and measurement

**Çalıştırma:**
```bash
# Performance-optimized build
g++ -O3 -march=native -flto -o game_perf game_performance.cpp -pthread

# Debug/profiling build
g++ -O1 -g -fno-omit-frame-pointer -o game_debug game_performance.cpp -pthread

# Run performance tests
./game_perf

# Profile with perf (Linux)
perf record -g ./game_perf
perf report

# Memory analysis
valgrind --tool=massif ./game_perf
```

**Öne Çıkan Öğrenimler:**
- Game engine string management strategies
- Cache-friendly data layout for UI strings
- Message ID systems vs direct string storage
- Localization system architecture
- Real-time performance requirements (16.67ms per frame)

### 3. `web_server.cpp`
**Senaryo:** High-performance web server backend optimization.

**Özellikler:**
- HTTP response template optimization
- Connection pooling with optimized strings
- Concurrent request handling
- Response caching strategies
- Performance metrics and benchmarking

**Çalıştırma:**
```bash
# Production-level build
g++ -O3 -march=native -flto -DNDEBUG -pthread -o web_server web_server.cpp

# Run load test simulation
./web_server

# Stress test with external tools (if server is actually listening)
# ab -n 100000 -c 100 http://localhost:8080/
# wrk -t12 -c400 -d30s http://localhost:8080/
```

**Öne Çıkan Öğrenimler:**
- Template-based response generation
- Pre-computed HTTP headers in .rodata
- Connection pooling optimization
- Concurrent programming with optimized strings
- Enterprise-grade performance targets

## 🔬 Analysis Tools ve Workflows

### Binary Analysis Workflow

```bash
# 1. Compile all examples
make all  # Assuming Makefile exists

# 2. Size comparison across optimization levels
for opt in O0 O1 O2 O3 Os; do
    echo "=== -$opt ==="
    g++ -$opt -o test_$opt example.cpp
    size test_$opt
done

# 3. .rodata section analysis
objdump -s -j .rodata program_name
strings program_name | sort | uniq -c | sort -nr | head -20

# 4. Memory layout verification
objdump -h program_name
readelf -S program_name
```

### Performance Analysis Workflow

```bash
# 1. Basic timing
time ./program

# 2. CPU profiling (Linux)
perf stat -e instructions,cache-misses,cache-references ./program
perf record -g ./program
perf report --stdio

# 3. Memory profiling
valgrind --tool=massif ./program
ms_print massif.out.*

# 4. Thread analysis (for concurrent examples)
valgrind --tool=helgrind ./program
```

### Cross-Platform Analysis

```bash
# Linux (GCC/Clang)
g++ -O2 -march=native example.cpp -o linux_binary
objdump -d linux_binary > linux_assembly.txt

# Windows (MinGW-w64 or MSVC)
x86_64-w64-mingw32-g++ -O2 example.cpp -o windows_binary.exe
objdump -d windows_binary.exe > windows_assembly.txt

# macOS (Clang)
clang++ -O2 -march=native example.cpp -o macos_binary
objdump -d macos_binary > macos_assembly.txt

# Compare assembly across platforms
diff linux_assembly.txt macos_assembly.txt | head -50
```

## 📊 Performance Targets ve Benchmarks

### Embedded Systems (embedded_systems.cpp)

| Metric | Bad Approach | Good Approach | Improvement |
|--------|--------------|---------------|-------------|
| **RAM Usage** | 768+ bytes | ~0 bytes | 100% reduction |
| **ROM Usage** | Minimal | ~200 bytes | Acceptable trade-off |
| **Initialization Time** | Variable | Constant | Predictable |
| **Localization Cost** | RAM × Languages | ROM only | Scalable |

### Game Development (game_performance.cpp)

| Metric | Inefficient | Optimized | Target |
|--------|-------------|-----------|--------|
| **Frame Time** | Variable | Consistent | <16.67ms (60 FPS) |
| **String Allocations** | Thousands | Zero | Zero |
| **Cache Misses** | High | Low | <5% miss rate |
| **Memory Usage** | Growing | Constant | Predictable |

### Web Server (web_server.cpp)

| Metric | Bad Implementation | Optimized | Production Target |
|--------|-------------------|-----------|-------------------|
| **Response Time** | Variable | <1ms | <1ms cached |
| **Throughput** | Limited | High | >100K req/sec |
| **Memory Growth** | Linear with load | Constant | Flat |
| **Cache Hit Rate** | N/A | >90% | >95% |

## 🛠️ Development Best Practices

### 1. Compiler Flags for Different Scenarios

```bash
# Development (debugging)
CXXFLAGS="-g -O0 -Wall -Wextra -fsanitize=address"

# Testing (profiling)  
CXXFLAGS="-g -O1 -fno-omit-frame-pointer"

# Production (performance)
CXXFLAGS="-O3 -march=native -flto -DNDEBUG"

# Embedded (size-optimized)
CXXFLAGS="-Os -ffunction-sections -fdata-sections"
LDFLAGS="-Wl,--gc-sections"

# Security-hardened
CXXFLAGS="-O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong"
```

### 2. Platform-Specific Optimizations

```cpp
// Embedded systems
#ifdef __EMBEDDED__
    #define PROGMEM __attribute__((section(".progmem")))
    #define STRING_FROM_ROM(s) strcpy_P(buffer, s)
#else
    #define PROGMEM
    #define STRING_FROM_ROM(s) s
#endif

// Game development
#ifdef __GAMING_PLATFORM__
    constexpr size_t STRING_CACHE_SIZE = 1024;  // Larger cache
#else
    constexpr size_t STRING_CACHE_SIZE = 256;   // Conservative
#endif

// Web server
#ifdef __SERVER_BUILD__
    constexpr bool ENABLE_CONNECTION_POOLING = true;
#else
    constexpr bool ENABLE_CONNECTION_POOLING = false;
#endif
```

### 3. Testing ve Validation

```cpp
// Performance regression tests
void validate_performance() {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Run optimization test
    run_optimized_code();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Assert performance requirements
    assert(duration.count() < MAX_ALLOWED_MICROSECONDS);
}

// Memory usage validation
void validate_memory_usage() {
    size_t initial_memory = get_memory_usage();
    
    // Run memory test
    run_memory_constrained_code();
    
    size_t final_memory = get_memory_usage();
    
    // Assert memory constraints
    assert(final_memory - initial_memory < MAX_ALLOWED_MEMORY_GROWTH);
}
```

## 🚀 Production Deployment Checklist

### ✅ Pre-Deployment

- [ ] **Compile with production flags** (-O3, -march=native, -flto)
- [ ] **Profile critical paths** with real workload data
- [ ] **Validate memory usage** under peak load
- [ ] **Test on target hardware** and OS
- [ ] **Measure baseline performance** before optimization
- [ ] **Document optimization assumptions** and trade-offs

### ✅ Post-Deployment

- [ ] **Monitor performance metrics** continuously
- [ ] **Set up alerting** for performance regressions
- [ ] **Collect real-world profiling data**
- [ ] **Validate optimization effectiveness** with A/B testing
- [ ] **Document lessons learned** for future projects

Bu örnekler, string literal optimizasyonunun gerçek dünya uygulamalarında nasıl kritik performans iyileştirmeleri sağladığını gösterir. Her örnek, kendi domain'inde tipik olarak karşılaşılan sorunları ve bunların çözümlerini ele alır.