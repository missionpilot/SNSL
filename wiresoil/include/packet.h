#define REQ_OUT _LATB7
#define CONFIG_REQ_OUT() CONFIG_RB7_AS_DIG_OD_OUTPUT()

extern char nodeId[3];    //node id

extern void getNodeId(void);
void sendPacketChar(char b);

#define PACKET_START 0x1E

#define APP_STRING_DATA 0x03
#define APP_SMALL_DATA 0x04
#define APP_BIG_DATA 0x05
#define APP_DATA_PACKET 0x06

#define MONITOR_REQUEST_DATA_STATUS  0x33
#define MONITOR_SEND_DATA_PACKET 0x35


