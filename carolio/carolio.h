#pragma once

/*
   Wonderland Wars CUSTOM IO API

   Changelog:

   - 0x0100: Initial API version (assumed if carol_io_get_api_version is not
     exported)
*/

#include <windows.h>

#include <stdbool.h>
#include <stdint.h>

/* Get the version of the Wonderland Wars IO API that this DLL supports. This
   function should return a positive 16-bit integer, where the high byte is
   the major version and the low byte is the minor version (as defined by the
   Semantic Versioning standard).

   The latest API version as of this writing is 0x0101. */

uint16_t carol_io_get_api_version(void);

/* Initialize JVS-based input. This function will be called before any other
   carol_io_jvs_*() function calls. Errors returned from this function will
   manifest as a disconnected JVS bus.

   All subsequent calls may originate from arbitrary threads and some may
   overlap with each other. Ensuring synchronization inside your IO DLL is
   your responsibility.

   Minimum API version: 0x0100 */

HRESULT carol_io_jvs_init(void);

/* Poll JVS input.

   opbtn returns the cabinet test/service state, where bit 0 is Test and Bit 1
   is Service.

   beam returns the IR beams that are currently broken, where bit 0 is the
   lowest IR beam and bit 5 is the highest IR beam, for a total of six beams.

   Both bit masks are active-high.

   Note that you cannot instantly break the entire IR grid in a single frame to
   simulate hand movement; this will be judged as a miss. You need to simulate
   a gradual raising and lowering of the hands. Consult the proof-of-concept
   implementation for details.

   NOTE: Previous releases of Segatools mapped the IR beam inputs incorrectly.
   Please ensure that you advertise an API version of at least 0x0101 so that
   the correct mapping can be used.

   Minimum API version: 0x0100
   Latest API version: 0x0101 */

void carol_io_jvs_poll(uint8_t *opbtn, uint8_t *beams);

/* Read the current state of the coin counter. This value should be incremented
   for every coin detected by the coin acceptor mechanism. This count does not
   need to persist beyond the lifetime of the process.

   Minimum API version: 0x0100 */

void carol_io_jvs_read_coin_counter(uint16_t *total);

// Controller board test
HRESULT carol_io_controlbd_init(void);
typedef void (*carol_io_controlbd_callback_t)(const uint8_t *state);
void carol_io_controlbd_start(carol_io_controlbd_callback_t callback);
void carol_io_controlbd_stop(void);