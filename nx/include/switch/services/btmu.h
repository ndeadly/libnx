/**
 * @file btmu.h
 * @brief BTM (btm:u) service IPC wrapper.
 * @author ndeadly
 * @copyright libnx Authors
 */
#pragma once
#include "../types.h"
#include "../kernel/event.h"
#include "../sf/service.h"

Result btmuInitialize(void);
void btmuExit(void);
Service* btmuGetServiceSession(void);

Result btmuAcquireBleScanEvent(Event *event, u8 *flags);
Result btmuAcquireBleConnectionEvent(Event *event, u8 *flags);
Result btmuAcquireBlePairingEvent(Event *event, u8 *flags);
Result btmuAcquireBleServiceDiscoveryEvent(Event *event, u8 *flags);
Result btmuAcquireBleMtuConfigEvent(Event *event, u8 *flags);
