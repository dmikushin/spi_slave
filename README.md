# spi_slave

Implementation of slave mode SPI bus using WiringPi or WiringOp (OrangePi).

Library can be used for example to mock slave node during implementation or testing of master node.


## Limitations

* transmission of 8-bit words only,
* no driver support, so high frequencies can be an issue


## Details

This is software implementation of SPI bus. As hardware layer it uses Raspberry Pi's or Orange Pi's GPIO pins.

Module is prepared to operate in two modes: supporting and without _SS_ line. _NoSS_ mode is useful in case when there is only one slave, so _SS_ pin can be used for other purpose.


## Classes

* __PinAccess__ - dummy accessor to wires,
* __RPiGPIOAccess__ - accessor to RPi's GPIO pins,
* __SpiAbstractDevice__ - abstract representation of device,
* __SpiDevice__ - implementation of transmitter,
* __SSProtocol__ - implementation of SPI 8-bit word transmission prototcol,
* __NoSSProtocol__ - alternative 8-bit word transmission prototcol without use of SS line


### Examples

* `echo_client` - simple client receiving data from master and sending number sequences,
* `attiny85` - another implementation of client sending sequence of numbers


### References

* https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus - description of SPI bus
