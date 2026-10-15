# iPhone Kurulum Rehberi

[English](iphone-setup.md) | **Türkçe**

Menü adları iOS sürümüne göre biraz farklı olabilir. Bu rehber iOS 17–26 için yazıldı.

## 0. Hazırlık

- **İkinci bir Apple hesabı aç.** Bu hesap sadece Ekran Süresi şifresini kurtarmak için kullanılacak.
  - Şifresini uzun ve akılda kalmayacak şekilde seç, kağıda yaz, zarfa koy, kolay ulaşamayacağın bir yerde sakla.
  - Hesabın kurtarma e-postasına senin erişebilmen gerekiyor, yoksa ilerde kurtarma yapamazsın.
- Programı bilgisayarına kur (README'ye bak), ama **henüz çalıştırma**.

## 1. Ekran Süresi'ni geçici bir şifreyle kilitle

Ayarlar → Ekran Süresi → **Ekran Süresi Ayarlarını Kilitle**

- **Bildiğin geçici bir şifre** gir, örneğin 1357. Aşağıdaki ayarlar sırasında bu şifreyi defalarca gireceksin.
- Kurtarma için Apple hesabı sorulduğunda **ikinci hesabı** gir, ana hesabını değil.

## 2. Kapalı Kalma Süresi

Ekran Süresi → **Kapalı Kalma Süresi**

1. **Zamanlamaya Göre** seçeneğini aç ve saatleri gir. Örneğin her gün 21:00–20:00 kapalı olursa telefon günde 1 saat açık kalır.
2. **Kapalı Kalma Süresinde Engelle** seçeneğini aç.
3. Ekran Süresi → **Her Zaman İzin Verilenler** listesine gir. Sosyal medya uygulamaları orada varsa çıkar. Telefon, Mesajlar, Haritalar gibi gerekli uygulamalar kalabilir.

## 3. İçerik ve Gizlilik Kısıtlamaları

Ekran Süresi → **İçerik ve Gizlilik Kısıtlamaları** → en üstteki anahtarı **aç**.

**Uygulama yükleme ve silme.** **App Store Alışverişleri** altında:

| Ayar | Değer |
|---|---|
| Uygulama Yükleme | İzin Verme |
| Uygulama Silme | İzin Verme |
| Uygulama İçi Satın Alma | İzin Verme (isteğe bağlı) |

Bu sayede bir uygulamayı silip yeniden yükleyerek engeli sıfırlayamazsın.

**Değişikliklere izin ver.** Aynı ekranda aşağıda:

| Ayar | Değer |
|---|---|
| Şifre Değişiklikleri | İzin Verme |
| Hesap Değişiklikleri | İzin Verme |

**Saat hilesine karşı.** Kapalı Kalma Süresi telefonun saatine bakar, saati elle değiştirmek engeli bozabilir:

1. Ayarlar → Genel → Tarih ve Saat → **Otomatik Ayarla** açık olsun.
2. Ayarlar → Gizlilik ve Güvenlik → Konum Servisleri → Sistem Servisleri → **Saat Dilimini Ayarlama** açık olsun.
3. İçerik ve Gizlilik Kısıtlamaları → **Konum Servisleri** → **İzin Verme**. Bu, yukarıdaki ayarları kilitler.

## 4. Tarayıcı engeli

İçerik Kısıtlamaları → **Web İçeriği** → **Yetişkinlere Yönelik Siteleri Sınırla** → **Asla İzin Verme** altında **Web Sitesi Ekle**'ye bas ve şu siteleri ekle:

```
instagram.com
tiktok.com
x.com
twitter.com
facebook.com
youtube.com
reddit.com
```

- Bu engel kapalı kalma saatleriyle sınırlı değil, **günün her saatinde** geçerlidir.
- Chrome gibi başka tarayıcılar yüklüyse onları da sil ya da kapalı kalma süresinde engelle.

## 5. Test et

Hâlâ geçici şifredeyken şunları dene:

- Kapalı kalma saatinde bir sosyal medya uygulamasını açmayı
- Bir uygulamayı silmeyi
- Safari'den instagram.com'a girmeyi

Üçü de engellenmeli. Engellenmeyen varsa ayarları geçici şifreyle düzelt.

## 6. Gerçek şifreye geç

1. Bilgisayarda programı çalıştır → **2) İlk kurulum**.
2. Onay cümleni seç ve iki kez yaz.
3. iPhone'da Ayarlar → Ekran Süresi → **Ekran Süresi Şifresini Değiştir**:
   - Eski şifre olarak **geçici şifreni** kendin gir.
   - Yeni şifreyi program **rakam rakam** gösterir. İki kez girersin.
4. Program telefonun yeni şifreyi kabul edip etmediğini sorar; **E** de.
5. `vault.dat` dosyasını USB belleğe yedekle.

Artık telefondaki şifreyi sen bilmiyorsun.

## Kriz planı

| Durum | Ne yapmalı |
|---|---|
| `vault.dat` kayboldu, yedek var | Yedeği programın klasörüne kopyala. |
| `vault.dat` kayboldu, yedek yok | Ekran Süresi Şifresini Değiştir → *Şifrenizi mi unuttunuz?* → ikinci Apple hesabı → yeni şifre → programda İlk kurulum. |
| İkinci hesabın şifresini unuttun | Başka bir cihazdan iforgot.apple.com. |
| Hiçbiri olmuyor | Son çare: Ayarlar → Genel → Aktar veya Sıfırla → Tüm İçerikleri ve Ayarları Sil (ana Apple hesabının şifresi gerekir; önce yedek al). |
| Acil durum | Telefon aramaları ve acil arama hiçbir zaman engellenmez. |

Telefonda şifreyi tahmin etmeye çalışma. Her yanlış denemede bekleme süresi uzar.
