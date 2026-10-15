# Screen Addict Freezer

[English](README.md) | **Türkçe**

Ekran Süresi şifresini kendin koyduğun takdirde o döngüden çıkışın çok kolay oluyor; istediğin an değiştirip kaldırabiliyorsun. Bu küçük program şifreyi senin yerine tutuyor: telefon kilitli kalıyor, şifre bilgisayarında duruyor ve sen o şifreyi bilmiyorsun.

## Ne yapıyor

- 4 haneli, rastgele bir Ekran Süresi şifresi üretiyor. Rakamların hepsi farklı oluyor, çünkü 1171 gibi şifreler hemen akılda kalıyor.
- Şifreyi **rakam rakam** gösteriyor. Dört haneyi hiçbir zaman aynı anda görmüyorsun.
- Şifreyi görmek için önce bilgisayarın başına geçip kendi seçtiğin bir cümleyi yazıyorsun. Mesela: *"evet ben iradesizim ve şifreye muhtacım"*. Böyle bir anda bunu yazmak insanı biraz düşündürüyor.
- Şifre aklında kaldıysa tek komutla yeniliyorsun. Bildiğin şifre çöpe gidiyor.

Mac, ücretli Apple hesabı ya da telefona uygulama kurmak gerekmiyor. iPhone'un kendi Ekran Süresi ayarı ve bilgisayarda küçük bir C++ programı yeterli.

## Nasıl çalışıyor

```
 iPhone                                   Bilgisayar
┌───────────────────────────┐      ┌──────────────────────────────┐
│ Ekran Süresi              │      │ screen_addict_freezer.exe     │
│  - Kapalı Kalma / Sınır   │      │                              │
│  - uygulama yükle/sil yok │◄─────│ vault.dat  ──► şifre 4-8-1-6 │
│  - web kısıtlamaları      │ şifre│ cümleni yazınca, rakam rakam  │
│  - şifre: ????            │      │                              │
└───────────────────────────┘      └──────────────────────────────┘
 Kurtarma: günlük kullanmadığın İKİNCİ bir Apple hesabı
```

## Baştan söyleyeyim

Bu bir güvenlik programı değil. Kendine koyduğun, uğraştırıcı ve caydırıcı bir engel.

- Şifrenin durduğu dosya sadece gizlenmiş (XOR), gerçekten şifrelenmiş değil. Anahtar bu repoda açıkça duruyor. Dosyayı ve kodu alan biri şifreyi çözebilir.
- Kurtarma için kullandığın Apple hesabı Ekran Süresi şifresini sıfırlayabilir.
- Gerçekten kırmak istersen kırarsın. Amaç, zayıf bir anında önünde yeterince uzun bir yol olması: bilgisayarı aç, programı çalıştır, cümleyi yaz, rakamları tek tek al. Çoğu zaman bu kadarı vazgeçirmeye yetiyor.

## Neye ihtiyacın var

- Ekran Süresi olan bir iPhone (iOS 12 ve sonrası)
- İkinci bir Apple hesabı. Sadece Ekran Süresi şifresini kurtarmak için.
- Windows bilgisayar. Linux ve macOS'ta da kaynak koddan derleyip kullanabilirsin.

## Kurulum

### Windows'ta MinGW ile

1. Bir C++ derleyicisi kur, örneğin [MSYS2](https://www.msys2.org/) ya da [WinLibs](https://winlibs.com/) MinGW-w64.
2. Repoyu indir (yeşil **Code** düğmesi → *Download ZIP*) ve zip'i aç.
3. `build.bat` dosyasına çift tıkla ya da klasörde terminalden çalıştır.
4. Çıkan `screen_addict_freezer.exe` dosyasını şifreni tutacağın klasöre kopyala. Repo klasörünün içine koyma.

### CMake ile (her platform)

```bash
cmake -B build
cmake --build build --config Release
```

## Kullanım

Ayrıntılı, adım adım anlatım [docs/iphone-kurulum.md](docs/iphone-kurulum.md) dosyasında. Kısaca:

1. iPhone'da Ekran Süresi'ni **kendi seçtiğin geçici bir şifreyle** kilitle. Kurtarma hesabı olarak ikinci Apple hesabını gir.
2. Ayarları bu geçici şifreyle yap. Önce sana uyan modu seç:
   - **Sert mod (Kapalı Kalma Süresi):** Belirlediğin saatler dışında telefonun neredeyse tüm internet tabanlı işlevi sıfırlanıyor, web sitelerine erişimin de kapanıyor.
   - **Dengeli mod (Uygulama Sınırları), önerilen:** Sadece senin seçeceğin sosyal medyalara günlük bir süre konuyor. Safari ve geri kalan her şey açık kalıyor.
3. Programı aç → **2) İlk kurulum** → onay cümleni seç.
4. iPhone'da *Ekran Süresi Şifresini Değiştir*'e gir. Önce geçici şifreni yaz, sonra programın rakam rakam gösterdiği yeni şifreyi.
5. `vault.dat` dosyasını bir USB belleğe yedekle.

Bundan sonra telefondaki şifreyi sen bilmiyorsun.

| Menü | Ne yapar |
|---|---|
| 1) Kilidi aç | Cümleni yazarsın, şifre rakam rakam gösterilir. Şifre değişmez. |
| 2) İlk kurulum | Sadece henüz kasa yokken çalışır. |
| 3) Şifreyi yenile | Cümleni yazarsın, eski ve yeni şifre gösterilir. Telefona bilmediğin yeni bir şifre girersin. |
| 4) Onay cümlesini değiştir | Önce şu anki cümleyi yazman gerekir. |
| 5) Dil | English / Türkçe |

## Dosyalar

| Dosya | İçinde ne var |
|---|---|
| `vault.dat` | Şu anki şifre, cümlen ve dil seçimin. **Yedekle, repoya asla ekleme.** |
| `vault_pending.dat` | Sadece şifre değişirken oluşur. Bir şey yarıda kalırsa program bir sonraki açılışta toparlar. |
| `kasa.dat` | İlk sürümün dosyası. Program bunu kendiliğinden `vault.dat`'a taşır. |

Program bu dosyaları **hangi klasörden çalıştırdıysan** oraya yazar.

## Bir şey ters giderse

- **`vault.dat` kayboldu ya da bozuldu:** Yedeği geri koy. Yedek yoksa: iPhone → Ayarlar → Ekran Süresi → Ekran Süresi Şifresini Değiştir → *Şifrenizi mi unuttunuz?* → kurtarma Apple hesabıyla giriş yap → yeni şifre belirle → programda *İlk kurulum*'u tekrar yap.
- **Kurtarma hesabının şifresini unuttun:** Başka bir cihazdan [iforgot.apple.com](https://iforgot.apple.com) adresinde sıfırla.
- **Acil durum:** Kapalı Kalma Süresi telefon aramalarını hiçbir zaman engellemez. Acil arama her zaman çalışır.

## Kodun içi

```
src/
  main.cpp                  giriş noktası
  app.h/.cpp                menü ve bütün akışlar
  vault.h/.cpp              şifreyi saklayan dosya (gizleme + sağlama toplamı)
  password_generator.h/.cpp rakamları farklı, tahmini zor 4 haneli şifre
  phrase.h/.cpp             cümle karşılaştırma (büyük/küçük harf, Türkçe harf, boşluk)
  console.h/.cpp            konsol yardımcıları, rakam rakam gösterim
  i18n.h/.cpp               İngilizce / Türkçe metinler
docs/                       iPhone kurulum rehberleri (EN / TR)
```

## Lisans

[MIT](LICENSE). Kodu istediğin gibi kullanabilir, değiştirebilir, paylaşabilirsin.
