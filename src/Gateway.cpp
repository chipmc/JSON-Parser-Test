#include "Gateway.h"

Gateway *Gateway::_instance;

// [static]
Gateway &Gateway::instance() {
    if (!_instance) {
        _instance = new Gateway();
    }
    return *_instance;
}

Gateway::Gateway() {}

Gateway::~Gateway() {}

uint8_t buf[50];
// Format of a data report - From the Node to the Gateway so includes a token - most common message from node to gateway
/*
*** Header Section - Common to all Nodes
buf[0 - 1] magicNumber                      // Magic number that identifies the Gateway's network
buf[2] nodeNumber                           // nodeNumber - unique to each node on the gateway's network
buf[3 - 4] Token                            // Token given to the node and good for 24 hours
buf[5] sensorType                           // What sensor type is it
buf[6 - 9] uniqueID                         // This is a 4-byte identifier that is unique to each node and is only set once
*** Payload Section - 12 Bytes but interpretion is different for each sensor type
buf[10 - 17] payload                        // Payload - 8 bytes sensor type determines interpretation
*** Status Data - Common to all Nodes
buf[18] temp;                               // Enclosure temp (single byte signed integer -127 to 127 degrees C)
buf[19] battChg;                            // State of charge (single byte signed integer -1 to 100%)
buf[20] battState;                          // Battery State (single byte signed integer 0 to 6)
buf[21] resets                              // Reset count
buf[22-23] RSSI                             // From the Node's perspective
buf[24-25] SNR                              // From the Node's perspective
*** Re-Transmission Data - Common to all Nodes
buf[26] Re-Tries                            // This byte is dedicated to RHReliableDatagram.cpp to update the number of re-transmissions
buf[27] Re-Transmission Delay               // This byte is dedicated to RHReliableDatagram.cpp to update the accumulated delay with each re-transmission
*/
void Gateway::processDataReport(DataReport report) {
    
    report.getReportData(buf, sizeof(buf));
    current.set_alertCodeNode(0);			// reset data report
    current.set_nodeNumber(buf[2]);							
    current.set_token(buf[3] << 8 | buf[4]);									
    current.set_sensorType(buf[5]);												
    current.set_uniqueID(buf[6] << 24 | buf[7] << 16 | buf[8] << 8 | buf[9]);	

    // Assume the node is properly configured and ignore MagicNumber checks

    Log.info("Node %d with uniqueID %lu sent a data report message", current.get_nodeNumber(), current.get_uniqueID());

    Gateway::instance().decipherDataReport();
}

void Gateway::decipherDataReport() {
    // buf[2] - nodeNumber processed above
    // buf[3] - buf[4] is token - processed above
    // buf[5] - Sensor type - processed above
    // buf[6] - buf[9] is the unique ID of the node - processed above
    current.set_payload1(buf[10]);
    current.set_payload2(buf[11]);
    current.set_payload3(buf[12]);
    current.set_payload4(buf[13]);
    current.set_payload5(buf[14]);
    current.set_payload6(buf[15]);
    current.set_payload7(buf[16]);
    current.set_payload8(buf[17]);
    // Then, we will get the rest of the data from the report
    current.set_internalTempC(buf[18]);
    current.set_stateOfCharge(buf[19]);
    current.set_batteryState(buf[20]);
    current.set_resetCount(buf[21]);
    current.set_RSSI(buf[22] << 8 | buf[23]);
    current.set_SNR(buf[24] << 8 | buf[25]);
    current.set_retryCount(buf[26]);
    current.set_retransmissionDelay(buf[27]);
}
