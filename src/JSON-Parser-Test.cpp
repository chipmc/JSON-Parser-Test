#include "Particle.h"
#include "JsonParserGeneratorRK.h"

SerialLogHandler LogHandler;

bool runTest();
byte findNodeNumber(const char* deviceID);
void printNodeData();
bool changetype(int nodeNumber, int Newtype);
byte getType(int nodeNumber);
void printTokens(JsonParser &jp);
void printToken(JsonParser &jp, const JsonParserGeneratorRK::jsmntok_t *tok);

// Variables to build structure
String deviceID_1 = "e00fce6894585f1e783cd368";
int sensorType_1 = 3;

SYSTEM_MODE(MANUAL);

JsonParserStatic<1024, 50> jp;
String s;

const char * const data = "{\"nodes\":[{\"node\":1,\"dID\":\"aaaaaaaaaaaaaaaaaaaaa1\",\"last\":1667835489,\"type\":1}]}";   // Start with an empty JSON object

void setup() {
  delay(2000);                        // Give the serial port time to connect

  Log.info("Starting tests");

  jp.addString(data);

  jp.parse();

  findNodeNumber(deviceID_1);

  printTokens(jp);

  /*

  if (runTest()) Log.info("Passed!");
  else Log.info("Failed");

  char deviceToFind[25] = "aaaaaaaaaaaaaaaaaaaaa1";
  byte nodeNumber = findNodeNumber(deviceToFind);
  if (nodeNumber > 0) Log.info("DeviceID found in node %d", nodeNumber );
  else Log.info("DeviceID not found");

  printNodeData();

  char deviceToFind2[25] = "aaaaaaaaaaaaaaaaaaaaa3";
  byte nodeNumber2 = findNodeNumber(deviceToFind2);
  if (nodeNumber2 > 0) Log.info("DeviceID found in node %d", nodeNumber2 );
  else Log.info("DeviceID not found");

  */

  printNodeData();

  /*

  changetype(3,5);

  printNodeData();

  Log.info("%s\n", jp.getBuffer());

  char data2[1024];

  strncpy(data2, jp.getBuffer(), jp.getBufferLen());

  Log.info(data2);

  jp.clear();

  jp.addString(data2); // Load from new string and retest

   if (runTest()) Log.info("Passed!");
  else Log.info("Failed");

  Log.info("Sensor Type for node 1 is: %d",getType(1));
  */

 Log.info("Finished test");

}

void loop() {}

bool runTest() {

	bool bResult = jp.parse();
	assert(bResult);

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;

	bResult = jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	assert(bResult);

	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;

	nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, 0);
	assert(nodeObjectContainer != NULL);

	int nodeNumber;
	String deviceID;
	int lastConnect;
	int type;

	bResult = jp.getValueByKey(nodeObjectContainer, "node", nodeNumber);
	assert(bResult);
	assert(nodeNumber == 1);

	bResult = jp.getValueByKey(nodeObjectContainer, "dID", deviceID);
	assert(bResult);
	assert(deviceID == "aaaaaaaaaaaaaaaaaaaaa1");

	bResult = jp.getValueByKey(nodeObjectContainer, "last", lastConnect);
	assert(bResult);
	assert(lastConnect == 1668008455);

	bResult = jp.getValueByKey(nodeObjectContainer, "type", type);
	assert(bResult);
	assert(type == 1);

	nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, 1);
	assert(nodeObjectContainer != NULL);

	bResult = jp.getValueByKey(nodeObjectContainer, "node", nodeNumber);
	assert(bResult);
	assert(nodeNumber == 2);

	bResult = jp.getValueByKey(nodeObjectContainer, "dID", deviceID);
	assert(bResult);
	assert(deviceID == "aaaaaaaaaaaaaaaaaaaaa2");

	bResult = jp.getValueByKey(nodeObjectContainer, "last", lastConnect);
	assert(bResult);
	assert(lastConnect == 1667836600);

	bResult = jp.getValueByKey(nodeObjectContainer, "type", type);
	assert(bResult);
	assert(type == 2);

/*
	nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, 2);
	assert(nodeObjectContainer == NULL);

	*/

  return true;

}

byte findNodeNumber(const char* deviceID) {
	String nodeDeviceID;
	int node;
	int index = 1;				// Start at one since node zero is the gateway

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	for (int i=0; i<10; i++) {												// Iterate through the array looking for a match
		nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, i);
		if(nodeObjectContainer == NULL) break;								// Ran out of entries - no match found
		jp.getValueByKey(nodeObjectContainer, "dID", nodeDeviceID);	// Get the deviceID and compare
		if (nodeDeviceID == deviceID) {
			jp.getValueByKey(nodeObjectContainer, "nodeNumber", node);// A match!
			return node;
		}
		index++;															// This will be the node number for the next node if no match is found
	}

	JsonModifier mod(jp);

	Log.info("Did not find the deviceID, new node of %d",index);

	mod.startAppend(jp.getOuterArray());
		mod.startObject();
		mod.insertKeyValue("node", (int)index);
		mod.insertKeyValue("dID", deviceID_1);
		mod.insertKeyValue("last", Time.now());
		mod.insertKeyValue("type", (int)sensorType_1);
		mod.finishObjectOrArray();
	mod.finish();

	return index;
}

String findDeviceID(uint8_t node) {
	String nodeDeviceID;

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, node-1);
	if(nodeObjectContainer == NULL) return "null";							// Ran out of entries - no match found
	jp.getValueByKey(nodeObjectContainer, "dID", nodeDeviceID);	// Get the deviceID and compare
	if (nodeDeviceID == NULL) return "null";

	return nodeDeviceID;
}

void printNodeData() {

	int nodeNumber;
	String nodeDeviceID;
	int lastConnect;
	int type;
	char data[128];

	Log.info("Node Database currently configured:");

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	for (int i=0; i<10; i++) {												// Iterate through the array looking for a match
		nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, i);
		if(nodeObjectContainer == NULL) {
			Log.info("Ran out of entries with i = %d",i);
			break;								// Ran out of entries
		}
		jp.getValueByKey(nodeObjectContainer, "dID", nodeDeviceID);
		jp.getValueByKey(nodeObjectContainer, "node", nodeNumber);
		jp.getValueByKey(nodeObjectContainer, "last", lastConnect);
		jp.getValueByKey(nodeObjectContainer, "type", type);

		snprintf(data, sizeof(data), "Node %d, deviceID: %s, lastConnected: %s, type %d", nodeNumber, nodeDeviceID.c_str(),Time.timeStr(lastConnect).c_str(), type);
		Log.info(data);
		if (Particle.connected()) Particle.publish("nodeData", data, PRIVATE);
	}

}


bool changetype(int nodeNumber, int Newtype) {

	int type;

	const JsonParserGeneratorRK::jsmntok_t *nodesArrayContainer;			// Token for the outer array
	jp.getValueTokenByKey(jp.getOuterObject(), "nodes", nodesArrayContainer);
	const JsonParserGeneratorRK::jsmntok_t *nodeObjectContainer;			// Token for the objects in the array (I beleive)

	nodeObjectContainer = jp.getTokenByIndex(nodesArrayContainer, nodeNumber-1);
	if(nodeObjectContainer == NULL) return false;								// Ran out of entries

	jp.getValueByKey(nodeObjectContainer, "type", type);

	Log.info("Changing sensor type from %d to %d", type, Newtype);

	const JsonParserGeneratorRK::jsmntok_t *value;

	jp.getValueTokenByKey(nodeObjectContainer, "type", value);

	JsonModifier mod(jp);

	mod.startModify(value);

	mod.insertValue((int)Newtype);
	mod.finish();

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

// Function to dump the token table. Used while debugging the JsonModify code.
void printTokens(JsonParser &jp) {
	Log.info("printing tokens");
	JsonParserGeneratorRK::jsmntok_t *tokensEnd = jp.getTokensEnd();

	for(JsonParserGeneratorRK::jsmntok_t *tok = jp.getTokens(); tok < tokensEnd; tok++) {
		printToken(jp, tok);
	}

}

void printToken(JsonParser &jp, const JsonParserGeneratorRK::jsmntok_t *tok) {
	char tempBuf[1024];

	const char *typeName = "UNKNOWN";
	switch(tok->type) {
	case JsonParserGeneratorRK::JSMN_UNDEFINED:
		typeName = "UNDEFINED";
		break;

	case JsonParserGeneratorRK::JSMN_OBJECT:
		typeName = "OBJECT";
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

	memcpy(tempBuf, jp.getBuffer() + tok->start, tok->end - tok->start);
	tempBuf[tok->end - tok->start] = 0;

	Log.info("type=%s start=%d end=%d size=%d %s\n", typeName, tok->start, tok->end, tok->size, tempBuf);
}
