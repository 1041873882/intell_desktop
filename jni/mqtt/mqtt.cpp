#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mqtt.h"
#include "cJSON.h"
#include "sys.h"
#include "SysLogger.h"
#include "SysSecurity.h"

Mqtt mqtt;

volatile MQTTAsync_token deliveredtoken;

typedef void (*event_process_t)(void *payload, int length);

typedef struct {
	char topic[64];
	event_process_t proc;
} event_data_t;

#define MAX_EVENT		64
static int event_idx = 0;
static event_data_t event_data[MAX_EVENT];

static void event_setup(const char *topic, event_process_t proc)
{
	if (event_idx < MAX_EVENT) {
		strcpy(event_data[event_idx].topic, topic);
		event_data[event_idx].proc = proc;
		event_idx++;
	}
}

void onSubscribe(void* context, MQTTAsync_successData* response)
{

}

void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	printf("Subscribe failed, rc %d\n", response ? response->code : 0);
	//finished = 1;
}

//连接成功
static void on_connect(void* context, MQTTAsync_successData* response)
{
	mqtt.m_status = 1;

	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	//MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	int rc;

	printf("Successful connection\n");
	opts.onSuccess = onSubscribe;
	opts.onFailure = onSubscribeFailure;
	opts.context = client;

	deliveredtoken = 0;
	char *topics[mqtt.m_idx];
	for (int i=0; i<mqtt.m_idx; i++)
		topics[i] = mqtt.m_topic[i];

	if (mqtt.m_idx > 0) {
		if ((rc = MQTTAsync_subscribeMany(client, mqtt.m_idx, topics, mqtt.m_qos, &opts)) != MQTTASYNC_SUCCESS) {
			printf("Failed to start subscribe, return code %d\n", rc);
		}
	}
}

//连接失败
static void on_connect_failure(void* context, MQTTAsync_failureData* response)
{
	mqtt.m_status = 0;
	printf("failure connection\n");
}

//断开连接
static void connlost(void *context, char *cause)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	int rc;

	printf("\nConnection lost\n");
	printf("     cause: %s\n", cause);

	printf("Reconnecting\n");
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.onSuccess = on_connect;
	conn_opts.onFailure = on_connect_failure;
	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
		printf("Failed to start connect, return code %d\n", rc);
		//finished = 1;
	}
}

static int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
	printf("topic: %s \n", topicName);
	printf("topicLen: %d \n", topicLen);
	printf("payload: %s \n", message->payload);
	printf("payloadlen: %d \n", message->payloadlen);
	//此处收到推送进行处理

	if (topicName == NULL)
		return -1;

	for (int i=0; i<event_idx; i++) {
		if (!strcmp(event_data[i].topic, topicName))
			event_data[i].proc(message->payload, message->payloadlen);
	}
	MQTTAsync_freeMessage(&message);
	MQTTAsync_free(topicName);
	return 1;
}

static void mqtt_text(void *payload, int length)
{
	cJSON *json = NULL, *data = NULL;
	if (payload == NULL || length <= 0)
		return;

	do {
		if ((json = cJSON_Parse((const char *)payload)) == NULL)
			break;

		char *from = NULL, *to = NULL, *group = NULL, *text = NULL;

		if (cJSON_GetObjectItem(json, "group") != NULL)
			group = cJSON_GetObjectItem(json, "group")->valuestring;
		if (cJSON_GetObjectItem(json, "from") != NULL)
			from = cJSON_GetObjectItem(json, "from")->valuestring;
		if (cJSON_GetObjectItem(json, "to") != NULL)
			to = cJSON_GetObjectItem(json, "to")->valuestring;
		data = cJSON_GetObjectItem(json, "data");

		if (from == NULL || to == NULL || data == NULL)
			break;

		char id[32];
		sprintf(id, "%d%02d%02d%02d", sys.talk.build(), sys.talk.unit(), sys.talk.floor(),
				sys.talk.family());
		if (strcmp(sys.mqtt.group(), group) == 0 && (memcmp(to, id, strlen(id)) == 0 || strcmp(to, sys.net.ip()) == 0)) {
			if (cJSON_GetObjectItem(data, "text") != NULL) {
				text = cJSON_GetObjectItem(data, "text")->valuestring;
				sLogger.msg.insert(text, time(NULL));
			}
		}
	} while (0);

	if (json)
		cJSON_Delete(json);
}

static void mqtt_setting_get(void *payload, int length)
{

}

static void mqtt_setting_set(void *payload, int length)
{
	cJSON *json = NULL, *data = NULL;

	if (payload == NULL || length <= 0)
		return;

	do {
		if ((json = cJSON_Parse((const char *)payload)) == NULL)
			break;

		char *group = NULL, *from = NULL, *to = NULL;
		if (cJSON_GetObjectItem(json, "group") != NULL)
			group = cJSON_GetObjectItem(json, "group")->valuestring;
		if (cJSON_GetObjectItem(json, "from") != NULL)
			from = cJSON_GetObjectItem(json, "from")->valuestring;
		if (cJSON_GetObjectItem(json, "to") != NULL)
			to = cJSON_GetObjectItem(json, "to")->valuestring;

		if (from == NULL || to == NULL || group == NULL)
			break;

		char id[32];
		sprintf(id, "%d%02d%02d%02d%01d", sys.talk.build(), sys.talk.unit(), sys.talk.floor(),
				sys.talk.family(), sys.talk.dcode());
		if (strcmp(sys.mqtt.group(), group) == 0 && (strcmp(to, id) == 0 || strcmp(to, sys.net.ip()) == 0)) {

		}
	} while (0);

	if (json)
		cJSON_Delete(json);
}

static void mqtt_alarm_io(void *payload, int length)
{
	cJSON *json = NULL, *data = NULL;

	if (payload == NULL || length <= 0)
		return;

	do {
		if ((json = cJSON_Parse((const char *)payload)) == NULL)
			break;

		char *group = NULL, *from = NULL, *to = NULL;

		if (cJSON_GetObjectItem(json, "group") != NULL)
			group = cJSON_GetObjectItem(json, "group")->valuestring;
		if (cJSON_GetObjectItem(json, "from") != NULL)
			from = cJSON_GetObjectItem(json, "from")->valuestring;
		if (cJSON_GetObjectItem(json, "to") != NULL)
			to = cJSON_GetObjectItem(json, "to")->valuestring;
		data = cJSON_GetObjectItem(json, "data");

		if (from == NULL || to == NULL || group == NULL || data == NULL)
			break;

		char id[32];
		sprintf(id, "%d%02d%02d%02d%01d", sys.talk.build(), sys.talk.unit(), sys.talk.floor(),
				sys.talk.family(), sys.talk.dcode());
		if (strcmp(sys.mqtt.group(), group) == 0 && (strcmp(to, id) == 0 || strcmp(to, sys.net.ip()) == 0)) {
			cJSON *list = cJSON_GetObjectItem(data, "io");
			if (list == NULL)
				break;
			int count = cJSON_GetArraySize(list);
			int io[MAX_SECURITY_SZ];

			for(int i=0; i<count; i++) {
				if (cJSON_GetArrayItem(list, i) != NULL)
					io[i] = cJSON_GetArrayItem(list, i)->valueint;
			}
			sSecurity.process(io, count);
		}
	} while (0);

	if (json)
		cJSON_Delete(json);
}

static void mqtt_alarm_defence(void *payload, int length)
{
	cJSON *json = NULL, *data = NULL;

	if (payload == NULL || length <= 0)
		return;

	do {
		if ((json = cJSON_Parse((const char *)payload)) == NULL)
			break;

		char *group = NULL, *from = NULL, *to = NULL;

		if (cJSON_GetObjectItem(json, "group") != NULL)
			group = cJSON_GetObjectItem(json, "group")->valuestring;
		if (cJSON_GetObjectItem(json, "from") != NULL)
			from = cJSON_GetObjectItem(json, "from")->valuestring;
		if (cJSON_GetObjectItem(json, "to") != NULL)
			to = cJSON_GetObjectItem(json, "to")->valuestring;
		data = cJSON_GetObjectItem(json, "data");

		if (from == NULL || to == NULL || group == NULL || data == NULL)
			break;

		char id[32];
		sprintf(id, "%d%02d%02d%02d%01d", sys.talk.build(), sys.talk.unit(), sys.talk.floor(),
				sys.talk.family(), sys.talk.dcode());

		if (strcmp(sys.mqtt.group(), group) == 0 && (strcmp(to, id) == 0 || strcmp(to, sys.net.ip()) == 0)) {
			int defence = -1;
			printf("sSecurity.defence defence =%d\n", defence);
			if (cJSON_GetObjectItem(data, "defence") == NULL)
				break;
			defence = cJSON_GetObjectItem(json, "defence")->valueint;
			if (defence < 0 || defence > 3)
				break;
			sSecurity.defence(defence);
		}
	} while (0);

	if (json)
		cJSON_Delete(json);
}

Mqtt::Mqtt()
{
	m_status = 0;
	m_idx = 0;
	m_conn_opts = MQTTAsync_connectOptions_initializer;

	event_setup("text", mqtt_text);
	event_setup("setting/get", mqtt_setting_get);
	event_setup("setting/set", mqtt_setting_set);
	event_setup("alarm/io", mqtt_alarm_io);
	event_setup("alarm/defence", mqtt_alarm_defence);
}

Mqtt::~Mqtt()
{

}

int Mqtt::start()
{
	int rc;
	this->subscribe("text", MQTT_QOS2);
	this->subscribe("settings/get", MQTT_QOS2);
	this->subscribe("settings/set", MQTT_QOS2);
	this->subscribe("alarm/io", MQTT_QOS2);
	this->subscribe("alarm/defence", MQTT_QOS2);

	char addr[64];
	sprintf(addr, "tcp://%s:%d", MQTT_IP, MATT_PORT);
	MQTTAsync_create(&m_client, addr, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	MQTTAsync_setCallbacks(m_client, m_client, connlost, msgarrvd, NULL);

	m_conn_opts.keepAliveInterval = 20;
	m_conn_opts.cleansession = 1;
	m_conn_opts.onSuccess = on_connect;
	m_conn_opts.onFailure = on_connect_failure;
	m_conn_opts.context = m_client;

	if ((rc = MQTTAsync_connect(m_client, &m_conn_opts)) != MQTTASYNC_SUCCESS) {
		printf("Failed to start connect, return code %d\n", rc);
		return -1;
	}
	return 0;
}

void Mqtt::stop()
{
	MQTTAsync_destroy(&m_client);
}

int Mqtt::subscribe(const char *topic, int qos)
{
	int ret = -1;

	if (topic != NULL && (qos >= 0 && qos <= 2) && m_idx < SUBSCRIBE_TOPIC_MAX && strlen(topic) < TOPIC_MAX_LEN) {
		memcpy(m_topic[m_idx], topic, strlen(topic));
		m_qos[m_idx] = qos;
		m_idx++;
		ret = 0;
	}
	return ret;
}

int Mqtt::publish(const char *topic, int qos, const char *payload)
{
	int rc;
	if (m_status == 0 || topic == NULL || payload == NULL)
		return -1;

	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	pubmsg.payload = (void *)payload;
	pubmsg.payloadlen = strlen(payload);
	pubmsg.qos = qos;
	pubmsg.retained = 0;
	deliveredtoken = 0;

	if ((rc = MQTTAsync_sendMessage(m_client, topic, &pubmsg, &opts)) != MQTTASYNC_SUCCESS) {
		printf("Failed to start sendMessage, return code %d\n", rc);
		return -1;
	}
	return 0;
}

int Mqtt::alarm(int zone, int d)
{
	cJSON *json = cJSON_CreateObject();
	cJSON *data = cJSON_CreateObject();
	char id[32];

	sprintf(id, "%d%02d%02d%02d", sys.talk.build(), sys.talk.unit(), sys.talk.floor(),
			sys.talk.family());

	cJSON_AddStringToObject(json, "group", sys.mqtt.group());
	cJSON_AddStringToObject(json, "from", id);
	cJSON_AddStringToObject(json, "to", "");

	cJSON_AddNumberToObject(data, "zone", zone);
	cJSON_AddNumberToObject(data, "d", d);
	cJSON_AddItemToObject(json, "data", data);

	char *ss = cJSON_Print(json);
	if (ss) {
		this->publish("alarm/record/alarm", MQTT_QOS2, ss);
		free(ss);
	}
	cJSON_Delete(json);
	return 0;
}

int Mqtt::defence(int defence)
{
	cJSON *json = cJSON_CreateObject();
	cJSON *data = cJSON_CreateObject();
	char id[32];

	sprintf(id, "%d%02d%02d%02d", sys.talk.build(), sys.talk.unit(), sys.talk.floor(),
			sys.talk.family());
	cJSON_AddStringToObject(json, "group", sys.mqtt.group());
	cJSON_AddStringToObject(json, "from", id);
	cJSON_AddStringToObject(json, "to", "");

	cJSON_AddNumberToObject(data, "defence", defence);
	cJSON_AddItemToObject(json, "data", data);

	char *ss = cJSON_Print(json);
	if (ss) {
		this->publish("alarm/record/defence", MQTT_QOS2, ss);
		free(ss);
	}
	cJSON_Delete(json);
	return 0;
}

int Mqtt::talk_logger(const char *id, int type, int timeout, time_t ts)
{
	cJSON *json = cJSON_CreateObject();
	cJSON *data = cJSON_CreateObject();
	char tmp[32];

	sprintf(tmp, "%d%02d%02d%02d", sys.talk.build(), sys.talk.unit(), sys.talk.floor(),
			sys.talk.family());

	cJSON_AddStringToObject(json, "group", sys.mqtt.group());
	cJSON_AddStringToObject(json, "from", tmp);
	cJSON_AddStringToObject(json, "to", "");

	cJSON_AddStringToObject(data, "id", id);
	cJSON_AddNumberToObject(data, "type", type);
	cJSON_AddNumberToObject(data, "timeout", timeout);
	cJSON_AddNumberToObject(data, "ts", ts);
	cJSON_AddItemToObject(json, "data", data);

	char *ss = cJSON_Print(json);
	if (ss) {
		this->publish("talk/record", MQTT_QOS2, ss);
		free(ss);
	}
	cJSON_Delete(json);
	return 0;
}
