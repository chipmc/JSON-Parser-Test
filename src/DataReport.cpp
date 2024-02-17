#include "DataReport.h"

// Constructor with default initialization to 0
DataReport::DataReport(uint32_t uniqueID) : 
                            magicNumber(0), 
                            nodeNumber(0), 
                            token(12345), 
                            sensorType(0),
                            uniqueID(uniqueID), 
                            occupancyGross(0), 
                            occupancyNet(0),
                            space(0), 
                            placement(0),
                            multi(0), 
                            zoneMode(0),
                            internalTempC(0), 
                            stateOfCharge(0), 
                            batteryState(0),
                            resetCount(0), 
                            RSSI(0), 
                            SNR(0),
                            retransmissions(0), 
                            retransmissionDelays(0) {}

// Setters
void DataReport::setMagicNumber(uint16_t magicNumber) {
    this->magicNumber = magicNumber;
}

void DataReport::setNodeNumber(uint8_t nodeNumber) {
    this->nodeNumber = nodeNumber;
}

void DataReport::setToken(uint16_t token) {
    this->token = token;
}

void DataReport::setSensorType(uint8_t sensorType) {
    this->sensorType = sensorType;
}

void DataReport::setOccupancyGross(uint16_t occupancyGross) {
    this->occupancyGross = occupancyGross;
}

void DataReport::setOccupancyNet(uint16_t occupancyNet) {
    this->occupancyNet = occupancyNet;
}

void DataReport::setSpace(uint8_t space) {
    this->space = space;
}

void DataReport::setPlacement(uint8_t placement) {
    this->placement = placement;
}

void DataReport::setMulti(uint8_t multi) {
    this->multi = multi;
}

void DataReport::setZoneMode(uint8_t zoneMode) {
    this->zoneMode = zoneMode;
}

void DataReport::setInternalTempC(uint8_t internalTempC) {
    this->internalTempC = internalTempC;
}

void DataReport::setStateOfCharge(uint8_t stateOfCharge) {
    this->stateOfCharge = stateOfCharge;
}

void DataReport::setBatteryState(uint8_t batteryState) {
    this->batteryState = batteryState;
}

void DataReport::setResetCount(uint8_t resetCount) {
    this->resetCount = resetCount;
}

void DataReport::setRSSI(uint16_t RSSI) {
    this->RSSI = RSSI;
}

void DataReport::setSNR(uint16_t SNR) {
    this->SNR = SNR;
}

void DataReport::setRetransmissions(uint8_t retransmissions) {
    this->retransmissions = retransmissions;
}

void DataReport::setRetransmissionDelays(uint8_t retransmissionDelays) {
    this->retransmissionDelays = retransmissionDelays;
}

// Function to populate a passed-in buffer with the data
bool DataReport::getReportData(uint8_t* buf, size_t size) const {
    if (size < 28) {
        return false;
    }
    buf[0] = highByte(magicNumber);
    buf[1] = lowByte(magicNumber);
    buf[2] = nodeNumber;
    buf[3] = highByte(token);
    buf[4] = lowByte(token);
    buf[5] = sensorType;
    buf[6] = uniqueID >> 24;
    buf[7] = uniqueID >> 16;
    buf[8] = uniqueID >> 8;
    buf[9] = uniqueID;
    buf[10] = highByte(occupancyGross);
    buf[11] = lowByte(occupancyGross);
    buf[12] = highByte(occupancyNet);
    buf[13] = lowByte(occupancyNet);
    buf[14] = space;
    buf[15] = placement;
    buf[16] = multi;
    buf[17] = zoneMode;
    buf[18] = internalTempC;
    buf[19] = stateOfCharge;
    buf[20] = batteryState;
    buf[21] = resetCount;
    buf[22] = highByte(RSSI);
    buf[23] = lowByte(RSSI);
    buf[24] = highByte(SNR);
    buf[25] = lowByte(SNR);
    buf[26] = retransmissions;
    buf[27] = retransmissionDelays;

    return true;
}