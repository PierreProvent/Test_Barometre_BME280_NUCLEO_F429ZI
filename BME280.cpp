#include "mbed.h"
#include "BME280.h"

BME280::BME280(PinName sda, PinName scl):i2c(sda, scl)
{
    init();
    read_calibration_data() ;
}

void BME280::init()
{
    // Configuration de la mesure de température, pression et humidité
    // Mode ""normal" pour des mesures régulières et précises de P, T, H.
    // T et P filtrées par IIR et suréchantillonnage
    // Choix d'un mode "indoor navigation" p 17-18 doc constructeur
    // Registres :   ctrl_hum  osrs_h[2:0] = 001  oversampling humidity x1
    //               ctrl_hum = 00000 001
    //               ctrl_meas osrs_t[2:0] = 010  oversampling temperature x2
    //               ctrl_meas osrs_p[2:0] = 101  oversampling pressure x16
    //               ctrl_meas mode[1:0] = 11     Normal mode
    //               ctrl_meas = 010 101 11
    //               config t_sb[2:0] = 000     t_standby 0.5 ms
    //               config filter[2:0] = 100   filtre IIR à 16 coefficients
    //               config   =  000 100 00
    data_write[0] = BME280_CTRL_HUM_ADDR ;
    data_write[1] = 0b00000001  ;
    i2c.write(BME280_ADDRESS, data_write, 2);
    data_write[0] = BME280_CTRL_MEAS_ADDR ;
    data_write[1] = 0b01010111  ;
    i2c.write(BME280_ADDRESS, data_write, 2);
    data_write[0] = BME280_CONFIG_ADDR ;
    data_write[1] = 0b00010000 ;
    i2c.write(BME280_ADDRESS, data_write, 2);
}

void BME280::read_calibration_data()
{
//Lecture des données de calibration de température et de pression du capteur
    // 3 mots de 16 bits pour la température
    // 9 mots de 16 bits pour la pression
    // p 22-23
    // Démarre la lecture à partir de l'adresse BME280_REG_DIG_T1 0x88
    // Jusqu'à l'adresse BME280_REG_DIG_P9 0x9E
    // Lecture des 24 octets en une seule fois
    // 8 bits LSB en premier dans data_read[0], 8 bits MSB en second dans data_read[1] ;
    // Et lecture de 2 octets supplémentaires, adresse 0xA0 et 0xA1
    // 0xA1 contient une donnée de calibration pour l'humidité
    // Les 7 autres octets de calibration d'humidité seront lus après de l'adresse 0xE1 à 0xE7

    data_write[0] = BME280_REG_DIG_T1 ;
    i2c.write(BME280_ADDRESS, data_write, 1);
    i2c.read(BME280_ADDRESS, data_read, 26);

    dig_T1 = (uint16_t) ( (uint16_t) (data_read[1] << 8) | (uint16_t) data_read[0] ) ;
    dig_T2 = (int16_t) ( (int16_t) (data_read[3] << 8) | (int16_t) data_read[2] ) ;
    dig_T3 = (int16_t) ( (int16_t) (data_read[5] << 8) | (int16_t) data_read[4] ) ;
    dig_P1 = (uint16_t) ( (uint16_t) (data_read[7] << 8) | (uint16_t) data_read[6] ) ;
    dig_P2 = (int16_t) ( (int16_t) (data_read[9] << 8) | (int16_t) data_read[8] ) ;
    dig_P3 = (int16_t) ( (int16_t) (data_read[11] << 8) | (int16_t) data_read[10] ) ;
    dig_P4 = (int16_t) ( (int16_t) (data_read[13] << 8) | (int16_t) data_read[12] ) ;
    dig_P5 = (int16_t) ( (int16_t) (data_read[15] << 8) | (int16_t) data_read[14] ) ;
    dig_P6 = (int16_t) ( (int16_t) (data_read[17] << 8) | (int16_t) data_read[16] ) ;
    dig_P7 = (int16_t) ( (int16_t) (data_read[19] << 8) | (int16_t) data_read[18] ) ;
    dig_P8 = (int16_t) ( (int16_t) (data_read[21] << 8) | (int16_t) data_read[20] ) ;
    dig_P9 = (int16_t) ( (int16_t) (data_read[23] << 8) | (int16_t) data_read[22] ) ;
    dig_H1 = (uint8_t) data_read[25] ;

//Lecture des données de calibration d'humidité
    // 7 octets de l'adresse 0xE1 à 0xE7
    data_write[0] = BME280_REG_DIG_H2 ;
    i2c.write(BME280_ADDRESS, data_write, 1);
    i2c.read(BME280_ADDRESS, data_read, 7);

    dig_H2 = (int16_t) ( (int16_t) (data_read[1] << 8) | (int16_t) data_read[0] ) ;
    dig_H3 = (uint8_t) data_read[2] ;
    dig_H4 = (int16_t) ( (int16_t) (data_read[4] & 0x0F) | (int16_t) data_read[3] *16 ) ;
    dig_H5 = (int16_t) ( ((int16_t) data_read[5] * 16 | (int16_t) data_read[4]) >> 4 ) ;
    dig_H6 = (int8_t) data_read[6] ;
}

double BME280::temperature()
{
    double temperature ; // Température en double précision
    int32_t temp_32bits;

    data_write[0] = BME280_REG_TEMPDATA ;
    i2c.write(BME280_ADDRESS, data_write, 1);
    i2c.read(BME280_ADDRESS, data_read, 3);
    temp_32bits = (int32_t) ( ((int32_t) data_read[0] << 12 | (int32_t) data_read[1] << 4 | (int16_t) data_read[2] >> 4) ) ;
    var1 = (((double) temp_32bits) / 16384.0 - ((double) dig_T1) / 1024.0) * ((double)dig_T2) ;
    var2 = ((((double) temp_32bits) / 131072.0 - ((double) dig_T1) / 8192.0) *
            (((double) temp_32bits) / 131072.0 - ((double) dig_T1) / 8192.0)) *
           ((double) dig_T3);
    temperature = ((var1 + var2) / 5120.0);
    t_fine = (int32_t) (var1 + var2);
    return temperature ;
}

double BME280::pression()
{
    double pression ; // Pression en double precision
    int32_t press_32bits;

    data_write[0] = BME280_REG_PRESSUREDATA ;
    i2c.write(BME280_ADDRESS, data_write, 1);
    i2c.read(BME280_ADDRESS, data_read, 3);
    press_32bits = (int32_t) ( ((int32_t) data_read[0] << 12 | (int32_t) data_read[1] << 4 | (int16_t) data_read[2] >> 4) ) ;
    var1 = ((double) t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double) dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double) dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double) dig_P4) * 65536.0);
    var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) /
           524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double) dig_P1);
    if (var1 == 0.0)
        return 0; // avoid exception caused by division by zero
    pression = 1048576.0 - (double)press_32bits ;
    pression = (pression-(var2/4096.0))* 6250.0/var1;
    var1 = ((double) dig_P9) * pression * pression / 2147483648.0;
    var2 = pression * ((double) dig_P8) / 32768.0;
    pression = pression + (var1 + var2 + ((double) dig_P7)) / 16.0;
    return pression ;
}

double BME280::humidite()
{
    double humidite ; // Humidite en double precision
    int32_t hum_32bits;
    data_write[0] = BME280_REG_HUMDATA ;
    i2c.write(BME280_ADDRESS, data_write, 1);
    i2c.read(BME280_ADDRESS, data_read, 2);
    hum_32bits = (int32_t) ( ((int32_t) data_read[0] << 8 | (int32_t) data_read[1] ) ) ;
    humidite = (double)t_fine - 76800.0;
    humidite = (hum_32bits - (((double)dig_H4) * 64.0 + ((double)dig_H5) / 16384.0 * humidite)) *
            (((double)dig_H2) / 65536.0 * (1.0 + ((double)dig_H6) / 67108864.0 * humidite *
                                           (1.0 + ((double)dig_H3) / 67108864.0 * humidite)));
    humidite = humidite * (1.0 - ((double)dig_H1) * humidite / 524288.0);
    if (humidite > 100.0)
        humidite = 100.0;
    else if (humidite < 0.0)
        humidite = 0.0;
    return humidite;
}