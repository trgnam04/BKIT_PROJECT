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