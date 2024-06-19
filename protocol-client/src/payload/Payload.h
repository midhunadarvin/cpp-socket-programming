struct PAYLOAD {
	int id;
	char name[255];
};

/**
 * Packet Types for the Wire Level Protocol for the FILE transfer
 */
enum PacketType
{
    CONNECT = 1,     
    CONNECT_RESPONSE = 2,
    CHECK_STATUS = 3,
    CHECK_STATUS_RESPONSE = 4
};

struct CONNECT_PACKET {
	int packet_type;
	char action[255];
};

struct CONNECT_RESPONSE_PACKET {
	int packet_type;
	char flag; // 0 -> no, 1 -> yes
	char task_id[255];
};

struct CHECK_STATUS_PACKET {
	int packet_type;
	char task_id[255];
};

struct CHECK_STATUS_RESPONSE_PACKET {
	int packet_type;
	char status[255]; // pending , complete
	char response[255];
};
