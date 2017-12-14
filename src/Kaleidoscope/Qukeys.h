/* -*- mode: c++ -*-
 * Kaleidoscope-Qukeys -- Assign two keycodes to a single key
 * Copyright (C) 2017  Michael Richters
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <Kaleidoscope.h>

namespace kaleidoscope {

// Maximum length of the pending queue
static constexpr byte QUKEYS_QUEUE_MAX = 8;

// Boolean values for storing qukey state
static constexpr bool QUKEY_STATE_PRIMARY = false;
static constexpr bool QUKEY_STATE_ALTERNATE = true;

// Initialization addr value for empty key_queue. This seems to be
// unnecessary, because we rely on keeping track of the lenght of the
// queue, anyway.
static constexpr KeyAddr QUKEY_UNKNOWN_ADDR = 0xFF;

// Value to return when no match is found in Qukeys.dict. A successful
// match returns an index in the array, so this must be negative. Also
// used for failed search of the key_queue.
static constexpr int8_t QUKEY_NOT_FOUND = -1;
// Wildcard value; this matches any layer
static constexpr int8_t QUKEY_ALL_LAYERS = -1;

// Data structure for an individual qukey
struct Qukey {
 public:
  Qukey(void) {}
  Qukey(int8_t layer, KeyAddr key_addr, Key alt_keycode);
  Qukey(int8_t layer, byte row, byte col, Key alt_keycode);

  int8_t layer;
  KeyAddr addr;
  Key alt_keycode;
};

// Data structure for an entry in the key_queue
struct QueueItem {
  KeyAddr addr;        // keyswitch coordinates
  uint32_t flush_time; // time past which a qukey gets flushed
};

// The plugin itself
class Qukeys : public KaleidoscopePlugin {
  // I could use a bitfield to get the state values, but then we'd
  // have to check the key_queue (there are three states). Or use a
  // second bitfield for the indeterminite state. Using a bitfield
  // would enable storing the qukey list in PROGMEM, but I don't know
  // if the added complexity is worth it.
 public:
  Qukeys(void);

  void init();
  static void activate(void) {
    active_ = true;
  }
  static void deactivate(void) {
    active_ = false;
  }
  static void toggle(void) {
    active_ = !active_;
  }
  static void setTimeout(uint16_t time_limit) {
    time_limit_ = time_limit;
  }

  static Qukey * qukeys;
  static uint8_t qukeys_count;

  bool eventHandlerHook(Key &mapped_key, const EventKey &event_key);
  void preReportHook(void);

 private:
  static bool active_;
  static uint16_t time_limit_;
  static QueueItem key_queue_[QUKEYS_QUEUE_MAX];
  static uint8_t key_queue_length_;

  // Qukey state bitfield
  static uint8_t qukey_state_[(TOTAL_KEYS) / 8 + ((TOTAL_KEYS) % 8 ? 1 : 0)];
  static bool getQukeyState(KeyAddr addr) {
    return bitRead(qukey_state_[addr / 8], addr % 8);
  }
  static void setQukeyState(KeyAddr addr, boolean qukey_state) {
    bitWrite(qukey_state_[addr / 8], addr % 8, qukey_state);
  }

  static int8_t lookupQukey(KeyAddr key_addr);
  static void enqueue(KeyAddr key_addr);
  static int8_t searchQueue(KeyAddr key_addr);
  static void flushKey(bool qukey_state, uint8_t keyswitch_state);
  static void flushQueue(int8_t index);
  static void flushQueue(void);
};

} // namespace kaleidoscope {

extern kaleidoscope::Qukeys Qukeys;

// macro for use in sketch file to simplify definition of qukeys
#define QUKEYS(qukey_defs...) {						\
  static kaleidoscope::Qukey qk_table[] = { qukey_defs };		\
  Qukeys.qukeys = qk_table;						\
  Qukeys.qukeys_count = sizeof(qk_table) / sizeof(kaleidoscope::Qukey); \
}
