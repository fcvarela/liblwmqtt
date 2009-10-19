/*
 *  libmqtt.c
 *  libmqtt
 *
 *  Created by Filipe Varela on 09/10/16.
 *  Copyright 2009 Caixa Mágica Software. All rights reserved.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "libmqtt.h"

int mqtt_connect(mqtt_broker_handle_t *broker)
{
	if (strlen(broker->clientid) > 127)
		return -1;
	
	if ((broker->socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
	
	// create the stuff we need to connect
	broker->connected = 0;
	broker->socket_address.sin_family = AF_INET;
	broker->socket_address.sin_port = htons(broker->port);
	broker->socket_address.sin_addr.s_addr = inet_addr(broker->hostname);
	
	// connect
	if ((connect(broker->socket, (struct sockaddr *)&broker->socket_address, sizeof(broker->socket_address))) < 0)
		return -1;
	
	// variable header
	uint8_t var_header[] = {0x00,0x06,0x4d,0x51,0x49,0x73,0x64,0x70,0x03,0x02,0x00,KEEPALIVE/500,0x00,strlen(broker->clientid)};
	
	// fixed header: 2 bytes, big endian
	uint8_t fixed_header[] = {MQTTCONNECT,12+strlen(broker->clientid)+2};
	
	uint8_t packet[sizeof(fixed_header)+sizeof(var_header)+strlen(broker->clientid)];
	memset(packet,0,sizeof(packet));
	memcpy(packet,fixed_header,sizeof(fixed_header));
	memcpy(packet+sizeof(fixed_header),var_header,sizeof(var_header));
	memcpy(packet+sizeof(fixed_header)+sizeof(var_header),broker->clientid,strlen(broker->clientid));
		
	if (send(broker->socket, packet, sizeof(packet), 0) < sizeof(packet)) {
		close(broker->socket);
		return -1;
	}
	
	// set connected flag
	broker->connected = 1;
	
	return 1;
}

int mqtt_publish(mqtt_broker_handle_t *broker, const char *topic, char *msg)
{
	if (!broker->connected) {
		if (mqtt_connect(broker) < 0)
			return -1;
	}
	
	uint8_t var_header[strlen(topic)+3];
	strcpy((char *)&var_header[2], topic);
	var_header[0] = 0;
	var_header[1] = strlen(topic);
	var_header[sizeof(var_header)-1] = 0;
	
	uint8_t fixed_header[] = {MQTTPUBLISH,sizeof(var_header)+strlen(msg)};
	
	uint8_t packet[sizeof(fixed_header)+sizeof(var_header)+strlen(msg)];
	memset(packet,0,sizeof(packet));
	memcpy(packet,fixed_header,sizeof(fixed_header));
	memcpy(packet+sizeof(fixed_header),var_header,sizeof(var_header));
	memcpy(packet+sizeof(fixed_header)+sizeof(var_header),msg,strlen(msg));

	if (send(broker->socket, packet, sizeof(packet), 0) < sizeof(packet))
		return -1;
		
	return 1;
}

int mqtt_subscribe(mqtt_broker_handle_t *broker, const char *topic, void *(*callback)(mqtt_callback_data_t *))
{
	if (!broker->connected) {
		if (mqtt_connect(broker) < 0)
			return -1;
	}
	
	uint8_t var_header[] = {0,10};	
	uint8_t fixed_header[] = {MQTTSUBSCRIBE,sizeof(var_header)+strlen(topic)+3};
	
	// utf topic
	uint8_t utf_topic[strlen(topic)+3];
	strcpy((char *)&utf_topic[2], topic);

	utf_topic[0] = 0;
	utf_topic[1] = strlen(topic);
	utf_topic[sizeof(utf_topic)-1] = 0;
	
	char packet[sizeof(var_header)+sizeof(fixed_header)+strlen(topic)+3];
	memset(packet,0,sizeof(packet));
	memcpy(packet,fixed_header,sizeof(fixed_header));
	memcpy(packet+sizeof(fixed_header),var_header,sizeof(var_header));
	memcpy(packet+sizeof(fixed_header)+sizeof(var_header),utf_topic,sizeof(utf_topic));
	
	if (send(broker->socket, packet, sizeof(packet), 0) < sizeof(packet))
		return -1;
		
	return 1;
}