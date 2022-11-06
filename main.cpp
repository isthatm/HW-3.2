#include "mbed.h"
#include <string.h>
  
I2C i2c_master(D14, D15); // (SDA,SCL)
I2CSlave i2c_slave(D5, D11);
static BufferedSerial serial_port(USBTX, USBRX);

#define SLAVE_ADDR 0xA0
#define GENERAL_ADDR 0x00
#define BUFFER_SIZE 2

Thread slave_thread;
Thread master_thread;

int slave(){
i2c_slave.address(SLAVE_ADDR);   
//i2c_slave.address(GENERAL_ADDR); //uncomment this line to make general write

char buf[BUFFER_SIZE];
char msg[BUFFER_SIZE];
int mode;
int value;

  while (1) {
    int receive_code = i2c_slave.receive();
    switch (receive_code) {
    //The master has requested a read from this slave.
    case I2CSlave::ReadAddressed:
      if (mode==1) { 
        value = value + 1;
        msg[0] = mode;
        msg[1] = value;
        printf("Slave Write: mode=%d, value=%d\n", mode, value);
        i2c_slave.write(msg, 2);
      } else if (mode==2) {
        value = value + 2;
        msg[0] = mode;
        msg[1] = value;
        printf("Slave Write: mode=%d, value=%d\n", mode, value);
        i2c_slave.write(msg, 2);
      } else { 
        msg[0] = mode;
        msg[1] = value;
        printf("Slave Write: mode=%d, value=%d\n", mode, value);
        i2c_slave.write(msg, 2);
      }
      break;

    //The master is writing to all slave
    case I2CSlave::WriteGeneral:
      mode = i2c_slave.read(buf, 2);
      mode = buf[0];
      value = buf[1];
      printf("Slave Read General: mode=%d, value=%d\n", mode, value);
      break;

    //[DONE] The master is writing to this slave
    case I2CSlave::WriteAddressed:
      i2c_slave.read(buf, 2);
      mode = buf[0];
      value = buf[1];
      printf("Slave Read Addressed: mode=%d, value=%d\n", mode, value);
      break;
    }
    for (int i = 0; i < 2; i++) {
      buf[i] = 0; // Clear buffer
    }
  }
}

void master(){

    char buf[BUFFER_SIZE] = {0, 10};
    char generalWrite[BUFFER_SIZE] = {8, 8};
    char resp[BUFFER_SIZE];
    char modeGeneralWrite = 7;
    char valueGeneralWrite = 10;

    // Write to a chosen slave
    printf("//WRITE TO A SPECIFIC SLAVE\n");
    for (int i = 0; i < 3; i++){
        buf[0] = i;
        int ack1 = i2c_master.write(SLAVE_ADDR, buf, BUFFER_SIZE);
        ThisThread::sleep_for(100ms);
        if (ack1) {
            printf("[master]Failed to write command to slave!\n");
        } 
        else {
            printf("[master] Written to slave: mode=%d value=%d\n", buf[0], buf[1]);
        }
        ThisThread::sleep_for(100ms);
    }

    // Read from a chosen slave
    printf("//READ FROM A SPECIFIC SLAVE\n");
    for (int i = 1; i < 3; i++){
        buf[0] = i;
        // update mode and value first
        int ack2 = i2c_master.write(SLAVE_ADDR, buf, BUFFER_SIZE);
        ThisThread::sleep_for(100ms);
        // then read from slave 
        if (i2c_master.read(SLAVE_ADDR, resp, BUFFER_SIZE)) {
        printf("[master]Failed to read from slave!\n");
        } 
        else {
        printf("[master]Read from slave: mode=%d, value=%d\n", resp[0], resp[1]);
        }
        ThisThread::sleep_for(100ms);
    }
    
    //Write to every slave
    printf("//WRITE TO EVERY SLAVE\n");
    int ack3 = i2c_master.write(GENERAL_ADDR, generalWrite, BUFFER_SIZE);
    //ThisThread::sleep_for(100ms);
    if (ack3) {
        //printf("[master]General command to slave: mode=%d value=%d,\n", generalWrite[0], generalWrite[1]);
        printf("Failed to write general command to slave!\n");
    } 
    else {
        printf("[master]General command to slave: mode=%d value=%d,\n", generalWrite[0], generalWrite[1]);
        //printf("Failed to write general command to slave!\n");
    } 
    
    /*
    printf("//WRITE TO EVERY SLAVE\n");
    int ack3 = i2c_master.write(modeGeneralWrite);
    if (ack3) {
        printf("General command to slave: %d\n", modeGeneralWrite);
    } 
    else {
        printf("Failed to write general command to slave!\n");
    }
    
    //then send value
    int ack4 = i2c_master.write(valueGeneralWrite);
    if (ack4) {
        printf("General value to slave: %d\n", valueGeneralWrite);
    } 
    else {
        printf("Failed to write general value to slave!\n");
    }
    ThisThread::sleep_for(100ms); 
    */
}

int main(){
    master_thread.start(master);
    slave_thread.start(slave);
}
  



