#include <argp.h>
#include <iostream>
#include <stdexcept>
#include <queue>
#include <thread>
#include <chrono>
#include <bitset>
#include <argp.h>

#include "spi_slave/rpi_gpio_access.h"
#include "spi_slave/spi_device.h"

class EchoSlaveNoSS : public SpiDevice {
public:
    EchoSlaveNoSS() : SpiDevice(), dataAccess(RPiGPIOAccess::getInstance()) {
        dataAccess.start();
        // Wait for sequence 0b10101010
    }
    void prepareData() override {
        std::cout << "echo prepareData: " << sendBuffer << "[" << std::bitset<10>(sendBuffer) << "]" << std::endl;
    }
    void dataReceived() override {
        std::cout << "echo received: " << receiveBuffer << "[" << std::bitset<10>(receiveBuffer) << "]" << std::endl;
        if (receiveBuffer == 170) {
            sendBuffer = 55;
        } else {
            sendBuffer = 0;
        }
    }
    void run() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
private:
    RPiGPIOAccess &dataAccess;
};

class EchoSlave : public SpiDevice {
public:
    EchoSlave() : SpiDevice(), dataAccess(RPiGPIOAccess::getInstance()), counter(0) {
        dataAccess.start();
    }
    void prepareData() override {
        counter += 1;
        sendBuffer = counter;
        std::cout << "echo prepareData: " << sendBuffer << "[" << std::bitset<10>(sendBuffer) << "]" << std::endl;
    }
    void dataReceived() override {
        std::cout << "echo received: " << receiveBuffer << "[" << std::bitset<10>(receiveBuffer) << "]" << std::endl;
    }
    void run() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
private:
    RPiGPIOAccess &dataAccess;
    int counter;
};

const char *argp_program_version = "program 1.0";
const char *argp_program_bug_address = "<bug@yourprogram.com>";

static char doc[] = "Your program description";
static char args_doc[] = "[FILENAME...]";

static struct argp_option options[] = {
    {"noss", 'n', 0, 0, "No SS echo version - always activated"},
    {0}
};

struct arguments {
    int noss;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    auto *arguments = reinterpret_cast<struct arguments*>(state->input);

    switch (key) {
        case 'n':
            arguments->noss = 1;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
    struct arguments arguments;
    arguments.noss = 0;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    try {
        std::cout << "Waiting for master" << std::endl;
        if (arguments.noss) {
            EchoSlaveNoSS echo;
            echo.run();
        } else {
            EchoSlave echo;
            echo.run();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

