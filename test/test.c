#include <stdio.h>
#include "libmqtt.h"

int main(int argc, char **argv) {
	mqtt_broker_handle_t broker;
	
	broker.port = 1883;
	strcpy(broker.hostname, "127.0.0.1");
	strcpy(broker.clientid, "client1");

//	int result = mqtt_connect(&broker);
//	printf("Connect: %d\n", result);

	//result = mqtt_subscribe(&broker, "hello/world", NULL);
	//printf("Subscribe: %d\n", result);
	//return 0;
	int i;
	for (i=0; i<40000; i++) {
		if (i % 1000 == 0)
			printf("Sent %d messages\n", i);
		mqtt_publish(&broker, "hello/world", "client1 present");
	}
	return 0;
}

