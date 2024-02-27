#include <iostream>
#include <esp_log.h>
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include <string.h>

using namespace std;

const char *PUBLISH_TAG = "PUBLISH";

//* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Publish Bool ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void aws_publish_bool(AWS_IoT_Client *pClient, const char *PUBLISH_TOPIC, const char *key, bool value)
{
    IoT_Publish_Message_Params params;
    char cPayload[100];

    params.qos = QOS1;
    params.payload = (void *)cPayload;
    params.isRetained = 0;

    if (aws_iot_mqtt_is_client_connected(pClient))
    {
        // Message to publish
        sprintf(cPayload, "{\"%s\" : %d}", key, value);
        params.payloadLen = strlen(cPayload);
        IoT_Error_t rc = aws_iot_mqtt_publish(pClient, PUBLISH_TOPIC, strlen(PUBLISH_TOPIC), &params);
        if (rc == MQTT_REQUEST_TIMEOUT_ERROR)
        {
            ESP_LOGW(PUBLISH_TAG, "QOS1 publish ack not received.");
            rc = SUCCESS;
        }
    }
}

void aws_publish_int(AWS_IoT_Client *pClient, const char *PUBLISH_TOPIC, const char *key, int value)
{
    IoT_Publish_Message_Params params;
    char cPayload[100];

    params.qos = QOS1;
    params.payload = (void *)cPayload;
    params.isRetained = 0;

    if (aws_iot_mqtt_is_client_connected(pClient))
    {
        // Message to publish
        sprintf(cPayload, "{\"%s\" : %d}", key, value);
        params.payloadLen = strlen(cPayload);
        IoT_Error_t rc = aws_iot_mqtt_publish(pClient, PUBLISH_TOPIC, strlen(PUBLISH_TOPIC), &params);
        if (rc == MQTT_REQUEST_TIMEOUT_ERROR)
        {
            ESP_LOGW(PUBLISH_TAG, "QOS1 publish ack not received.");
            rc = SUCCESS;
        }
    }
}

void aws_publish_string(AWS_IoT_Client *pClient, const char *PUBLISH_TOPIC, const char *key, const char *value)
{
    IoT_Publish_Message_Params params;
    char cPayload[100];

    params.qos = QOS1;
    params.payload = (void *)cPayload;
    params.isRetained = 0;

    if (aws_iot_mqtt_is_client_connected(pClient))
    {
        // Message to publish
        sprintf(cPayload, "{\"%s\" : %s}", key, value);
        params.payloadLen = strlen(cPayload);
        IoT_Error_t rc = aws_iot_mqtt_publish(pClient, PUBLISH_TOPIC, strlen(PUBLISH_TOPIC), &params);
        if (rc == MQTT_REQUEST_TIMEOUT_ERROR)
        {
            ESP_LOGW(PUBLISH_TAG, "QOS1 publish ack not received.");
            rc = SUCCESS;
        }
    }
}