# DEFINE A MODBUS DEVICE SLAVE BY BKIT - Arm4
## Slave address 0x02
## Slave behavior
   + Read temperature(float), current of slave(float), voltage of slave(float), light intensity(uint16_t), potentiormeter(uint16_t)
   + Turn on/off led Y0, Y1, X0, X1, X2, X3 on slave device
## Register Address
   + Potentiometer register address: 0x00
   + Led register address: 0x02
   + Light intensity register address: 0x04
   + Temperature register address: 0x06
   + Current register address: 0x0A
   + Voltage register address: 0x0E
## Default baudrate 9600
   
