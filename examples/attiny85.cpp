//
// Pinout ATtiny85 -> RPi:
//      SCK (pin7)    -> SCLK-GPIO11 (pin23) - IN mode
//      DO  (pin6)    -> MISO-GPIO9  (pin21) - IN mode
//      DI  (pin5)    -> M0SI-GPIO10 (pin19) - OUT mode
//
//      DO/DI area crossed with MISO/M0SI to keep compatibility with MCU programmer
//

#include <iostream>
#include <wiringPi.h>
#include <queue>
#include <exception>

#define PIN_SCK 11 // SCLK-GPIO11 (pin23)
#define PIN_DI   9 // MISO-GPIO9  (pin21)
#define PIN_DO  10 // MOSI-GPIO10 (pin19)
#define PIN_SS  22 // GPIO12      (pin15)

enum SlaveState {
    IDLE = 1,
    SENDING_VALUE = 2,
    RECEIVING_ECHO = 3
};

std::queue<std::exception> exceptionQueue;

class SpiSlave {
public:
    static SpiSlave& getInstance() {
        static SpiSlave instance;
        return instance;
    }

    void receivedByte() {
        if (state == IDLE) {
            if (receiveBuffer == 0b10101010) {
                sendBuffer = counter;
                std::cout << "echo command -- sending echo request: " << sendBuffer << std::endl;
                state = SENDING_VALUE;
                slaveSelected();
            }
        } else if (state == SENDING_VALUE) {
            std::cout << "sending completed -- receiving echo" << std::endl;
            state = RECEIVING_ECHO;
        } else if (state == RECEIVING_ECHO) {
            std::cout << "echo received: " << receiveBuffer << std::endl;
            state = IDLE;
            counter++;
        }
    }

    void sendBit() {
        if (state != SENDING_VALUE) return;
        sendBuffer = (sendBuffer << 1);
        int bit = (sendBuffer & 0x100) != 0 ? 1 : 0;
        digitalWrite(PIN_DO, bit);
    }

    void receiveBit() {
        pinDI = digitalRead(PIN_DI);
        receiveBuffer = (receiveBuffer << 1) & 0xFF | pinDI;
    }

    void clock_tick() {
        try {
            pinSCK = digitalRead(PIN_SCK);
            if (unknownState) {
                handleUnknownState();
                return;
            }

            if (pinSCK == 0) {
                sendBit();
            } else {
                receiveBit();
            }

            if (edgeCounter < 15) {
                edgeCounter++;
                return;
            }

            receivedByte();
            edgeCounter = 0;
            receiveBuffer = 0;
        } catch (std::exception& e) {
            exceptionQueue.push(e);
        }
    }

    void handleUnknownState() {
        if (pinSCK == 1) {
            receiveBit();
            return;
        }

        if (receiveBuffer == 0b10101010) {
            unknownState = false;
            edgeCounter = 15;
            receivedByte();
            edgeCounter = 0;
            receiveBuffer = 0;
        } else {
            std::cout << "unknown state: " << receiveBuffer << std::endl;
        }
    }

    void slaveSelected() {
        sendBit();
    }

    void slave_tick() {
        try {
            int pinSS = digitalRead(PIN_SS);
            if (pinSS == 1) return;
            slaveSelected();
        } catch (std::exception& e) {
            exceptionQueue.push(e);
        }
    }

private:
    SpiSlave() : unknownState(true), state(IDLE), receiveBuffer(0), edgeCounter(0), sendBuffer(0), counter(0), pinSCK(0), pinDI(0) {}
    SpiSlave(SpiSlave const&) = delete;
    void operator=(SpiSlave const&) = delete;
    
    bool unknownState;
    SlaveState state;
    int receiveBuffer;
    int edgeCounter;
    int sendBuffer;
    int counter;
    int pinSCK;
    int pinDI;
};

static void clock_tick() { SpiSlave::getInstance().clock_tick(); }
static void slave_tick() { SpiSlave::getInstance().slave_tick(); }

int main() {
    wiringPiSetupGpio();

    pinMode(PIN_SCK, INPUT);
    pullUpDnControl(PIN_SCK, PUD_DOWN);
    pinMode(PIN_DI, INPUT);
    pullUpDnControl(PIN_DI, PUD_DOWN);
    pinMode(PIN_DO, OUTPUT);
    pinMode(PIN_SS, INPUT);
    pullUpDnControl(PIN_SS, PUD_DOWN);

    wiringPiISR(PIN_SCK, INT_EDGE_BOTH, &clock_tick);
    wiringPiISR(PIN_SS, INT_EDGE_BOTH, &slave_tick);

    while (true) {
        try {
            std::exception e = exceptionQueue.front();
            exceptionQueue.pop();
            std::cout << "Exception: " << e.what() << std::endl;
            return 1;
        } catch (std::exception& e) {
            // timeout
        }
    }

    return 0;
}

