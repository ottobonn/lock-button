#include "HID-Project.h"
#include "DebouncedInput.h"

const bool DEBUG = false;

DebouncedInput lockButtonInput(2, INPUT_PULLUP, LOW, 100);

// We can't send serial messages within the interrupt handler, so we wait until
// the next turn of the loop.
volatile bool shouldSend = false;
void sendOnNextLoop() {
  shouldSend = true;
}

// Locks the Mac by sending SHIFT+CTRL+Power
void sendLockCommand(void) {
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_CTRL);
  System.write(SYSTEM_POWER_DOWN);
  Keyboard.releaseAll();
}

void sendTestMessage(void) {
  Keyboard.write('!');
}

void setup(void) {
  Keyboard.begin();
  System.begin();
  lockButtonInput.onInput(DEBUG ? sendTestMessage : sendLockCommand);
  attachInterrupt(digitalPinToInterrupt(lockButtonInput.pin), DebouncedInput::isr<&lockButtonInput>, CHANGE);
}

void loop(void) {
  // polling style: lockButtonInput.poll();
  if (shouldSend) {
    sendTestMessage();
    shouldSend = false;
  }
}
