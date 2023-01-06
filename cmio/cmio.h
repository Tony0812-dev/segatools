#pragma once

#include <windows.h>

#include <stdint.h>

enum {
    cm_IO_OPBTN_TEST = 0x01,
    cm_IO_OPBTN_SERVICE = 0x02,
    cm_IO_OPBTN_COIN = 0x04,
};

/* Get the version of the CardMaker IO API that this DLL supports. This
   function should return a positive 16-bit integer, where the high byte is
   the major version and the low byte is the minor version (as defined by the
   Semantic Versioning standard).

   The latest API version as of this writing is 0x0100. */

uint16_t cm_io_get_api_version(void);

/* Initialize the IO DLL. This is the second function that will be called on
   your DLL, after cm_io_get_api_version.

   All subsequent calls to this API may originate from arbitrary threads.

   Minimum API version: 0x0100 */

HRESULT cm_io_init(void);

/* Send any queued outputs (of which there are currently none, though this may
   change in subsequent API versions) and retrieve any new inputs.

   Minimum API version: 0x0100 */

HRESULT cm_io_poll(void);

/* Get the state of the cabinet's operator buttons as of the last poll. See
   cm_IO_OPBTN enum above: this contains bit mask definitions for button
   states returned in *opbtn. All buttons are active-high.

   Minimum API version: 0x0100 */

void cm_io_get_opbtns(uint8_t *opbtn);