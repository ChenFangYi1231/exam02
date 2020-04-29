#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#define UINT14_MAX        16383
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7

I2C i2c( PTD9,PTD8);
Serial pc(USBTX, USBRX);
InterruptIn sw2(SW2);
DigitalOut led(LED3);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread q;

int m_addr = FXOS8700CQ_SLAVE_ADDR1;
float t[3];

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);

void logger()
{
    int i, j, log[100];
    float x[100], y[100], z[100], dis = 0;

    for(i = 0, j = 0; i < 100; i++, j++){
        x[i] = t[0];
        y[i] = t[1];
        z[i] = t[2];
        if(i != 0){
            if(x[i] > x[0]){
                dis += x[i] * 9.8 * 0.01 / 2;
            }
            else{
                dis -= x[i] * 9.8 * 0.01 / 2;
            }
            
        }
        if(dis >= 5){
            log[i] = 1;
        }
        else{
            log[i] = 0;
        }
        if(j == 5){
            j = 0;
            led = !led;
        }
        wait(0.1);
        
    }
    
    for(i = 0; i < 100; i++){
        pc.printf("%d\r\n", log[i]);
        pc.printf("%f\r\n", x[i]);
        pc.printf("%f\r\n", y[i]);
        pc.printf("%f\r\n", z[i]);
        wait(0.001);
    }

}

void trig_sw2()
{
    led = !led;
    queue.call(logger);

}

int main() {
    
    led = 1;
    pc.baud(115200);

    uint8_t who_am_i, data[2], res[6];
    int16_t acc16;

    // Enable the FXOS8700Q

    FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
    data[1] |= 0x01;
    data[0] = FXOS8700Q_CTRL_REG1;
    FXOS8700CQ_writeRegs(data, 2);

    // Get the slave address
    FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);

    q.start(callback(&queue, &EventQueue::dispatch_forever));
    sw2.rise(trig_sw2);
    led = 1;
    while (true) {

        FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);

        acc16 = (res[0] << 6) | (res[1] >> 2);
        if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
        t[0] = ((float)acc16) / 4096.0f;

        acc16 = (res[2] << 6) | (res[3] >> 2);
        if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
        t[1] = ((float)acc16) / 4096.0f;

        acc16 = (res[4] << 6) | (res[5] >> 2);
        if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
        t[2] = ((float)acc16) / 4096.0f;
      
   }
}

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
    char t = addr;
    i2c.write(m_addr, &t, 1, true);
    i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
    i2c.write(m_addr, (char *)data, len);
}