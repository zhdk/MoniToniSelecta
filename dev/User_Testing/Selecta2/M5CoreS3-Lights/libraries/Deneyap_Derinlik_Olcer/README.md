# Deneyap ToF Range Finder Sensor Arduino Library
[FOR TURKISH VERSION](docs/README_tr.md) ![trflag](https://github.com/deneyapkart/deneyapkart-arduino-core/blob/master/docs/tr.png)

***** Add photo ****

Arduino library for Deneyap ToF Range Finder Sensor VL53L0X

## :mag_right:Specifications 
- `Product ID` **M06**, **mpv1.0**
- `MCU` ST Microelectronics VL53L0CXV0DH/1
- `Weight` 
- `Module Dimension` 25,4 mm x 25,4 mm
- `I2C address` 0x29

| Address |  | 
| :---    | :---     |
| 0x29    | default address |

## :closed_book:Documentation
[Deneyap ToF Range Finder Sensor](https://docs.deneyapkart.org/en/content/contentDetail/deneyap-module-deneyap-tof-m03)

[Deneyap ToF Range Finder Sensor Shematic](https://cdn.deneyapkart.org/media/upload/userFormUpload/2BzP7yaI6mR6uKiXusAbPDFn6WJYzYyN.pdf)

[Deneyap ToF Range Finder Sensor Mechanical Drawing](https://cdn.deneyapkart.org/media/upload/userFormUpload/N7QALHBGi8NnzE6OPQASMRvJpT15uQDI.pdf)

[VL53L0X-dataheet](https://www.st.com/content/ccc/resource/technical/document/datasheet/group3/b2/1e/33/77/c6/92/47/6b/DM00279086/files/DM00279086.pdf/jcr:content/translations/en.DM00279086.pdf)

[How to install a Arduino Library](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries)

## :pushpin:Deneyap ToF Range Finder Sensor
This Arduino library allows you to use Deneyap ToF Range Finder Sensor VL53L0X with I2C peripheral. You can use this library in your projects with any Arduino compatible board with I2C peripheral.

## :globe_with_meridians:Repository Contents
- `/docs ` README_tr.md and product photos
- `/examples ` Examples with .ino extension sketches
- `/src ` Source files (.cpp .h)
- `keywords.txt ` Keywords from this library that will be highlighted in the Arduino IDE
- `library.properties ` General library properties for the Arduino package manager

## Version History
1.0.2 - update library function(`begin`)

1.0.1 - bug fix

1.0.0 - initial release

## :rocket:Hardware Connections
- Deneyap ToF Range Finder Sensor and Board can be connected with I2C cable
- or 3V3, GND, SDA and SCL pins can be connected with jumper cables

|ToF Range Finder| Function | Board pins | 
|:--- |   :---  | :---|
|3.3V | Power   |3.3V |      
|GND  | Ground  | GND | 
|SDA  | I2C Data  | SDA pin |
|SCL  | I2C Clock | SCL pin |
| XSHT | Shut Down | |
| GPIO | Interrupt Output | |

## License Information
Please review the [LICENSE](https://github.com/deneyapkart/deneyap-derinlik-olcer-arduino-library/blob/master/LICENSE) file for license information.