/**
 * @file btmdbg.h
 * @brief BTM (btm:dbg) service IPC wrapper.
 * @author ndeadly
 * @copyright libnx Authors
 */

#pragma once
#include "../types.h"
#include "../kernel/event.h"
#include "../services/btdrv.h"

/// Initialize btm:dbg.
Result btmdbgInitialize(void);
/// Exit btm:dbg.
void btmdbgExit(void);
/// Gets the Service object for the actual btm:dbg service session.
Service* btmdbgGetServiceSession(void);

Result btmdbgAcquireDiscoveryEvent(Event *event);
Result btmdbgStartDiscovery(void);
Result btmdbgCancelDiscovery(void);
//Result btmdbgGetDeviceProperty();
Result btmdbgCreateBond(const BluetoothAddress *address);
Result btmdbgCancelBond(const BluetoothAddress *address);
//Result btmdbgSetTsiMode();
//Result btmdbgGeneralTest();
Result btmdbgHidConnect(const BluetoothAddress *address);
//Result btmdbgGeneralGet();
//Result btmdbgGetGattClientDisconnectionReason();
//Result btmdbgGetBleConnectionParameter();
//Result btmdbgGetBleConnectionParameterRequest();