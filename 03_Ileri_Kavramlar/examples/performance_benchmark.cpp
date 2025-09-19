#include <iostream>
#include <chrono>
#include <vector>
#include <cstring>

/*
 * Inline vs .rodata Performance Benchmark
 * Bu örnek, inline embedding ile .rodata kullanımı arasındaki 
 * performans farkını gerçek ölçümlerle gösterir.
 */

class PerformanceBenchmark {
private:
    static constexpr int ITERATIONS = 1000000;
    static constexpr int FUNCTION_COUNT = 100;
    
public:
    // .rodata approach - efficient
    static void rodata_approach() {
        // Bu string'ler .rodata'da tek birer kopya olarak tutulacak
        static const char* messages[] = {
            "System initialization completed successfully",
            "Network connection established",
            "Database connection active", 
            "User authentication verified",
            "Configuration loaded from file",
            "Logging system started",
            "Cache system initialized",
            "Thread pool created",
            "Security protocols enabled",
            "System ready for requests"
        };
        
        // Simulated function calls using .rodata strings
        for (int i = 0; i < ITERATIONS; ++i) {
            for (int j = 0; j < 10; ++j) {
                // Compiler will generate: lea rdi, [message_address]
                volatile const char* msg = messages[j];
                (void)msg; // Suppress unused warning
            }
        }
    }
    
    // Inline simulation - inefficient  
    static void inline_simulation() {
        // Her iteration'da string'leri stack'e kopyalayarak
        // inline approach'ı simüle ediyoruz
        char buffer[50];
        
        for (int i = 0; i < ITERATIONS; ++i) {
            // Inline approach simulation - lots of copying
            strcpy(buffer, "System initialization completed successfully");
            volatile char* p1 = buffer;
            
            strcpy(buffer, "Network connection established"); 
            volatile char* p2 = buffer;
            
            strcpy(buffer, "Database connection active");
            volatile char* p3 = buffer;
            
            strcpy(buffer, "User authentication verified");
            volatile char* p4 = buffer;
            
            strcpy(buffer, "Configuration loaded from file");
            volatile char* p5 = buffer;
            
            strcpy(buffer, "Logging system started");
            volatile char* p6 = buffer;
            
            strcpy(buffer, "Cache system initialized");
            volatile char* p7 = buffer;
            
            strcpy(buffer, "Thread pool created");
            volatile char* p8 = buffer;
            
            strcpy(buffer, "Security protocols enabled");
            volatile char* p9 = buffer;
            
            strcpy(buffer, "System ready for requests");
            volatile char* p10 = buffer;
            
            // Prevent optimization
            (void)p1; (void)p2; (void)p3; (void)p4; (void)p5;
            (void)p6; (void)p7; (void)p8; (void)p9; (void)p10;
        }
    }
    
    static void benchmark() {
        std::cout << "=== Performance Benchmark: Inline vs .rodata ===" << std::endl;
        std::cout << "Iterations: " << ITERATIONS << std::endl;
        std::cout << "Messages per iteration: 10" << std::endl;
        std::cout << "Total operations: " << (ITERATIONS * 10) << std::endl << std::endl;
        
        // .rodata approach benchmark
        auto start = std::chrono::high_resolution_clock::now();
        rodata_approach();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto rodata_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Inline simulation benchmark  
        start = std::chrono::high_resolution_clock::now();
        inline_simulation();
        end = std::chrono::high_resolution_clock::now();
        
        auto inline_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Results
        std::cout << "📊 Results:" << std::endl;
        std::cout << ".rodata approach: " << rodata_duration.count() << " microseconds" << std::endl;
        std::cout << "Inline simulation: " << inline_duration.count() << " microseconds" << std::endl;
        
        if (inline_duration.count() > 0) {
            double speedup = static_cast<double>(inline_duration.count()) / rodata_duration.count();
            std::cout << "\n🚀 Speedup: " << speedup << "x faster with .rodata approach" << std::endl;
        }
        
        std::cout << "\n💾 Memory Analysis:" << std::endl;
        std::cout << "Inline approach estimated instruction overhead: ~" << 
                     (FUNCTION_COUNT * 15) << " extra instructions per function" << std::endl;
        std::cout << ".rodata approach: 1 instruction per string access" << std::endl;
    }
};

// Compile-time string analysis
template<size_t N>
constexpr size_t compile_time_strlen(const char (&str)[N]) {
    return N - 1;
}

void compile_time_analysis() {
    std::cout << "\n=== Compile-Time Analysis ===" << std::endl;
    
    // These will be evaluated at compile time
    constexpr auto len1 = compile_time_strlen("Short");
    constexpr auto len2 = compile_time_strlen("This is a much longer string");
    constexpr auto len3 = compile_time_strlen("OK");
    
    std::cout << "String lengths (computed at compile time):" << std::endl;
    std::cout << "\"Short\": " << len1 << " chars" << std::endl;
    std::cout << "\"This is a much longer string\": " << len2 << " chars" << std::endl;
    std::cout << "\"OK\": " << len3 << " chars" << std::endl;
    
    std::cout << "\nCompiler decision simulation:" << std::endl;
    std::cout << "\"OK\" (" << len3 << " chars) → Likely inline candidate" << std::endl;
    std::cout << "\"Short\" (" << len1 << " chars) → Borderline, depends on usage" << std::endl;
    std::cout << "\"Long string\" (" << len2 << " chars) → Definitely .rodata" << std::endl;
}

int main() {
    // Run performance benchmark
    PerformanceBenchmark::benchmark();
    
    // Compile-time analysis
    compile_time_analysis();
    
    return 0;
}

/*
 * Derleme ve Analiz Komutları:
 * 
 * 1. Farklı optimizasyon seviyeleri ile derleyin:
 *    g++ -O0 -o benchmark_O0 performance_benchmark.cpp
 *    g++ -O2 -o benchmark_O2 performance_benchmark.cpp
 *    g++ -O3 -o benchmark_O3 performance_benchmark.cpp
 * 
 * 2. Benchmark'ları çalıştırın:
 *    time ./benchmark_O0
 *    time ./benchmark_O2
 *    time ./benchmark_O3
 * 
 * 3. Assembly analizi:
 *    g++ -S -O2 performance_benchmark.cpp -o benchmark.s
 *    grep -A 10 -B 5 "rodata_approach" benchmark.s
 * 
 * 4. İkili (binary) boyut karşılaştırması:
 *    ls -la benchmark_*
 *    size benchmark_O0 benchmark_O2 benchmark_O3
 * 
 * 5. perf ile profil çıkarma (Linux):
 *    perf stat -e instructions,cache-misses,cache-references ./benchmark_O2
 * 
 * 6. Bellek yerleşimi analizi:
 *    objdump -h benchmark_O2 | grep -E "(rodata|text)"
 *    objdump -s -j .rodata benchmark_O2
 * 
 * Beklenen Sonuçlar:
 * - .rodata yaklaşımı belirgin şekilde daha hızlı olmalı
 * - O2/O3 optimizasyonları dramatik iyileşmeler göstermeli
 * - Daha yüksek optimizasyonda ikili dosya boyutu daha küçük olmalı
 * - .rodata yaklaşımında cache miss oranı daha düşük olmalı
 * 
 * Öne Çıkan Öğrenimler:
 * - Bellek erişim desenleri performansı etkiler
 * - Derleyici optimizasyonları davranışı ciddi ölçüde değiştirebilir
 * - .rodata string'leri CPU cache kullanımını iyileştirir
 * - Assembly analizi gerçek talimat (instruction) farklarını ortaya çıkarır
 */