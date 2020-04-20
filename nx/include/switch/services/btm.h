/**
 * @file btm.h
 * @brief BTM (btm) service IPC wrapper.
 * @author ndeadly
 * @copyright libnx Authors
 */

#pragma once
#include "../types.h"
#include "../kernel/event.h"
#include "../services/btdrv.h"

typedef struct {
    BluetoothAddress        address;
    BluetoothDeviceClass    cod;
    BluetoothLocalName      name;
    u8                      _unk;
} BtmHostDeviceProperty;

typedef struct {
    BluetoothAddress        address;
    u16                     _pad0;
    u32                     _unk0;
    BluetoothLocalName      name;
    u8                      _unk1[0x1c];
    u16                     vid;
    u16                     pid;
    u8                      _unk2[0x20];
} BtmConnectedDevice;

typedef struct {
    u32                 _unk0;
    bool                _unk1;
    bool                _unk2;
    u8                  paired_count;
    u8                  connected_count;
    BtmConnectedDevice  devices[8];
} BtmDeviceCondition;

typedef struct {
    BluetoothAddress        address;
    BluetoothDeviceClass    device_class;
    BluetoothRemoteName     name;
    u8                      _unk0[0xb];
    BluetoothLinkKey        link_key;
    u8                      _unk1[0x7];
    u16                     vid;
    u16                     pid;
    u8                      _unk2[0x1c];
} BtmDeviceInfo;

typedef struct {
    u32 count;
    BtmDeviceInfo devices[10];
} BtmPairedDevices;

/// Initialize btm.
Result btmInitialize(void);
/// Exit btm/
void btmExit(void);
/// Gets the Service object for the actual btm service session.
Service* btmGetServiceSession(void);

//Result btmGetState();
Result btmGetHostDeviceProperty(BtmHostDeviceProperty *property);
Result btmAcquireDeviceConditionEvent(Event *event, u8 *flags);
Result btmGetDeviceCondition(BtmDeviceCondition *condition);
//Result btmSetBurstMode();
//Result btmSetSlotMode();
//Result btmSetBluetoothMode();
//Result btmSetWlanMode();
Result btmAcquireDeviceInfoEvent(Event *event, u8 *flags);
Result btmGetDeviceInfo(BtmPairedDevices *devices);
Result btmAddDeviceInfo(const BtmDeviceInfo *info);
Result btmRemoveDeviceInfo(const BluetoothAddress *address);
Result btmIncreaseDeviceInfoOrder(const BluetoothAddress *address);
//Result btmLlrNotify();
Result btmEnableRadio(void);
Result btmDisableRadio(void);
Result btmHidDisconnect(const BluetoothAddress *address);
//Result btmHidSetRetransmissionMode();
Result btmAcquireAwakeReqEvent(Event *event, u8 *flags);

Result btmProtectDeviceInfo(const BluetoothAddress *address, u8 unk);
