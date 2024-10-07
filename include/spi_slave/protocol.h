#include "pin_access.h"
#include "spi_device.h"

class SSProtocol {
public:
    SSProtocol(PinAccess* dataAccessor = nullptr, SpiAbstractDevice* spiDevice = nullptr) {
        if (dataAccessor == nullptr) {
            dataAccess = new PinAccess();
        } else {
            dataAccess = dataAccessor;
        }

        if (spiDevice == nullptr) {
            device = new SpiAbstractDevice();
        } else {
            device = spiDevice;
        }

        dataAccess->registerForSCK(this, &SSProtocol::clock_tick);
        dataAccess->registerForSS(this, &SSProtocol::slave_tick);
        clockCouter = 0;
        selected = false;
    }

    void sendBit() {
        if (clockCouter <= 0) {
            return;
        }
        int bit = device->loadBitToSend();
        dataAccess->writeDO(bit);
    }

    void receiveBit() {
        int bit = dataAccess->readDI();
        device->storeReceivedBit(bit);
    }

    void clock_tick(int pinSCK) {
        if (selected == false) {
            return;
        }
        if (pinSCK == 0) {
            sendBit();
        } else {
            receiveBit();
        }
        clockCouter -= 1;
    }

    void slave_tick(int pinSS) {
        if (pinSS == 0) {
            clockCouter = 15;
            selected = true;
            device->activate();
            sendBit();
        } else {
            clockCouter = 0;
            selected = false;
            device->deactivate();
        }
    }

private:
    PinAccess* dataAccess;
    SpiAbstractDevice* device;
    int clockCouter;
    bool selected;
};
