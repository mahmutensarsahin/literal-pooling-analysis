#include <iostream>

/*
 * Size Comparison Test
 * Bu örnek, farklı sabit türlerinin bellek kullanımını karşılaştırır.
 */

// Global sabitler - .rodata'ya gidecek
const int GLOBAL_SMALL = 42;
const int GLOBAL_LARGE = 0x7FFFFFFF;
const char* GLOBAL_STRING = "Bu bir global string literal";
const double GLOBAL_PI = 3.14159265359;

// Çok sayıda string literal kullanımı
void spam_strings() {
    // Aynı string'i birçok kez kullan
    for (int i = 0; i < 5; i++) {
        printf("Repeated message\n");
        printf("Repeated message\n");  // Duplicate
        printf("Repeated message\n");  // Duplicate
    }
}

void different_constants() {
    // Küçük sabitler - muhtemelen inline
    const int a = 1;
    const int b = 2; 
    const int c = 3;
    const int d = 4;
    const int e = 5;
    
    // Büyük sabitler - .rodata'ya gidebilir
    const long long big1 = 0x123456789ABCDEF0LL;
    const long long big2 = 0xFEDCBA9876543210LL;
    
    // String literaller - kesinlikle .rodata
    const char* msg1 = "Bu küçük bir mesaj";
    const char* msg2 = "Bu çok daha uzun bir mesaj ve muhtemelen .rodata bölümünde saklanacak";
    const char* msg3 = "Kısa";
    
    // Kullanım
    std::cout << "Küçük sabitler toplamı: " << (a+b+c+d+e) << std::endl;
    std::cout << "Büyük sabit 1: 0x" << std::hex << big1 << std::endl;
    std::cout << "Büyük sabit 2: 0x" << std::hex << big2 << std::endl;
    std::cout << msg1 << std::endl;
    std::cout << msg2 << std::endl;
    std::cout << msg3 << std::endl;
}

// Array sabitler
const int LOOKUP_TABLE[10] = {1, 4, 9, 16, 25, 36, 49, 64, 81, 100};
const char* ERROR_MESSAGES[3] = {
    "File not found",
    "Permission denied", 
    "Out of memory"
};

void use_arrays() {
    std::cout << "\n=== Array Constants ===" << std::endl;
    
    for (int i = 0; i < 10; i++) {
        std::cout << "Square of " << (i+1) << " = " << LOOKUP_TABLE[i] << std::endl;
    }
    
    for (int i = 0; i < 3; i++) {
        std::cout << "Error " << i << ": " << ERROR_MESSAGES[i] << std::endl;
    }
}

int main() {
    std::cout << "=== Size Comparison Test ===" << std::endl;
    
    // Global sabitleri kullan
    std::cout << "Global small: " << GLOBAL_SMALL << std::endl;
    std::cout << "Global large: " << GLOBAL_LARGE << std::endl;
    std::cout << "Global string: " << GLOBAL_STRING << std::endl;
    std::cout << "Global PI: " << GLOBAL_PI << std::endl;
    
    // Spam strings
    std::cout << "\n=== Spam Strings (Duplicate Test) ===" << std::endl;
    spam_strings();
    
    // Different constants
    std::cout << "\n=== Different Constants ===" << std::endl;
    different_constants();
    
    // Array constants
    use_arrays();
    
    return 0;
}

/*
 * Test komutları:
 * 
 * 1. Normal derleme:
 *    g++ -o size_test size_comparison.cpp
 * 
 * 2. Optimizasyonsuz derleme:
 *    g++ -O0 -o size_test_O0 size_comparison.cpp
 * 
 * 3. Optimize derleme:
 *    g++ -O2 -o size_test_O2 size_comparison.cpp
 * 
 * 4. Binary boyutlarını karşılaştır:
 *    ls -la size_test*
 * 
 * 5. .rodata boyutunu incele:
 *    objdump -h size_test | grep rodata
 * 
 * 6. Tüm string'leri listele:
 *    strings size_test
 * 
 * 7. Symbol tablosunu incele:
 *    nm size_test | grep rodata
 * 
 * 8. Assembly kodunu karşılaştır:
 *    objdump -d size_test_O0 > assembly_O0.txt
 *    objdump -d size_test_O2 > assembly_O2.txt
 *    diff assembly_O0.txt assembly_O2.txt
 * 
 * Analiz noktaları:
 * - O0 vs O2 arasında .rodata boyut farkı
 * - "Repeated message" string'inin kaç kez göründüğü
 * - Büyük sabitlerin nasıl saklandığı
 * - Array'lerin .rodata'daki yerleşimi
 */