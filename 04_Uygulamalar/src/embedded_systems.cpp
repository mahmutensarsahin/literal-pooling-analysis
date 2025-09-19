#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>

/*
 * Embedded Systems Simulation
 * Bu örnek, gömülü sistemlerde string literal optimizasyonunu
 * Nintendo DS benzeri kısıtlar altında simüle eder.
 */

// Simulated platform constraints
namespace EmbeddedPlatform {
    // Nintendo DS-like constraints
    constexpr size_t MAIN_RAM_SIZE = 4 * 1024 * 1024;    // 4MB
    constexpr size_t GAME_ROM_SIZE = 32 * 1024 * 1024;   // 32MB
    constexpr size_t STACK_SIZE = 64 * 1024;             // 64KB
    constexpr size_t HEAP_SIZE = 1 * 1024 * 1024;        // 1MB available for heap
    
    // Simulated memory regions
    static size_t ram_usage = 0;
    static size_t rom_usage = 0;
    void* allocate_ram(size_t size) {
        if (ram_usage + size > HEAP_SIZE) {
            std::cout << "❌ RAM allocation failed! Needed: " << size 
                      << " bytes, Available: " << (HEAP_SIZE - ram_usage) << " bytes" << std::endl;
            return nullptr;
        }
        ram_usage += size;
        std::cout << "📱 RAM allocated: " << size << " bytes (Total: " << ram_usage << "/" << HEAP_SIZE << ")" << std::endl;
        return malloc(size);
    }
    
    void deallocate_ram(void* ptr, size_t size) {
        if (ptr) {
            free(ptr);
            ram_usage -= size;
            std::cout << "📱 RAM freed: " << size << " bytes (Total: " << ram_usage << "/" << HEAP_SIZE << ")" << std::endl;
        }
    }
    
    // Track ROM usage for a string literal referenced via const char*.
    // Using runtime strlen keeps compatibility with const char* tables.
    inline void register_rom_string(const char* str) {
        if (str) {
            rom_usage += std::strlen(str) + 1; // include null terminator
        }
    }
}

// ❌ BAD APPROACH: RAM-wasting message system
class RAMWastingMessageSystem {
private:
    char* error_buffer;
    char* status_buffer;
    char* debug_buffer;
    
    static constexpr size_t BUFFER_SIZE = 256;
    
public:
    RAMWastingMessageSystem() {
        error_buffer = (char*)EmbeddedPlatform::allocate_ram(BUFFER_SIZE);
        status_buffer = (char*)EmbeddedPlatform::allocate_ram(BUFFER_SIZE);
        debug_buffer = (char*)EmbeddedPlatform::allocate_ram(BUFFER_SIZE);
        
        if (!error_buffer || !status_buffer || !debug_buffer) {
            std::cout << "❌ CRITICAL: Message system initialization failed!" << std::endl;
        }
    }
    
    ~RAMWastingMessageSystem() {
        EmbeddedPlatform::deallocate_ram(error_buffer, BUFFER_SIZE);
        EmbeddedPlatform::deallocate_ram(status_buffer, BUFFER_SIZE);
        EmbeddedPlatform::deallocate_ram(debug_buffer, BUFFER_SIZE);
    }
    
    void show_error(const char* message) {
        if (error_buffer) {
            strncpy(error_buffer, message, BUFFER_SIZE - 1);
            error_buffer[BUFFER_SIZE - 1] = '\0';
            std::cout << "🚨 ERROR: " << error_buffer << std::endl;
        }
    }
    
    void show_status(const char* message) {
        if (status_buffer) {
            strncpy(status_buffer, message, BUFFER_SIZE - 1);
            status_buffer[BUFFER_SIZE - 1] = '\0';
            std::cout << "ℹ️  STATUS: " << status_buffer << std::endl;
        }
    }
    
    void show_debug(const char* message) {
        if (debug_buffer) {
            strncpy(debug_buffer, message, BUFFER_SIZE - 1);
            debug_buffer[BUFFER_SIZE - 1] = '\0';
            std::cout << "🔍 DEBUG: " << debug_buffer << std::endl;
        }
    }
};

// ✅ GOOD APPROACH: ROM-optimized message system
class ROMOptimizedMessageSystem {
private:
    // All strings stored in .rodata (ROM) - no RAM usage!
    static constexpr const char* ERROR_MESSAGES[] = {
        "Save file corrupted",
        "Battery low",
        "Cartridge error", 
        "Network timeout",
        "Memory card full",
        "Invalid save data",
        "Connection lost",
        "System overheated"
    };
    
    static constexpr const char* STATUS_MESSAGES[] = {
        "Game saved",
        "Level complete",
        "Loading...",
        "Connected to WiFi",
        "Achievement unlocked",
        "High score!",
        "Checkpoint reached",
        "Mission briefing"
    };
    
    static constexpr const char* DEBUG_MESSAGES[] = {
        "FPS: 60",
        "Memory: OK",
        "Audio: Ready", 
        "Graphics: Loaded",
        "Input: Responsive",
        "AI: Initialized",
        "Physics: Active",
        "Network: Stable"
    };
    
public:
    ROMOptimizedMessageSystem() {
        // Register ROM usage for tracking
        for (const auto& msg : ERROR_MESSAGES) {
            EmbeddedPlatform::register_rom_string(msg);
        }
        for (const auto& msg : STATUS_MESSAGES) {
            EmbeddedPlatform::register_rom_string(msg);
        }
        for (const auto& msg : DEBUG_MESSAGES) {
            EmbeddedPlatform::register_rom_string(msg);
        }
        
        std::cout << "✅ ROM-optimized message system initialized" << std::endl;
        std::cout << "💾 ROM usage: " << EmbeddedPlatform::rom_usage << " bytes" << std::endl;
    }
    
    void show_error(int error_id) {
        if (error_id >= 0 && error_id < sizeof(ERROR_MESSAGES)/sizeof(ERROR_MESSAGES[0])) {
            std::cout << "🚨 ERROR: " << ERROR_MESSAGES[error_id] << std::endl;
        }
    }
    
    void show_status(int status_id) {
        if (status_id >= 0 && status_id < sizeof(STATUS_MESSAGES)/sizeof(STATUS_MESSAGES[0])) {
            std::cout << "ℹ️  STATUS: " << STATUS_MESSAGES[status_id] << std::endl;
        }
    }
    
    void show_debug(int debug_id) {
        if (debug_id >= 0 && debug_id < sizeof(DEBUG_MESSAGES)/sizeof(DEBUG_MESSAGES[0])) {
            std::cout << "🔍 DEBUG: " << DEBUG_MESSAGES[debug_id] << std::endl;
        }
    }
    
    // Bonus: Multi-language support without RAM overhead
    enum Language { EN, JP, FR };
    static Language current_language;
    
    static constexpr const char* LOCALIZED_ERRORS[3][3] = {
        // English
        {"Save failed", "Battery low", "Connection error"},
        // Japanese  
        {"セーブに失敗", "バッテリー残量低", "接続エラー"},
        // French
        {"Échec sauvegarde", "Batterie faible", "Erreur connexion"}
    };
    
    void show_localized_error(int error_id) {
        if (error_id >= 0 && error_id < 3) {
            std::cout << "🌍 ERROR (" << (current_language == EN ? "EN" : current_language == JP ? "JP" : "FR") 
                      << "): " << LOCALIZED_ERRORS[current_language][error_id] << std::endl;
        }
    }
};

// Static member initialization
ROMOptimizedMessageSystem::Language ROMOptimizedMessageSystem::current_language = ROMOptimizedMessageSystem::EN;

// Game simulation class
class EmbeddedGame {
private:
    ROMOptimizedMessageSystem message_system;
    bool is_running = true;
    int frame_count = 0;
    
public:
    void initialize() {
        std::cout << "\n=== Embedded Game Initialization ===" << std::endl;
        message_system.show_status(2); // "Loading..."
        
        // Simulate initialization delays
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        message_system.show_debug(3); // "Graphics: Loaded"
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        message_system.show_debug(2); // "Audio: Ready"
        
        message_system.show_status(1); // "Level complete" (as ready)
        std::cout << "✅ Game initialized successfully" << std::endl;
    }
    
    void run_game_loop() {
        std::cout << "\n=== Running Game Loop (60 FPS target) ===" << std::endl;
        
        auto last_frame = std::chrono::high_resolution_clock::now();
        constexpr auto frame_duration = std::chrono::microseconds(16667); // ~60 FPS
        
        for (int i = 0; i < 180; ++i) { // 3 seconds at 60 FPS
            auto frame_start = std::chrono::high_resolution_clock::now();
            
            // Simulate game logic
            update_game();
            render_frame();
            
            // Show periodic debug info
            if (frame_count % 60 == 0) {
                message_system.show_debug(0); // "FPS: 60"
                message_system.show_debug(1); // "Memory: OK"
            }
            
            // Simulate occasional events
            if (frame_count == 120) {
                message_system.show_status(4); // "Achievement unlocked"
            }
            
            // Frame timing
            auto frame_end = std::chrono::high_resolution_clock::now();
            auto elapsed = frame_end - frame_start;
            
            if (elapsed < frame_duration) {
                std::this_thread::sleep_for(frame_duration - elapsed);
            }
            
            frame_count++;
        }
        
        std::cout << "🎮 Game loop completed. Total frames: " << frame_count << std::endl;
    }
    
    void simulate_error_conditions() {
        std::cout << "\n=== Simulating Error Conditions ===" << std::endl;
        
        // Simulate various error scenarios
        message_system.show_error(1); // "Battery low"
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        message_system.show_error(3); // "Network timeout"
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Test localization
        std::cout << "\n--- Testing Localization ---" << std::endl;
        ROMOptimizedMessageSystem::current_language = ROMOptimizedMessageSystem::EN;
        message_system.show_localized_error(0);
        
        ROMOptimizedMessageSystem::current_language = ROMOptimizedMessageSystem::JP; 
        message_system.show_localized_error(0);
        
        ROMOptimizedMessageSystem::current_language = ROMOptimizedMessageSystem::FR;
        message_system.show_localized_error(0);
    }
    
private:
    void update_game() {
        // Simulate game logic - no string operations needed
        // All game state updates happen here
    }
    
    void render_frame() {
        // Simulate rendering - strings only used for debug
        // Actual rendering would use graphics APIs
    }
};

// Memory usage comparison function
void compare_memory_usage() {
    std::cout << "\n=== Memory Usage Comparison ===" << std::endl;
    
    size_t initial_ram = EmbeddedPlatform::ram_usage;
    
    std::cout << "\n--- Testing RAM-wasting approach ---" << std::endl;
    {
        RAMWastingMessageSystem bad_system;
        bad_system.show_error("Test error message");
        bad_system.show_status("Test status message");
        bad_system.show_debug("Test debug message");
        
        size_t ram_after_bad = EmbeddedPlatform::ram_usage;
        std::cout << "RAM used by bad system: " << (ram_after_bad - initial_ram) << " bytes" << std::endl;
    } // bad_system destructor runs here
    
    std::cout << "\n--- Testing ROM-optimized approach ---" << std::endl;
    {
        ROMOptimizedMessageSystem good_system;
        good_system.show_error(0);
        good_system.show_status(0);  
        good_system.show_debug(0);
        
        size_t ram_after_good = EmbeddedPlatform::ram_usage;
        std::cout << "RAM used by good system: " << (ram_after_good - initial_ram) << " bytes" << std::endl;
    }
    
    std::cout << "\n📊 Summary:" << std::endl;
    std::cout << "RAM savings with ROM optimization: " << (3 * 256) << " bytes" << std::endl;
    std::cout << "ROM usage: " << EmbeddedPlatform::rom_usage << " bytes" << std::endl;
    std::cout << "Net benefit: " << ((3 * 256) - EmbeddedPlatform::rom_usage) << " bytes saved" << std::endl;
}

int main() {
    std::cout << "=== Embedded Systems String Optimization Demo ===" << std::endl;
    std::cout << "Platform Constraints:" << std::endl;
    std::cout << "📱 Main RAM: " << (EmbeddedPlatform::MAIN_RAM_SIZE / 1024) << "KB" << std::endl;
    std::cout << "💾 ROM: " << (EmbeddedPlatform::GAME_ROM_SIZE / 1024) << "KB" << std::endl;
    std::cout << "🔄 Available Heap: " << (EmbeddedPlatform::HEAP_SIZE / 1024) << "KB" << std::endl;
    
    // Memory usage comparison
    compare_memory_usage();
    
    // Full game simulation
    EmbeddedGame game;
    game.initialize();
    game.run_game_loop();
    game.simulate_error_conditions();
    
    std::cout << "\n=== Final Memory Report ===" << std::endl;
    std::cout << "Final RAM usage: " << EmbeddedPlatform::ram_usage << " bytes" << std::endl;
    std::cout << "ROM usage: " << EmbeddedPlatform::rom_usage << " bytes" << std::endl;
    std::cout << "RAM efficiency: " << (100.0 - (double)EmbeddedPlatform::ram_usage / EmbeddedPlatform::HEAP_SIZE * 100) << "% free" << std::endl;
    
    return 0;
}

/*
 * Derleme ve Analiz:
 * 
 * 1. Standart derleme:
 *    g++ -O2 -o embedded_sim embedded_systems.cpp -pthread
 * 
 * 2. Boyut odaklı derleme (embedded tarzı):
 *    g++ -Os -ffunction-sections -fdata-sections -o embedded_opt embedded_systems.cpp -Wl,--gc-sections -pthread
 * 
 * 3. .rodata kullanımını kontrol et:
 *    objdump -s -j .rodata embedded_opt | wc -l
 *    strings embedded_opt | head -20
 * 
 * 4. İkili dosya boyutu karşılaştırması:
 *    ls -la embedded_sim embedded_opt
 * 
 * 5. Bellek yerleşimi analizi:
 *    objdump -h embedded_opt | grep -E "(rodata|text|data|bss)"
 * 
 * Öne Çıkan Öğrenimler:
 * - Gömülü sistemler .rodata kullanımından ciddi fayda sağlar
 * - RAM’e kıyasla ROM/Flash genellikle daha boldur
 * - Kaynak kısıtlı ortamlarda string deduplikasyonu kritik önemdedir
 * - Düzenli .rodata yerleşimi ile yerelleştirme verimli uygulanabilir
 * - Gerçek zaman kısıtları dikkatli bellek tahsisi stratejileri gerektirir
 * 
 * Gerçek Dünya Uygulamaları:
 * - Nintendo DS/3DS oyun geliştirme
 * - Arduino/mikrodenetleyici projeleri  
 * - IoT cihaz yazılımları (firmware)
 * - Otomotiv gömülü sistemleri
 * - Medikal cihaz yazılımları
 */