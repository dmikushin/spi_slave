#include "spi_slave/pin_access.h"

#include <wiringPi.h>

#define PIN_SCK 11 // SCLK-GPIO11 (pin23)
#define PIN_DI   9 // MISO-GPIO9  (pin21)
#define PIN_DO  10 // MOSI-GPIO10 (pin19)
#define PIN_SS  27 // GPIO2       (pin13)

class RPiGPIOAccess : public PinAccess {
public:
    static RPiGPIOAccess& getInstance() {
        static RPiGPIOAccess instance;
        return instance;
    }

    void start() override {
        enabled = true;
    }

    int readDI() override {
        return digitalRead(PIN_DI);
    }

    void writeDO(int value) override {
        digitalWrite(PIN_DO, value);
    }

    void registerForSCK(void (*callback)(int)) override {
        sckCallback = callback;
    }

    void registerForSS(void (*callback)(int)) override {
        ssCallback = callback;
    }

    static void clock_tick() {
        RPiGPIOAccess& instance = getInstance();
        if (!instance.enabled) return;
        if (instance.sckCallback == nullptr) return;
        int pinSCK = digitalRead(PIN_SCK);
        instance.sckCallback(pinSCK);
    }

    static void slave_tick() {
        RPiGPIOAccess& instance = getInstance();
        if (!instance.enabled) return;
        if (instance.ssCallback == nullptr) return;
        int pinSS = digitalRead(PIN_SS);
        instance.ssCallback(pinSS);
    }

    ~RPiGPIOAccess() {
        // Cleanup all GPIO
        digitalWrite(PIN_DO, 0); // Turn off PIN_DO
        pinMode(PIN_DO, INPUT); // Set PIN_DO back to input
    }

private:
    RPiGPIOAccess() : sckCallback(nullptr), ssCallback(nullptr), enabled(false) {
        wiringPiSetupGpio(); // Use Broadcom GPIO numbering
        pinMode(PIN_SCK, INPUT);
        pullUpDnControl(PIN_SCK, PUD_DOWN);
        pinMode(PIN_DI, INPUT);
        pullUpDnControl(PIN_DI, PUD_DOWN);
        pinMode(PIN_DO, OUTPUT);
        pinMode(PIN_SS, INPUT);
        pullUpDnControl(PIN_SS, PUD_DOWN);
        wiringPiISR(PIN_SCK, INT_EDGE_BOTH, &RPiGPIOAccess::clock_tick);
        wiringPiISR(PIN_SS, INT_EDGE_BOTH, &RPiGPIOAccess::slave_tick);
    }

    void (*sckCallback)(int);
    void (*ssCallback)(int);
    bool enabled;

    RPiGPIOAccess(RPiGPIOAccess const&) = delete;
    void operator=(RPiGPIOAccess const&) = delete;
};

