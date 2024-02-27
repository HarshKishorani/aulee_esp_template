#include <stdlib.h>
#include <stdint.h>
#include <sys/param.h>
#include "cJSON.h"
#include <inttypes.h>
#include "include/app_wifi.h"
#include <mdns.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_local_ctrl.h>
#include <esp_https_server.h>
#include "config.h"

static const char *LOCAL_CTRL_TAG = "local_control";

/* Custom allowed property types */
enum property_types
{
    PROP_TYPE_NODE_CONFIG = 1,
    PROP_TYPE_NODE_PARAMS,
};

/* Custom flags that can be set for a property */
enum property_flags
{
    PROP_FLAG_READONLY = (1 << 0)
};
/**************************** Config and Params functions ************************************/

/// @brief Get All Details about the Device.
/// @return 
char *get_aulee_node_config()
{
    cJSON *json = cJSON_CreateObject();

    // Add Properties
    cJSON_AddStringToObject(json, "deviceId", deviceConfig.name.c_str());
    cJSON_AddStringToObject(json, "model", "ESP32");

    // convert the cJSON object to a JSON string
    char *json_str = cJSON_Print(json);
    cJSON_Delete(json);
    return json_str;
}

/// @brief Get All Details about all the parameters.
/// @return 
char *get_aulee_node_params()
{
    cJSON *json = cJSON_CreateObject();

    /* 
        TODO : Get Params to be sent to Mobile Device.
    */

    // convert the cJSON object to a JSON string
    char *json_str = cJSON_Print(json);
    cJSON_Delete(json);
    return json_str;
}

/// @brief Update params based on instructions received from Device using Local Control.
/// @param data 
/// @param size 
/// @return 
esp_err_t set_param_values(char *data, size_t size)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI(LOCAL_CTRL_TAG, "Received from Local Control : %s", data);
    /*
        TODO : Set Params Received from Mobile Device.
    */
    return ret;
}

/********* Handler functions for responding to control requests / commands *********/

static esp_err_t get_property_values(size_t props_count,
                                     const esp_local_ctrl_prop_t props[],
                                     esp_local_ctrl_prop_val_t prop_values[],
                                     void *usr_ctx)
{
    esp_err_t ret = ESP_OK;
    uint32_t i;
    for (i = 0; i < props_count; i++)
    {
        ESP_LOGI(LOCAL_CTRL_TAG, "Reading property : %s", props[i].name);
        switch (props[i].type)
        {
        case PROP_TYPE_NODE_CONFIG:
        {

            char *node_config = get_aulee_node_config();
            if (!node_config)
            {
                ESP_LOGE(LOCAL_CTRL_TAG, "Failed to allocate memory for %s", props[i].name);
                ret = ESP_ERR_NO_MEM;
            }
            else
            {
                prop_values[i].size = strlen(node_config);
                prop_values[i].data = node_config;
                prop_values[i].free_fn = free;
            }
            break;
        }
        case PROP_TYPE_NODE_PARAMS:
        {

            char *node_params = get_aulee_node_params();
            if (!node_params)
            {
                ESP_LOGE(LOCAL_CTRL_TAG, "Failed to allocate memory for %s", props[i].name);
                ret = ESP_ERR_NO_MEM;
            }
            else
            {
                prop_values[i].size = strlen(node_params);
                prop_values[i].data = node_params;
                prop_values[i].free_fn = free;
            }
            break;
        }
        default:
            break;
        }
    }
    if (ret != ESP_OK)
    {
        for (uint32_t j = 0; j <= i; j++)
        {
            if (prop_values[j].free_fn)
            {
                ESP_LOGI(LOCAL_CTRL_TAG, "Freeing memory for %s", props[j].name);
                prop_values[j].free_fn(prop_values[j].data);
                prop_values[j].free_fn = NULL;
                prop_values[j].data = NULL;
                prop_values[j].size = 0;
            }
        }
    }
    return ret;
}

static esp_err_t set_property_values(size_t props_count,
                                     const esp_local_ctrl_prop_t props[],
                                     const esp_local_ctrl_prop_val_t prop_values[],
                                     void *usr_ctx)
{
    esp_err_t ret = ESP_OK;
    uint32_t i;
    /* First check if any of the properties are read-only properties. If found, just abort */
    for (i = 0; i < props_count; i++)
    {
        /* Cannot set the value of a read-only property */
        if (props[i].flags & PROP_FLAG_READONLY)
        {
            ESP_LOGE(LOCAL_CTRL_TAG, "%s is read-only", props[i].name);
            return ESP_ERR_INVALID_ARG;
        }
    }
    for (i = 0; i < props_count && ret == ESP_OK; i++)
    {
        switch (props[i].type)
        {
        case PROP_TYPE_NODE_PARAMS:
            ret = set_param_values((char *)prop_values[i].data,
                                   prop_values[i].size);

            break;
        default:
            break;
        }
    }
    return ret;
}

/******************************************************************************/

/* Function used by app_main to start the esp_local_ctrl service */
void start_esp_local_ctrl_service(void)
{
    /* Set the configuration */
    static httpd_ssl_config_t http_conf = HTTPD_SSL_CONFIG_DEFAULT();
    http_conf.transport_mode = HTTPD_SSL_TRANSPORT_INSECURE;
    http_conf.port_insecure = 8080;
    http_conf.httpd.ctrl_port = 12312;
    http_conf.httpd.stack_size = 6144;
#ifdef CONFIG_USE_PROTOCOMM_SECURITY_VERSION_1
    /* What is the security level that we want (0, 1, 2):
     *      - PROTOCOMM_SECURITY_0 is simply plain text communication.
     *      - PROTOCOMM_SECURITY_1 is secure communication which consists of secure handshake
     *          using X25519 key exchange and proof of possession (pop) and AES-CTR
     *          for encryption/decryption of messages.
     *      - PROTOCOMM_SECURITY_2 SRP6a based authentication and key exchange
     *        + AES-GCM encryption/decryption of messages
     */
    esp_local_ctrl_proto_sec_t security = PROTOCOM_SEC1;

    /* Do we want a proof-of-possession (ignored if Security 0 is selected):
     *      - this should be a string with length > 0
     *      - NULL if not used
     */
    const char *pop = "abcd1234";

    /* This is the structure for passing security parameters
     * for the protocomm security 1.
     */
    protocomm_security1_params_t sec_params = {
        .data = (const uint8_t *)pop,
        .len = static_cast<uint16_t>(strlen(pop)),
    };

#else /* CONFIG_LOCAL_CTRL_PROTOCOMM_SECURITY_VERSION_0 */
    esp_local_ctrl_proto_sec_t security = PROTOCOM_SEC0;
    const void *sec_params = NULL;

#endif
    esp_local_ctrl_config_t config = {
        .transport = ESP_LOCAL_CTRL_TRANSPORT_HTTPD,
        .transport_config = {
            .httpd = &http_conf,
        },
        .proto_sec = {
            .version = security,
            .custom_handle = NULL,
            .sec_params = &sec_params,
        },
        .handlers = {/* User defined handler functions */
                     .get_prop_values = get_property_values,
                     .set_prop_values = set_property_values,
                     .usr_ctx = NULL,
                     .usr_ctx_free_fn = NULL},
        /* Maximum number of properties that may be set */
        .max_properties = 10};

    mdns_init();
    mdns_hostname_set(deviceConfig.name.c_str()); // Pass in Device Id.

    /* Add node_id in mdns */
    mdns_service_txt_item_set("_esp_local_ctrl", "_tcp", "node_id", deviceConfig.name.c_str());

    /* Start esp_local_ctrl service */
    ESP_ERROR_CHECK(esp_local_ctrl_start(&config));
    ESP_LOGI(LOCAL_CTRL_TAG, "esp_local_ctrl service started with name : %s", deviceConfig.name.c_str());

    /* Create the Node Config property */
    esp_local_ctrl_prop_t node_config = {
        .name = (char *)"config",
        .type = PROP_TYPE_NODE_CONFIG,
        .size = 0,
        .flags = PROP_FLAG_READONLY,
        .ctx = NULL,
        .ctx_free_fn = NULL};

    /* Create the Node Params property */
    esp_local_ctrl_prop_t node_params = {
        .name = (char *)"params",
        .type = PROP_TYPE_NODE_PARAMS,
        .size = 0,
        .flags = 0,
        .ctx = NULL,
        .ctx_free_fn = NULL};

    /* Now register the properties */
    ESP_ERROR_CHECK(esp_local_ctrl_add_property(&node_config));
    ESP_ERROR_CHECK(esp_local_ctrl_add_property(&node_params));
}
