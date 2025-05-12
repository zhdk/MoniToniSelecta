# Deneyap Derinlik Ölçer Arduino Kütüphanesi

***** Fotoğraf eklenecek ****

Deneyap Derinlik Ölçer VL53L0X için Arduino kütüphanesidir.

## :mag_right:Özellikler 
- `Ürün ID` **M06**, **mpv1.0**
- `MCU` ST Microelectronics VL53L0CXV0DH/1
- `Ağırlık`
- `Modul Boyutları` 25,4 mm x 25,4 mm
- `I2C Adres` 0x29

| Adres |  | 
| :--- | :---     |
| 0x29 | varsayılan adres |

## :closed_book:Dokümanlar
[Deneyap Derinlik Ölçer](https://docs.deneyapkart.org/tr/content/contentDetail/deneyap-module-deneyap-tof-m03)

[Deneyap Derinlik Ölçer Şematik](https://cdn.deneyapkart.org/media/upload/userFormUpload/2BzP7yaI6mR6uKiXusAbPDFn6WJYzYyN.pdf)

[Deneyap Derinlik Ölçer Teknik Resim](https://cdn.deneyapkart.org/media/upload/userFormUpload/N7QALHBGi8NnzE6OPQASMRvJpT15uQDI.pdf)

[VL53L0X-dataheet](https://www.st.com/content/ccc/resource/technical/document/datasheet/group3/b2/1e/33/77/c6/92/47/6b/DM00279086/files/DM00279086.pdf/jcr:content/translations/en.DM00279086.pdf)

[Arduino IDE'de Kütüphane Nasıl İndirilir](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries)

## :pushpin:Deneyap Derinlik Ölçer
Bu Arduino kütüphanesi Deneyap Derinlik Ölçer'nin I2C çevre birimi ile kullanılmasını sağlar. Arduino ile uyumlu, I2C çevre birimine sahip herhangi bir geliştirme kartı ile bu kütüphaneyi projelerinizde kullanabilirsiniz.

## :globe_with_meridians:Repo İçeriği
- `/docs` README_tr.md ve ürün fotoğrafları
- `/examples` .ino uzantılı örnek uygulamalar
- `/src` kütüphane için .cpp ve .h uzantılı dosyalar
- `keywords.txt` Arduino IDE'de vurgulanacak anahtar kelimeler
- `library.properties` Arduino yöneticisi için genel kütüphane özellikleri

## Sürüm Geçmişi
1.0.2 - kütüphane fonksiyonu(`begin`) güncellendi

1.0.1 - hata giderildi

1.0.0 - ilk sürüm

## :rocket:Donanım Bağlantıları
- Deneyap Derinlik Ölçer ile kullanılan geliştirme kartı I2C kablosu ile bağlanabilir
- veya jumper kablolar ile 3V3, GND, SDA ve SCL bağlantıları yapılabilir. 

| Derinlik Ölçer | Fonksiyon| Kart pinleri |
| :---     | :---   |   :---  |
| 3.3V     |3.3V Besleme Gerilimi| 3.3V    |
| GND      | Toprak |GND      |
| SDA      |Çift Yönlü Veri Hattı| SDA pini |
| SCL      |Veri Hattı Zaman Senkronizasyon İşaret| SCL pini|
| XSHT | Shut Down | | 
| GPIO | Çıktı Kesmesi | | 

## Lisans Bilgisi 
Lisans bilgileri için [LICENSE](https://github.com/deneyapkart/deneyap-derinlik-olcer-arduino-library/blob/master/LICENSE) dosyasını inceleyin.