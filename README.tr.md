# Screen Addict Freezer

[English](README.md) | **Türkçe**

iPhone'undaki sosyal medyayı **senin bilmediğin** bir şifrenin arkasına kilitle. O şifreyi bilgisayarın bilir.

iPhone'un Ekran Süresi özelliği uygulamaları engelleyebilir. Ama şifreyi kendin koyarsan engeli de iki saniyede kaldırırsın. Screen Addict Freezer şifreyi senin aklından alıp bilgisayarına taşır:

- Program rastgele 4 haneli bir Ekran Süresi şifresi üretir ve gizlenmiş bir dosyada saklar.
- Şifreyi **rakam rakam** gösterir, yani 4 haneyi hiçbir zaman aynı anda görmezsin.
- Şifreyi görmek için bilgisayarın başına geçip kendi seçtiğin bir cümleyi yazman gerekir. Örneğin: *"evet ben iradesizim ve şifreye muhtacım"*.
- Şifreyi istediğin zaman **yenileyebilirsin**. Böylece aklında kalmış bir şifre varsa işe yaramaz hale gelir.

Mac, ücretli Apple Developer hesabı, bulut servisi ya da telefona kurulan bir uygulama gerekmez. Sadece iPhone'un kendi Ekran Süresi özelliği ve küçük bir C++ konsol programı.

## Nasıl çalışır

```
 iPhone                                   Bilgisayar
┌───────────────────────────┐      ┌──────────────────────────────┐
│ Ekran Süresi              │      │ screen_addict_freezer.exe     │
│  - Kapalı Kalma (21-20)   │      │                              │
│  - uygulama yükle/sil yok │◄─────│ vault.dat  ──► şifre 4-8-1-6 │
│  - web kısıtlamaları      │ şifre│ cümleni yazınca, rakam rakam  │
│  - şifre: ????            │      │                              │
└───────────────────────────┘      └──────────────────────────────┘
 Kurtarma: günlük kullanmadığın İKİNCİ bir Apple hesabı
```

## Dürüst sınırlar

Bu bir güvenlik ürünü değil, **kendine karşı bir zahmet katmanı**.

- Kasa dosyası gizlenmiştir (XOR) ama **şifrelenmemiştir**. Anahtar bu repoda açıkça duruyor. Dosyaya ve koda sahip olan biri şifreyi çözebilir; kötü bir gününde sen de dahil.
- Kurtarma Apple hesabı Ekran Süresi şifresini sıfırlayabilir. Bu hesabın şifresini zor ulaşılan bir yerde tut: kapalı bir zarfta ya da bir aile üyesinde.
- Gerçekten kırmak istersen kırarsın. Amaç, zayıf bir anında yolun yeterince uzun olması: bilgisayar → program → cümle → rakamlar. Çoğu zaman bu kadarı vazgeçirir.

## Gereksinimler

- Ekran Süresi olan bir iPhone (iOS 12 ve sonrası)
- Sadece Ekran Süresi kurtarma hesabı olarak kullanılacak ikinci bir Apple hesabı
- Windows bilgisayar (Linux ve macOS'ta da kaynak koddan derleyerek çalışır)

## Kurulum

### Seçenek A: MinGW ile derleme (Windows)

1. Bir C++ derleyicisi kur, örneğin [MSYS2](https://www.msys2.org/) ya da [WinLibs](https://winlibs.com/) MinGW-w64.
2. Bu repoyu indir (yeşil **Code** düğmesi → *Download ZIP*) ve zip'i aç.
3. `build.bat` dosyasına çift tıkla ya da klasörde terminalden çalıştır.
4. `screen_addict_freezer.exe` dosyasını kasanı tutmak istediğin klasöre kopyala. Repo klasörünün içine değil.

### Seçenek B: CMake (her platform)

```bash
cmake -B build
cmake --build build --config Release
```

## Kullanım

Adım adım rehber için **[docs/iphone-kurulum.md](docs/iphone-kurulum.md)** dosyasına bak. Kısaca:

1. iPhone'da Ekran Süresi'ni **kendi seçtiğin geçici bir şifreyle** kilitle, kurtarma hesabı olarak ikinci Apple hesabını gir.
2. Tüm ayarları geçici şifreyle yap: Kapalı Kalma Süresi, içerik kısıtlamaları, web kısıtlamaları.
3. Programı çalıştır → **2) İlk kurulum** → onay cümleni seç.
4. iPhone'da *Ekran Süresi Şifresini Değiştir*: önce geçici şifreni, sonra programın rakam rakam gösterdiği yeni şifreyi gir.
5. `vault.dat` dosyasını yedekle, örneğin bir USB belleğe.

| Menü | Ne yapar |
|---|---|
| 1) Kilidi aç | Cümleni yazarsın → şifre rakam rakam gösterilir. Şifre değişmez. |
| 2) İlk kurulum | Sadece henüz kasa yokken çalışır. |
| 3) Şifreyi yenile | Cümleni yazarsın → eski ve yeni şifre gösterilir → telefona bilmediğin yeni bir şifre girilir. |
| 4) Onay cümlesini değiştir | Mevcut cümleyi yazmayı gerektirir. |
| 5) Dil | English / Türkçe |

## Dosyalar

| Dosya | İçerik |
|---|---|
| `vault.dat` | Geçerli şifre, cümlen ve dil seçimin. **Yedekle. Asla repoya ekleme.** |
| `vault_pending.dat` | Sadece şifre değişikliği sürerken bulunur. Program bir sonraki açılışta durumu çözer. |
| `kasa.dat` | İlk sürümün kasası. Otomatik olarak `vault.dat` dosyasına taşınır. |

Program bu dosyaları **çalıştırıldığı klasörde** oluşturur.

## Bir şeyler ters giderse

- **vault.dat kayboldu ya da bozuldu:** yedeği geri yükle. Yedek yoksa: iPhone → Ayarlar → Ekran Süresi → Ekran Süresi Şifresini Değiştir → *Şifrenizi mi unuttunuz?* → kurtarma Apple hesabıyla giriş yap → yeni şifre belirle → programda *İlk kurulum*'u tekrar yap.
- **Kurtarma hesabının şifresini unuttun:** başka bir cihazdan [iforgot.apple.com](https://iforgot.apple.com) adresinde sıfırla.
- **Acil durum:** Kapalı Kalma Süresi telefon aramalarını asla engellemez, acil aramalar her zaman çalışır.

## Proje yapısı

```
src/
  main.cpp                  giriş noktası
  app.h/.cpp                menü ve tüm akışlar
  vault.h/.cpp              sağlama toplamlı, gizlenmiş dosya kaydı
  password_generator.h/.cpp rastgele, kolay tahmin edilemeyen 4 haneli şifre
  phrase.h/.cpp             cümle sadeleştirme (büyük/küçük harf, Türkçe harf, boşluk)
  console.h/.cpp            konsol yardımcıları, rakam rakam gösterim
  i18n.h/.cpp               İngilizce / Türkçe metinler
docs/                       iPhone kurulum rehberleri (EN / TR)
```

## Lisans

[MIT](LICENSE)
