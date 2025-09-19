#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <cassert>
#include <cstdint>

/*
 * Advanced Memory Layout Analysis
 * Bu örnek, string literalleri ve çeşitli veri türlerinin
 * bellekteki yerleşimini detaylı olarak analiz eder.
 */

class MemoryLayoutAnalyzer {
private:
    // Global constants for layout analysis
    static const char* GLOBAL_STRING;
    static constexpr int GLOBAL_INT = 42;
    static constexpr double GLOBAL_DOUBLE = 3.14159;
    
    // Array of strings in .rodata
    static const char* const ERROR_MESSAGES[];
    static const int LOOKUP_TABLE[];

public:
    static void analyze_global_constants() {
        std::cout << "=== Global Constants Memory Layout ===" << std::endl;
        
        std::cout << "📍 Addresses:" << std::endl;
        std::cout << "GLOBAL_STRING: " << (void*)GLOBAL_STRING << std::endl;
        std::cout << "GLOBAL_INT: " << (void*)&GLOBAL_INT << std::endl; 
        std::cout << "GLOBAL_DOUBLE: " << (void*)&GLOBAL_DOUBLE << std::endl;
        std::cout << "ERROR_MESSAGES: " << (void*)ERROR_MESSAGES << std::endl;
        std::cout << "LOOKUP_TABLE: " << (void*)LOOKUP_TABLE << std::endl;
        
        // Analyze address ranges to determine sections
        uintptr_t global_str_addr = (uintptr_t)GLOBAL_STRING;
        uintptr_t global_int_addr = (uintptr_t)&GLOBAL_INT;
        uintptr_t array_addr = (uintptr_t)ERROR_MESSAGES;
        
        std::cout << "\n📊 Address Analysis:" << std::endl;
        std::cout << "String literal range: 0x" << std::hex << global_str_addr << std::dec << std::endl;
        std::cout << "Global int range: 0x" << std::hex << global_int_addr << std::dec << std::endl;
        std::cout << "Array range: 0x" << std::hex << array_addr << std::dec << std::endl;
        
        // Check if they're in the same memory region (.rodata)
        if (abs((long)(global_str_addr - array_addr)) < 0x10000) {
            std::cout << "✅ Global constants appear to be in the same section (.rodata)" << std::endl;
        }
    }
    
    static void analyze_local_vs_global() {
        std::cout << "\n=== Local vs Global Constants ===" << std::endl;
        
        // Local constants
        const char* local_string = "Local string literal";
        const int local_int = 100;
        const double local_double = 2.718;
        
        // Local array initialization
        const int local_array[] = {1, 2, 3, 4, 5};
        
        std::cout << "🏠 Local addresses:" << std::endl;
        std::cout << "local_string: " << (void*)local_string << std::endl;
        std::cout << "local_int: " << (void*)&local_int << std::endl;
        std::cout << "local_double: " << (void*)&local_double << std::endl;
        std::cout << "local_array: " << (void*)local_array << std::endl;
        
        std::cout << "\n🌐 Global addresses:" << std::endl;
        std::cout << "GLOBAL_STRING: " << (void*)GLOBAL_STRING << std::endl;
        std::cout << "GLOBAL_INT: " << (void*)&GLOBAL_INT << std::endl;
        
        // Compare stack vs .rodata/other sections
        uintptr_t stack_addr = (uintptr_t)&local_int;
        uintptr_t rodata_addr = (uintptr_t)GLOBAL_STRING;
        
        if (stack_addr > rodata_addr) {
            std::cout << "✅ Stack addresses are higher than .rodata (typical layout)" << std::endl;
        }
        
        std::cout << "📏 Address difference: 0x" << std::hex << (stack_addr - rodata_addr) << std::dec << std::endl;
    }
    
    static void analyze_string_storage_strategies() {
        std::cout << "\n=== String Storage Strategy Analysis ===" << std::endl;
        
        // Different string creation methods
        const char* literal1 = "Shared literal";
        const char* literal2 = "Shared literal";  // Same content
        const char* literal3 = "Different content";
        
        std::string str1("Shared literal");  // Copy from .rodata to heap
        std::string str2("Shared literal");  // Another copy to heap
        
        std::string small_str("Hi");  // SSO candidate
        std::string large_str("This is a very long string that definitely exceeds SSO buffer limits");
        
        std::cout << "🔗 Literal deduplication test:" << std::endl;
        std::cout << "literal1 == literal2: " << (literal1 == literal2 ? "YES (same address)" : "NO") << std::endl;
        std::cout << "literal1 == literal3: " << (literal1 == literal3 ? "YES" : "NO (different address)") << std::endl;
        
        std::cout << "\n📍 String object data locations:" << std::endl;
        std::cout << "str1.data(): " << (void*)str1.data() << std::endl;
        std::cout << "str2.data(): " << (void*)str2.data() << std::endl;
        std::cout << "small_str.data(): " << (void*)small_str.data() << std::endl;
        std::cout << "large_str.data(): " << (void*)large_str.data() << std::endl;
        
        // SSO detection
        bool small_is_sso = (small_str.data() >= (const char*)&small_str && 
                           small_str.data() < (const char*)&small_str + sizeof(small_str));
        bool large_is_sso = (large_str.data() >= (const char*)&large_str && 
                            large_str.data() < (const char*)&large_str + sizeof(large_str));
        
        std::cout << "\n🔍 SSO Analysis:" << std::endl;
        std::cout << "Small string uses SSO: " << (small_is_sso ? "YES" : "NO") << std::endl;
        std::cout << "Large string uses SSO: " << (large_is_sso ? "YES" : "NO") << std::endl;
        std::cout << "sizeof(std::string): " << sizeof(std::string) << " bytes" << std::endl;
    }
    
    static void analyze_cache_layout() {
        std::cout << "\n=== Cache Layout Analysis ===" << std::endl;
        
        // Create strings that will be placed sequentially in .rodata
        const char* sequential_strings[] = {
            "String 01", "String 02", "String 03", "String 04", "String 05",
            "String 06", "String 07", "String 08", "String 09", "String 10",
            "String 11", "String 12", "String 13", "String 14", "String 15",
            "String 16"
        };
        
        std::cout << "🗂️ Sequential string addresses:" << std::endl;
        for (int i = 0; i < 16; ++i) {
            std::cout << "String " << (i+1) << ": " << (void*)sequential_strings[i] << std::endl;
        }
        
        // Calculate address differences to estimate cache line utilization
        uintptr_t first_addr = (uintptr_t)sequential_strings[0];
        uintptr_t last_addr = (uintptr_t)sequential_strings[15];
        
        std::cout << "\n📏 Address span: " << (last_addr - first_addr) << " bytes" << std::endl;
        std::cout << "Cache lines used (64-byte lines): " << ((last_addr - first_addr) / 64 + 1) << std::endl;
        
        // This demonstrates spatial locality of .rodata strings
        if ((last_addr - first_addr) < 1024) {
            std::cout << "✅ All strings fit within ~1KB region (excellent cache locality)" << std::endl;
        }
    }
    
    static void memory_section_detection() {
        std::cout << "\n=== Memory Section Detection ===" << std::endl;
        
        // Get addresses of different types of data
        uintptr_t code_addr = (uintptr_t)&analyze_global_constants;  // .text
        uintptr_t rodata_addr = (uintptr_t)"Static string";          // .rodata
        
        static int static_var = 123;                  // .data
        uintptr_t data_addr = (uintptr_t)&static_var;
        
        static int uninitialized_var;                 // .bss
        uintptr_t bss_addr = (uintptr_t)&uninitialized_var;
        
        int stack_var = 456;                          // Stack
        uintptr_t stack_addr = (uintptr_t)&stack_var;
        
        int* heap_var = new int(789);                 // Heap
        uintptr_t heap_addr = (uintptr_t)heap_var;
        
        std::cout << "📂 Memory sections (typical order low to high):" << std::endl;
        std::cout << ".text (code): 0x" << std::hex << code_addr << std::dec << std::endl;
        std::cout << ".rodata: 0x" << std::hex << rodata_addr << std::dec << std::endl; 
        std::cout << ".data: 0x" << std::hex << data_addr << std::dec << std::endl;
        std::cout << ".bss: 0x" << std::hex << bss_addr << std::dec << std::endl;
        std::cout << "heap: 0x" << std::hex << heap_addr << std::dec << std::endl;
        std::cout << "stack: 0x" << std::hex << stack_addr << std::dec << std::endl;
        
        delete heap_var;
        
        std::cout << "\n✅ Memory layout analysis complete" << std::endl;
    }
};

// Static member definitions
const char* MemoryLayoutAnalyzer::GLOBAL_STRING = "Global string in .rodata";

const char* const MemoryLayoutAnalyzer::ERROR_MESSAGES[] = {
    "File not found",
    "Permission denied", 
    "Network timeout",
    "Invalid argument",
    "Out of memory"
};

const int MemoryLayoutAnalyzer::LOOKUP_TABLE[] = {
    1, 4, 9, 16, 25, 36, 49, 64, 81, 100
};

// Define constexpr static members to satisfy ODR-use when taking their addresses
constexpr int MemoryLayoutAnalyzer::GLOBAL_INT;
constexpr double MemoryLayoutAnalyzer::GLOBAL_DOUBLE;

int main() {
    std::cout << "=== Advanced Memory Layout Analysis ===" << std::endl;
    std::cout << "Architecture: " << sizeof(void*)*8 << "-bit" << std::endl;
    std::cout << "sizeof(int): " << sizeof(int) << " bytes" << std::endl;
    std::cout << "sizeof(double): " << sizeof(double) << " bytes" << std::endl;
    std::cout << "sizeof(std::string): " << sizeof(std::string) << " bytes" << std::endl;
    
    MemoryLayoutAnalyzer::analyze_global_constants();
    MemoryLayoutAnalyzer::analyze_local_vs_global();
    MemoryLayoutAnalyzer::analyze_string_storage_strategies();
    MemoryLayoutAnalyzer::analyze_cache_layout();
    MemoryLayoutAnalyzer::memory_section_detection();
    
    return 0;
}

/*
 * Analiz Komutları:
 * 
 * 1. Derle ve çalıştır:
 *    g++ -O1 -o memory_layout memory_layout_analysis.cpp
 *    ./memory_layout
 * 
 * 2. Bellek haritası analizi (Linux):
 *    cat /proc/$(pgrep memory_layout)/maps
 * 
 * 3. Bölüm (section) analizi:
 *    objdump -h memory_layout
 *    objdump -s -j .rodata memory_layout
 *    objdump -s -j .data memory_layout
 * 
 * 4. Sembol tablosu:
 *    nm memory_layout | sort
 *    readelf -s memory_layout
 * 
 * 5. GDB ile çalışma zamanı analizi:
 *    gdb memory_layout
 *    (gdb) break main
 *    (gdb) run
 *    (gdb) info proc mappings
 *    (gdb) x/10s 0x[rodata_address]
 * 
 * 6. Valgrind bellek analizi:
 *    valgrind --tool=memcheck ./memory_layout
 * 
 * Beklenen Gözlemler:
 * - Aynı olan string literal'ler adreslerini paylaşır
 * - Global sabitler .rodata bölümünde yer alır
 * - std::string nesneleri ayrı heap alanı ayırır
 * - Küçük string'ler SSO (Small String Optimization) kullanabilir
 * - Bellek bölümleri tipik ELF yerleşimini takip eder
 * - .rodata kümeleşmesi cache yerelliğine fayda sağlar
 * 
 * Öne Çıkan Öğrenimler:
 * - Adres alanı yerleşimi optimizasyon stratejilerini ortaya çıkarır
 * - String deduplikasyonu tüm program genelinde çalışır
 * - SSO küçük string'ler için heap tahsisini azaltır
 * - Bellek bölümleri farklı amaçlara hizmet eder ve farklı özelliklere sahiptir
 * - Cache-dostu yerleşim performansı artırır
 */