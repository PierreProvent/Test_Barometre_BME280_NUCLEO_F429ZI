#include "mbed.h"

#define BME280_ADDRESS   (0x76 << 1) // Adresse décalée de 1 bit car I2C Mbed utilise des adresses sur 8 bits

#define BME280_REG_DIG_T1    0x88
#define BME280_REG_DIG_T2    0x8A
#define BME280_REG_DIG_T3    0x8C
#define BME280_REG_DIG_P1    0x8E
#define BME280_REG_DIG_P2    0x90
#define BME280_REG_DIG_P3    0x92
#define BME280_REG_DIG_P4    0x94
#define BME280_REG_DIG_P5    0x96
#define BME280_REG_DIG_P6    0x98
#define BME280_REG_DIG_P7    0x9A
#define BME280_REG_DIG_P8    0x9C
#define BME280_REG_DIG_P9    0x9E
#define BME280_REG_DIG_H1    0xA1
#define BME280_REG_CHIPID    0xD0
#define BME280_REG_SOFTRESET 0xE0
#define BME280_REG_DIG_H2    0xE1
#define BME280_REG_DIG_H3    0xE3
#define BME280_REG_DIG_H4    0xE4
#define BME280_REG_DIG_H5    0xE5
#define BME280_REG_DIG_H6    0xE7
#define BME280_CTRL_HUM_ADDR       0xF2
#define BME280_STATUS_ADDR         0xF3
#define BME280_CTRL_MEAS_ADDR      0xF4
#define BME280_CONFIG_ADDR         0xF5
#define BME280_REG_PRESSUREDATA    0xF7
#define BME280_REG_TEMPDATA        0xFA
#define BME280_REG_HUMDATA         0xFD

#define BME280_CHIP_ID             0x60
class BME280
{
public:
    BME280(PinName sda, PinName scl);// constructor
    double temperature(void) ;
    double pression(void) ;
    double humidite() ;
private:
    void   init(void);
    void   read_calibration_data(void) ;
    // Données de calibration du capteur BME280 T, P, H
    uint16_t dig_T1;  // 16 bits non signés
    int16_t dig_T2;   // 16 bits signés
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    uint8_t dig_H1 ;
    int16_t dig_H2 ;
    uint8_t dig_H3 ;  // 8 bits non signés
    int16_t dig_H4 ;
    int16_t dig_H5 ;
    int8_t dig_H6 ;  // 8 bits  signés

    char data_write[2] ;  // Données de commande fournies au capteur
    char data_read[26] ;  // Données de calibration du capteur : T, P, H
    int32_t t_fine;
    double var1, var2;
    I2C i2c;
};
