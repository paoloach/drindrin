/* http_get - Retrieves a web page over HTTP GET.
 *
 * See http_get_ssl for a TLS-enabled version.
 *
 * This sample code is in the public domain.,
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"

#include <unistd.h>
#include <string.h>
#include <lwip/tcp.h>

#include "FreeRTOS.h"
#include "task.h"

#include "lwip/err.h"
#include "lwip/raw.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "ssid_config.h"

struct tcp_pcb* tcpPcb;

static void tcpErr(void * arg, err_t err);
static err_t tcpConnect(void *arg, struct tcp_pcb *tpcb, err_t err);
static err_t tcpSent(void *arg, struct tcp_pcb *tpcb, u16_t  len);
static ip_addr_t addr;

enum Status {
    GETTING_IP,
    WAIT_PRESS,
    CONNECT,
    CONNECTING,
    CONNECTED,
    SENDING,
    SENT,
    CLOSE,
    ERROR
};

#define BUTTON_PIN 4

enum Status status=GETTING_IP;
enum Status oldStatus=GETTING_IP;

void buttonTask(void *pvParameters)
{
    printf("Button task starting...\r\n");
    status = GETTING_IP;
    gpio_enable(BUTTON_PIN, GPIO_INPUT);
    gpio_set_pullup(BUTTON_PIN, true, true);
    while(1) {
        if (status != oldStatus) {
            printf("Status: %d\n", status);
            oldStatus = status;
        }
        switch(status){
            case GETTING_IP:{
                if (sdk_wifi_station_get_connect_status() == STATION_GOT_IP){
                    status = WAIT_PRESS;
                }
                break;
            }
            case WAIT_PRESS: {
                if (!gpio_read(BUTTON_PIN)) {
                    printf("BUTTON PRESS\n");
                    status = CONNECT;
                }
                break;
            }
            case CONNECT: {
                status = CONNECTING;
                tcpPcb = tcp_new();
                if (tcpPcb == NULL) {
                    printf("null tcp_pcb\n");
                    status = WAIT_PRESS;
                    break;
                }
                tcp_err(tcpPcb, tcpErr);
                tcp_sent(tcpPcb, tcpSent);


                IP4_ADDR(&addr, 192, 168, 1, 105);
                err_t conRes = tcp_connect(tcpPcb, &addr, 6789, tcpConnect);
                if (conRes != ERR_OK) {
                    printf("%d\n",__LINE__);
                    printf("Error calling connection: %d", conRes);
                    status = WAIT_PRESS;
                    break;
                }
                break;
            }
            case CONNECTED: {
                    err_t writRes = tcp_write(tcpPcb, "DrinDrin", 8, 0);
                    if (writRes != ERR_OK) {
                        printf("Error writing data: %d", writRes);
                        status = WAIT_PRESS;
                        break;
                    }
                    err_t outRes = tcp_output(tcpPcb);
                    if (outRes != ERR_OK) {
                        printf("Error output data: %d", outRes);
                        status = WAIT_PRESS;
                        break;
                    }
                    status = SENDING;
                }
                break;
            case SENT: {
                printf("Close\n");
                tcp_close(tcpPcb);
                status = WAIT_PRESS;
                sys_unlock_tcpip_core();
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                break;
            }
        }
        taskYIELD();
    }
}

err_t tcpConnect(void *arg, struct tcp_pcb *tpcb, err_t err){
    printf("tcpConnect: %d\n", err);
    if (err == ERR_OK){
        status = CONNECTED;
    } else {
        status = WAIT_PRESS;
        printf("Error connecting...: %d\n", err);
        printf("status: %d\n", status);
    }
    return ERR_OK;
}

err_t tcpSent(void *arg, struct tcp_pcb *tpcb, u16_t  len) {
    printf("Sent %d bytes\n", len);
    status = SENT;
    return ERR_OK;
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    };

    /* required to call wifi_set_opmode before station_set_config */
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    xTaskCreate(&buttonTask, "buttonTask", 384, NULL, 2, NULL);
}

static void tcpErr(void * arg, err_t err) {
    printf("Error connecting: %d\n", err);
    status = WAIT_PRESS;
}

