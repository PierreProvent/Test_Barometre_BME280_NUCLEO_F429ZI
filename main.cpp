#include "mbed.h"
 
I2C mon_i2c(PB_9,PB_8); // I2C_SDA I2C_SCL;
Serial pc(SERIAL_TX, SERIAL_RX);

#define BMP280_ADDRESS   (0x77 << 1) // Adresse déclalée de 1 bit car I2C Mbed utilise des adresses sur 8 bits

#define BMP280_REG_DIG_T1    0x88
#define BMP280_REG_DIG_T2    0x8A
#define BMP280_REG_DIG_T3    0x8C

#define BMP280_REG_DIG_P1    0x8E
#define BMP280_REG_DIG_P2    0x90
#define BMP280_REG_DIG_P3    0x92
#define BMP280_REG_DIG_P4    0x94
#define BMP280_REG_DIG_P5    0x96
#define BMP280_REG_DIG_P6    0x98
#define BMP280_REG_DIG_P7    0x9A
#define BMP280_REG_DIG_P8    0x9C
#define BMP280_REG_DIG_P9    0x9E

#define BMP280_REG_CHIPID          0xD0
#define BMP280_REG_VERSION         0xD1
#define BMP280_REG_SOFTRESET       0xE0

#define BMP280_STATUS_ADDR         0xF3
#define BMP280_CTRL_MEAS_ADDR      0xF4
#define BMP280_REG_CONFIG          0xF5

#define BMP280_REG_PRESSUREDATA    0xF7
#define BMP280_REG_TEMPDATA        0xFA

#define BMP280_CHIP_ID             0x58

int main()
{
 
// Données de calibration du capteur BMP280
    uint16_t dig_T1;  // uin16_t 16 bits non signés
    int16_t dig_T2;   // int16_t 16 bits signés
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

    char data_write[2] ;
    char data_read[24] ;
   
    pc.printf("\nTemperature/pression issues du barometre BMP280 de Bosch Sensortec\n");
    // wait_ms(500) ;
    data_write[0] = BMP280_REG_CHIPID ;
    int status = mon_i2c.write(BMP280_ADDRESS, data_write, 1, 0);
    
    if (status != 0) // Si le capteur est présent on recoit un acknoledgement
        pc.printf("Barometre BMP280 non trouve sur bus I2C \n");
    else
        pc.printf("Barometre BMP280 trouve sur bus I2C \n");
    
    pc.printf("Lecture de l'identificateur de la puce BMP280 \n");
    mon_i2c.read(BMP280_ADDRESS, data_read, 1, 0);
    if (data_read[0] == BMP280_CHIP_ID)
        pc.printf("ID barometre correct =  %#x \n",data_read[0]);
    else
        pc.printf("ID barometre incorrect =  %#x \n",data_read[0]);
    
    // Configuration de la précision pour la mesure de température et de pression

    // oversampling Temperature (3bits): oversampling Pression (3bits): Power control mode (2bits)
    // voir documentation constructeur p 25-26 et p 15
    // Ici on choisit le mode handheld device low-power avec filtre RII décrit p 14 de la doc constructeur 
    // T oversampling *2, P oversampling *16, Normal mode, IIR filter coeff = 4
    // Registre BMP280_CTRL_MEAS_ADDR  (0xF4)  
    // osrs_t(2:0) : osrs_p(2:0) : mode(1:0)
    // 010 : 111 : 11   = 0x5F hexa
    
    data_write[0] = BMP280_CTRL_MEAS_ADDR ;
    data_write[1] = 0b01011111  ; //0x5F
    mon_i2c.write(BMP280_ADDRESS, data_write, 2, 0);

    // Registre BMP280_REG_CONFIG  (0xF5)
    // 000 : 111 : 00   = 0x1C en hexadécimal

    data_write[0] = BMP280_REG_CONFIG ;
    data_write[1] = 0b00011100  ; //0x1C
    mon_i2c.write(BMP280_ADDRESS, data_write, 2, 0);
    
    // Laisser le temps au capteur d'effectuer quelques mesures avant de récupérer les données
    wait_ms(100) ; 
    
    //Lecture des données de calibration du capteur par paquet de 16 bits
    // 3 mots de 16 bits pour la température
    // 9 mots de 16 bits pour la pression 
    // Voir p 21 de la documentation constructeur
    // Démarre la lecture à partir de l'adresse BMP280_REG_DIG_T1 0x88
    // Jusqu'à l'adresse BMP280_REG_DIG_P9 0x9E
    // Lecture des 24 octets en une seule fois //
    // 8 bits LSB en premier dans data_read[0], 8 bits MSB en second dans data_read[1] ;
    
    data_write[0] = BMP280_REG_DIG_T1 ;
    mon_i2c.write(BMP280_ADDRESS, data_write, 1, 0);
    mon_i2c.read(BMP280_ADDRESS, data_read, 24, 0);
    
    dig_T1 = (uint16_t) ( ((uint16_t) (data_read[1] << 8) | (uint16_t) data_read[0]) ) ;
    dig_T2 = (int16_t) ( ((int16_t) (data_read[3] << 8) | (int16_t) data_read[2]) ) ;
    dig_T3 = (int16_t) ( ((int16_t) (data_read[5] << 8) | (int16_t) data_read[4]) ) ;   
    dig_P1 = (uint16_t) ( ((uint16_t) (data_read[7] << 8) | (uint16_t) data_read[6]) ) ;;
    dig_P2 = (int16_t) ( ((int16_t) (data_read[9] << 8) | (int16_t) data_read[8]) ) ;   
    dig_P3 = (int16_t) ( ((int16_t) (data_read[11] << 8) | (int16_t) data_read[10]) ) ;
    dig_P4 = (int16_t) ( ((int16_t) (data_read[13] << 8) | (int16_t) data_read[12]) ) ;
    dig_P5 = (int16_t) ( ((int16_t) (data_read[15] << 8) | (int16_t) data_read[14]) ) ;
    dig_P6 = (int16_t) ( ((int16_t) (data_read[17] << 8) | (int16_t) data_read[16]) ) ;
    dig_P7 = (int16_t) ( ((int16_t) (data_read[19] << 8) | (int16_t) data_read[18]) ) ;
    dig_P8 = (int16_t) ( ((int16_t) (data_read[21] << 8) | (int16_t) data_read[20]) ) ;
    dig_P9 = (int16_t) ( ((int16_t) (data_read[23] << 8) | (int16_t) data_read[22]) ) ;
    
    // Lecture des données de température fournies par le capteur
    int32_t temp_32bits = 0;
    data_write[0] = BMP280_REG_TEMPDATA ;
    mon_i2c.write(BMP280_ADDRESS, data_write, 1, 0);
    mon_i2c.read(BMP280_ADDRESS, data_read, 3, 0);
    temp_32bits = (int32_t) ( ((int32_t) (data_read[0] << 12) | (int32_t) (data_read[1] << 4) | (int16_t) data_read[2] >> 4) ) ;
    
    // pc.printf("Temperature en 32 bits = %d\n",temp_32bits) ;
    /* @brief This API is used to get the compensated temperature from
     * uncompensated temperature. This API uses double floating precision.
     */
    // API extraite du construteur https://github.com/BoschSensortec/BMP280_driver et modifiée pour les besoins du programme
    double var1, var2;
    
    double temperature ; // Température en double précision
    
    var1 = (((double) temp_32bits) / 16384.0 - ((double) dig_T1) / 1024.0) * ((double)dig_T2) ;
    var2 = ((((double) temp_32bits) / 131072.0 - ((double) dig_T1) / 8192.0) *
             (((double) temp_32bits) / 131072.0 - ((double) dig_T1) / 8192.0)) *
            ((double) dig_T3);
    temperature = ((var1 + var2) / 5120.0);
    
    int32_t t_fine = 0;
    t_fine = (int32_t) (var1 + var2);
    
    pc.printf("Temperature lue = %.2lf C \n",temperature) ;

    // Lecture des données de pression fournies par le capteur
    int32_t press_32bits = 0;
    data_write[0] = BMP280_REG_PRESSUREDATA ;
    mon_i2c.write(BMP280_ADDRESS, data_write, 1, 0);
    mon_i2c.read(BMP280_ADDRESS, data_read, 3, 0);
    press_32bits = (int32_t) ( ((int32_t) (data_read[0] << 12) | (int32_t) (data_read[1] << 4) | (int16_t) data_read[2] >> 4) ) ;
    
    /* @brief This API is used to get the compensated pressure from
     * uncompensated pressure. This API uses double floating precision.
     */
    // API extraite du constructeur https://github.com/BoschSensortec/BMP280_driver et modifiée pour les besoins du programme
    
    double pression ; // Pression en double précision
    var1 = ((double) t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double) dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double) dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double) dig_P4) * 65536.0);
    var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) /
            524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double) dig_P1);
    press_32bits = (uint32_t)(1048576.0 - (double) press_32bits);
    press_32bits = (uint32_t)((press_32bits - (var2 / 4096.0)) * 6250.0 / var1);
    var1 = ((double) dig_P9) * press_32bits * press_32bits / 2147483648.0;
    var2 = press_32bits * ((double) dig_P8) / 32768.0;
    pression = (press_32bits + (var1 + var2 + ((double) dig_P7)) / 16.0);

    pc.printf("Pression lue = %.2lf hPa \n",pression/100) ;
     
    while (1) {
        
    }
 
}
 
