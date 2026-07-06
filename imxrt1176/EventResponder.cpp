/* EventResponder - Simple event-based programming for Arduino
 * Copyright 2017 Paul Stoffregen
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * imxrt1176: minimal EventResponder implementation — yield-deferred and
 * immediate paths only. Ported verbatim from the Teensy 4 core's
 * EventTypeYield logic; the EventTypeInterrupt (PendSV) and MillisTimer
 * machinery are intentionally omitted (attachInterrupt() falls back to
 * attachImmediate() in EventResponder.h, so no software-interrupt list is
 * ever needed).
 */

#include <Arduino.h>
#include "EventResponder.h"

EventResponder * EventResponder::firstYield = nullptr;
EventResponder * EventResponder::lastYield = nullptr;
bool EventResponder::runningFromYield = false;

void EventResponder::triggerEventNotImmediate()
{
	bool irq = disableInterrupts();
	if (_triggered == false) {
		if (_type == EventTypeYield) {
			if (firstYield == nullptr) {
				_next = nullptr;
				_prev = nullptr;
				firstYield = this;
				lastYield = this;
			} else {
				_next = nullptr;
				_prev = lastYield;
				_prev->_next = this;
				lastYield = this;
			}
		}
		_triggered = true;
	}
	enableInterrupts(irq);
}

bool EventResponder::clearEvent()
{
	bool ret = false;
	bool irq = disableInterrupts();
	if (_triggered) {
		if (_type == EventTypeYield) {
			if (_prev) {
				_prev->_next = _next;
			} else {
				firstYield = _next;
			}
			if (_next) {
				_next->_prev = _prev;
			} else {
				lastYield = _prev;
			}
		}
		_triggered = false;
		ret = true;
	}
	enableInterrupts(irq);
	return ret;
}

// this detach must be called with interrupts disabled
void EventResponder::detachNoInterrupts()
{
	if (_type == EventTypeYield) {
		if (_triggered) {
			if (_prev) {
				_prev->_next = _next;
			} else {
				firstYield = _next;
			}
			if (_next) {
				_next->_prev = _prev;
			} else {
				lastYield = _prev;
			}
		}
		_type = EventTypeDetached;
	}
}
