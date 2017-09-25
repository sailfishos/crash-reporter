#ifndef QNETWORKCONFIGURATION_H
#define QNETWORKCONFIGURATION_H

// Stub for QNetworkConfiguration
class QNetworkConfiguration
{
public:
    enum BearerType {
        BearerUnknown,
        BearerEthernet,
        BearerWLAN,
        Bearer2G,
        BearerCDMA2000,
        BearerWCDMA,
        BearerHSPA,
        BearerBluetooth,
        BearerWiMAX
    };

    enum StateFlags {
        Undefined        = 0x0000001,
        Defined          = 0x0000002,
        Discovered       = 0x0000006,
        Active           = 0x000000e
    };

    BearerType bearerType() const;

    StateFlags state() const;
};

#endif  // QNETWORKCONFIGURATION_H
