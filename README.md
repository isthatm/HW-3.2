# HW-3.2: I2C protocol
This program uses I2C protocol to communicate with slave at different modes. If mode is 1, value+1 will be returned. 
And If mode is 2, value+2 will be returned. Otherwise, mode = 8 and value = 8 will be returned when master make a general
write to all slaves. To make a general write, the slave address is changed to 0x00.
