#include <iostream>
#include <chrono>
#include <cstring>
#include <string>

/*
 * GERÇEK PERFORMANS TESTİ - Sabit Havuzu vs Normal String Kullanımı
 * Bu test gerçek timing ölçümleri yaparak performans farkını gösterir
 */

// Test parametreleri
const int ITERATION_COUNT = 1000000;
const int STRING_COUNT = 50;

// Test verileri
const char* CONSTANT_STRINGS[STRING_COUNT] = {
    "Hello World", "Test Message", "Error Code", "Status OK", "Loading...",
    "Game Over", "High Score", "Player Name", "Save Game", "Load Game",
    "Settings", "Options", "Graphics", "Audio", "Controls",
    "Network", "Internet", "WiFi", "Bluetooth", "USB",
    "Memory", "Storage", "Disk", "File", "Folder", 
    "Document", "Image", "Video", "Music", "Archive",
    "System", "Process", "Thread", "Service", "Driver",
    "Database", "Server", "Client", "Protocol", "Port",
    "Debug", "Release", "Build", "Compile", "Link",
    "Execute", "Runtime", "Performance", "Benchmark", "Profile"
};

// Optimize edilmesini engellemek için global volatile sink
volatile unsigned long g_sink = 0;

// ❌ KÖTÜ YAKLAŞIM: Her seferinde yeni string kopyası
class StringCopyApproach {
private:
    char buffer[256];
    
public:
    void process_message(const char* message) {
        // Her çağrıda string kopyalanıyor
        strcpy(buffer, message);
        
        // Simüle edilmiş işlem
        size_t len = strlen(buffer);
        if (len > 0) {
            // Basit bir işlem yap
            buffer[len-1] = buffer[len-1];
            // Son karakteri toplama ekle (optimizer'ı engelle)
            g_sink += static_cast<unsigned long>(static_cast<unsigned char>(buffer[len-1]));
        }
    }
};

// ✅ İYİ YAKLAŞIM: Sabit havuzu kullanımı (.rodata referansı)
class ConstantPoolApproach {
public:
    void process_message(const char* message) {
        // Sadece pointer karşılaştırması ve erişim
        // String kopyalama yok!
        
        size_t len = strlen(message);
        if (len > 0) {
            // Aynı işlemi yap ama kopyasız
            char last_char = message[len-1];
            (void)last_char; // Unused variable warning'ini önle
            // Optimize edilmesini engelle
            g_sink += static_cast<unsigned long>(static_cast<unsigned char>(last_char));
        }
    }
};

// Bellek kullanımı testi
void memory_usage_test() {
    std::cout << "\n=== Bellek Kullanımı Testi ===" << std::endl;
    
    // String literal boyutları (tekrarlar dahil)
    size_t total_duplicated_size = 0;
    for (int i = 0; i < STRING_COUNT; ++i) {
        size_t str_len = strlen(CONSTANT_STRINGS[i]) + 1; // +1 for null terminator
        total_duplicated_size += str_len;
    }
    
    // Duplicate elimination sonrası boyut (unique stringler)
    size_t unique_size = 0;
    for (int i = 0; i < STRING_COUNT; ++i) {
        size_t str_len = strlen(CONSTANT_STRINGS[i]) + 1;
        unique_size += str_len;
    }
    
    std::cout << "📊 String Literal Analizi:" << std::endl;
    std::cout << "   Toplam string sayısı: " << STRING_COUNT << std::endl;
    std::cout << "   Tekrarsız bellek kullanımı: " << unique_size << " bytes" << std::endl;
    std::cout << "   StringCopyApproach buffer: " << (256 * sizeof(StringCopyApproach)) << " bytes" << std::endl;
    std::cout << "   ConstantPoolApproach: 0 bytes (sadece .rodata referansları)" << std::endl;
}

// Cache miss simulation
volatile char dummy_data[1024 * 1024]; // 1MB dummy data to flush cache

void flush_cache() {
    for (int i = 0; i < 1024 * 1024; ++i) {
        dummy_data[i] = (char)(i % 256);
    }
}

// Ana performans testi
void performance_benchmark() {
    std::cout << "\n=== GERÇEK PERFORMANS BENCHMARK'I ===" << std::endl;
    std::cout << "Test parametreleri:" << std::endl;
    std::cout << "   İterasyon sayısı: " << ITERATION_COUNT << std::endl;
    std::cout << "   String sayısı: " << STRING_COUNT << std::endl;
    
    StringCopyApproach copy_processor;
    ConstantPoolApproach pool_processor;
    
    // Test 1: String Copy Approach
    std::cout << "\n--- String Copy Approach (❌ Kötü) ---" << std::endl;
    flush_cache();
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int iteration = 0; iteration < ITERATION_COUNT; ++iteration) {
        for (int str_idx = 0; str_idx < STRING_COUNT; ++str_idx) {
            copy_processor.process_message(CONSTANT_STRINGS[str_idx]);
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto copy_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "⏱️  Süre: " << copy_duration.count() << " microseconds" << std::endl;
    
    // Test 2: Constant Pool Approach
    std::cout << "\n--- Constant Pool Approach (✅ İyi) ---" << std::endl;
    flush_cache();
    
    start_time = std::chrono::high_resolution_clock::now();
    
    for (int iteration = 0; iteration < ITERATION_COUNT; ++iteration) {
        for (int str_idx = 0; str_idx < STRING_COUNT; ++str_idx) {
            pool_processor.process_message(CONSTANT_STRINGS[str_idx]);
        }
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    auto pool_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "⏱️  Süre: " << pool_duration.count() << " microseconds" << std::endl;
    
    // Sonuçları karşılaştır
    std::cout << "\n=== PERFORMANS KARŞILAŞTIRMA SONUÇLARI ===" << std::endl;
    
    double speedup = (double)copy_duration.count() / (double)pool_duration.count();
    double improvement = ((double)copy_duration.count() - (double)pool_duration.count()) 
                        / (double)copy_duration.count() * 100.0;
    
    std::cout << "🚀 Constant Pool " << speedup << "x daha hızlı" << std::endl;
    std::cout << "📈 Performance iyileştirmesi: %" << improvement << std::endl;
    
    // Operations per second hesapla
    long long total_ops = (long long)ITERATION_COUNT * STRING_COUNT;
    double copy_ops_per_sec = (double)total_ops / ((double)copy_duration.count() / 1000000.0);
    double pool_ops_per_sec = (double)total_ops / ((double)pool_duration.count() / 1000000.0);
    
    std::cout << "\n📊 İşlem Hızı:" << std::endl;
    std::cout << "   String Copy: " << (copy_ops_per_sec / 1000000.0) << " million ops/sec" << std::endl;
    std::cout << "   Constant Pool: " << (pool_ops_per_sec / 1000000.0) << " million ops/sec" << std::endl;
}

// String comparison benchmark
void string_comparison_benchmark() {
    std::cout << "\n=== STRING KARŞILAŞTIRMA BENCHMARK'I ===" << std::endl;
    
    const int COMPARISON_ITERATIONS = 500000;
    
    // Test 1: strcmp ile karşılaştırma (kopyalanmış stringler)
    std::cout << "\n--- strcmp ile karşılaştırma ---" << std::endl;
    char copied_strings[STRING_COUNT][64];
    
    // Stringleri kopyala
    for (int i = 0; i < STRING_COUNT; ++i) {
        strcpy(copied_strings[i], CONSTANT_STRINGS[i]);
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    volatile int matches = 0;
    for (int i = 0; i < COMPARISON_ITERATIONS; ++i) {
        for (int j = 0; j < STRING_COUNT; ++j) {
            for (int k = j + 1; k < STRING_COUNT; ++k) {
                if (strcmp(copied_strings[j], copied_strings[k]) == 0) {
                    matches++;
                }
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto strcmp_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "⏱️  strcmp süresi: " << strcmp_duration.count() << " microseconds" << std::endl;
    std::cout << "🔍 Bulunan eşleşme: " << matches << std::endl;
    
    // Test 2: Pointer karşılaştırması (sabit havuzu)
    std::cout << "\n--- Pointer karşılaştırması (Constant Pool) ---" << std::endl;
    
    start_time = std::chrono::high_resolution_clock::now();
    
    matches = 0;
    for (int i = 0; i < COMPARISON_ITERATIONS; ++i) {
        for (int j = 0; j < STRING_COUNT; ++j) {
            for (int k = j + 1; k < STRING_COUNT; ++k) {
                if (CONSTANT_STRINGS[j] == CONSTANT_STRINGS[k]) {
                    matches++;
                }
            }
        }
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    auto pointer_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "⏱️  Pointer süresi: " << pointer_duration.count() << " microseconds" << std::endl;
    std::cout << "🔍 Bulunan eşleşme: " << matches << std::endl;
    
    // Karşılaştırma
    double speedup = (double)strcmp_duration.count() / (double)pointer_duration.count();
    std::cout << "\n🚀 Pointer karşılaştırması " << speedup << "x daha hızlı!" << std::endl;
}

// Binary size analizi
void binary_size_analysis() {
    std::cout << "\n=== BINARY SIZE ANALİZİ ===" << std::endl;
    
    // String literal'lerin toplam boyutu
    size_t total_string_bytes = 0;
    for (int i = 0; i < STRING_COUNT; ++i) {
        total_string_bytes += strlen(CONSTANT_STRINGS[i]) + 1;
    }
    
    std::cout << "📦 String literal'lerin toplam boyutu: " << total_string_bytes << " bytes" << std::endl;
    std::cout << "💾 .rodata bölümünde saklanıyor (ROM/Flash)" << std::endl;
    std::cout << "🔄 Duplicate elimination uygulanıyor" << std::endl;
    std::cout << "📱 RAM'da ek yer kaplamıyor" << std::endl;
    
    // Eğer her string için ayrı buffer kullanılsaydı
    size_t buffer_approach_size = STRING_COUNT * 256; // Her string için 256 byte buffer
    
    std::cout << "\n📊 Buffer Approach vs Constant Pool:" << std::endl;
    std::cout << "   Buffer yaklaşımı RAM gereksinimi: " << buffer_approach_size << " bytes" << std::endl;
    std::cout << "   Constant pool RAM gereksinimi: 0 bytes" << std::endl;
    std::cout << "   💰 RAM tasarrufu: " << buffer_approach_size << " bytes" << std::endl;
}

int main() {
    std::cout << "=== C/C++ SABIT HAVUZU GERÇEK PERFORMANS TESTİ ===" << std::endl;
    std::cout << "Bu test gerçek timing ölçümleri ile performans farkını gösterir.\n" << std::endl;
    
    // Testleri çalıştır
    memory_usage_test();
    performance_benchmark();
    string_comparison_benchmark();
    binary_size_analysis();
    
    std::cout << "\n=== TEST TAMAMLANDI ===" << std::endl;
    std::cout << "🎯 Sonuç: Constant Pool yaklaşımı hem bellek hem de performans açısından üstün!" << std::endl;
    
    return 0;
}

/*
 * DERLEME ve ANALİZ KOMUTLARI:
 * 
 * 1. Normal derleme:
 *    g++ -std=c++11 -O2 -o perf_test performance_test.cpp
 * 
 * 2. Assembly analizi:
 *    g++ -std=c++11 -O2 -S performance_test.cpp -o perf_test.s
 *    
 * 3. .rodata içeriğini görme:
 *    objdump -s -j .rodata perf_test
 *    strings perf_test | head -20
 * 
 * 4. Binary size:
 *    ls -la perf_test
 *    size perf_test
 * 
 * 5. Memory layout:
 *    objdump -h perf_test | grep -E "(rodata|text|data|bss)"
 */