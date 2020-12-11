#ifndef __MQTT_H__
#define __MQTT_H__

#include "MQTTAsync.h"

#define MQTT_IP "192.168.11.83"
#define MATT_PORT 1883
#define CLIENTID    "ExampleClientSub"
#define SUBSCRIBE_TOPIC_MAX 64
#define TOPIC_MAX_LEN 64

enum {
	MQTT_QOS0 = 0,
	MQTT_QOS1,
	MQTT_QOS2
};

class Mqtt {
public:
	Mqtt();
	~Mqtt();
	int start();
	void stop();

	int subscribe(const char *topic, int qos);
	int publish(const char *topic, int qos, const char *payload);

	int alarm(int zone, int d);
	int defence(int defence);
	int talk_logger(const char *id, int type, int timeout, time_t ts);
public:
	int m_status;

	MQTTAsync m_client;
	MQTTAsync_connectOptions m_conn_opts;
	MQTTAsync_message m_pubmsg;
	MQTTAsync_token m_token;

	char m_topic[SUBSCRIBE_TOPIC_MAX][TOPIC_MAX_LEN];
	int m_qos[SUBSCRIBE_TOPIC_MAX];
	int m_idx;
};

extern Mqtt mqtt;

#endif
