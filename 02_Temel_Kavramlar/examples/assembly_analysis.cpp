#include <iostream>
#include <string>

/*
 * Assembly Analysis Example
 * Bu örnek, farklı sabit türlerinin assembly kodunda nasıl göründüğünü inceler.
 */

// Persistent global array to avoid dangling pointer to a temporary compound literal
static const int GLOBAL_ARRAY_FOR_POINTER[5] = {10, 20, 30, 40, 50};

// Test fonksiyonları - her biri farklı bir sabit türü kullanır

// 1. Küçük integer sabitler
int use_small_constants() {
    int a = 5;           // Küçük pozitif
    int b = -10;         // Küçük negatif  
    int c = 0;           // Sıfır
    int d = 1;           // Bir
    
    return a + b + c + d;
}

// 2. Büyük integer sabitler
long long use_large_constants() {
    long long big1 = 0x123456789ABCDEF0LL;
    long long big2 = 1000000000000LL;
    long long big3 = -9999999999999LL;
    
    return big1 + big2 + big3;
}

// 3. String literaller
void use_string_literals() {
    const char* short_str = "Hi";
    const char* medium_str = "Hello World";
    const char* long_str = "Bu çok uzun bir string literal ve kesinlikle .rodata bölümünde saklanacak";
    
    printf("Short: %s\n", short_str);
    printf("Medium: %s\n", medium_str);
    printf("Long: %s\n", long_str);
}

// 4. Floating point sabitler
double use_float_constants() {
    float f1 = 3.14f;
    float f2 = 0.0f;
    double d1 = 2.718281828;
    double d2 = 1.414213562373095;
    
    return f1 + f2 + d1 + d2;
}

// 5. Array/pointer karşılaştırması
void use_arrays_vs_pointers() {
    // Array - stack'te ama initialize data .rodata'da
    int local_array[5] = {1, 2, 3, 4, 5};
    
    // Pointer - .rodata'daki kalıcı global array'i gösterir (dangling risk yok)
    const int* global_array = GLOBAL_ARRAY_FOR_POINTER;
    
    // String array
    const char* messages[] = {
        "First message",
        "Second message", 
        "Third message"
    };
    
    std::cout << "Local array[0]: " << local_array[0] << std::endl;
    std::cout << "Global array[0]: " << global_array[0] << std::endl;
    std::cout << "Message[0]: " << messages[0] << std::endl;
}

// 6. Template ile sabitler (compile-time evaluation)
template<int N>
constexpr int factorial() {
    return N * factorial<N-1>();
}

template<>
constexpr int factorial<0>() {
    return 1;
}

void use_compile_time_constants() {
    // Bu değerler compile time'da hesaplanır
    constexpr int fact5 = factorial<5>();    // 120
    constexpr int fact10 = factorial<10>();  // 3628800
    
    std::cout << "5! = " << fact5 << std::endl;
    std::cout << "10! = " << fact10 << std::endl;
}

// 7. std::string vs C string karşılaştırması
void string_comparison() {
    // C-style string literal
    const char* c_string = "C-style string in .rodata";
    
    // std::string - heap allocation
    std::string cpp_string = "C++ string copied to heap";
    
    // Copy constructor
    std::string copy_string(c_string);
    
    std::cout << "C string address: " << (void*)c_string << std::endl;
    std::cout << "C++ string data(): " << (void*)cpp_string.data() << std::endl;
    std::cout << "Copy string data(): " << (void*)copy_string.data() << std::endl;
}

int main() {
    std::cout << "=== Assembly Analysis Example ===" << std::endl;
    
    std::cout << "\n1. Small constants result: " << use_small_constants() << std::endl;
    
    std::cout << "\n2. Large constants result: " << use_large_constants() << std::endl;
    
    std::cout << "\n3. String literals:" << std::endl;
    use_string_literals();
    
    std::cout << "\n4. Float constants result: " << use_float_constants() << std::endl;
    
    std::cout << "\n5. Arrays vs Pointers:" << std::endl;
    use_arrays_vs_pointers();
    
    std::cout << "\n6. Compile-time constants:" << std::endl;
    use_compile_time_constants();
    
    std::cout << "\n7. String comparison:" << std::endl;
    string_comparison();
    
    return 0;
}

/*
 * Assembly analiz komutları:
 * 
 * 1. Derleme (debug bilgisiyle):
 *    g++ -g -O1 -o assembly_test assembly_analysis.cpp
 * 
 * 2. Assembly kodu üretme:
 *    g++ -S -O1 assembly_analysis.cpp -o assembly_analysis.s
 * 
 * 3. Disassembly (tüm program):
 *    objdump -d assembly_test > disassembly.txt
 * 
 * 4. Specific function disassembly:
 *    objdump -d assembly_test | grep -A 20 "use_small_constants>"
 * 
 * 5. .rodata section içeriği:
 *    objdump -s -j .rodata assembly_test
 * 
 * 6. Symbol table:
 *    nm assembly_test | sort
 * 
 * 7. Section headers:
 *    objdump -h assembly_test
 * 
 * 8. String literals:
 *    strings assembly_test
 * 
 * 9. GDB ile step-by-step analiz:
 *    gdb assembly_test
 *    (gdb) break main
 *    (gdb) run
 *    (gdb) disas use_small_constants
 *    (gdb) disas use_string_literals
 * 
 * Aranacak assembly patterns:
 * 
 * x86-64:
 * - mov $5, %eax          # Küçük sabit immediate
 * - movq $.LC0, %rdi      # String literal adresi
 * - movsd .LC1(%rip), %xmm0  # Float sabit
 * 
 * ARM:
 * - mov r0, #5            # Küçük sabit immediate
 * - ldr r0, =.LC0         # String literal adresi
 * - vldr d0, .LC1         # Float sabit
 * 
 * Notlar:
 * - Küçük sabitler (0-255 arası) genellikle immediate olarak gömülür
 * - Büyük sabitler ve float'lar .rodata'dan yüklenir
 * - String'ler her zaman .rodata'da, pointer'lar adresi yükler
 * - Compile-time constant'lar runtime hesaplama gerektirmez
 */