# Giriş: C/C++ Derleyici Sabit Havuzu ve Bellek Yönetimi

## 🎯 Öğrenme Hedefleri

Bu ders sonunda öğrenciler:
- Derleyicilerin sabit değerleri nasıl yönettiğini anlayacak
- `.rodata` bölümünün ne olduğunu ve nasıl kullanıldığını kavrayacak
- String literalleri ile sayısal sabitlerin farklarını açıklayabilecek
- Bellek optimizasyonlarının önemini anlayacak
- Assembly kod ile high-level kod arasındaki ilişkiyi görebilecek

## 📋 Ön Gereksinimler

- **C/C++ programlama:** Orta seviye
- **Bellek kavramları:** Stack, Heap temel bilgisi
- **Assembly:** Temel okuma yeteneği (isteğe bağlı)
- **Derleyici temel bilgisi:** Derleme sürecinin genel akışı

## 🤔 Neden Bu Konu Önemli?

Bir C++ programcısı olarak kendinize şu soruları sordunuz mu?

```cpp
// Bu kodda "Hello World" string'i bellekte kaç kez saklanır?
void func1() { printf("Hello World"); }
void func2() { printf("Hello World"); }
void func3() { std::cout << "Hello World"; }

// Bu sabitler nerede tutulur?
const int MAX_SIZE = 1000;
const char* MESSAGE = "Sistem hazır";

// Bu iki kod arasında performans farkı var mı?
int x = 42;          // vs
int y = 0x7FFFFFFF;  // vs
```

### 💡 Gerçek Dünya Senaryoları

**🔹 Gömülü Sistem Geliştirme:**
```
❌ Problem: 32KB RAM'li mikrocontroller'da string literalleri RAM'de tutuluyor
✅ Çözüm: .rodata bölümü sayesinde Flash memory kullanımı
```

**🔹 Oyun Geliştirme:**
```
❌ Problem: Aynı "Game Over" mesajı 100 kez farklı yerlerde RAM'de
✅ Çözüm: Derleyici duplicate kontrolü ile tek kopya
```

**🔹 Web Server:**
```
❌ Problem: HTTP status mesajları instruction cache'i şişiriyor  
✅ Çözüm: .rodata kullanımı ile optimize edilmiş kod boyutu
```

## 🏗️ Ders Yapısı

### 📚 Modül 1: Temel Kavramlar
- Derleyici Sabit Havuzu (Constant Pool)
- `.rodata` bölümünün tanıtımı
- Küçük sabitler vs String literalleri
- İlk pratik örnekler

### 🧠 Modül 2: İleri Kavramlar  
- Inline embedding vs External storage
- Duplicate kontrolü mekanizması
- Assembly kod analizi
- Optimizasyon stratejileri

### 🛠️ Modül 3: Uygulamalar
- Embedded systems örnekleri
- Performance critical kod yazımı
- Memory-constrained ortamlar
- Case studies

### 📋 Modül 4: Özet ve İpuçları
- Best practices
- Common pitfalls
- Debugging teknikleri
- İleri okuma kaynakları

## 🔍 İlk Örnek: Gözle Görebileceğiniz Fark

İşte basit bir C++ programı:

```cpp
#include <iostream>

int main() {
    const int small_num = 5;           // Küçük sabit
    const char* message = "Hello";     // String literal
    
    std::cout << small_num << ": " << message << std::endl;
    return 0;
}
```

**Derleyici bu kodu nasıl işler?**
1. `5` sayısı büyük ihtimalle instruction içine gömülür
2. `"Hello"` string'i `.rodata` bölümünde ayrı bir yerde tutulur
3. `message` değişkeni bu string'in adresini tutar

Bu derste bunun **neden** böyle olduğunu ve **nasıl** kontrol edebileceğinizi öğreneceksiniz.

## 🚀 Hazır mısınız?

Bir sonraki bölümde derleyici sabit havuzunun nasıl çalıştığına dair temel mekanizmaları inceleyeceğiz. Kod örnekleri ile bu soyut kavramları somutlaştıracağız.

---

**📌 Not:** Bu ders boyunca GCC ve Clang derleyicilerini referans alacağız, ancak kavramlar diğer modern derleyiciler için de geçerlidir.

**🔧 Araçlar:** `objdump`, `nm`, `readelf` gibi araçları kullanarak binary analizi yapacağız.

**💻 Platform:** Linux/macOS ortamında örnekler verilecek, Windows kullanıcıları için alternatif araçlar belirtilecek.
