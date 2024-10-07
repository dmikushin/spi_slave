class SpiAbstractDevice {
public:
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual int loadBitToSend() = 0;
    virtual void storeReceivedBit(int bit) = 0;
};

class SpiDevice : public SpiAbstractDevice {
public:
    SpiDevice() : receiveBuffer(0), sendBuffer(0), loadBitIndex(0) {}

    virtual void prepareData() {
        // override
    }

    virtual void dataReceived() {
        // override
    }

    void activate() override {
        receiveBuffer = 0;
        loadBitIndex = 7;
        prepareData();
    }

    void deactivate() override {
        sendBuffer = 0;
        dataReceived();
    }

    int loadBitToSend() override {
        int bitMask = (1 << loadBitIndex);
        int value = sendBuffer & bitMask;
        int bit = int(value != 0);
        loadBitIndex -= 1;
        if (loadBitIndex < 0) {
            loadBitIndex = 7;
            sendBuffer = sendBuffer >> 8;
        }
        return bit;
    }

    void storeReceivedBit(int bit) override {
        receiveBuffer = (receiveBuffer << 1) | (bit & 1);
    }

protected:
    int receiveBuffer;
    int sendBuffer;
    int loadBitIndex;
};
