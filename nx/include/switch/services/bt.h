/**
 * @file bt.h
 * @brief Bluetooth (bt) service IPC wrapper.
 * @author ndeadly
 * @copyright libnx Authors
 */

#pragma once
#include "../types.h"
#include "../kernel/event.h"
#include "../services/btdrv.h"

/// Initialize bt.
Result btInitialize(void);
/// Exit bt.
void btExit(void);
/// Gets the Service object for the actual bt service session.
Service* btGetServiceSession(void);

Result btLeClientReadCharacteristic(u32 connId, const GattId *svcId, bool primary, const GattId *charId, u8 auth);
Result btLeClientReadDescriptor(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const GattId *descrId, u8 auth);
Result btLeClientWriteCharacteristic(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const void *data, u32 length, u8 auth, bool response);
Result btLeClientWriteDescriptor(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const GattId *descrId, const void *data, u32 length, u8 auth);
Result btLeClientRegisterNotification(u32 connId, const GattId *svcId, bool primary, const GattId *charId);
Result btLeClientDeregisterNotification(u32 connId, const GattId *svcId, bool primary, const GattId *charId);
Result btSetLeResponse(u8 a, const GattAttributeUuid *b, const GattAttributeUuid *c, const void *data, u32 length);
Result btLeSendIndication(u8 a, const GattAttributeUuid *b, const GattAttributeUuid *c, const void *data, u32 length, bool f);
Result btGetLeEventInfo(BleEventType *type, u8 *buffer, u16 length);
Result btRegisterBleEvent(Event *event);
