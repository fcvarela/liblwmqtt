/*
 *  libmqtt.h
 *  libmqtt
 *
 *  Created by Filipe Varela on 09/10/16.
 *  Copyright 2009 Caixa Mágica Software. All rights reserved.
 *
 */
 
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define KEEPALIVE 15000
#define MQTTCONNECT 1<<4
#define MQTTPUBLISH 3<<4
#define MQTTSUBSCRIBE 8<<4

typedef struct {
	int socket;
	struct sockaddr_in socket_address;
	short port;
	char hostname[128];
	char clientid[33];
	int connected;
} mqtt_broker_handle_t;

typedef struct {
	mqtt_broker_handle_t *broker;
	char *msg;
} mqtt_callback_data_t;

int mqtt_connect(mqtt_broker_handle_t *broker);
int mqtt_publish(mqtt_broker_handle_t *broker, const char *topic, char *msg);
int mqtt_subscribe(mqtt_broker_handle_t *broker, const char *topic, void *(*callback)(mqtt_callback_data_t *));
