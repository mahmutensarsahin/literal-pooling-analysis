# Temel Kavramlar: Derleyici Sabit Havuzu ve .rodata Bölümü

## 🎯 Bu Bölümde Öğrenecekleriniz

- Derleyici sabit havuzunun (constant pool) ne olduğu
- `.rodata` bölümünün yapısı ve amacı
- Küçük sabitler ile string literalleri arasındaki temel farklar
- Duplicate kontrolü mekanizması
- Local vs Global sabitler

---

## 1. 📚 Derleyici Sabit Havuzu (Constant Pool) Nedir?

**Tanım:** Derleyici sabit havuzu, derleme aşamasında derleyicinin belleğinde oluşturduğu ve programdaki tüm sabit değerleri topladığı geçici bir tablodur.

### 🔍 Nasıl Çalışır?

```cpp
// Kaynak kod örneği
void func1() { printf("Hello"); }
void func2() { printf("World"); }
void func3() { printf("Hello"); }  // Aynı string tekrar!
```

**Derleme sırasında derleyici şunu yapar:**

```
Derleyici Sabit Havuzu (Compile-time):
┌─────────────────────┬─────────────┬──────────┐
│ Literal Value       │ First Used  │ Count    │
├─────────────────────┼─────────────┼──────────┤
│ "Hello"             │ func1       │ 2        │
│ "World"             │ func2       │ 1        │
└─────────────────────┴─────────────┴──────────┘
```

**✅ Sonuç:** `"Hello"` sadece bir kez `.rodata`'ya yerleştirilir!

---

## 2. 🗂️ .rodata Bölümü (Read-Only Data Segment)

### Tanım ve Amaç

`.rodata` = **Read-Only Data** (Salt Okunur Veri)

- Programın çalışma zamanında **değiştirilemez** veriler
- String literalleri ve büyük sabitler burada tutulur
- İşletim sistemi tarafından **yazma korumalı** olarak yüklenir

### 📊 Bellek Layout'u

```
Program Memory Layout:
┌─────────────────────┐ ← Yüksek adresler
│      Stack          │
├─────────────────────┤
│      Heap           │
├─────────────────────┤
│      .bss           │ (Initialized globals)
├─────────────────────┤
│      .data          │ (Uninitialized globals)
├─────────────────────┤
│   ➤ .rodata ←       │ 🔒 Salt okunur sabitler
├─────────────────────┤
│      .text          │ (Kod segmenti)
└─────────────────────┘ ← Düşük adresler
```

### 🔬 Pratik Örnek: .rodata İçeriğini Görme

```cpp
// test.cpp
#include <iostream>

int main() {
    const char* msg1 = "Learning C++";
    const char* msg2 = "Learning C++";  // Aynı string!
    const char* msg3 = "Different text";
    
    std::cout << "msg1 address: " << (void*)msg1 << std::endl;
    std::cout << "msg2 address: " << (void*)msg2 << std::endl;
    std::cout << "msg3 address: " << (void*)msg3 << std::endl;
    
    return 0;
}
```

**Terminal komutu:**
```bash
g++ -o test test.cpp
./test
```

**Beklenen çıktı:**
```
msg1 address: 0x100000f50
msg2 address: 0x100000f50  ← Aynı adres!
msg3 address: 0x100000f5c  ← Farklı adres
```

**Binary analizi:**
```bash
objdump -s -j .rodata test
```

---

## 3. 🔢 Küçük Sabitler vs String Literalleri

### Karşılaştırma Tablosu

| Özellik | Küçük Sabit (ör. `5`) | String Literal (ör. `"hello"`) |
|---------|-----------------------|--------------------------------|
| **Gömme stratejisi** | ✅ Instruction içine gömülür | ❌ Ayrı bellek konumu |
| **Bellek yeri** | Genellikle register/stack | 📍 .rodata bölümü |
| **Pointer gereksinimi** | ❌ Direkt değer | ✅ Adres referansı |
| **Duplicate kontrolü** | Gereksiz | ✅ Tekrarlar birleştirilir |

### 💻 Assembly Kodu Karşılaştırması

**C++ Kodu:**
```cpp
void test_small() {
    int x = 5;           // Küçük sabit
    const char* s = "Hi"; // String literal
}
```

**Assembly çıktısı (ARM):**
```assembly
test_small:
    MOV   R0, #5         ; ← Küçük sabit direkt gömüldü
    LDR   R1, =string_hi ; ← String adresi yüklendi
    ...

.rodata:
string_hi: .asciz "Hi"   ; ← String ayrı bölümde
```

**Assembly çıktısı (x86-64):**
```assembly
test_small:
    movl  $5, %eax       ; ← Küçük sabit direkt
    movq  $.LC0, %rdi    ; ← String adresini yükle
    ...

.rodata:
.LC0:
    .string "Hi"         ; ← String literal
```

---

## 4. 🎯 Duplicate Kontrolü Mekanizması

### Nasıl Çalışır?

```cpp
// Çoklu kullanım örneği
void error_handler1() { printf("Error occurred"); }
void error_handler2() { printf("Error occurred"); }  // Aynı string
void log_function()   { printf("Error occurred"); }  // Yine aynı!

// Farklı string
void success_handler() { printf("Success!"); }
```

**Derleyici işlem sırası:**

```
1. İlk "Error occurred" görüldüğünde:
   ✅ .rodata'ya ekle, adres: 0x1000

2. İkinci "Error occurred" görüldüğünde:  
   🔍 Havuzda var mı? → EVET!
   ➤ Mevcut adresi kullan: 0x1000

3. Üçüncü "Error occurred" görüldüğünde:
   🔍 Havuzda var mı? → EVET!  
   ➤ Mevcut adresi kullan: 0x1000

4. "Success!" görüldüğünde:
   🔍 Havuzda var mı? → HAYIR!
   ✅ .rodata'ya ekle, adres: 0x1010
```

### 📈 Memory Savings Hesabı

```cpp
// 1000 fonksiyon, her biri "Connection failed" mesajını kullanıyor

// ❌ Duplicate kontrolü OLMASA:
// "Connection failed" = 18 byte
// 1000 kopya = 18,000 byte

// ✅ Duplicate kontrolü VARSA:  
// 1 kopya = 18 byte
// Toplam tasarruf = 17,982 byte (~17.5 KB)
```

---

## 5. 🏠 Local vs Global/Static Sabitler

### Local Const Değişkenler

```cpp
void function() {
    const int x = 42;    // Local const
    const int y = 1000;  // Local const
    
    // Derleyici optimizasyonu: Register'a yerleştirebilir
    // Eğer adres alınmazsa .rodata'ya gitmez
}
```

**Assembly (optimize edilmiş):**
```assembly
function:
    mov eax, 42      ; Direkt değer
    mov ebx, 1000    ; Direkt değer
    ; .rodata kullanılmadı!
```

### Global/Static Const Değişkenler

```cpp
const int BUFFER_SIZE = 8192;      // Global const
static const char* VERSION = "1.0"; // Static const

void use_constants() {
    printf("Buffer: %d, Version: %s", BUFFER_SIZE, VERSION);
}
```

**Assembly:**
```assembly
.rodata:
    BUFFER_SIZE: .int 8192
    VERSION_STR: .asciz "1.0"

use_constants:
    mov eax, [BUFFER_SIZE]    ; .rodata'dan yükle
    mov rdi, VERSION_STR      ; String adresi
```

---

## 6. 🧪 Pratik Deneyim: Örneklerle Pekiştirme

### Örnek 1: Duplicate Detection Test

```cpp
// examples/duplicate_test.cpp dosyasında tam kod
```

### Örnek 2: Size Comparison

```cpp  
// examples/size_comparison.cpp dosyasında tam kod
```

### Örnek 3: Assembly Analysis

```cpp
// examples/assembly_analysis.cpp dosyasında tam kod  
```

---

## 📋 Bu Bölüm Özeti

**✅ Öğrendiklerimiz:**
- Derleyici sabit havuzu, derleme zamanında duplicate kontrolü yapar
- `.rodata` bölümü salt okunur verileri tutar ve yazma korumalıdır
- Küçük sabitler genellikle instruction içine gömülür
- String literalleri .rodata'da tek kopya olarak tutulur
- Local const'lar genellikle register'da, global const'lar .rodata'da

**🔑 Kritik Noktalar:**
- Duplicate kontrolü memory ve code size optimizasyonu sağlar
- Assembly kod inceleme yeteneği debugging'de çok değerli
- Platform ve derleyici farklılıkları olabilir ama temel mantık aynı

**📍 Sonraki Adım:** 
İleri kavramlar bölümünde inline embedding ile external storage'ın derinlemesine karşılaştırmasını yapacağız.

---

**🔧 Pratik Egzersizler:**
1. `examples/` klasöründeki kodları derleyip test edin
2. `objdump` ile kendi kodunuzun .rodata bölümünü inceleyin  
3. Farklı optimizasyon seviyelerinde (-O0, -O1, -O2) sonuçları karşılaştırın
