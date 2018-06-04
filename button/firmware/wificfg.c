/*
 * Example Wifi configuration via an access point.
 *
 * Copyright (C) 2016 OurAirQuality.org
 *
 * Licensed under the Apache License, Version 2.0, January 2004 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *      http://www.apache.org/licenses/
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS WITH THE SOFTWARE.
 *
 */

#include <string.h>
#include <ctype.h>

#include <espressif/esp_common.h>
#include <espressif/user_interface.h>
#include <esp/uart.h>
#include <FreeRTOS.h>
#include <task.h>

#include <sdk_internal.h>
#include <esplibs/libmain.h>

#include "lwip/sockets.h"

#include "wificfg/wificfg.h"

#include "sysparam.h"

static const char http_success_header[] = "HTTP/1.1 200 \r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Cache-Control: no-store\r\n"
    "Transfer-Encoding: chunked\r\n"
    "Connection: close\r\n"
    "\r\n";
static const char *http_index_content[] = {
#include "content/index.html"
};

static int handle_index(int s, wificfg_method method,
                        uint32_t content_length,
                        wificfg_content_type content_type,
                        char *buf, size_t len)
{
    if (wificfg_write_string(s, http_success_header) < 0) return -1;

    if (method != HTTP_METHOD_HEAD) {
        if (wificfg_write_string_chunk(s, http_index_content[0], buf, len) < 0) return -1;
        if (wificfg_write_html_title(s, buf, len, "Home") < 0) return -1;
        if (wificfg_write_string_chunk(s, http_index_content[1], buf, len) < 0) return -1;

        if (wificfg_write_string_chunk(s, "<dl class=\"dlh\">", buf, len) < 0) return -1;

        struct sockaddr_storage addr;
        socklen_t addr_len = sizeof(addr);
        if (getpeername(s, (struct sockaddr *)&addr, &addr_len) == 0) {
            if (((struct sockaddr *)&addr)->sa_family == AF_INET) {
                struct sockaddr_in *sa = (struct sockaddr_in *)&addr;
                if (wificfg_write_string_chunk(s, "<dt>Peer address</dt>", buf, len) < 0) return -1;
                snprintf(buf, len, "<dd>" IPSTR ", port %u</dd>",
                         IP2STR((ip4_addr_t *)&sa->sin_addr.s_addr), ntohs(sa->sin_port));
                if (wificfg_write_string_chunk(s, buf, buf, len) < 0) return -1;
            }

#if LWIP_IPV6
            if (((struct sockaddr *)&addr)->sa_family == AF_INET6) {
                struct sockaddr_in6 *sa = (struct sockaddr_in6 *)&addr;
                if (wificfg_write_string_chunk(s, "<dt>Peer address</dt><dd>", buf, len) < 0) return -1;
                if (inet6_ntoa_r(sa->sin6_addr, buf, len)) {
                    if (wificfg_write_string_chunk(s, buf, buf, len) < 0) return -1;
                }
                snprintf(buf, len, ", port %u</dd>", ntohs(sa->sin6_port));
                if (wificfg_write_string_chunk(s, buf, buf, len) < 0) return -1;
            }
#endif
        }

        if (wificfg_write_string_chunk(s, "</dl>", buf, len) < 0) return -1;
        if (wificfg_write_string_chunk(s, http_index_content[2], buf, len) < 0) return -1;
        if (wificfg_write_chunk_end(s) < 0) return -1;
    }
    return 0;
}

static const wificfg_dispatch dispatch_list[] = {
    {"/", HTTP_METHOD_GET, handle_index, false},
    {"/index.html", HTTP_METHOD_GET, handle_index, false},
    {NULL, HTTP_METHOD_ANY, NULL}
};

extern uint32_t  flash_size;
extern uint8_t    memoryType;

#define SPI_BASE 0x60000200
#define SPI(i) (*(struct SPI_REGS *)(0x60000200 - (i)*0x100))

void user_init(void)
{
    uint32_t buf32[sizeof(struct sdk_g_ic_saved_st) / 4];
    uint8_t *buf8 = (uint8_t *)buf32;

    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());



    SPI(0).USER0 |= SPI_USER0_CS_SETUP;
    if ( sdk_SPIRead(0, buf32, 4) == SPI_FLASH_RESULT_OK) { ;
        memoryType = buf8[3] >> 4;

        printf("Data: ");
        for (uint8_t i = 0; i < 5; i++)
            printf("%d = %02X ", i, (uint32_t) (buf8[i]));
        printf("\n");

        switch (buf8[3] >> 4) {
            case 0x0:   // 4 Mbit (512 KByte)
                flash_size = 524288;
                break;
            case 0x1:  // 2 Mbit (256 Kbyte)
                flash_size = 262144;
                break;
            case 0x2:  // 8 Mbit (1 Mbyte)
                flash_size = 1048576;
                break;
            case 0x3:  // 16 Mbit (2 Mbyte)
            case 0x5:  // 16 Mbit (2 Mbyte)
                flash_size = 2097152;
                break;
            case 0x4:  // 32 Mbit (4 Mbyte)
            case 0x6:  // 32 Mbit (4 Mbyte)
                flash_size = 4194304;
                break;
            case 0x8:  // 64 Mbit (8 Mbyte)
                flash_size = 8388608;
                break;
            case 0x9:  // 128 Mbit (16 Mbyte)
                flash_size = 16777216;
                break;
            default:   // Invalid -- Assume 4 Mbit (512 KByte)
                flash_size = 524288;
        }
        uint32_t sysparam_addr = flash_size - (5 + DEFAULT_SYSPARAM_SECTORS) * sdk_flashchip.sector_size;
        sysparam_status_t status = sysparam_init(sysparam_addr, flash_size);
        if (status == SYSPARAM_NOTFOUND) {
            status = sysparam_create_area(sysparam_addr, DEFAULT_SYSPARAM_SECTORS, false);
            if (status == SYSPARAM_OK) {
                status = sysparam_init(sysparam_addr, 0);
            }
        }
        printf("Flash type (%d), size: %d\n", memoryType, flash_size);
    } else {
        printf("Error reading from SPI\n");
    }
    sdk_wifi_set_sleep_type(WIFI_SLEEP_MODEM);

    wificfg_init(80, dispatch_list);
}
