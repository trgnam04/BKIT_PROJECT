<<<<<<< HEAD
DEFINE A MODBUS DEVICE SLAVE BY BKIT - Arm4
 Slave behavior:
   + Read temperature, light intensity, potentiormeter, 
 Register Address:



## DATA PROCESSING
### Read Temperature, Current, Voltage
Datatype: Float
Number of byte: 4
Example return frame: `<slave address> <0x03> <4> <byte 1> <byte 2> <byte 3> <byte 4> <CRC Low> <CRC High>`
### Read Potentiormeter, Light
Datatype: Unsigned Char
Number of byte: 2
Example return frame: `<slave address> <0x03> <2> <byte 1> <byte 2> <CRC Low> <CRC High>`
### Write Led
Register Address: 0x0002
Example transmit frame: `<slave address> <0x06> <register address> <byte 1> <byte 2> <CRC Low> <CRC High>`
=======
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
   
>>>>>>> a3245a3540a295e973c986202bbcf63410e736b6
