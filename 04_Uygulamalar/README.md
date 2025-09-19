# Uygulamalar: Gerçek Dünya Senaryoları ve Vaka Çalışmaları

## 🎯 Bu Bölümde Ele Alacağımız Konular

- Gömülü sistemlerde bellek optimizasyonu
- Oyun geliştirmede performans kritik kod yazımı
- Web sunucusu ve backend uygulamalarda string yönetimi
- Yüksek performanslı hesaplama (HPC) senaryoları
- Belleği kısıtlı ortamlarda optimizasyon stratejileri

---

## 1. 🔧 Gömülü Sistemler

### 1.1 Nintendo DS Vaka Çalışması

**Platform Özellikleri:**
- ARM9 + ARM7 çift CPU
- 4MB Ana RAM (kıymetli!)
- 32MB Oyun Kartı ROM (bol)
- 96KB VRAM

**Problem:** String literalleri RAM'de tutuluyor, RAM sınırları aşılıyor.

```cpp
// ❌ KÖTÜ: RAM'i boşa harcayan yaklaşım
class GameMessages {
private:
    char error_buffer[100];
    char status_buffer[200];
    char debug_buffer[500];
    
public:
    void show_error(const char* msg) {
        strcpy(error_buffer, msg);  // RAM'e kopyalar!
        display_message(error_buffer);
    }
    
    void show_status(const char* msg) {
        strcpy(status_buffer, msg); // Daha fazla RAM kullanımı!
        display_message(status_buffer);
    }
};

// ✅ İYİ: ROM'da sabit tutar, RAM'i korur
class OptimizedGameMessages {
private:
    // .rodata'da saklanır → ROM/Flash'da
    static constexpr const char* ERROR_MSGS[] = {
        "Save file corrupted",
        "Low battery warning", 
        "Connection failed",
        "Invalid cartridge"
    };
    
    static constexpr const char* STATUS_MSGS[] = {
        "Game saved successfully",
        "Loading level...",
        "Connecting to WiFi",
        "Achievement unlocked!"
    };
    
public:
    void show_error(int error_id) {
        // Direkt ROM'dan okur, RAM kullanmaz
        if (error_id < sizeof(ERROR_MSGS)/sizeof(ERROR_MSGS[0])) {
            display_message(ERROR_MSGS[error_id]);
        }
    }
};
```

**📊 Bellek Tasarrufu:**
```
❌ Kötü yaklaşım:
- RAM kullanımı: örnek başına 800+ bayt
- Birden çok örnek: 800 × N bayt

✅ İyi yaklaşım:  
- RAM kullanımı: ~0 bayt (sadece pointer'lar)
- ROM kullanımı: toplam ~200 bayt (tüm örnekler arasında paylaşılan)
- RAM tasarrufu: ~800 × N bayt!
```

### 1.2 Arduino/Mikrodenetleyici Örneği

```cpp
// Arduino Uno: 2KB RAM, 32KB Flash
#include <Arduino.h>

// ❌ BAD: SRAM (RAM) kullanır
const char menu_item1[] = "1. Read sensors";
const char menu_item2[] = "2. Configure WiFi"; 
const char menu_item3[] = "3. System info";

// ✅ İYİ: Flash belleği (program hafızası) kullanır
const char menu_item1[] PROGMEM = "1. Read sensors";
const char menu_item2[] PROGMEM = "2. Configure WiFi";
const char menu_item3[] PROGMEM = "3. System info";

void display_menu() {
    char buffer[50];
    
    // Flash'tan RAM'e kopyalar (geçici)
    strcpy_P(buffer, menu_item1);
    Serial.println(buffer);
    
    strcpy_P(buffer, menu_item2); 
    Serial.println(buffer);
    
    strcpy_P(buffer, menu_item3);
    Serial.println(buffer);
    // buffer kapsamı dışına çıkınca SRAM serbest kalır
}
```

---

## 2. 🎮 Oyun Geliştirme Senaryoları

### 2.1 Gerçek Zamanlı Oyun Döngüsü Optimizasyonu

```cpp
class GameEngine {
private:
    // Sık kullanılan mesajlar - önbellek dostu yerleşim
    static constexpr const char* HOT_MESSAGES[] = {
        "OK",           // 3 bayt - sık kullanılır
        "Error",        // 6 bayt  
        "Loading",      // 8 bayt
        "Ready"         // 6 bayt
        // Toplam: 23 bayt → tek bir önbellek satırı içinde
    };
    
    // Daha seyrek kullanılan mesajlar - dağınık olabilir
    static constexpr const char* COLD_MESSAGES[] = {
        "Initializing graphics subsystem",
        "Connecting to multiplayer server", 
        "Loading game assets from disk",
        "Saving game progress to file"
    };
    
public:
    // 60 FPS oyun döngüsü - her karede çağrılır
    void update_frame() {
        // Sıcak yol - önbellek isabeti garanti
        if (is_error()) {
            show_message(HOT_MESSAGES[1]); // "Error"
        } else {
            show_message(HOT_MESSAGES[3]); // "Ready"
        }
    }
    
    // Soğuk yol - başlatma sırasında çağrılır
    void initialize_systems() {
        show_message(COLD_MESSAGES[0]); // Cache miss kabul edilebilir
        setup_graphics();
        show_message(COLD_MESSAGES[1]);
        setup_network();
        // ...
    }
};
```

### 2.2 Yerelleştirme ve i18n

```cpp
// Çok dilli oyun desteği
class LocalizedMessages {
private:
    enum Language { EN, TR, DE, FR, ES };
    static Language current_language;
    
    // Dil paketleri - .rodata'da düzenlenmiş
    static constexpr const char* MESSAGES_EN[] = {
        "Game Over",
        "High Score!",
        "Continue?",
        "Settings"
    };
    
    static constexpr const char* MESSAGES_TR[] = {
        "Oyun Bitti",
        "Yüksek Skor!",
        "Devam?",
        "Ayarlar"  
    };
    
    static constexpr const char* MESSAGES_DE[] = {
        "Spiel Vorbei",
        "Highscore!",
        "Weitermachen?",
        "Einstellungen"
    };
    
    static constexpr const char* const* LANGUAGE_PACKS[] = {
        MESSAGES_EN, MESSAGES_TR, MESSAGES_DE
    };
    
public:
    static const char* get_message(int msg_id) {
        // Tek dolaylı erişim - önbellek dostu
        return LANGUAGE_PACKS[current_language][msg_id];
    }
};

// Kullanım
void show_game_over() {
    const char* msg = LocalizedMessages::get_message(0);
    display_text(msg); // "Game Over" / "Oyun Bitti" / "Spiel Vorbei"
}
```

---

## 3. 🌐 Web Sunucusu ve Backend Uygulamaları

### 3.1 HTTP Durum Mesajları

```cpp
#include <unordered_map>
#include <string_view>

class HTTPServer {
private:
    // HTTP durum mesajları - .rodata'da tekilleştirme ile
    static constexpr const char* STATUS_200 = "200 OK";
    static constexpr const char* STATUS_404 = "404 Not Found";
    static constexpr const char* STATUS_500 = "500 Internal Server Error";
    
    // Yanıt şablonları
    static constexpr const char* HTML_TEMPLATE = 
        "HTTP/1.1 {}\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: {}\r\n"
        "\r\n"
        "{}";
        
    static constexpr const char* JSON_TEMPLATE =
        "HTTP/1.1 {}\r\n" 
        "Content-Type: application/json\r\n"
        "Content-Length: {}\r\n"
        "\r\n"
        "{}";
        
public:
    // Yüksek frekanslı istek işleyici
    void handle_request(const std::string& path) {
        if (path == "/") {
            // Sıcak yol - sık erişilir
            send_response(STATUS_200, get_homepage_content());
        } else if (path == "/api/status") {
            // API uç noktası - bu da sıcak  
            send_response(STATUS_200, R"({"status":"ok"})");
        } else {
            // 404 - daha seyrek ama yine de önemli
            send_response(STATUS_404, get_404_page());
        }
    }
    
private:
    void send_response(const char* status, const char* content) {
        // Şablon string'ler .rodata'da saklanır
        // Tekrarlanan string tahsisi yok
        printf(HTML_TEMPLATE, status, strlen(content), content);
    }
};
```

### 3.2 Veritabanı Sorgu Optimizasyonu

```cpp
class DatabaseManager {
private:
    // Önceden tanımlı sorgu metinleri - .rodata'da paylaşılan
    static constexpr const char* SELECT_USER = 
        "SELECT id, username, email FROM users WHERE id = ?";
    static constexpr const char* INSERT_USER = 
        "INSERT INTO users (username, email, created_at) VALUES (?, ?, NOW())";
    static constexpr const char* UPDATE_USER = 
        "UPDATE users SET username = ?, email = ? WHERE id = ?";
        
    // Hata mesajları
    static constexpr const char* ERR_CONNECTION = "Database connection failed";
    static constexpr const char* ERR_QUERY = "Query execution failed";
    static constexpr const char* ERR_NO_RESULTS = "No results found";
    
public:
    User get_user(int user_id) {
        // Hazırlanmış ifade ile sorgu metninin yeniden kullanımı
        auto stmt = prepare_statement(SELECT_USER); 
        stmt.bind(1, user_id);
        
        if (!stmt.execute()) {
            throw std::runtime_error(ERR_QUERY); // .rodata'dan
        }
        
        if (!stmt.has_results()) {
            throw std::runtime_error(ERR_NO_RESULTS); // .rodata'dan
        }
        
        return stmt.get_user();
    }
};
```

---

## 4. ⚡ Yüksek Performanslı Hesaplama (HPC)

### 4.1 Bilimsel Hesaplama Günlükleme

```cpp
#include <chrono>
#include <fstream>

class PerformanceLogger {
private:
    // Günlük (log) seviye string'leri - .rodata'da
    static constexpr const char* LEVEL_DEBUG = "DEBUG";
    static constexpr const char* LEVEL_INFO = "INFO";
    static constexpr const char* LEVEL_WARNING = "WARNING";
    static constexpr const char* LEVEL_ERROR = "ERROR";
    static constexpr const char* LEVEL_CRITICAL = "CRITICAL";
    
    // Bileşen adları - bilimsel hesaplama modülleri
    static constexpr const char* COMP_SOLVER = "LinearSolver";
    static constexpr const char* COMP_MATRIX = "MatrixOps";
    static constexpr const char* COMP_VECTOR = "VectorOps";
    static constexpr const char* COMP_IO = "FileIO";
    
    std::ofstream log_file;
    
public:
    // Hesaplama döngülerinde yüksek frekanslı günlükleme
    void log_iteration(int iteration, double residual) {
        // Optimize edilmiş string biçimlendirme - minimum tahsis
        log_file << LEVEL_DEBUG << " " << COMP_SOLVER 
                << " Yineleme: " << iteration 
                << " Kalıntı: " << residual << '\n';
                // '\n' vs std::endl - flush overhead yok
    }
    
    void log_performance_metric(const char* metric_name, double value) {
        log_file << LEVEL_INFO << " " << COMP_MATRIX
                << " " << metric_name << ": " << value << '\n';
    }
};

// CUDA çekirdeğinde sabit mesajlar
__device__ const char* CUDA_ERROR_MSGS[] = {
    "Thread divergence detected",
    "Memory coalescing failed", 
    "Shared memory bank conflict",
    "Register pressure too high"
};

__global__ void compute_kernel(float* data, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (idx >= n) {
        // GPU'dan hata raporlama
        printf("%s: idx=%d, n=%d\n", CUDA_ERROR_MSGS[0], idx, n);
        return;
    }
    
    // Hesaplama...
}
```

### 4.2 MPI (Message Passing Interface) İletişimi

```cpp
#include <mpi.h>

class MPIApplication {
private:
    int rank, size;
    
    // MPI iletişim etiketleri ve mesajları - .rodata
    static constexpr const char* MSG_MASTER_INIT = "Master process initialized";
    static constexpr const char* MSG_WORKER_READY = "Worker process ready";  
    static constexpr const char* MSG_COMPUTATION_START = "Starting computation";
    static constexpr const char* MSG_COMPUTATION_DONE = "Computation finished";
    
public:
    void run_parallel_computation() {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        
        if (rank == 0) {
            // Ana süreç
            printf("[Rank %d] %s\n", rank, MSG_MASTER_INIT);
            
            // İşi tüm işçilere (worker) yayınla
            for (int i = 1; i < size; i++) {
                MPI_Send(MSG_COMPUTATION_START, 
                        strlen(MSG_COMPUTATION_START), 
                        MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }
            
        } else {
            // İşçi süreçler
            char buffer[256];
            MPI_Recv(buffer, 256, MPI_CHAR, 0, 0, 
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    
            printf("[Rank %d] Received: %s\n", rank, buffer);
            
            // Hesaplama yap...
            
            MPI_Send(MSG_COMPUTATION_DONE, 
                    strlen(MSG_COMPUTATION_DONE),
                    MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        }
    }
};
```

---

## 5. 📱 Mobil Uygulama Geliştirme

### 5.1 iOS/Android Kaynak Yönetimi

```cpp
// Çapraz platform mobil string yönetimi
class MobileStringManager {
private:
    // Platforma özgü optimizasyon bayrakları
    #ifdef __ANDROID__
        static constexpr bool USE_AGGRESSIVE_COMPRESSION = true;
    #elif defined(__APPLE__)
        static constexpr bool USE_AGGRESSIVE_COMPRESSION = false;
    #endif
    
    // Arayüz (UI) string'leri - sık erişilir
    static constexpr const char* UI_STRINGS[] = {
        "Loading...",     // Açılış ekranı
        "Tap to continue", // Kullanıcı istemi
        "Settings",       // Gezinme
        "Back",           // Gezinme  
        "Done",           // İşlem onayı
        "Cancel",         // İşlem iptali
        "OK",             // Diyalog onayı
        "Error"           // Hata gösterimi
    };
    
    // Ağ hata mesajları - daha seyrek
    static constexpr const char* NETWORK_ERRORS[] = {
        "No internet connection available",
        "Server temporarily unavailable",
        "Request timeout occurred",
        "Invalid server response received"
    };
    
public:
    // Kritik yol - arayüz oluşturma (render)
    const char* get_ui_string(int string_id) {
        // Sıcak yol optimizasyonu - release derlemede sınır kontrolü elenir
        assert(string_id < sizeof(UI_STRINGS)/sizeof(UI_STRINGS[0]));
        return UI_STRINGS[string_id];
    }
    
    // Hata işleme - soğuk yol
    const char* get_network_error(int error_code) {
        if (error_code < sizeof(NETWORK_ERRORS)/sizeof(NETWORK_ERRORS[0])) {
            return NETWORK_ERRORS[error_code];
        }
        return "Unknown network error";
    }
};

// Platforma özgü bellek kısıtları
void optimize_for_platform() {
    #ifdef __ANDROID__
        // Android: Farklı RAM yapılandırmaları için optimize edin
        constexpr size_t STRING_CACHE_SIZE = 64; // Tedbirli
    #elif defined(__APPLE__)
        // iOS: Daha tutarlı donanım, daha agresif olunabilir  
        constexpr size_t STRING_CACHE_SIZE = 256;
    #endif
    
    // Gerektiğinde dinamik string'ler için LRU önbellek uygulayın...
}
```

---

## 6. 🔒 Güvenlik Kritik Uygulamalar

### 6.1 Kriptografik Hata Yönetimi

```cpp
#include <openssl/evp.h>

class CryptographicModule {
private:
    // Güvenlik hata mesajları - özenle hazırlanmış
    // İç uygulama ayrıntılarını ifşa etmez
    static constexpr const char* ERR_GENERIC = "Cryptographic operation failed";
    static constexpr const char* ERR_KEY_INVALID = "Invalid key format";
    static constexpr const char* ERR_BUFFER_SIZE = "Buffer size insufficient";
    static constexpr const char* ERR_NOT_INITIALIZED = "Module not initialized";
    
    // Dahili hata ayıklama mesajları - release derlemede dışarıda bırakılır
    #ifdef DEBUG
        static constexpr const char* DBG_KEY_GENERATED = "Key pair generated successfully";
        static constexpr const char* DBG_HASH_COMPUTED = "Hash computation completed";
    #endif
    
public:
    bool encrypt_data(const uint8_t* plaintext, size_t len, 
                     uint8_t* ciphertext, size_t* cipher_len) {
        if (!plaintext || !ciphertext) {
            log_error(ERR_BUFFER_SIZE);
            return false;
        }
        
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            log_error(ERR_GENERIC); // OpenSSL ayrıntılarını ifşa etmeyin
            return false;  
        }
        
        // Şifreleme mantığı...
        
        #ifdef DEBUG
            log_debug(DBG_HASH_COMPUTED);
        #endif
        
        EVP_CIPHER_CTX_free(ctx);
        return true;
    }
    
private:
    void log_error(const char* message) {
        // Güvenlik denetim günlüğü - mesaj .rodata'dan
        audit_logger.log_security_event(LEVEL_ERROR, message);
    }
};
```

---

## 📋 Bu Bölüm Özeti

### 🔑 Kritik Çıkarımlar

1. **Platform Farkındalığı**: Her platformun bellek kısıtları farklıdır
2. **Sıcak vs Soğuk Yol**: Sık kullanılan string'ler önbellek dostu şekilde yerleştirilmeli  
3. **Yerelleştirme Stratejisi**: Çoklu dil desteği için düzenli .rodata yerleşimi
4. **Güvenlik Hususları**: Hata mesajları bilgi sızdırmamalı
5. **Performans Açısından Kritik Yollar**: Oyun döngülerinde ve HPC'de string tahsisi en aza indirilmeli

### 📊 Optimizasyon Stratejileri Özeti

| Senaryo | RAM Kısıtı | Strateji | Araçlar |
|---------|----------------|----------|-------|
| **Gömülü** | Aşırı (KB) | .rodata+Flash, PROGMEM | Arduino IDE, Keil |
| **Oyun Geliştirme** | Yüksek (GB) | Önbellek yakınlığı, sıcak/soğuk | Profiler, VTune |
| **Web Sunucusu** | Orta (GB) | Şablon yeniden kullanımı, havuzlama | Valgrind, perf |
| **HPC** | Değişken | Minimum tahsis, günlükleme optimizasyonu | MPI profilleri, CUDA araçları |
| **Mobil** | Cihaza bağlı | Platforma özgü bayraklar | Xcode Instruments, Android Studio |
| **Güvenlik** | Herhangi | Bilgi gizleme, denetim | Statik analiz araçları |

### 🚀 Sonraki Adım

Bir sonraki bölümde bu tüm kavramları özetleyeceğiz ve pratik ipuçları ile sorun giderme teknikleri sunacağız.

---

**💡 İpucu**: Gerçek projelerinizde bu optimizasyonları uygularken, önce profilleyin, sonra optimize edin. Erken optimizasyon tüm kötülüklerin köküdür — ancak bilinçli optimizasyon mükemmelliğe giden yoldur!
