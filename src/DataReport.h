#ifndef DATA_REPORT_H
#define DATA_REPORT_H

#include <Arduino.h>

/** 
 * Data Report object
 * 
 * Description: Mocks up a LoRa Data report, following the format of a SeeInsights-LoRa-Node
 *
 */
class DataReport {
public:
    uint8_t buf[28];

    // Constructor with default initialization to 0
    DataReport(uint32_t uniqueID);

    // Setter methods for each member variable
    void setMagicNumber(uint16_t magicNumber);
    void setNodeNumber(uint8_t nodeNumber);
    void setToken(uint16_t token);
    void setSensorType(uint8_t sensorType);
    void setOccupancyGross(uint16_t occupancyGross);
    void setOccupancyNet(uint16_t occupancyNet);
    void setSpace(uint8_t space);
    void setPlacement(uint8_t placement);
    void setMulti(uint8_t multi);
    void setZoneMode(uint8_t zoneMode);
    void setInternalTempC(uint8_t internalTempC);
    void setStateOfCharge(uint8_t stateOfCharge);
    void setBatteryState(uint8_t batteryState);
    void setResetCount(uint8_t resetCount);
    void setRSSI(uint16_t RSSI);
    void setSNR(uint16_t SNR);
    void setRetransmissions(uint8_t retransmissions);
    void setRetransmissionDelays(uint8_t retransmissionDelays);

    // Function to populate a passed-in buffer with the data
    bool getReportData(uint8_t* buf, size_t size) const;

private:
    uint16_t magicNumber;
    uint8_t nodeNumber;
    uint16_t token;
    uint8_t sensorType;
    uint32_t uniqueID;
    uint16_t occupancyGross;
    uint16_t occupancyNet;
    uint8_t space;
    uint8_t placement;
    uint8_t multi;
    uint8_t zoneMode;
    uint8_t internalTempC;
    uint8_t stateOfCharge;
    uint8_t batteryState;
    uint8_t resetCount;
    uint16_t RSSI;
    uint16_t SNR;
    uint8_t retransmissions;
    uint8_t retransmissionDelays;
};

#endif // DATA_REPORT_H
