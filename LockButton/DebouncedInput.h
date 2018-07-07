#ifndef DEBOUNCED_INPUT_H
#define DEBOUNCED_INPUT_H

typedef void (*InputListenerFunction)(void);

class DebouncedInput {
public:
  const int pin;
  const int mode;
  const int activeValue;
  const long debounceMS;

  DebouncedInput(int pin, int mode, int activeValue, long debounceMS);
  template<DebouncedInput *input> static void isr(void);
  void onInput(InputListenerFunction fn);
  void poll();

private:
  InputListenerFunction inputListener;

  volatile long lastTime;
  volatile int lastValue;
  volatile bool waiting;
};

DebouncedInput::DebouncedInput(int pin, int mode, int activeValue, long debounceMS)
: pin(pin),
  mode(mode),
  activeValue(activeValue),
  debounceMS(debounceMS),
  lastTime(0),
  lastValue(activeValue == HIGH ? LOW : HIGH),
  waiting(false)
{
  pinMode(pin, mode);
}

template<DebouncedInput *input> void DebouncedInput::isr(void) {
  input->poll();
}

void DebouncedInput::onInput(InputListenerFunction fn) {
  this->inputListener = fn;
}

void DebouncedInput::poll() {
  // Ignore inputs during the debounce window
  if (waiting) {
    long now = millis();
    if (now - lastTime >= debounceMS)  {
      waiting = false;
    } else {
      return;
    }
  }

  int inputValue = digitalRead(pin);

  if (inputValue == lastValue) {
    // The input is held down; ignore it
    return;
  }

  lastValue = inputValue;

  if (inputValue != activeValue) {
    return;
  }

  if (inputListener) {
    this->inputListener();
  }

  waiting = true;
  lastTime = millis();
}

#endif
