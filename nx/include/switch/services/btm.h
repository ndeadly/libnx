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

/* 1.0.0 - 5.0.2 */
typedef struct {
    u32                 _unk0;
    u32                 _unk1;
    u8                  _unk2;
    u8                  _unk3;
    u8                  max_count;
    u8                  connected_count;
    BtmConnectedDevice  devices[8];
} BtmDeviceConditionV100;

/* 5.1.0 - 7.0.1 */
typedef struct {
    u32                 _unk0;
    u32                 _unk1;
    u8                  _unk2;
    u8                  _unk3[2];
    u8                  max_count;
    u8                  connected_count;
    u8                  pad[3];
    BtmConnectedDevice  devices[8];
} BtmDeviceConditionV510;

/* 8.0.0 - 8.1.1 */
typedef BtmDeviceConditionV100 BtmDeviceConditionV800;

/* 9.0.0+ */
typedef struct {
    u32                 _unk0;
    u8                  _unk1;
    u8                  _unk2;
    u8                  max_count;
    u8                  connected_count;
    BtmConnectedDevice  devices[8];
} BtmDeviceConditionV900;

typedef union {
    BtmDeviceConditionV100 v100;
    BtmDeviceConditionV510 v510;
    BtmDeviceConditionV800 v800;
    BtmDeviceConditionV900 v900;
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
} BtmDevice;

typedef struct {
    u32 count;
    BtmDevice devices[10];
} BtmDeviceInfo;

/// Initialize btm.
Result btmInitialize(void);
/// Exit btm/
void btmExit(void);
/// Gets the Service object for the actual btm service session.
Service* btmGetServiceSession(void);

Result btmGetState(u32 *state);
Result btmGetHostDeviceProperty(BtmHostDeviceProperty *property);
Result btmAcquireDeviceConditionEvent(Event *event, u8 *flags);
Result btmGetDeviceCondition(BtmDeviceCondition *condition);
//Result btmSetBurstMode();
//Result btmSetSlotMode();
//Result btmSetBluetoothMode();
//Result btmSetWlanMode();
Result btmAcquireDeviceInfoEvent(Event *event, u8 *flags);
Result btmGetDeviceInfo(BtmDeviceInfo *devices);
Result btmAddDeviceInfo(const BtmDevice *info);
Result btmRemoveDeviceInfo(const BluetoothAddress *address);
Result btmIncreaseDeviceInfoOrder(const BluetoothAddress *address);
//Result btmLlrNotify();
Result btmEnableRadio(void);
Result btmDisableRadio(void);
Result btmHidDisconnect(const BluetoothAddress *address);
//Result btmHidSetRetransmissionMode();
Result btmAcquireAwakeReqEvent(Event *event, u8 *flags);

Result btmProtectDeviceInfo(const BluetoothAddress *address, u8 unk);
