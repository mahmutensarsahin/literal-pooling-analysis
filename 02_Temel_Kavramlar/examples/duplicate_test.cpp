#include <iostream>
#include <cstdio>

/*
 * Duplicate Detection Test
 * Bu örnek, derleyicinin aynı string literallerini nasıl birleştirdiğini gösterir.
 */

void function1() {
    printf("Hello World");
}

void function2() {
    printf("Hello World");  // Aynı string literal
}

void function3() {
    std::cout << "Hello World";  // Yine aynı string (farklı API ile)
}

void function4() {
    printf("Goodbye World");  // Farklı string
}

int main() {
    // String literallerinin adreslerini karşılaştırma
    const char* str1 = "Hello World";
    const char* str2 = "Hello World";  // Aynı literal
    const char* str3 = "Goodbye World"; // Farklı literal
    
    std::cout << "=== Duplicate Detection Test ===" << std::endl;
    std::cout << "str1 address: " << (void*)str1 << std::endl;
    std::cout << "str2 address: " << (void*)str2 << std::endl;
    std::cout << "str3 address: " << (void*)str3 << std::endl;
    
    // Adres karşılaştırması
    if (str1 == str2) {
        std::cout << "✅ SUCCESS: str1 and str2 point to the same memory location!" << std::endl;
        std::cout << "   Derleyici duplicate kontrolü çalışıyor." << std::endl;
    } else {
        std::cout << "❌ WARNING: str1 and str2 point to different locations." << std::endl;
    }
    
    if (str1 != str3) {
        std::cout << "✅ SUCCESS: str1 and str3 point to different locations (as expected)." << std::endl;
    }
    
    // Fonksiyonları çağır
    std::cout << "\n=== Function Calls ===" << std::endl;
    function1();
    std::cout << " (function1)" << std::endl;
    
    function2(); 
    std::cout << " (function2)" << std::endl;
    
    function3();
    std::cout << " (function3)" << std::endl;
    
    function4();
    std::cout << " (function4)" << std::endl;
    
    return 0;
}

/*
 * Test komutları:
 * 
 * 1. Derleme:
 *    g++ -o duplicate_test duplicate_test.cpp
 * 
 * 2. Çalıştırma:
 *    ./duplicate_test
 * 
 * 3. .rodata bölümünü inceleme:
 *    objdump -s -j .rodata duplicate_test
 * 
 * 4. String'leri görme:
 *    strings duplicate_test | grep "Hello\|Goodbye"
 * 
 * 5. Assembly kodu görme:
 *    objdump -d duplicate_test
 * 
 * Beklenen sonuç:
 * - "Hello World" string'i sadece bir kez .rodata'da bulunmalı
 * - str1 ve str2 aynı adresi göstermeli
 * - "Goodbye World" farklı bir adreste olmalı
 */