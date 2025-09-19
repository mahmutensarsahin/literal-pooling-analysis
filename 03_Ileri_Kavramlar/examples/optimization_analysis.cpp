#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <iomanip>

/*
 * Compiler Optimization Deep Dive
 * Bu örnek, farklı optimizasyon seviyelerinin 
 * string literal handling üzerindeki etkilerini detaylı analiz eder.
 */

// Test class with various constant usage patterns
// Forward declaration of class to allow early template specializations
class OptimizationTestbed;

// Primary template declaration
template<int N>
const char* OptimizationTestbed_get_template_message_impl();

// Explicit specializations BEFORE use
template<>
const char* OptimizationTestbed_get_template_message_impl<1>() { return "Template message 1"; }
template<>
const char* OptimizationTestbed_get_template_message_impl<2>() { return "Template message 2"; }
template<>
const char* OptimizationTestbed_get_template_message_impl<3>() { return "Template message 3"; }

class OptimizationTestbed {
private:
    // Different categories of constants for optimization analysis
    
    // Category 1: Single-use constants (might be inlined)
    static const char* get_single_use_message() {
        return "This message is used only once in the entire program";
    }
    
    // Category 2: Multiple-use constants (definitely .rodata)
    static const char* get_common_message() {
        return "Common message used frequently";  // Will be called multiple times
    }
    
    // Category 3: Size-varying constants
    static const char* get_tiny_message() { return "OK"; }
    static const char* get_small_message() { return "Warning"; }
    static const char* get_medium_message() { return "Processing your request"; }
    static const char* get_large_message() { 
        return "This is a significantly larger message that contains much more text and definitely exceeds any reasonable inline threshold";
    }
    
    // Category 4: Numeric constants of different sizes
    static int get_tiny_number() { return 5; }
    static int get_medium_number() { return 0x12345; }
    static long long get_large_number() { return 0x123456789ABCDEF0LL; }
    
public:
    // Function that uses single-use constant
    static void function_single_use() {
        std::cout << get_single_use_message() << std::endl;
    }
    
    // Functions that use common constants (testing duplicate handling)
    static void function_common_use_1() {
        std::cout << "Function 1: " << get_common_message() << std::endl;
    }
    
    static void function_common_use_2() {
        std::cout << "Function 2: " << get_common_message() << std::endl;
    }
    
    static void function_common_use_3() {
        std::cout << "Function 3: " << get_common_message() << std::endl;
    }
    
    // Size-based optimization test
    static void size_optimization_test() {
        std::cout << "Tiny: " << get_tiny_message() << std::endl;
        std::cout << "Small: " << get_small_message() << std::endl;
        std::cout << "Medium: " << get_medium_message() << std::endl;
        std::cout << "Large: " << get_large_message() << std::endl;
    }
    
    // Numeric constant optimization test
    static void numeric_optimization_test() {
        std::cout << "Tiny number: " << get_tiny_number() << std::endl;
        std::cout << "Medium number: " << get_medium_number() << std::endl;
        std::cout << "Large number: " << std::hex << get_large_number() << std::dec << std::endl;
    }
    
    // Loop-based repetition test (compiler should optimize repeated access)
    static void repetition_optimization_test() {
        const char* msg = get_common_message();
        for (int i = 0; i < 1000; ++i) {
            // Compiler should optimize this to avoid repeated function calls
            if (i % 100 == 0) {
                std::cout << "Iteration " << i << ": " << msg << std::endl;
            }
        }
    }
    
    // Template-based compile-time optimization
    // C++11-compatible template: primary template + specializations
    template<int N>
    static const char* get_template_message() { return OptimizationTestbed_get_template_message_impl<N>(); }

public:
    // Public wrapper to access common message from other classes
    static const char* common_message() { return get_common_message(); }
    
    static void template_optimization_test() {
        std::cout << get_template_message<1>() << std::endl;
        std::cout << get_template_message<2>() << std::endl;
        std::cout << get_template_message<3>() << std::endl;
        std::cout << get_template_message<99>() << std::endl;
    }
};

// Provide default implementation for unspecified N
template<int N>
const char* OptimizationTestbed_get_template_message_impl() { return "Default template message"; }

// Benchmarking class for performance measurement
class OptimizationBenchmark {
private:
    static constexpr int BENCHMARK_ITERATIONS = 10000;
    
public:
    static void benchmark_string_access_patterns() {
        std::cout << "\n=== String Access Pattern Benchmarks ===" << std::endl;
        
        // Benchmark 1: Repeated literal access
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < BENCHMARK_ITERATIONS; ++i) {
            volatile const char* msg = "Benchmark message";
            (void)msg;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto direct_literal_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        
        // Benchmark 2: Function call returning literal
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < BENCHMARK_ITERATIONS; ++i) {
            volatile const char* msg = OptimizationTestbed::common_message();
            (void)msg;
        }
        end = std::chrono::high_resolution_clock::now();
        auto function_call_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        
        // Benchmark 3: Template-based access
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < BENCHMARK_ITERATIONS; ++i) {
            volatile const char* msg = OptimizationTestbed::get_template_message<1>();
            (void)msg;
        }
        end = std::chrono::high_resolution_clock::now();
        auto template_call_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        
        std::cout << "📊 Performance Results (average per operation):" << std::endl;
        std::cout << "Direct literal access: " << 
                     std::fixed << std::setprecision(2) << 
                     static_cast<double>(direct_literal_time.count()) / BENCHMARK_ITERATIONS << " ns" << std::endl;
        std::cout << "Function call: " << 
                     std::fixed << std::setprecision(2) <<
                     static_cast<double>(function_call_time.count()) / BENCHMARK_ITERATIONS << " ns" << std::endl;
        std::cout << "Template call: " << 
                     std::fixed << std::setprecision(2) <<
                     static_cast<double>(template_call_time.count()) / BENCHMARK_ITERATIONS << " ns" << std::endl;
        
        // Analysis
        if (template_call_time <= direct_literal_time * 1.1) {  // Within 10%
            std::cout << "✅ Template calls optimized to direct access level" << std::endl;
        }
        if (function_call_time > direct_literal_time * 2) {
            std::cout << "⚠️  Function call overhead detected (not fully optimized)" << std::endl;
        } else {
            std::cout << "✅ Function calls well optimized" << std::endl;
        }
    }
    
    static void analyze_binary_sections() {
        std::cout << "\n=== Binary Section Analysis Hints ===" << std::endl;
        std::cout << "To analyze the generated binary sections, use these commands:" << std::endl;
        std::cout << std::endl;
        
        std::cout << "1. Section sizes:" << std::endl;
        std::cout << "   size optimization_analysis" << std::endl;
        std::cout << std::endl;
        
        std::cout << "2. .rodata content:" << std::endl;
        std::cout << "   objdump -s -j .rodata optimization_analysis" << std::endl;
        std::cout << std::endl;
        
        std::cout << "3. String literals:" << std::endl;
        std::cout << "   strings optimization_analysis | head -20" << std::endl;
        std::cout << std::endl;
        
        std::cout << "4. Assembly analysis:" << std::endl;
        std::cout << "   objdump -d optimization_analysis | grep -A 5 -B 5 'get_.*_message'" << std::endl;
        std::cout << std::endl;
        
        std::cout << "5. Symbol analysis:" << std::endl;
        std::cout << "   nm optimization_analysis | grep -i rodata" << std::endl;
    }
};

// Function to generate runtime optimization analysis
void generate_optimization_report() {
    std::cout << "\n=== Optimization Analysis Report ===" << std::endl;
    
    // Address analysis for optimization verification
    const char* common_1 = "Common message used frequently";
    const char* common_2 = "Common message used frequently";
    const char* different = "Different message";
    
    std::cout << "🔍 String deduplication check:" << std::endl;
    std::cout << "common_1 address: " << (void*)common_1 << std::endl;
    std::cout << "common_2 address: " << (void*)common_2 << std::endl;
    std::cout << "different address: " << (void*)different << std::endl;
    
    if (common_1 == common_2) {
        std::cout << "✅ String deduplication working (same address)" << std::endl;
    } else {
        std::cout << "❌ String deduplication not working (different addresses)" << std::endl;
    }
    
    // Function pointer analysis
    void (*func1)() = OptimizationTestbed::function_common_use_1;
    void (*func2)() = OptimizationTestbed::function_common_use_2;
    
    std::cout << "\n🎯 Function addresses:" << std::endl;
    std::cout << "function_common_use_1: " << (void*)func1 << std::endl;
    std::cout << "function_common_use_2: " << (void*)func2 << std::endl;
    
    // Template instantiation check
    auto template_func1 = OptimizationTestbed::get_template_message<1>;
    auto template_func2 = OptimizationTestbed::get_template_message<2>;
    
    std::cout << "template<1> func: " << (void*)template_func1 << std::endl;
    std::cout << "template<2> func: " << (void*)template_func2 << std::endl;
}

int main() {
    std::cout << "=== Compiler Optimization Deep Dive ===" << std::endl;
    std::cout << "Compiled with: " << 
#ifdef __OPTIMIZE__
                 "Optimization enabled"
#else
                 "No optimization (-O0)"
#endif
                 << std::endl;
    
    std::cout << "Compiler: " <<
#ifdef __GNUC__
                 "GCC " << __GNUC__ << "." << __GNUC_MINOR__
#elif defined(__clang__)
                 "Clang " << __clang_major__ << "." << __clang_minor__
#elif defined(_MSC_VER)
                 "MSVC " << _MSC_VER
#else
                 "Unknown"
#endif
                 << std::endl << std::endl;
    
    // Run all test functions
    std::cout << "=== Running Optimization Tests ===" << std::endl;
    OptimizationTestbed::function_single_use();
    OptimizationTestbed::function_common_use_1();
    OptimizationTestbed::function_common_use_2();
    OptimizationTestbed::function_common_use_3();
    
    std::cout << "\n--- Size-based optimization ---" << std::endl;
    OptimizationTestbed::size_optimization_test();
    
    std::cout << "\n--- Numeric optimization ---" << std::endl;
    OptimizationTestbed::numeric_optimization_test();
    
    std::cout << "\n--- Template optimization ---" << std::endl;
    OptimizationTestbed::template_optimization_test();
    
    std::cout << "\n--- Repetition optimization ---" << std::endl;
    OptimizationTestbed::repetition_optimization_test();
    
    // Run benchmarks
    OptimizationBenchmark::benchmark_string_access_patterns();
    
    // Generate analysis report
    generate_optimization_report();
    
    // Provide analysis guidance
    OptimizationBenchmark::analyze_binary_sections();
    
    return 0;
}

/*
 * Kapsamlı Analiz Komutları:
 * 
 * 1. Farklı optimizasyon seviyelerinde derleyin:
 *    g++ -O0 -g -o opt_O0 optimization_analysis.cpp
 *    g++ -O1 -g -o opt_O1 optimization_analysis.cpp  
 *    g++ -O2 -g -o opt_O2 optimization_analysis.cpp
 *    g++ -O3 -g -o opt_O3 optimization_analysis.cpp
 *    g++ -Os -g -o opt_Os optimization_analysis.cpp  # Boyut odaklı optimizasyon
 * 
 * 2. İkili (binary) boyutlarını karşılaştırın:
 *    ls -la opt_O* | awk '{print $9, $5}' | column -t
 * 
 * 3. Bölüm (section) boyutlarını karşılaştırın:
 *    for f in opt_O*; do echo "=== $f ==="; size $f; done
 * 
 * 4. Assembly karşılaştırması:
 *    objdump -d opt_O0 > asm_O0.txt
 *    objdump -d opt_O2 > asm_O2.txt
 *    diff -u asm_O0.txt asm_O2.txt | head -50
 * 
 * 5. .rodata bölümü karşılaştırması:
 *    for f in opt_O*; do 
 *        echo "=== $f rodata ===" 
 *        objdump -s -j .rodata $f | wc -l
 *    done
 * 
 * 6. Function inlining analizi:
 *    objdump -t opt_O0 | grep get_.*_message
 *    objdump -t opt_O2 | grep get_.*_message
 * 
 * 7. Performans karşılaştırması:
 *    echo "O0:"; time ./opt_O0 > /dev/null
 *    echo "O2:"; time ./opt_O2 > /dev/null
 *    echo "O3:"; time ./opt_O3 > /dev/null
 * 
 * 8. Ayrıntılı profil (Linux):
 *    perf stat -e instructions,cache-misses ./opt_O0
 *    perf stat -e instructions,cache-misses ./opt_O2
 * 
 * Beklenen Optimizasyon Davranışları:
 * - O0: Optimizasyon yok, tüm fonksiyonlar mevcut
 * - O1: Temel inlining, sınırlı string deduplikasyonu
 * - O2: Agresif inlining, tam string deduplikasyonu
 * - O3: Maksimum optimizasyon, template özelleştirme
 * - Os: Boyut odaklı, maksimum string paylaşımı
 * 
 * Dikkat Edilecek Temel Gözlemler:
 * 1. Inlining ile fonksiyonların elenmesi
 * 2. Derleme birimleri arasında string literal deduplikasyonu
 * 3. Template instantiation optimizasyonu
 * 4. Sayısal sabitlerin inline edilmesi vs .rodata'da saklanması
 * 5. Yüksek optimizasyon seviyelerinde performans iyileşmeleri
 */