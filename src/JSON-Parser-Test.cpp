#include "Particle.h"
#include "MyPersistentData.h"
#include "DataReport.h"
#include "Gateway.h"
#include "JsonParserGeneratorRK.h"
#include <cstdlib> // For rand() and srand()

SerialLogHandler LogHandler;

byte findNodeNumber(uint32_t uniqueID);					// This will create a new node number if the uniqueID is not found
int printNodeData(bool publish);
bool parseJoinPayloadValues(uint8_t sensorType, uint8_t compressedJoinPayload, uint8_t& payload1, uint8_t& payload2, uint8_t& payload3, uint8_t& payload4);
uint8_t compressData(uint8_t data[], uint8_t bitSizes[]);
void decompressData(uint8_t compressedData, uint8_t data[], uint8_t bitSizes[]);
bool setType(int nodeNumber, int newType);
byte getType(int nodeNumber);
void printTokens(JsonParser &jp, bool verbose = false);
void printToken(JsonParser &jp, const JsonParserGeneratorRK::jsmntok_t *tok);
bool setJsonData1(int nodeNumber, int sensorType, int newJsonData1);
bool setJsonData2(int nodeNumber, int sensorType, int newJsonData2);

bool saveNeeded = false;											// Use this to rate limit saves to the nodeDatabase
unsigned long lastSave = 0;											// Use this to rate limit saves to the nodeDatabase
unsigned long saveFrequency = 1000;									// Use this to rate limit saves to the nodeDatabase

// Variables to build structure
uint32_t uniqueID_1 = 2613470560;
uint32_t uniqueID_2 = 2121360342;
int sensorType_1 = 3;
int sensorType_2 = 4;
int sensorType_3 = 10;
int numberOfNodes = 0;


SYSTEM_MODE(MANUAL);

JsonParserStatic<3072, 550> jp;
String s;

/* NodeID JSON structure
{nodes:[
	{
		{node:(int)nodeNumber},
		{uID: (uint32_t)uniqueID},
		{type: (int)sensorType},
		{p: (int)compressedPayload},
		{p1: (int)payload1},						//  Added two additional payload fields - for Sensor type 10-19
		{p2: (int)payload2},
		{pend: (int)pendingAlerts},
		{cont: (int)pendingAlertContext}
	},
	....]
}
*/


/*
Sample Data Set - 
Raquetball 1 : 2613470559 ["1","false","false"]
Raquetball 2 : 2121360342 ["2","false","false"]

Spin Studio : 2113381891 ["3","false","false"] 

Gym High : 2222090124 ["4","true","true"]
Gym Low : 2839639610 ["4","false","true"]

Hallway (to pool): 95839962 ["5","true","false"]

Men's Locker Room Inside : 3818678341 ["6","false","true"]
Men's Locker Room Outside : 2824039299 ["6","false","true"]

Women's Locker Room Inside : 2561435892 ["7","false","true"]
Women's Locker Room Outside : 3633933507 ["7","false","true"]

Staircase : 2647744414 ["8","false","true"]

Group Excercise Hallway : 3662503554 ["8","true","true"]
Group Exercise Corner : 2585746525 ["8","false","true"]

Stretch Area : 660218114 ["9","true","true"]

*/

const char * const data = "{\"nodes\":[{\"node\":1,\"uID\":2613470559,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":1},{\"node\":2,\"uID\":2121360342,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":2},{\"node\":3,\"uID\":2113381891,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":3},{\"node\":4,\"uID\":2222090124,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":4},{\"node\":5,\"uID\":2839639610,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":5},{\"node\":6,\"uID\":95839962,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":6},{\"node\":7,\"uID\":3818678341,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":7},{\"node\":8,\"uID\":2824039299,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":8},{\"node\":9,\"uID\":2561435892,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":9},{\"node\":10,\"uID\":3633933507,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":10},{\"node\":11,\"uID\":2647744414,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":11},{\"node\":12,\"uID\":3662503554,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":12},{\"node\":13,\"uID\":2585746525,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":13},{\"node\":14,\"uID\":660218114,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":14}]}"; 
//const char * const data = "{\"nodes\":[{\"node\":1,\"uID\":2613470559,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":1},{\"node\":2,\"uID\":2121360342,\"type\":1,\"p\":0,\"p1\":0,\"p2\":0,\"pend\":0,\"cont\":2}]}"; 

// Array to store the unique IDs. We will loop through these in our data report test.
const uint32_t uniqueIDs[] = {
    2613470559,
    2121360342,
    2113381891,
    2222090124,
    2839639610,
    95839962,
    3818678341,
    2824039299,
    2561435892,
    3633933507,
    2647744414,
    3662503554,
    2585746525,
    660218114
};
const size_t uniqueIDsCount = sizeof(uniqueIDs) / sizeof(uniqueIDs[0]); // calculate the size for the loop boundary

void setup() {

	delay(2000);                        // Give the serial port time to connect

	nodeIDData::instance().setup();		// Initialize the nodeDatabase

	Log.info("Starting tests");

	Log.info("First Test, we will clear the Node Database and load the sample data set");

	nodeDatabase.resetNodeIDs();	// Clear the Node Database

	printTokens(jp, false);

	Log.info("Now that the nodeDatabase is empty, we will load the sample data set");
	
	jp.addString(data);

	if (jp.parse()) Log.info("Parsed Successfully");
	else {
		nodeDatabase.resetNodeIDs();
		Log.info("Parsing error");
	}

	printTokens(jp, true);

	Log.info("Next, we will load and store the node database into memory");

	nodeDatabase.set_nodeIDJson(data);				// Load the text object from this sketch into the nodeDatabase

	nodeDatabase.flush(false);						// Store the nodeDatabase into memory

	Log.info("Note that our JSON object allocation is %4.2f%% of the memory allocated (less than 100%% is OK)", 100*(float)jp.getBufferLen()/(float)nodeDatabase.nodeDataStorageSize());

	jp.clear();										// Clear the JSON object from memory

	// Here is where we load the JSON object from memory and parse
	jp.addString(nodeDatabase.get_nodeIDJson());	// Read in the JSON string from memory
	Log.info("The node string is: %s",nodeDatabase.get_nodeIDJson().c_str());

	if (jp.parse()) Log.info("Parsed Successfully");
	else {
		nodeDatabase.resetNodeIDs();
		Log.info("Parsing error");
	}

	printTokens(jp, false);

	Log.info("Finally, we will add a new node to the database and print the database");

	byte nodeNumber = findNodeNumber(uniqueID_1);

	Log.info("The node number is: %d",nodeNumber);

	printTokens(jp, false);

	printNodeData(false);

	Log.info("Now we will change the type of the node and print the database");

	nodeNumber = findNodeNumber(uniqueID_1);

	byte currentType = getType(nodeNumber);

	Log.info("The current type for node number %d is: %d",nodeNumber, currentType);

	setType(nodeNumber, sensorType_2);

	currentType = getType(nodeNumber);

	Log.info("The new type for node number %d is: %d",nodeNumber, currentType);

	Log.info("Now we will change the type which will alter its structure and print the database");

	setType(nodeNumber, sensorType_3);

	printTokens(jp, false);

	printNodeData(false);

	Log.info("Now we will change the type on a note that is in the middle of the pack and then change it back. ");

	nodeNumber = findNodeNumber(uniqueID_2);

	currentType = getType(nodeNumber);

	Log.info("The current type for node number %d is: %d",nodeNumber, currentType);

	setType(nodeNumber, sensorType_2);

	printNodeData(false);

	currentType = getType(nodeNumber);

	Log.info("The new type for node number %d is: %d",nodeNumber, currentType);

	setType(nodeNumber, sensorType_3);

	currentType = getType(nodeNumber);

	Log.info("The new type for node number %d is: %d",nodeNumber, currentType);

	printTokens(jp, false);

	numberOfNodes = printNodeData(false);

	Log.info("Notice how the new node is now at the end of the outer ojbect - this will break our code");

	Log.info("Finished Parsing Test. Running data report tests in a loop.");

}

void loop() {

	delay(2000);
	
    static int numLoops = 0;

    int currentIndex = numLoops % numberOfNodes;

	int nodeNumber = currentIndex + 1;

    // Get the current time using Particle's Time.now()
    unsigned long currentTime = Time.now();
    // Seed a random number generator with the current time
    std::srand(currentTime);
	// Create two random numbers of digits 1-4
	int num1 = std::rand() % 4 + 1;
	int num2 = std::rand() % 4 + 1;
    // Create occupancyGross and occupancyNet with num1 and num2 amount of characters
    int occupancyGross = 0;
    int occupancyNet = 0;
	// Create two random numbers with the respective amount of digits
    for (int i = 0; i < num1; ++i) {
        occupancyGross = occupancyGross * 10 + std::rand() % 10;
    }
    for (int i = 0; i < num2; ++i) {
        occupancyNet = occupancyNet * 10 + std::rand() % 10;
    }

    // Initialize a new DataReport object with the next uniqueID in the list
    DataReport report = DataReport(uniqueIDs[currentIndex]); 
    report.setNodeNumber(currentIndex + 1); // The nodeNumber is equal to the index + 1 for all uniqueIDs
    report.setSensorType(10); // sensorType is 10 for occupancy nodes
    report.setOccupancyNet(occupancyNet); // The data report will have a random value for occupancyNet
    report.setOccupancyGross(occupancyGross); // The data report will have a random value for occupancyGross
    // Set other parameters here if you wish to test them ... cont might be good to test. 

    Gateway::instance().processDataReport(report); // Send the data report to the gateway to populate MyPersistentData

    // Update the JSON database with the data that is set after the report. This simulates the way the JSON is updated over time through data reports.
    bool result1 = setJsonData1(currentIndex + 1, 10, current.get_payload3() <<8 | current.get_payload4());
    bool result2 = setJsonData2(currentIndex + 1, 10, current.get_payload1() <<8 | current.get_payload2());

    if(!(result1 && result2)){
        Log.info("Failed to set Json Data!! Tokens:");
    }

   	// Print all of the tokens to see if there are any cases where the string becomes unparsible
   	printTokens(jp, false);

	// Print the last 32 characters of the node string to see if there are any issues with JSON structure that could 
	// cause a failure when converting to and from a string to an object.
	const char* jsonString = nodeDatabase.get_nodeIDJson();
	const char* last32Characters = jsonString + strlen(jsonString) - 32;
	Log.info("Last 32 characters of node string: %s", last32Characters); 
    numLoops++;

	if (saveNeeded && (millis() - lastSave > saveFrequency)) {
		nodeDatabase.flush(false);
		saveNeeded = false;
		lastSave = millis();
		Log.info("Saved the nodeDatabase");
	}
}

byte findNodeNumber(uint32_t uniqueID) {
	uint32_t nodeUniqueID;
	int node = 0;
	int index = 1;				// Start at one since node zero is the gateway

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	for (int i=0; i<50; i++) {												// Iterate through the array looking for a match
		nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, i);
		if(nodeObjectContainer == NULL) break;								// Ran out of entries - no match found
		jp.getValueByKey(nodeObjectContainer, "uID", nodeUniqueID);			// Get the uniqueID and compare
		if (nodeUniqueID == uniqueID) {
			jp.getValueByKey(nodeObjectContainer, "node", node);		// A match!
			Log.info("Found the uniqueID, node of %d",node);
			return node;
		}
		index++;															// This will be the node number for the next node if no match is found
	}

	JsonModifier mod(jp);

	Log.info("Did not find the uniqueID, new node of %d",index);

	mod.startAppend(jp.getOuterArray());
		mod.startObject();
		mod.insertKeyValue("node", (int)index);
		mod.insertKeyValue("uID", (uint32_t)uniqueID);
		mod.insertKeyValue("type", (int)sensorType_1);
		mod.insertKeyValue("p", 0);
		mod.insertKeyValue("p1", 0);
		mod.insertKeyValue("p2", 0);
		mod.insertKeyValue("pend", 0);	
		mod.insertKeyValue("cont", 0);
		mod.finishObjectOrArray();
	mod.finish();

	saveNeeded = true;

	return index;
}

uint32_t findUniqueID(uint8_t node) {
	uint32_t nodeUniqueID;

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, node-1);
	if(nodeObjectContainer == NULL) return 0;							// Ran out of entries - no match found
	if ((jp.getValueByKey(nodeObjectContainer, "uID", nodeUniqueID))) return nodeUniqueID;
	else return 0;	// Get the deviceID and compare
}

bool setType(int nodeNumber, int newType) {
	if (nodeNumber == 0 || nodeNumber == 255) return false;
	int type;
	uint32_t uniqueID;


	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, nodeNumber-1);
	if(nodeObjectContainer == NULL) { 
		Log.info("Ran out of entries");
		return false;								// Ran out of entries 
	}

	JsonModifier mod(jp);

	jp.getValueByKey(nodeObjectContainer, "uID", uniqueID);
	jp.getValueByKey(nodeObjectContainer, "type", type);

	Log.info("Changing sensor type from %d to %d", type, newType);

	const JsonParserGeneratorRK::jsmntok_t *value;

	jp.getValueTokenByKey(nodeObjectContainer, "type", value);
	mod.startModify(value);
	mod.insertValue((int)newType);

	jp.getValueTokenByKey(nodeObjectContainer, "p", value);
	mod.startModify(value);
	mod.insertValue((int)0);

	jp.getValueTokenByKey(nodeObjectContainer, "p1", value);
	mod.startModify(value);
	mod.insertValue((int)0);

	jp.getValueTokenByKey(nodeObjectContainer, "p2", value);
	mod.startModify(value);
	mod.insertValue((int)0);

	jp.getValueTokenByKey(nodeObjectContainer, "pend", value);
	mod.startModify(value);
	mod.insertValue((int)0);

	jp.getValueTokenByKey(nodeObjectContainer, "cont", value);
	mod.startModify(value);
	mod.insertValue((int)0);

	mod.finish();

	saveNeeded = true;

	return true;
}


byte getType(int nodeNumber) {

	int type;

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, nodeNumber-1);
	if(nodeObjectContainer == NULL) return false;								// Ran out of entries

	jp.getValueByKey(nodeObjectContainer, "type", type);

	return type;

}

int printNodeData(bool publish) {
	int nodeNumber;
	uint32_t uniqueID;
	int sensorType;
	uint8_t  payload1;
	uint8_t  payload2;
	uint8_t  payload3;
	uint8_t  payload4;
	int compressedJoinPayload;
	int uncompressedJoinPayload1;
	int uncompressedJoinPayload2;	
	int pendingAlertCode;
	int pendingAlertContext;
	char data[622];  // max size

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	for (int i=0; i<100; i++) {												// Iterate through the array looking for a match
		nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, i);
		if(nodeObjectContainer == NULL) {
			break;								// Ran out of entries 
		} 
		jp.getValueByKey(nodeObjectContainer, "uID", uniqueID);
		jp.getValueByKey(nodeObjectContainer, "node", nodeNumber);
		jp.getValueByKey(nodeObjectContainer, "type", sensorType);
		jp.getValueByKey(nodeObjectContainer, "p", compressedJoinPayload);
		jp.getValueByKey(nodeObjectContainer, "p1", uncompressedJoinPayload1);
		jp.getValueByKey(nodeObjectContainer, "p2", uncompressedJoinPayload2);
		jp.getValueByKey(nodeObjectContainer, "pend", pendingAlertCode);
		jp.getValueByKey(nodeObjectContainer, "cont", pendingAlertContext);

		parseJoinPayloadValues(sensorType, compressedJoinPayload, payload1, payload2, payload3, payload4);


	//  ***************************  This could likely be simplified ***************************
	//  ***************************  This could likely be simplified ***************************
	//  ***************************  This could likely be simplified ***************************

		// Type specific JSON variables
		switch (sensorType) {
			case 1 ... 9: {    						// Counter
				snprintf(data, sizeof(data), "Node %d, uniqueID %lu, type %d, payload (%d/%d/%d/%d) with pending alert %d and alert context %d", nodeNumber, uniqueID, sensorType, payload1, payload2, payload3, payload4, pendingAlertCode, pendingAlertContext);
			} break;
			case 10 ... 19: {   					// Occupancy
				int occupancyNet;
				int occupancyGross;
				jp.getValueByKey(nodeObjectContainer, "p1", occupancyNet);
				jp.getValueByKey(nodeObjectContainer, "p2", occupancyGross);
				snprintf(data, sizeof(data), "Node %d, uniqueID %lu, type %d, occupancyNet %d, occupancyGross %d, payload (%d/%d/%d/%d) with pending alert %d and alert context %d", nodeNumber, uniqueID, sensorType, occupancyNet, occupancyGross, payload1, payload2, payload3, payload4, pendingAlertCode, pendingAlertContext);
			} break;
			case 20 ... 29: {   					// Sensor
				snprintf(data, sizeof(data), "Node %d, uniqueID %lu, type %d, payload (%d/%d/%d/%d) with pending alert %d and alert context %d", nodeNumber, uniqueID, sensorType, payload1, payload2, payload3, payload4, pendingAlertCode, pendingAlertContext);
			} break;
			default: {          		
				Log.info("Unknown sensor type in printNodeData %d", sensorType);
				if (Particle.connected()) Particle.publish("Alert", "Unknown sensor type in printNodeData", PRIVATE);
			} break;
    	}

		Log.info(data);
		if (Particle.connected() && publish) {
			Particle.publish("nodeData", data, PRIVATE);
			delay(1000);
		}
	}
	Log.info(nodeDatabase.get_nodeIDJson());  // See the raw JSON string

	return nodeNumber;
}

// Function to dump the token table. Used while debugging the JsonModify code.
void printTokens(JsonParser &jp, bool verbose) {
	int storageSize = 0;
	int tokenCount = 0;
	char tempBuf[3076];

	JsonParserGeneratorRK::jsmntok_t *tokensEnd = jp.getTokensEnd();

	// The first token is the outer object - here we get the total size of the object
	JsonParserGeneratorRK::jsmntok_t *tok = jp.getTokens();
	memcpy(tempBuf, jp.getBuffer() + tok->start, tok->end - tok->start);
	tempBuf[tok->end - tok->start] = 0;
	storageSize += tok->end;
	if (verbose) Log.info("Outer object start=%d end=%d tokens=%d - %s", tok->start, tok->end, tok->size, tempBuf);

	for(JsonParserGeneratorRK::jsmntok_t *tok = jp.getTokens(); tok < tokensEnd; tok++) {
		if (tok->start > 0) {
			tokenCount += tok->size;
		}
		if (verbose) {
			printToken(jp, tok);
		}
	}

	Log.info("Total tokens=%d (%4.2f%% full) storage=%d (%4.2f%% full)", tokenCount, (100*((float)tokenCount/(float)jp.getMaxTokens())), storageSize, (100*((float)storageSize/(float)jp.getBufferLen())));
}

void printToken(JsonParser &jp, const JsonParserGeneratorRK::jsmntok_t *tok) {
	char tempBuf[4096];

	const char *typeName = "UNKNOWN";
	switch(tok->type) {
	case JsonParserGeneratorRK::JSMN_UNDEFINED:
		typeName = "UNDEFINED";
		break;

	case JsonParserGeneratorRK::JSMN_OBJECT:
		typeName = "OBJECT";
		memcpy(tempBuf, jp.getBuffer() + tok->start, tok->end - tok->start);
		tempBuf[tok->end - tok->start] = 0;
		Log.info("type=%s start=%d end=%d tokens=%d - %s", typeName, tok->start, tok->end, tok->size, tempBuf);
		break;

	case JsonParserGeneratorRK::JSMN_ARRAY:
		typeName = "ARRAY";
		break;

	case JsonParserGeneratorRK::JSMN_STRING:
		typeName = "STRING";
		break;

	case JsonParserGeneratorRK::JSMN_PRIMITIVE:
		typeName = "PRIMITIVE";
		break;
	}

}

bool parseJoinPayloadValues(uint8_t sensorType, uint8_t compressedJoinPayload, uint8_t& payload1, uint8_t& payload2, uint8_t& payload3, uint8_t& payload4) {
    uint8_t data[4] = {0};
    uint8_t bitSizes[4] = {0};

    switch (sensorType) {
        case 1 ... 9: {    						// Counter
            bitSizes[0] = 1; // 2-Way (1 bit)
        } break;
        case 10 ... 19: {   					// Occupancy
            bitSizes[0] = 6; // space (6 bits)
            bitSizes[1] = 1; // placement (1 bit)
            bitSizes[2] = 1; // multi (1 bit)
        } break;
        case 20 ... 29: {   					// Sensor
            bitSizes[0] = 6; // space (6 bits)
            bitSizes[1] = 1; // placement (1 bit)
        } break;
        default: {
            Log.info("Unknown sensor type in parseJoinPayloadValues %d", sensorType);
            if (Particle.connected()) Particle.publish("Alert", "Unknown sensor type in parseJoinPayloadValues", PRIVATE);
            return false;
        } break;
    }

    decompressData(compressedJoinPayload, data, bitSizes);

    payload1 = data[0];
    payload2 = data[1];
    payload3 = data[2];
    payload4 = data[3];

    return true;
}

uint8_t compressData(uint8_t data[], uint8_t bitSizes[]) {
    uint8_t compressedData = 0;
    uint8_t bitOffset = 0;
    for (uint8_t i = 0; i < 4; ++i) {
        data[i] = (data[i] < (1 << bitSizes[i])) ? data[i] : ((1 << bitSizes[i]) - 1);
        compressedData |= (data[i] * (1 << bitOffset));
        bitOffset += bitSizes[i];
    }
    return compressedData;
}

void decompressData(uint8_t compressedData, uint8_t data[], uint8_t bitSizes[]) {
    uint8_t bitOffset = 0;
    for (uint8_t i = 0; i < 4; ++i) {
        data[i] = (compressedData >> bitOffset) & ((1 << bitSizes[i]) - 1);
        bitOffset += bitSizes[i];
    }
}

bool setJsonData1(int nodeNumber, int sensorType, int newJsonData1) {
	if (nodeNumber == 0 || nodeNumber == 255) return false;
	if (sensorType > 29) return false; 					// Return false if node is not a valid sensor type
	int jsonData1;

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, nodeNumber-1);
	if(nodeObjectContainer == NULL) return false;						    // Ran out of entries 

	jp.getValueByKey(nodeObjectContainer, "p1", jsonData1);

	Log.info("Updating jsonData1 value for node %d from %d to %d", nodeNumber, jsonData1, newJsonData1);

	const JsonParserGeneratorRK::jsmntok_t *value;

	jp.getValueTokenByKey(nodeObjectContainer, "p1", value);

	JsonModifier mod(jp);

	mod.startModify(value);
	mod.insertValue((int)newJsonData1);
	mod.finish();

	nodeDatabase.set_nodeIDJson(jp.getBuffer());									// This should backup the nodeID database - now updated to persistent storage

	saveNeeded = true;

	return true;
}

bool setJsonData2(int nodeNumber, int sensorType, int newJsonData2) {
	if (nodeNumber == 0 || nodeNumber == 255) return false;
	if (sensorType < 10 || sensorType > 19) return false; 					// Return false if node is not an occupancy counter

	int jsonData2;

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, nodeNumber-1);
	if(nodeObjectContainer == NULL) return false;								// Ran out of entries 

	jp.getValueByKey(nodeObjectContainer, "p2", jsonData2);

	Log.info("Updating jsonData2 value for node %d from %d to %d", nodeNumber, jsonData2, newJsonData2);

	const JsonParserGeneratorRK::jsmntok_t *value;

	jp.getValueTokenByKey(nodeObjectContainer, "p2", value);

	JsonModifier mod(jp);

	mod.startModify(value);
	mod.insertValue((int)newJsonData2);
	mod.finish();

	nodeDatabase.set_nodeIDJson(jp.getBuffer());									// This should backup the nodeID database - now updated to persistent storage

	saveNeeded = true;

	return true;
}