// -*- c++ -*-

#include <Arduino.h>

#include KALEIDOSCOPE_HARDWARE_H
#include KALEIDOSCOPE_KEYADDR_H
#include <kaleidoscope/Key.h>
#include <kaleidoscope/Plugin.h>


namespace kaleidoscope {
namespace qukeys {

// Constants (can be overridden in the sketch)
__attribute__((weak))
constexpr uint16_t timeout{200};

__attribute__((weak))
constexpr byte qukey_release_delay{0};

__attribute__((weak))
constexpr uint16_t grace_period_offset{4096};

__attribute__((weak))
constexpr byte queue_max{8};

// Qukey structure
struct Qukey {
  Key  primary;
  Key  alternate;
  byte release_delay{qukey_release_delay};
};

// QueueEntry structure
struct QueueEntry {
  KeyAddr  addr;
  uint16_t start_time;
};


class Plugin : public kaleidoscope::Plugin {

 public:
  Plugin(const Qukey* const qukeys, const byte qukey_count, Keymap& keymap)
      : qukeys_(qukeys), qukey_count_(qukey_count), keymap_(keymap) {}

  void activate(void) {
    plugin_active_ = true;
  }
  void deactivate(void) {
    plugin_active_ = false;
  }
  void toggle(void) {
    plugin_active_ = !plugin_active_;
  }

  bool keyswitchEventHook(KeyswitchEvent& event,
                          kaleidoscope::Plugin*& caller) override;

  void preScanHook() override;

 private:
  // An array of Qukey objects
  const Qukey* const qukeys_;
  const byte         qukey_count_;

  // A reference to the keymap for lookups
  Keymap& keymap_;

  // A reference to the (shared) array of active key values
  //KeyArray& active_keys_;

  // The queue of keypress events
  QueueEntry key_queue_[queue_max];
  byte       key_queue_length{0};

  // Pointer to the qukey at the head of the queue
  Qukey* queue_head_p_{nullptr};

  // Release delay for key at head of queue
  byte qukey_release_delay_{0};

  // Runtime controls
  bool plugin_active_{true};

  Qukey* lookupQukey(Key key);
  Qukey* lookupQukey(KeyAddr k);
  Qukey* lookupQukey(QueueEntry entry);

  void enqueueKey(KeyAddr k, Qukey* qp = nullptr);
  int8_t searchQueue(KeyAddr k);
  void shiftQueue();
  void flushQueue(bool alt_key);
  void flushQueue(int8_t queue_index);
  void flushKey(bool alt_key = false);
  void flushQueue();
  void updateReleaseDelay();
  
};

} // namespace qukeys {
} // namespace kaleidoscope {
