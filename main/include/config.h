#pragma once
#include <string>
using namespace std;

typedef struct device_config
{
    string name; // This is device id.
    char *SUBSCRIBE_TOPIC;
    char *PUBLISH_TOPIC;

} device_config;

device_config deviceConfig;
