#include <windows.h>

#include <process.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "chuniio/chuniio.h"
#include "chuniio/config.h"

#include "util/dprintf.h"

static unsigned int __stdcall chuni_io_slider_thread_proc(void *ctx);

static bool chuni_io_coin;
static uint16_t chuni_io_coins;
static uint8_t chuni_io_hand_pos;
static HANDLE chuni_io_slider_thread;
static bool chuni_io_slider_stop_flag;
static struct chuni_io_config chuni_io_cfg;
static HANDLE chuni_io_slider_led_port;

uint16_t chuni_io_get_api_version(void)
{
    return 0x0101;
}

HRESULT chuni_io_jvs_init(void)
{
    chuni_io_config_load(&chuni_io_cfg, L".\\segatools.ini");

    return S_OK;
}

void chuni_io_jvs_read_coin_counter(uint16_t *out)
{
    if (out == NULL) {
        return;
    }

    if (GetAsyncKeyState(chuni_io_cfg.vk_coin)) {
        if (!chuni_io_coin) {
            chuni_io_coin = true;
            chuni_io_coins++;
        }
    } else {
        chuni_io_coin = false;
    }

    *out = chuni_io_coins;
}

void chuni_io_jvs_poll(uint8_t *opbtn, uint8_t *beams)
{
    size_t i;

    if (GetAsyncKeyState(chuni_io_cfg.vk_test) & 0x8000) {
        *opbtn |= 0x01; /* Test */
    }

    if (GetAsyncKeyState(chuni_io_cfg.vk_service) & 0x8000) {
        *opbtn |= 0x02; /* Service */
    }

    if (GetAsyncKeyState(chuni_io_cfg.vk_coin) & 0x8000) {
        *opbtn |= 0x04; /* Coin */
    }

    if (chuni_io_cfg.vk_ir_emu) {
        // Use emulated AIR
        if (GetAsyncKeyState(chuni_io_cfg.vk_ir_emu)) {
            if (chuni_io_hand_pos < 6) {
                chuni_io_hand_pos++;
            }
        } else {
            if (chuni_io_hand_pos > 0) {
                chuni_io_hand_pos--;
            }
        }

        for (i = 0 ; i < 6 ; i++) {
            if (chuni_io_hand_pos > i) {
                *beams |= (1 << i);
            }
        }
    } else {
        // Use actual AIR
        // IR format is beams[5:0] = {b5,b6,b3,b4,b1,b2};
        for (i = 0 ; i < 3 ; i++) {
            if (GetAsyncKeyState(chuni_io_cfg.vk_ir[i*2]) & 0x8000)
                *beams |= (1 << (i*2+1));
            if (GetAsyncKeyState(chuni_io_cfg.vk_ir[i*2+1]) & 0x8000)
                *beams |= (1 << (i*2));
        }
    }
}

HRESULT chuni_io_slider_init(void)
{
    return S_OK;
}

void chuni_io_slider_start(chuni_io_slider_callback_t callback)
{
    BOOL status;

    if (chuni_io_slider_thread != NULL) {
        return;
    }

    chuni_io_slider_thread = (HANDLE) _beginthreadex(
            NULL,
            0,
            chuni_io_slider_thread_proc,
            callback,
            0,
            NULL);

    chuni_io_slider_led_port = CreateFileW(chuni_io_cfg.led_com,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);
    
    if (chuni_io_slider_led_port == INVALID_HANDLE_VALUE)
        dprintf("Chunithm LEDs: Failed to open COM port (Attempted on %S)\n", chuni_io_cfg.led_com);
    else
        dprintf("Chunithm LEDs: COM Port Success!\n");
            
    DCB dcb_serial_params = { 0 };
    dcb_serial_params.DCBlength = sizeof(dcb_serial_params);
    status = GetCommState(chuni_io_slider_led_port, &dcb_serial_params);
    
    dcb_serial_params.BaudRate = CBR_115200;  // Setting BaudRate = 115200
    dcb_serial_params.ByteSize = 8;         // Setting ByteSize = 8
    dcb_serial_params.StopBits = ONESTOPBIT;// Setting StopBits = 1
    dcb_serial_params.Parity   = NOPARITY;  // Setting Parity = None
    SetCommState(chuni_io_slider_led_port, &dcb_serial_params);
    
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout         = 50; // in milliseconds
    timeouts.ReadTotalTimeoutConstant    = 50; // in milliseconds
    timeouts.ReadTotalTimeoutMultiplier  = 10; // in milliseconds
    timeouts.WriteTotalTimeoutConstant   = 50; // in milliseconds
    timeouts.WriteTotalTimeoutMultiplier = 10; // in milliseconds
    
    SetCommTimeouts(chuni_io_slider_led_port, &timeouts);

}

void chuni_io_slider_stop(void)
{
    if (chuni_io_slider_thread == NULL) {
        return;
    }

    chuni_io_slider_stop_flag = true;

    WaitForSingleObject(chuni_io_slider_thread, INFINITE);
    CloseHandle(chuni_io_slider_thread);
    chuni_io_slider_thread = NULL;
    chuni_io_slider_stop_flag = false;

    dprintf("Chunithm LEDs: Closing COM port\n");
    CloseHandle(chuni_io_slider_led_port);
}

void chuni_io_slider_set_leds(const uint8_t *rgb)
{
    if (chuni_io_slider_led_port != INVALID_HANDLE_VALUE)
    {
        char led_buffer[100];
        DWORD bytes_to_write;         // No of bytes to write into the port
        DWORD bytes_written = 0;     // No of bytes written to the port
        bytes_to_write = sizeof(led_buffer);
        BOOL status;
        
        led_buffer[0] = 0xAA;
        led_buffer[1] = 0xAA;
        memcpy(led_buffer+2, rgb, sizeof(uint8_t) * 96);
        led_buffer[98] = 0xDD;
        led_buffer[99] = 0xDD;
        
        status = WriteFile(chuni_io_slider_led_port,        // Handle to the Serial port
                           led_buffer,     // Data to be written to the port
                           bytes_to_write,  //No of bytes to write
                           &bytes_written, //Bytes written
                           NULL);
    }

}

static unsigned int __stdcall chuni_io_slider_thread_proc(void *ctx)
{
    chuni_io_slider_callback_t callback;
    uint8_t pressure[32];
    size_t i;

    callback = ctx;

    while (!chuni_io_slider_stop_flag) {
        for (i = 0 ; i < _countof(pressure) ; i++) {
            if (GetAsyncKeyState(chuni_io_cfg.vk_cell[i]) & 0x8000) {
                pressure[i] = 128;
            } else {
                pressure[i] = 0;
            }
        }

        callback(pressure);
        Sleep(1);
    }

    return 0;
}
