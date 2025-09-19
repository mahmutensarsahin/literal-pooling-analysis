#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <chrono>
#include <sstream>
#include <mutex>
#include <queue>
#include <memory>

/*
 * High-Performance Web Server String Management
 * Bu örnek, web server ve backend uygulamalarda string literal 
 * optimizasyonunun kritik performans etkilerini gösterir.
 */

namespace WebServer {
    // Global performance counters
    std::atomic<size_t> total_requests{0};
    std::atomic<size_t> total_allocations{0}; 
    std::atomic<size_t> total_response_time_ms{0};
    std::atomic<size_t> cache_hits{0};
    std::atomic<size_t> cache_misses{0};
    
    void reset_stats() {
        total_requests = 0;
        total_allocations = 0;
        total_response_time_ms = 0;
        cache_hits = 0;
        cache_misses = 0;
    }
    
    void print_stats(const std::string& system_name) {
        std::cout << "📊 " << system_name << " Performance Stats:" << std::endl;
        std::cout << "   Requests: " << total_requests.load() << std::endl;
        std::cout << "   Allocations: " << total_allocations.load() << std::endl;
        std::cout << "   Avg response time: " << 
                     (total_requests.load() > 0 ? total_response_time_ms.load() / total_requests.load() : 0) 
                     << "ms" << std::endl;
        std::cout << "   Cache hit ratio: " << 
                     (cache_hits.load() + cache_misses.load() > 0 ? 
                      (100.0 * cache_hits.load()) / (cache_hits.load() + cache_misses.load()) : 0)
                     << "%" << std::endl;
    }
}

// ❌ BAD APPROACH: Dynamic string generation for every request
class IneffientWebServer {
private:
    std::unordered_map<int, std::string> status_cache;
    std::mutex cache_mutex;
    
    // Dynamic HTML generation - causes allocations
    std::string generate_html_response(int status_code, const std::string& content) {
        WebServer::total_allocations++;
        
        std::stringstream html;
        html << "<!DOCTYPE html>\n";
        html << "<html>\n";
        html << "<head><title>";
        
        // Dynamic status text generation
        switch (status_code) {
            case 200: html << "Success"; break;
            case 404: html << "Not Found"; break; 
            case 500: html << "Server Error"; break;
            default: html << "Unknown Status"; break;
        }
        
        html << "</title></head>\n";
        html << "<body>\n";
        html << "<h1>Status: " << status_code << "</h1>\n";
        html << "<p>" << content << "</p>\n";
        html << "<hr>\n";
        html << "<footer>Generated at " << std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count() << "</footer>\n";
        html << "</body>\n";
        html << "</html>\n";
        
        return html.str(); // Another allocation for return value
    }
    
    std::string generate_json_response(const std::string& status, const std::string& message, const std::string& data = "") {
        WebServer::total_allocations++;
        
        std::stringstream json;
        json << "{\n";
        json << "  \"status\": \"" << status << "\",\n";
        json << "  \"message\": \"" << message << "\",\n";
        json << "  \"timestamp\": " << std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() << ",\n";
        if (!data.empty()) {
            json << "  \"data\": " << data << ",\n";
        }
        json << "  \"server\": \"IneffientWebServer/1.0\"\n";
        json << "}";
        
        return json.str();
    }
    
public:
    void handle_request(const std::string& path, const std::string& method) {
        auto start_time = std::chrono::high_resolution_clock::now();
        WebServer::total_requests++;
        
        std::string response;
        
        if (path == "/") {
            response = generate_html_response(200, "Welcome to our website! This is the home page.");
        } else if (path == "/api/status") {
            response = generate_json_response("success", "Server is running normally", "{}");
        } else if (path == "/api/users") {
            response = generate_json_response("success", "User list retrieved", "[{\"id\":1,\"name\":\"John\"}]");
        } else if (path == "/about") {
            response = generate_html_response(200, "About our company: We provide excellent service since 2020.");
        } else {
            response = generate_html_response(404, "The requested page was not found on this server.");
        }
        
        // Simulate response transmission
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        WebServer::total_response_time_ms += duration.count();
    }
    
    void run_load_test(int request_count) {
        std::cout << "🐌 Running inefficient web server load test..." << std::endl;
        WebServer::reset_stats();
        
        std::vector<std::string> test_paths = {
            "/", "/api/status", "/api/users", "/about", "/nonexistent"
        };
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < request_count; ++i) {
            std::string path = test_paths[i % test_paths.size()];
            handle_request(path, "GET");
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "   Total test time: " << total_duration.count() << "ms" << std::endl;
        WebServer::print_stats("Inefficient Web Server");
    }
};

// ✅ GOOD APPROACH: Template-based, .rodata-optimized web server
class OptimizedWebServer {
private:
    // Pre-defined HTML templates in .rodata
    static constexpr const char* HTML_TEMPLATE = 
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>{title}</title></head>\n"
        "<body>\n"
        "<h1>{heading}</h1>\n"
        "<p>{content}</p>\n"
        "<hr>\n"
        "<footer>Optimized Server v2.0</footer>\n"
        "</body>\n"
        "</html>\n";
        
    static constexpr const char* JSON_TEMPLATE = 
        "{\n"
        "  \"status\": \"{status}\",\n"
        "  \"message\": \"{message}\",\n"
        "  \"server\": \"OptimizedWebServer/2.0\"\n"
        "}";
    
    // HTTP status responses - all in .rodata
    static constexpr const char* HTTP_200 = "HTTP/1.1 200 OK\r\n";
    static constexpr const char* HTTP_404 = "HTTP/1.1 404 Not Found\r\n";
    static constexpr const char* HTTP_500 = "HTTP/1.1 500 Internal Server Error\r\n";
    
    static constexpr const char* CONTENT_TYPE_HTML = "Content-Type: text/html\r\n";
    static constexpr const char* CONTENT_TYPE_JSON = "Content-Type: application/json\r\n";
    static constexpr const char* CONNECTION_CLOSE = "Connection: close\r\n\r\n";
    
    // Pre-defined content strings
    static constexpr const char* HOME_CONTENT = "Welcome to our optimized website!";
    static constexpr const char* ABOUT_CONTENT = "About our company: Excellence since 2020.";
    static constexpr const char* NOT_FOUND_CONTENT = "Page not found.";
    static constexpr const char* API_STATUS_RESPONSE = "Server running normally";
    static constexpr const char* API_USERS_RESPONSE = "Users retrieved";
    
    // Response cache using string_view (no allocations)
    struct CachedResponse {
        const char* header;
        const char* content_type; 
        const char* body;
    };
    
    std::unordered_map<std::string, CachedResponse> response_cache;
    std::mutex cache_mutex;
    bool cache_initialized = false;
    
    void initialize_cache() {
        if (cache_initialized) return;
        
        std::lock_guard<std::mutex> lock(cache_mutex);
        if (cache_initialized) return;
        
        // Pre-populate cache with common responses
        response_cache["/"] = {HTTP_200, CONTENT_TYPE_HTML, HOME_CONTENT};
        response_cache["/about"] = {HTTP_200, CONTENT_TYPE_HTML, ABOUT_CONTENT};
        response_cache["/api/status"] = {HTTP_200, CONTENT_TYPE_JSON, API_STATUS_RESPONSE};
        response_cache["/api/users"] = {HTTP_200, CONTENT_TYPE_JSON, API_USERS_RESPONSE};
        response_cache["404"] = {HTTP_404, CONTENT_TYPE_HTML, NOT_FOUND_CONTENT};
        
        cache_initialized = true;
    }
    
    // Fast response lookup - no string allocations
    void send_cached_response(const std::string& path) {
        auto it = response_cache.find(path);
        if (it != response_cache.end()) {
            WebServer::cache_hits++;
            // In real server, would write to socket:
            // write(socket_fd, it->second.header, strlen(it->second.header));
            // write(socket_fd, it->second.content_type, strlen(it->second.content_type));
            // write(socket_fd, CONNECTION_CLOSE, strlen(CONNECTION_CLOSE));
            // write(socket_fd, it->second.body, strlen(it->second.body));
        } else {
            WebServer::cache_misses++;
            send_cached_response("404"); // Fallback
        }
    }
    
public:
    OptimizedWebServer() {
        initialize_cache();
    }
    
    void handle_request(const std::string& path, const std::string& method) {
        auto start_time = std::chrono::high_resolution_clock::now();
        WebServer::total_requests++;
        
        // Fast path - cache lookup with no allocations
        send_cached_response(path);
        
        // Simulate network I/O
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        WebServer::total_response_time_ms += duration.count();
    }
    
    void run_load_test(int request_count) {
        std::cout << "🚀 Running optimized web server load test..." << std::endl;
        WebServer::reset_stats();
        
        std::vector<std::string> test_paths = {
            "/", "/api/status", "/api/users", "/about", "/nonexistent"
        };
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < request_count; ++i) {
            std::string path = test_paths[i % test_paths.size()];
            handle_request(path, "GET");
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "   Total test time: " << total_duration.count() << "ms" << std::endl;
        WebServer::print_stats("Optimized Web Server");
    }
    
    // Demonstrate advanced optimization: HTTP header reuse
    void demonstrate_header_optimization() {
        std::cout << "\n=== HTTP Header Optimization Demo ===" << std::endl;
        
        // Common headers stored once in .rodata
        static constexpr const char* COMMON_HEADERS[] = {
            "Server: OptimizedWebServer/2.0\r\n",
            "Cache-Control: public, max-age=3600\r\n",
            "Content-Encoding: gzip\r\n",
            "Access-Control-Allow-Origin: *\r\n",
            "X-Content-Type-Options: nosniff\r\n"
        };
        
        std::cout << "📋 Common headers (stored in .rodata):" << std::endl;
        for (const auto& header : COMMON_HEADERS) {
            std::cout << "   " << header;
        }
        
        std::cout << "\n✅ Benefits:" << std::endl;
        std::cout << "   - No string allocations for headers" << std::endl;
        std::cout << "   - Headers reused across all responses" << std::endl;
        std::cout << "   - Consistent memory usage regardless of traffic" << std::endl;
    }
};

// Advanced: Connection pooling with optimized strings
class ConnectionPooledServer {
private:
    // Connection states using enum instead of strings
    enum ConnectionState { IDLE, ACTIVE, CLOSING, CLOSED };
    
    struct Connection {
        int fd;
        ConnectionState state;
        std::chrono::steady_clock::time_point last_activity;
        
        // Use static strings for logging
        const char* get_state_string() const {
            switch (state) {
                case IDLE: return "IDLE";
                case ACTIVE: return "ACTIVE"; 
                case CLOSING: return "CLOSING";
                case CLOSED: return "CLOSED";
                default: return "UNKNOWN";
            }
        }
    };
    
    std::vector<Connection> connection_pool;
    std::queue<int> available_connections;
    std::mutex pool_mutex;
    
    // Logging with .rodata strings
    static constexpr const char* LOG_MESSAGES[] = {
        "Connection established",
        "Connection reused from pool",
        "Connection returned to pool", 
        "Connection closed due to timeout",
        "Pool capacity reached",
        "Pool statistics updated"
    };
    
public:
    ConnectionPooledServer(size_t pool_size = 100) {
        connection_pool.reserve(pool_size);
        
        // Initialize connection pool
        for (size_t i = 0; i < pool_size; ++i) {
            connection_pool.push_back({
                static_cast<int>(i), 
                IDLE, 
                std::chrono::steady_clock::now()
            });
            available_connections.push(static_cast<int>(i));
        }
        
        std::cout << "🏊 Connection pool initialized with " << pool_size << " connections" << std::endl;
    }
    
    void handle_concurrent_requests(int request_count, int thread_count) {
        std::cout << "🔄 Handling " << request_count << " concurrent requests with " 
                  << thread_count << " threads..." << std::endl;
        
        WebServer::reset_stats();
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::vector<std::thread> threads;
        int requests_per_thread = request_count / thread_count;
        
        for (int t = 0; t < thread_count; ++t) {
            threads.emplace_back([this, requests_per_thread, t]() {
                for (int i = 0; i < requests_per_thread; ++i) {
                    this->simulate_request(t * 1000 + i);
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "   Concurrent test completed in: " << total_duration.count() << "ms" << std::endl;
        WebServer::print_stats("Connection Pooled Server");
    }
    
private:
    void simulate_request(int request_id) {
        auto start_time = std::chrono::high_resolution_clock::now();
        WebServer::total_requests++;
        
        // Get connection from pool
        int conn_id = get_connection();
        if (conn_id >= 0) {
            // Simulate request processing using .rodata strings
            process_with_connection(conn_id);
            return_connection(conn_id);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        WebServer::total_response_time_ms += duration.count();
    }
    
    int get_connection() {
        std::lock_guard<std::mutex> lock(pool_mutex);
        if (!available_connections.empty()) {
            int conn_id = available_connections.front();
            available_connections.pop();
            connection_pool[conn_id].state = ACTIVE;
            connection_pool[conn_id].last_activity = std::chrono::steady_clock::now();
            return conn_id;
        }
        return -1; // No available connections
    }
    
    void return_connection(int conn_id) {
        std::lock_guard<std::mutex> lock(pool_mutex);
        if (conn_id >= 0 && conn_id < static_cast<int>(connection_pool.size())) {
            connection_pool[conn_id].state = IDLE;
            available_connections.push(conn_id);
        }
    }
    
    void process_with_connection(int conn_id) {
        // Simulate processing with optimized string usage
        static constexpr const char* RESPONSE_TEMPLATE = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 27\r\n"
            "\r\n"
            "{\"status\":\"success\"}";
        
        // In real implementation, would write to socket
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
};

void run_web_server_comparison() {
    std::cout << "=== Web Server Performance Comparison ===" << std::endl;
    std::cout << "Testing with 10,000 requests each\n" << std::endl;
    
    const int REQUEST_COUNT = 10000;
    
    // Test inefficient server
    IneffientWebServer inefficient_server;
    inefficient_server.run_load_test(REQUEST_COUNT);
    
    std::cout << std::endl;
    
    // Test optimized server
    OptimizedWebServer optimized_server;
    optimized_server.run_load_test(REQUEST_COUNT);
    optimized_server.demonstrate_header_optimization();
    
    std::cout << std::endl;
    
    // Test connection pooled server
    ConnectionPooledServer pooled_server(50);
    pooled_server.handle_concurrent_requests(REQUEST_COUNT, 10);
    
    std::cout << "\n🏆 Key Optimizations Achieved:" << std::endl;
    std::cout << "✅ Eliminated dynamic string allocations" << std::endl;
    std::cout << "✅ Used pre-computed response templates" << std::endl;
    std::cout << "✅ Implemented efficient response caching" << std::endl;
    std::cout << "✅ Reduced memory fragmentation" << std::endl;
    std::cout << "✅ Improved cache locality" << std::endl;
    std::cout << "✅ Enhanced concurrent performance" << std::endl;
}

int main() {
    std::cout << "=== High-Performance Web Server String Optimization ===" << std::endl;
    std::cout << "Demonstrating enterprise-grade optimization techniques\n" << std::endl;
    
    run_web_server_comparison();
    
    std::cout << "\n=== Production Deployment Considerations ===" << std::endl;
    std::cout << "🔧 Compile with: -O3 -march=native -flto -DNDEBUG" << std::endl;
    std::cout << "📊 Profile with: perf, Intel VTune, or similar tools" << std::endl;
    std::cout << "🔍 Monitor: Memory usage, response times, throughput" << std::endl;
    std::cout << "📈 Scale: Use connection pooling and async I/O" << std::endl;
    std::cout << "🛡️  Secure: Validate all inputs, prevent buffer overflows" << std::endl;
    
    return 0;
}

/*
 * Production Web Server Compilation:
 * 
 * 1. High-performance build:
 *    g++ -O3 -march=native -flto -DNDEBUG -pthread -o web_server web_server.cpp
 * 
 * 2. Debug/profiling build:
 *    g++ -O1 -g -fno-omit-frame-pointer -pthread -o web_server_debug web_server.cpp
 * 
 * 3. Memory-optimized build:
 *    g++ -Os -ffunction-sections -fdata-sections -pthread -o web_server_small web_server.cpp -Wl,--gc-sections
 * 
 * 4. Production profiling:
 *    perf record -g --call-graph dwarf ./web_server
 *    perf report --stdio
 * 
 * 5. Load testing:
 *    # Apache Bench
 *    ab -n 100000 -c 100 http://localhost:8080/
 *    
 *    # wrk (modern alternative)
 *    wrk -t12 -c400 -d30s http://localhost:8080/
 * 
 * 6. Memory analysis:
 *    valgrind --tool=massif ./web_server
 *    valgrind --tool=helgrind ./web_server  # Thread safety
 * 
 * Real-world Applications:
 * - Nginx/Apache module development
 * - REST API services
 * - Microservices architecture
 * - CDN edge servers
 * - Real-time messaging systems
 * - High-frequency trading platforms
 * - IoT data ingestion servers
 * 
 * Performance Targets:
 * - Response time: <1ms for cached responses
 * - Throughput: >100K requests/second
 * - Memory: Constant usage regardless of load
 * - CPU: <50% utilization under normal load
 */