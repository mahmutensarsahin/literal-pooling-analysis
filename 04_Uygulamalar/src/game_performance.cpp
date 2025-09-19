#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <random>
#include <unordered_map>
#include <string>
#include <cstring>

/*
 * Game Development Performance Optimization
 * Bu örnek, gerçek zamanlı oyun geliştirmede string literal 
 * optimizasyonunun performans üzerindeki etkilerini gösterir.
 */

namespace GameEngine {
    // Game performance metrics
    struct PerformanceMetrics {
        double frame_time_ms = 0.0;
        int fps = 0;
        size_t memory_usage = 0;
        int cache_misses = 0;
        int string_allocations = 0;
    };
    
    // Global performance tracker
    PerformanceMetrics current_metrics;
    
    // Simulated cache system
    constexpr size_t CACHE_SIZE = 64; // 64-byte cache lines
    std::unordered_map<uintptr_t, int> cache_access_count;
    
    void track_cache_access(const void* ptr) {
        uintptr_t cache_line = ((uintptr_t)ptr) & ~(CACHE_SIZE - 1);
        cache_access_count[cache_line]++;
    }
    
    void reset_performance_metrics() {
        current_metrics = PerformanceMetrics{};
        cache_access_count.clear();
    }
}

// ❌ BAD APPROACH: String-heavy, cache-unfriendly game system
class IneffientGameSystem {
private:
    std::vector<std::string> dynamic_messages;
    std::random_device rd;
    std::mt19937 gen;
    
    // Dynamic string generation - causes heap allocations
    std::string generate_player_message(const std::string& player_name, int score) {
        GameEngine::current_metrics.string_allocations++;
        return "Player " + player_name + " scored " + std::to_string(score) + " points!";
    }
    
    std::string generate_enemy_message(const std::string& enemy_type, int health) {
        GameEngine::current_metrics.string_allocations++;
        return "Enemy " + enemy_type + " has " + std::to_string(health) + " HP remaining";
    }
    
    std::string generate_item_message(const std::string& item_name, int count) {
        GameEngine::current_metrics.string_allocations++;
        return "Collected " + std::to_string(count) + " x " + item_name;
    }
    
public:
    IneffientGameSystem() : gen(rd()) {}
    
    void simulate_game_frame() {
        auto frame_start = std::chrono::high_resolution_clock::now();
        
        // Simulate 100 game objects updating their status messages
        for (int i = 0; i < 100; ++i) {
            std::uniform_int_distribution<> dis(1, 3);
            int message_type = dis(gen);
            
            switch (message_type) {
                case 1: {
                    std::string msg = generate_player_message("Player" + std::to_string(i), i * 10);
                    dynamic_messages.push_back(msg);
                    GameEngine::track_cache_access(msg.data());
                    break;
                }
                case 2: {
                    std::string msg = generate_enemy_message("Goblin" + std::to_string(i), 100 - i);
                    dynamic_messages.push_back(msg);
                    GameEngine::track_cache_access(msg.data());
                    break;
                }
                case 3: {
                    std::string msg = generate_item_message("Coin" + std::to_string(i), i % 10);
                    dynamic_messages.push_back(msg);
                    GameEngine::track_cache_access(msg.data());
                    break;
                }
            }
        }
        
        // Clear messages (simulate UI update cycle)
        dynamic_messages.clear(); // Causes deallocation overhead
        
        auto frame_end = std::chrono::high_resolution_clock::now();
        GameEngine::current_metrics.frame_time_ms = 
            std::chrono::duration<double, std::milli>(frame_end - frame_start).count();
    }
    
    void run_performance_test(int frame_count) {
        std::cout << "🐌 Running inefficient game system test..." << std::endl;
        GameEngine::reset_performance_metrics();
        
        auto test_start = std::chrono::high_resolution_clock::now();
        
        for (int frame = 0; frame < frame_count; ++frame) {
            simulate_game_frame();
        }
        
        auto test_end = std::chrono::high_resolution_clock::now();
        double total_time_ms = std::chrono::duration<double, std::milli>(test_end - test_start).count();
        
        GameEngine::current_metrics.fps = static_cast<int>(frame_count / (total_time_ms / 1000.0));
        GameEngine::current_metrics.cache_misses = GameEngine::cache_access_count.size();
        
        std::cout << "📊 Inefficient System Results:" << std::endl;
        std::cout << "   Average frame time: " << (total_time_ms / frame_count) << " ms" << std::endl;
        std::cout << "   FPS: " << GameEngine::current_metrics.fps << std::endl;
        std::cout << "   String allocations: " << GameEngine::current_metrics.string_allocations << std::endl;
        std::cout << "   Unique cache lines accessed: " << GameEngine::current_metrics.cache_misses << std::endl;
    }
};

// ✅ GOOD APPROACH: .rodata-optimized, cache-friendly game system  
class OptimizedGameSystem {
private:
    // Pre-defined message templates in .rodata - cache friendly
    static constexpr const char* PLAYER_MESSAGES[] = {
        "Player scored points!",
        "Player gained experience!",
        "Player leveled up!",
        "Player acquired skill!",
        "Player completed quest!",
        "Player found treasure!",
        "Player defeated enemy!",
        "Player discovered location!"
    };
    
    static constexpr const char* ENEMY_MESSAGES[] = {
        "Enemy takes damage!",
        "Enemy attacks player!",
        "Enemy uses special ability!",
        "Enemy retreats!",
        "Enemy calls reinforcements!",
        "Enemy blocks attack!",
        "Enemy dodges!",
        "Enemy is defeated!"
    };
    
    static constexpr const char* ITEM_MESSAGES[] = {
        "Item collected!",
        "Item used successfully!",
        "Item durability decreased!",
        "Item enhanced!",
        "Item sold!",
        "Item equipped!",
        "Item crafted!",
        "Item discovered!"
    };
    
    static constexpr const char* HOT_MESSAGES[] = {
        "OK",        // 3 bytes - very frequent
        "Error",     // 6 bytes - frequent  
        "Loading",   // 8 bytes - frequent
        "Ready",     // 6 bytes - frequent
        "Done"       // 5 bytes - frequent
        // Total: 28 bytes - fits in single cache line!
    };
    
    std::random_device rd;
    std::mt19937 gen;
    
    // Message ID tracking instead of string storage
    std::vector<int> message_ids;
    std::vector<int> message_types;
    
public:
    OptimizedGameSystem() : gen(rd()) {
        // Pre-reserve capacity to avoid reallocations
        message_ids.reserve(1000);
        message_types.reserve(1000);
    }
    
    void simulate_game_frame() {
        auto frame_start = std::chrono::high_resolution_clock::now();
        
        // Clear previous frame data
        message_ids.clear();
        message_types.clear();
        
        // Simulate 100 game objects - but using message IDs instead of strings
        for (int i = 0; i < 100; ++i) {
            std::uniform_int_distribution<> type_dis(0, 2);
            int message_type = type_dis(gen);
            
            message_types.push_back(message_type);
            
            switch (message_type) {
                case 0: { // Player message
                    std::uniform_int_distribution<> msg_dis(0, 7);
                    int msg_id = msg_dis(gen);
                    message_ids.push_back(msg_id);
                    GameEngine::track_cache_access(PLAYER_MESSAGES[msg_id]);
                    break;
                }
                case 1: { // Enemy message
                    std::uniform_int_distribution<> msg_dis(0, 7);
                    int msg_id = msg_dis(gen);
                    message_ids.push_back(msg_id);
                    GameEngine::track_cache_access(ENEMY_MESSAGES[msg_id]);
                    break;
                }
                case 2: { // Item message
                    std::uniform_int_distribution<> msg_dis(0, 7);
                    int msg_id = msg_dis(gen);
                    message_ids.push_back(msg_id);
                    GameEngine::track_cache_access(ITEM_MESSAGES[msg_id]);
                    break;
                }
            }
        }
        
        // Simulate frequent status checks using hot messages
        for (int i = 0; i < 50; ++i) {
            std::uniform_int_distribution<> hot_dis(0, 4);
            int hot_msg = hot_dis(gen);
            GameEngine::track_cache_access(HOT_MESSAGES[hot_msg]);
        }
        
        auto frame_end = std::chrono::high_resolution_clock::now();
        GameEngine::current_metrics.frame_time_ms = 
            std::chrono::duration<double, std::milli>(frame_end - frame_start).count();
    }
    
    void run_performance_test(int frame_count) {
        std::cout << "🚀 Running optimized game system test..." << std::endl;
        GameEngine::reset_performance_metrics();
        
        auto test_start = std::chrono::high_resolution_clock::now();
        
        for (int frame = 0; frame < frame_count; ++frame) {
            simulate_game_frame();
        }
        
        auto test_end = std::chrono::high_resolution_clock::now();
        double total_time_ms = std::chrono::duration<double, std::milli>(test_end - test_start).count();
        
        GameEngine::current_metrics.fps = static_cast<int>(frame_count / (total_time_ms / 1000.0));
        GameEngine::current_metrics.cache_misses = GameEngine::cache_access_count.size();
        
        std::cout << "📊 Optimized System Results:" << std::endl;
        std::cout << "   Average frame time: " << (total_time_ms / frame_count) << " ms" << std::endl;
        std::cout << "   FPS: " << GameEngine::current_metrics.fps << std::endl;
        std::cout << "   String allocations: " << GameEngine::current_metrics.string_allocations << std::endl;
        std::cout << "   Unique cache lines accessed: " << GameEngine::current_metrics.cache_misses << std::endl;
    }
    
    // Demonstration of message retrieval (for actual display)
    void display_recent_messages(int count = 5) {
        std::cout << "\n📱 Recent Game Messages:" << std::endl;
        
        int display_count = std::min(count, static_cast<int>(message_ids.size()));
        
        for (int i = 0; i < display_count; ++i) {
            int msg_type = message_types[i];
            int msg_id = message_ids[i];
            
            const char* message = nullptr;
            switch (msg_type) {
                case 0: message = PLAYER_MESSAGES[msg_id]; break;
                case 1: message = ENEMY_MESSAGES[msg_id]; break;  
                case 2: message = ITEM_MESSAGES[msg_id]; break;
            }
            
            if (message) {
                std::cout << "   " << (i+1) << ". " << message << std::endl;
            }
        }
    }
};

// Localization system for international games
class LocalizedGameSystem {
private:
    enum Language { EN, FR, DE, JP, ES };
    static Language current_language;
    
    // Multi-language support with organized .rodata layout
    static constexpr const char* UI_STRINGS[5][8] = {
        // English
        {"Start Game", "Options", "Exit", "Pause", "Resume", "Save", "Load", "Help"},
        // French
        {"Commencer", "Options", "Quitter", "Pause", "Reprendre", "Sauver", "Charger", "Aide"},
        // German  
        {"Spiel Starten", "Optionen", "Beenden", "Pause", "Fortsetzen", "Speichern", "Laden", "Hilfe"},
        // Japanese
        {"ゲーム開始", "オプション", "終了", "ポーズ", "再開", "セーブ", "ロード", "ヘルプ"},
        // Spanish
        {"Iniciar Juego", "Opciones", "Salir", "Pausa", "Reanudar", "Guardar", "Cargar", "Ayuda"}
    };
    
public:
    static void set_language(Language lang) {
        current_language = lang;
        std::cout << "🌍 Language changed to: " << 
                     (lang == EN ? "English" : 
                      lang == FR ? "French" :
                      lang == DE ? "German" :
                      lang == JP ? "Japanese" : "Spanish") << std::endl;
    }
    
    static const char* get_ui_string(int string_id) {
        if (string_id >= 0 && string_id < 8) {
            return UI_STRINGS[current_language][string_id];
        }
        return UI_STRINGS[EN][0]; // Fallback to English "Start Game"
    }
    
    void demonstrate_localization() {
        std::cout << "\n=== Localization Demo ===" << std::endl;
        
        Language languages[] = {EN, FR, DE, JP, ES};
        
        for (Language lang : languages) {
            set_language(lang);
            std::cout << "Main Menu: " << get_ui_string(0) << " | " << get_ui_string(1) 
                      << " | " << get_ui_string(2) << std::endl;
        }
    }
};

// Static member initialization
LocalizedGameSystem::Language LocalizedGameSystem::current_language = LocalizedGameSystem::EN;

// Cache locality analysis
class CacheLocalityAnalyzer {
public:
    static void analyze_string_layout() {
        std::cout << "\n=== Cache Locality Analysis ===" << std::endl;
        
        // Analyze address proximity of related strings
        const char* related_strings[] = {
            "Health: ",
            "Mana: ",  
            "Experience: ",
            "Level: ",
            "Gold: "
        };
        
        std::cout << "🧭 Related UI strings addresses:" << std::endl;
        uintptr_t first_addr = 0;
        
        for (int i = 0; i < 5; ++i) {
            uintptr_t addr = (uintptr_t)related_strings[i];
            if (i == 0) first_addr = addr;
            
            std::cout << "   \"" << related_strings[i] << "\": 0x" << std::hex << addr << std::dec;
            
            if (i > 0) {
                ptrdiff_t diff = addr - first_addr;
                std::cout << " (offset: " << diff << " bytes)";
            }
            std::cout << std::endl;
        }
        
        uintptr_t last_addr = (uintptr_t)related_strings[4];
        ptrdiff_t total_span = last_addr - first_addr + strlen(related_strings[4]);
        
        std::cout << "\n📏 Analysis:" << std::endl;
        std::cout << "   Total memory span: " << total_span << " bytes" << std::endl;
        std::cout << "   Cache lines needed (64B): " << ((total_span + 63) / 64) << std::endl;
        
        if (total_span <= 64) {
            std::cout << "   ✅ All strings fit in single cache line!" << std::endl;
        } else if (total_span <= 256) {
            std::cout << "   ✅ Good locality - fits in few cache lines" << std::endl;
        } else {
            std::cout << "   ⚠️  Poor locality - spans many cache lines" << std::endl;
        }
    }
};

// Main benchmark runner
void run_game_performance_comparison() {
    std::cout << "=== Game Development Performance Comparison ===" << std::endl;
    std::cout << "Target: 60 FPS (16.67ms per frame)" << std::endl;
    std::cout << "Test duration: 1000 frames" << std::endl << std::endl;
    
    constexpr int FRAME_COUNT = 1000;
    
    // Test inefficient system
    IneffientGameSystem inefficient_game;
    inefficient_game.run_performance_test(FRAME_COUNT);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Test optimized system  
    OptimizedGameSystem optimized_game;
    optimized_game.run_performance_test(FRAME_COUNT);
    
    // Show recent messages from optimized system
    optimized_game.display_recent_messages(3);
    
    std::cout << "\n🏆 Performance Improvement Summary:" << std::endl;
    std::cout << "   String allocations reduced to: 0 (from thousands)" << std::endl;
    std::cout << "   Cache locality improved significantly" << std::endl;
    std::cout << "   Memory usage: Constant vs Growing" << std::endl;
    std::cout << "   Real-time performance: More consistent" << std::endl;
}

int main() {
    std::cout << "=== Game Development String Optimization Demo ===" << std::endl;
    std::cout << "Simulating real-time game performance scenarios" << std::endl << std::endl;
    
    // Main performance comparison
    run_game_performance_comparison();
    
    // Localization demonstration
    LocalizedGameSystem localization_system;
    localization_system.demonstrate_localization();
    
    // Cache locality analysis
    CacheLocalityAnalyzer::analyze_string_layout();
    
    std::cout << "\n=== Key Takeaways for Game Development ===" << std::endl;
    std::cout << "✅ Use .rodata string literals instead of dynamic allocation" << std::endl;
    std::cout << "✅ Group related strings together for cache locality" << std::endl;
    std::cout << "✅ Use message IDs instead of storing string objects" << std::endl;
    std::cout << "✅ Pre-define message templates for common scenarios" << std::endl;
    std::cout << "✅ Organize localization data for efficient access" << std::endl;
    std::cout << "✅ Profile your actual game to validate optimizations" << std::endl;
    
    return 0;
}

/*
 * Game Development Specific Compilation:
 * 
 * 1. Performance-optimized build:
 *    g++ -O3 -march=native -flto -o game_perf game_performance.cpp -pthread
 * 
 * 2. Debug build for profiling:
 *    g++ -O1 -g -fno-omit-frame-pointer -o game_debug game_performance.cpp -pthread
 * 
 * 3. Size-optimized for mobile:
 *    g++ -Os -ffunction-sections -fdata-sections -o game_mobile game_performance.cpp -Wl,--gc-sections -pthread
 * 
 * 4. Profile with game-specific tools:
 *    # Intel VTune (if available)
 *    vtune -collect hotspots -result-dir vtune_results ./game_perf
 * 
 *    # perf (Linux)
 *    perf record -g ./game_perf
 *    perf report
 * 
 *    # Callgrind (cross-platform)
 *    valgrind --tool=callgrind ./game_perf
 *    kcachegrind callgrind.out.*
 * 
 * 5. Memory analysis:
 *    valgrind --tool=massif ./game_perf
 *    ms_print massif.out.*
 * 
 * Real-world Applications:
 * - Unity/Unreal Engine plugin development
 * - Custom game engine string management
 * - Mobile game optimization (iOS/Android)
 * - Console game development (PlayStation, Xbox, Nintendo)
 * - VR/AR applications with strict performance requirements
 * - Multiplayer game server optimization
 * 
 * Performance Targets:
 * - 60 FPS: 16.67ms per frame
 * - 120 FPS: 8.33ms per frame (VR)
 * - Console games: Sub-millisecond string operations
 * - Mobile games: Minimal battery impact
 */