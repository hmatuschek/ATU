#include "log.h"
#include "config.h"
#include <avr/eeprom.h>
#include <util/atomic.h>

/** How to store "log messages" in the EEPROM. */
typedef struct {
  uint8_t  L;   ///< Current best L
  int8_t   C;   ///< Current best C
  uint16_t swr; ///< Best VSWR.
} LogEntry;

#define LOG_SIZE 256

// Represents the "log memory"
LogEntry _log[LOG_SIZE] EEMEM;

// Current index of the next log-massage.
uint8_t _log_index = 0;

/** Stores the current VSWR, L and C in the log. */
void add_log(uint16_t swr, uint8_t L, int8_t C) {
#ifdef LOG_TUING
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    LogEntry entry = {L, C, swr};
    eeprom_write_block(&entry, &(_log[_log_index++]), sizeof(LogEntry));
    if (LOG_SIZE == _log_index)
      _log_index = 0;
  }
#endif
}
