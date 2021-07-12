/*
IO part of fgohook code is developed and provided by Haruka @ discord with permission. Thanks.
*/
#include <windows.h>
#include <xinput.h>

#include <limits.h>
#include <stdint.h>

#include "fgoio.h"
#include "fgoio/config.h"

#include "util/dprintf.h"

static uint8_t fgo_opbtn;
static uint8_t fgo_btn;
static uint16_t fgo_stick_x;
static uint16_t fgo_stick_y;
static uint8_t focus_state = 1;
static HWND game_wnd = NULL;

static struct fgo_io_config fgo_io_cfg;

HWND get_game_window(){
	if (game_wnd != NULL){
		return game_wnd;
	}
	char wnd_title[256];
	HWND hwnd = GetForegroundWindow();
	GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
	if (strcmp(wnd_title, "fgo/Grand Order Arcade r29855") == 0){
		dprintf("fgoio: Game Window detected\n");
		game_wnd = hwnd;
		return game_wnd;
	}
	return NULL;
}

uint16_t fgo_io_get_api_version(void)
{
    return 0x0100;
}

HRESULT fgo_io_init(void)
{
	fgo_stick_x = 32767;
	fgo_stick_y = 32767;
    fgo_io_config_load(&fgo_io_cfg, L".\\segatools.ini");
    return S_OK;
}

HRESULT fgo_io_poll(void)
{
    if (fgo_io_cfg.background_input_allowed || GetForegroundWindow() == get_game_window()){
		if (!focus_state){
			dprintf("fgoio: Got focus\n");
			focus_state = 1;
		}

		XINPUT_STATE xi;
		WORD xb;

		fgo_opbtn = 0;
		fgo_btn = 0;

		if (GetAsyncKeyState(fgo_io_cfg.vk_test) & 0x8000) {
			fgo_opbtn |= FGO_IO_OPBTN_TEST;
		}

		if (GetAsyncKeyState(fgo_io_cfg.vk_service) & 0x8000) {
			fgo_opbtn |= FGO_IO_OPBTN_SERVICE;
		}

		memset(&xi, 0, sizeof(xi));
		XInputGetState(0, &xi);
		xb = xi.Gamepad.wButtons;

		if(fgo_io_cfg.input_mode == 2){
			if (GetAsyncKeyState(fgo_io_cfg.vk_treasure) & 0x8000) {
				fgo_btn |= IO_BTN_TREASURE;
			}

			if (GetAsyncKeyState(fgo_io_cfg.vk_target) & 0x8000) {
				fgo_btn |= IO_BTN_TARGET;
			}

			if (GetAsyncKeyState(fgo_io_cfg.vk_dash) & 0x8000) {
				fgo_btn |= IO_BTN_DASH;
			}

			if (GetAsyncKeyState(fgo_io_cfg.vk_attack) & 0x8000) {
				fgo_btn |= IO_BTN_ATTACK;
			}

			if (GetAsyncKeyState(fgo_io_cfg.vk_camera) & 0x8000) {
				fgo_btn |= IO_BTN_CAMERA;
			}

		}
		else if(fgo_io_cfg.input_mode == 1){
			if (xb & XINPUT_GAMEPAD_DPAD_LEFT) {
				fgo_btn |= IO_BTN_TREASURE;
			}

			if (xb & XINPUT_GAMEPAD_DPAD_UP) {
				fgo_btn |= IO_BTN_TARGET;
			}

			if (xb & XINPUT_GAMEPAD_A) {
				fgo_btn |= IO_BTN_DASH;
			}

			if (xb & XINPUT_GAMEPAD_X) {
				fgo_btn |= IO_BTN_ATTACK;
			}

			if (xb & XINPUT_GAMEPAD_Y) {
				fgo_btn |= IO_BTN_CAMERA;
			}

		}

		if(fgo_io_cfg.input_mode == 2){
			if (GetAsyncKeyState(fgo_io_cfg.vk_right) & 0x8000) {
				fgo_stick_x = USHRT_MAX;
			} else if (GetAsyncKeyState(fgo_io_cfg.vk_left) & 0x8000) {
                fgo_stick_x = 0;
            } else {
                fgo_stick_x = USHRT_MAX / 2;
            }
            if (GetAsyncKeyState(fgo_io_cfg.vk_down) & 0x8000) {
				fgo_stick_y = USHRT_MAX;
			} else if (GetAsyncKeyState(fgo_io_cfg.vk_up) & 0x8000) {
                fgo_stick_y = 0;
            } else {
                fgo_stick_y = USHRT_MAX / 2;
            }
		}
		else if(fgo_io_cfg.input_mode == 1){
			if (abs(xi.Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
				fgo_stick_x = xi.Gamepad.sThumbLX + (USHRT_MAX / 2);
			}

			if (abs(xi.Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
				fgo_stick_y = xi.Gamepad.sThumbLY + (USHRT_MAX / 2);
			}
		}

	} else {
		if (focus_state){
			dprintf("fgoio: Lost focus\n");
			focus_state = 0;
		}
	}

    return S_OK;
}

void fgo_io_get_opbtns(uint8_t *opbtn)
{
    if (opbtn != NULL) {
        *opbtn = fgo_opbtn;
    }
}

void fgo_io_get_gamebtns(uint8_t *btn)
{
    if (btn != NULL) {
        *btn = fgo_btn;
    }
}

void fgo_io_get_stick(uint16_t *x, uint16_t *y)
{
    if (x != NULL) {
        *x = fgo_stick_x;
    }
    if (y != NULL) {
        *y = fgo_stick_y;
    }
}
