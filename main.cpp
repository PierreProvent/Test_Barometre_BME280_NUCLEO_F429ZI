#include "mbed.h"
#include "BME280.h"

BME280 mon_BME280(PB_9,PB_8);
        // PB_9 : I2C broche SDA platine Grove
        // PB_8 : I2C broche SCL platine Grove
Serial pc(SERIAL_TX, SERIAL_RX);

int main()
{
    double temp, press, hum, alt ;
    pc.printf("\033[2J"); // Effacer la console TeraTerm
    pc.printf("\033[0;0H"); // Curseur en 0,0
    pc.printf("Temperature,pression, humidite Capteur BME280 Bosch Sensortec\n");
/*    data_write[0] = BME280_REG_CHIPID ;
    int status = mon_i2c.write(BME280_ADDRESS, data_write, 1);
    if (status == 0) // Si  capteur  présent acknowledgement = 0 sur bus I2C
        pc.printf("Capteur BME280 trouve sur bus I2C\n");
    else
        pc.printf("Capteur BME280 non trouve sur bus I2C\n");

    pc.printf("Lecture de l'identificateur de la puce BME280\n");
    mon_i2c.read(BME280_ADDRESS, data_read, 1);
    if (data_read[0] == BME280_CHIP_ID)
        pc.printf("ID barometre correct =  %#x \n",data_read[0]);
    else
        pc.printf("ID barometre incorrect =  %#x \n",data_read[0]); */

    while (1) {
        temp = mon_BME280.temperature() ;
        pc.printf("Temperature %.2lf C\n",temp) ;
        press = mon_BME280.pression() ;
        pc.printf("Pression %.3lf hPa\n",press/100) ;
        double A = press/101325;
        double B = 1 / 5.25588;
        alt = pow(A, B);
        alt = 1.0 - alt;
        alt = alt/0.0000225577;       
        pc.printf("Altitude %.2lf m\n",alt) ;
        hum = mon_BME280.humidite() ;
        pc.printf("Humidite %.2lf %%\n",hum) ;
        pc.printf("\033[4A");
        wait(1);
    }
}
