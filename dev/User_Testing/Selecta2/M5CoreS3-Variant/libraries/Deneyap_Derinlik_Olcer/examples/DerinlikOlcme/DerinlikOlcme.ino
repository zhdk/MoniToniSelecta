/*
 *   DerinlikOlcme örneği,  
 *   Bu örnekte temel konfigürasyon ayarları yapılmaktadır. 
 *   Sensörden gelen mesafe verilerini okumaktadır. Mesafe verilerini seri port ekranına yazdırmaktadır. 
 *   
 *   Bu algılayıcı I2C haberleşme protokolü ile çalışmaktadır.
 *   
 *   Bu örnek Deneyap Derinlik Ölçer için oluşturulmuştur
 *      ------> https://docs.deneyapkart.org/tr/content/contentDetail/deneyap-module-deneyap-tof-m03 <------
 *      ------> https://github.com/deneyapkart/deneyap-derinlik-olcer-arduino-library <------  
*/
#include <Deneyap_DerinlikOlcer.h>                  // Deneyap Derinlik Ölçer kütüphanesi eklenmesi

TofRangeFinder TofRangeFinder;                      // TofRangeFinder için class tanımlanması

void setup() {
    Serial.begin(115200);                           // Seri haberleşme başlatılması
    TofRangeFinder.begin(0x29);                     // begin(slaveAdress) fonksiyonu ile cihazların haberleşmesi başlatılması
}

void loop() {
    Serial.print("Derinlik: ");
    Serial.println(TofRangeFinder.ReadDistance());  // Derinliğin okunması ve seri port ekranına yazdırılması
    delay(50);                                      // 50 milisaniye bekleme süresi
}