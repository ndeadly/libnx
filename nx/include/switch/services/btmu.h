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
#include "../services/btdrv.h"

typedef struct  {
    u8 unk[0x08];
} BleAdvertisePacketParameter;

typedef struct {} BleScanResult;

typedef struct {} BleConnectionInfo;

typedef struct {
    u8 unk[0x24];
} GattService;

typedef struct {} GattCharacteristic;

typedef struct {} GattDescriptor;

typedef struct {
    u8 unk[0x18];
} BleDataPath;

Result btmuInitialize(void);
void btmuExit(void);
Service* btmuGetServiceSession(void);

Result btmuAcquireBleScanEvent(Event *event, u8 *flags);
Result btmuGetBleScanFilterParameter(BleAdvertisePacketParameter *param, u16 unk);
Result btmuGetBleScanFilterParameter2(GattAttributeUuid *uuid, u16 unk);
Result btmuStartBleScanForGeneral(const BleAdvertisePacketParameter *param);
Result btmuStopBleScanForGeneral(void);
Result btmuGetBleScanResultsForGeneral(BleScanResult *result, u8 unk);
Result btmuStartBleScanForPaired(const BleAdvertisePacketParameter *param);
Result btmuStopBleScanForPaired(void);
Result btmuStartBleScanForSmartDevice(const GattAttributeUuid *uuid);
Result btmuStopBleScanForSmartDevice(void);
Result btmuGetBleScanResultsForSmartDevice(BleScanResult *results, u8 unk);
Result btmuAcquireBleConnectionEvent(Event *event, u8 *flags);
Result btmuBleConnect(const BluetoothAddress *address);
Result btmuBleDisconnect(u32 unk);
Result btmuBleGetConnectionState(BleConnectionInfo *info, u8 unk);
Result btmuAcquireBlePairingEvent(Event *event, u8 *flags);
Result btmuBlePairDevice(u32 unk, const BleAdvertisePacketParameter *param);
Result btmuBleUnPairDevice(u32 unk, const BleAdvertisePacketParameter *param);
Result btmuBleUnPairDevice2(const BluetoothAddress *address, const BleAdvertisePacketParameter *param);
Result btmuBleGetPairedDevices(BluetoothAddress *address, u8 unk, const BleAdvertisePacketParameter *param);
Result btmuAcquireBleServiceDiscoveryEvent(Event *event, u8 *flags);
Result btmuGetGattServices(GattService *service, u8 unk1, u32 unk2);
Result btmuGetGattService(GattService *service, u32 unk, const GattAttributeUuid *uuid);
Result btmuGetGattIncludedServices(GattService *service, u8 unk1, u32 unk2, u16 unk3);
Result btmuGetBelongingGattService(GattService *service, u32 unk1, u16 unk2);
Result btmuGetGattCharacteristics(GattCharacteristic *characteristics, u8 unk1, u32 unk2, u16 unk3);
Result btmuGetGattDescriptors(GattDescriptor *descriptors, u8 unk1, u32 unk2, u16 unk3);
Result btmuAcquireBleMtuConfigEvent(Event *event, u8 *flags);
Result btmuConfigureBleMtu(u32 unk1, u16 unk2);
Result btmuGetBleMtu(u32 unk);
Result btmuRegisterBleGattDataPath(const BleDataPath *datapath);
Result btmuUnregisterBleGattDataPath(const BleDataPath *datapath);
