/**
 * @file btmsys.h
 * @brief BTM (btm:sys) service IPC wrapper.
 * @author ndeadly
 * @copyright libnx Authors
 */
#pragma once
#include "../types.h"
#include "../kernel/event.h"
#include "../sf/service.h"

Result btmsysInitialize(void);
void btmsysExit(void);
Service* btmsysGetServiceSession(void);

Result btmsysStartGamepadPairing(void);
Result btmsysCancelGamepadPairing(void);
Result btmsysClearGamepadPairingDatabase(void);
Result btmsysGetPairedGamepadCount(u8 *count);
Result btmsysEnableRadio(void);
Result btmsysDisableRadio(void);
Result btmsysGetRadioOnOff(bool *state);
Result btmsysAcquireRadioEvent(Event *event, u8 *flags);
Result btmsysAcquireGamepadPairingEvent(Event *event, u8 *flags);
Result btmsysIsGamepadPairingStarted(bool *started);
