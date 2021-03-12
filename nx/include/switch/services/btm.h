/**
 * @file btm.h
 * @brief btm service IPC wrapper.
 * @note See also: https://switchbrew.org/wiki/BTM_services
 * @author yellows8
 */
#pragma once
#include "../types.h"
#include "../kernel/event.h"
#include "../services/btdrv_types.h"
#include "../sf/service.h"

/// HostDeviceProperty
typedef struct {
    BtdrvAddress addr;         ///< Same as BtdrvAdapterProperty::addr.
    u8 cod[0x3];               ///< Same as BtdrvAdapterProperty::DeviceClass.
    char name[0x20];           ///< Same as BtdrvAdapterProperty::name (except the last byte which is always zero).
    u8 type6;                  ///< Same as BtdrvAdapterProperty::type6.
} BtmHostDeviceProperty;

typedef struct {
    BtdrvAddress address;
    u8 pad[2];
    u32 unk_x8;
    char name[0x20];
    u8 unk_x2c[0x1c];
    u16 vid;
    u16 pid;
    u8 _unk2[0x20];
} BtmConnectedDevice;

/* 1.0.0 - 5.0.2 */
typedef struct {
    u32 unk_x0;
    u32 unk_x4;
    u8 unk_x8;
    u8 unk_x9;
    u8 max_count;
    u8 connected_count;
    BtmConnectedDevice devices[8];
} BtmDeviceConditionV100;

/* 5.1.0 - 7.0.1 */
typedef struct {
    u32 unk_x0;
    u32 unk_x4;
    u8 unk_x8;
    u8 unk_x9[2];
    u8 max_count;
    u8 connected_count;
    u8 pad[3];
    BtmConnectedDevice devices[8];
} BtmDeviceConditionV510;

/* 8.0.0 - 8.1.1 */
typedef BtmDeviceConditionV100 BtmDeviceConditionV800;

/* 9.0.0+ */
typedef struct {
    u32 unk_x0;
    u8 unk_x4;
    u8 unk_x5;
    u8 max_count;
    u8 connected_count;
    BtmConnectedDevice devices[8];
} BtmDeviceConditionV900;

/// DeviceCondition
typedef union {
    BtmDeviceConditionV100 v100;
    BtmDeviceConditionV510 v510;
    BtmDeviceConditionV800 v800;
    BtmDeviceConditionV900 v900;
} BtmDeviceCondition;

/// DeviceSlotModeList
typedef struct {
    u8 unk_x0[0x64];              ///< Unknown
} BtmDeviceSlotModeList;

/// DeviceInfo
typedef struct {
    BtdrvAddress address;
    u8 device_class[0x3];
    char name[0x15];
    u8 _unk0[0xb];
    u8 link_key[0x10];
    u8 _unk1[0x7];
    u16 vid;
    u16 pid;
    u8 _unk2[0x1c];
} BtmDeviceInfo;

/// DeviceInfoList
typedef struct {
    u8 total_entries;             ///< Total entries.
    u8 pad[3];                    ///< Padding
    BtmDeviceInfo devices[10];    ///< Array of \ref BtmDeviceInfo with the above entry total.
} BtmDeviceInfoList;

/// DevicePropertyList
typedef struct {
    u8 unk_x0[0x268];             ///< Unknown
} BtmDevicePropertyList;

/// ZeroRetransmissionList
typedef struct {
    u8 unk_x0[0x11];              ///< Unknown
} BtmZeroRetransmissionList;

/// GattClientConditionList
typedef struct {
    u8 unk_x0[0x74];              ///< Unknown
} BtmGattClientConditionList;

/// GattService
typedef struct {
    u8 unk_x0[0x4];               ///< Unknown
    BtdrvGattAttributeUuid uuid;  ///< \ref BtdrvGattAttributeUuid
    u16 handle;                   ///< Handle
    u8 unk_x1A[0x2];              ///< Unknown
    u16 instance_id;              ///< InstanceId
    u16 end_group_handle;         ///< EndGroupHandle
    u8 primary_service;           ///< PrimaryService
    u8 pad[3];                    ///< Padding
} BtmGattService;

/// GattCharacteristic
typedef struct {
    u8 unk_x0[0x4];               ///< Unknown
    BtdrvGattAttributeUuid uuid;  ///< \ref BtdrvGattAttributeUuid
    u16 handle;                   ///< Handle
    u8 unk_x1A[0x2];              ///< Unknown
    u16 instance_id;              ///< InstanceId
    u8 properties;                ///< Properties
    u8 unk_x1F[0x5];              ///< Unknown
} BtmGattCharacteristic;

/// GattDescriptor
typedef struct {
    u8 unk_x0[0x4];               ///< Unknown
    BtdrvGattAttributeUuid uuid;  ///< \ref BtdrvGattAttributeUuid
    u16 handle;                   ///< Handle
    u8 unk_x1A[0x6];              ///< Unknown
} BtmGattDescriptor;

/// BleDataPath
typedef struct {
    u8 unk_x0;                    ///< Unknown
    u8 pad[3];                    ///< Padding
    BtdrvGattAttributeUuid uuid;  ///< \ref BtdrvGattAttributeUuid
} BtmBleDataPath;

/// Initialize btm.
Result btmInitialize(void);

/// Exit btm.
void btmExit(void);

/// Gets the Service object for the actual btm service session.
Service* btmGetServiceSession(void);

/**
 * @brief GetState
 * @param[out] out Output BtmState.
 */
Result btmGetState(u32 *out);

/**
 * @brief GetHostDeviceProperty
 * @param[out] out \ref BtmHostDeviceProperty
 */
Result btmGetHostDeviceProperty(BtmHostDeviceProperty *out);

/**
 * @brief AcquireDeviceConditionEvent
 * @note The Event must be closed by the user once finished with it.
 * @param[out] out_event Output Event with autoclear=true.
 */
Result btmAcquireDeviceConditionEvent(Event* out_event);

/**
 * @brief GetDeviceCondition
 * @param[out] out \ref BtmDeviceCondition
 */
Result btmGetDeviceCondition(BtmDeviceCondition *out);

/**
 * @brief SetBurstMode
 * @param[in] addr \ref BtdrvAddress
 * @param[in] flag Flag
 */
Result btmSetBurstMode(BtdrvAddress addr, bool flag);

/**
 * @brief SetSlotMode
 * @param[in] list \ref BtmDeviceSlotModeList
 */
Result btmSetSlotMode(const BtmDeviceSlotModeList *list);

/**
 * @brief SetBluetoothMode
 * @note Only available on pre-9.0.0.
 * @param[in] mode BluetoothMode
 */
Result btmSetBluetoothMode(u32 mode);

/**
 * @brief SetWlanMode
 * @param[in] mode WlanMode
 */
Result btmSetWlanMode(u32 mode);

/**
 * @brief AcquireDeviceInfoEvent
 * @note The Event must be closed by the user once finished with it.
 * @param[out] out_event Output Event with autoclear=true.
 */
Result btmAcquireDeviceInfoEvent(Event* out_event);

/**
 * @brief GetDeviceInfo
 * @param[out] out \ref BtmDeviceInfoList
 */
Result btmGetDeviceInfo(BtmDeviceInfoList *out);

/**
 * @brief AddDeviceInfo
 * @param[in] info \ref BtmDeviceInfo
 */
Result btmAddDeviceInfo(const BtmDeviceInfo *info);

/**
 * @brief RemoveDeviceInfo
 * @param[in] addr \ref BtdrvAddress
 */
Result btmRemoveDeviceInfo(BtdrvAddress addr);

/**
 * @brief IncreaseDeviceInfoOrder
 * @param[in] addr \ref BtdrvAddress
 */
Result btmIncreaseDeviceInfoOrder(BtdrvAddress addr);

/**
 * @brief LlrNotify
 * @param[in] addr \ref BtdrvAddress
 * @param[in] unk [9.0.0+] Unknown
 */
Result btmLlrNotify(BtdrvAddress addr, s32 unk);

/**
 * @brief EnableRadio
 */
Result btmEnableRadio(void);

/**
 * @brief DisableRadio
 */
Result btmDisableRadio(void);

/**
 * @brief HidDisconnect
 * @param[in] addr \ref BtdrvAddress
 */
Result btmHidDisconnect(BtdrvAddress addr);

/**
 * @brief HidSetRetransmissionMode
 * @param[in] addr \ref BtdrvAddress
 * @param[in] list \ref BtmZeroRetransmissionList
 */
Result btmHidSetRetransmissionMode(BtdrvAddress addr, const BtmZeroRetransmissionList *list);

/**
 * @brief AcquireAwakeReqEvent
 * @note Only available on [2.0.0+].
 * @note The Event must be closed by the user once finished with it.
 * @param[out] out_event Output Event with autoclear=true.
 */
Result btmAcquireAwakeReqEvent(Event* out_event);


/**
 * @brief AcquireLlrStateEvent
 * @note Only available on [4.0.0+].
 * @note The Event must be closed by the user once finished with it.
 * @param[out] out_event Output Event with autoclear=true.
 */
Result btmAcquireLlrStateEvent(Event* out_event);

/**
 * @brief IsLlrStarted
 * @note Only available on [4.0.0+].
 * @param[out] out Output flag.
 */
Result btmIsLlrStarted(bool *out);

/**
 * @brief EnableSlotSaving
 * @note Only available on [4.0.0+].
 * @param[in] flag Flag
 */
Result btmEnableSlotSaving(bool flag);

/**
 * @brief ProtectDeviceInfo
 * @note Only available on [5.0.0+].
 * @param[in] addr \ref BtdrvAddress
 * @param[in] flag Flag
 */
Result btmProtectDeviceInfo(BtdrvAddress addr, bool flag);

/**
 * @brief AcquireBleScanEvent
 * @note Only available on [5.0.0+].
 * @note The Event must be closed by the user once finished with it.
 * @param[out] out_event Output Event with autoclear=true.
 */
Result btmAcquireBleScanEvent(Event* out_event);

/**
 * @brief GetBleScanParameterGeneral
 * @note Only available on [5.1.0+].
 * @param[in] parameter_id Must be value 0x1 or 0xFFFF.
 * @param[out] out \ref BtdrvBleAdvertisePacketParameter
 */
Result btmGetBleScanParameterGeneral(u16 parameter_id, BtdrvBleAdvertisePacketParameter *out);

/**
 * @brief GetBleScanParameterSmartDevice
 * @note Only available on [5.1.0+].
 * @param[in] parameter_id Must be value 0x2.
 * @param[out] out \ref BtdrvGattAttributeUuid. The first 4-bytes is always 0.
 */
Result btmGetBleScanParameterSmartDevice(u16 parameter_id, BtdrvGattAttributeUuid *out);

/**
 * @brief StartBleScanForGeneral
 * @note Only available on [5.1.0+].
 * @param[in] param \ref BtdrvBleAdvertisePacketParameter
 */
Result btmStartBleScanForGeneral(BtdrvBleAdvertisePacketParameter param);

/**
 * @brief StopBleScanForGeneral
 * @note Only available on [5.1.0+].
 */
Result btmStopBleScanForGeneral(void);

/**
 * @brief GetBleScanResultsForGeneral
 * @note Only available on [5.1.0+].
 * @param[out] results Output array of \ref BtdrvBleScanResult.
 * @param[in] count Size of the results array in entries. The max is 10.
 * @param[out] total_out Total output entries.
 */
Result btmGetBleScanResultsForGeneral(BtdrvBleScanResult *results, u8 count, u8 *total_out);

/**
 * @brief StartBleScanForPaired
 * @note Only available on [5.1.0+].
 * @param[in] param \ref BtdrvBleAdvertisePacketParameter
 */
Result btmStartBleScanForPaired(BtdrvBleAdvertisePacketParameter param);

/**
 * @brief StopBleScanForPaired
 * @note Only available on [5.1.0+].
 */
Result btmStopBleScanForPaired(void);

/**
 * @brief StartBleScanForSmartDevice
 * @note Only available on [5.1.0+].
 * @param[in] uuid \ref BtdrvGattAttributeUuid
 */
Result btmStartBleScanForSmartDevice(const BtdrvGattAttributeUuid *uuid);

/**
 * @brief StopBleScanForSmartDevice
 * @note Only available on [5.1.0+].
 */
Result btmStopBleScanForSmartDevice(void);

/**
 * @brief GetBleScanResultsForSmartDevice
 * @note Only available on [5.1.0+].
 * @param[out] results Output array of \ref BtdrvBleScanResult.
 * @param[in] count Size of the results array in entries. The max is 10.
 * @param[out] total_out Total output entries.
 */
Result btmGetBleScanResultsForSmartDevice(BtdrvBleScanResult *results, u8 count, u8 *total_out);

/**
 * @brief AcquireBleConnectionEvent
 * @note Only available on [5.1.0+].
 * @note The Event must be closed by the user once finished with it.
 * @param[out] out_event Output Event with autoclear=true.
 */
Result btmAcquireBleConnectionEvent(Event* out_event);

/**
 * @brief BleConnect
 * @note Only available on [5.0.0+].
 * @note The \ref BtdrvAddress must not be already connected. A maximum of 4 devices can be connected.
 * @param[in] addr \ref BtdrvAddress
 */
Result btmBleConnect(BtdrvAddress addr);

/**
 * @brief BleOverrideConnection
 * @note Only available on [5.1.0+].
 * @param[in] id Same as \ref btmBleDisconnect.
 */
Result btmBleOverrideConnection(u32 id);

/**
 * @brief BleDisconnect
 * @note Only available on [5.0.0+].
 * @param[in] connection_handle This must match a BtdrvBleConnectionInfo::id from \ref btmBleGetConnectionState. [5.1.0+] 0xFFFFFFFF is invalid.
 */
Result btmBleDisconnect(u32 connection_handle);

/**
 * @brief BleGetConnectionState
 * @note Only available on [5.0.0+].
 * @param[out] info Output array of \ref BtdrvBleConnectionInfo.
 * @param[in] count Size of the info array in entries. Other cmds which use this internally use count=4.
 * @param[out] total_out Total output entries.
 */
Result btmBleGetConnectionState(BtdrvBleConnectionInfo *info, u8 count, u8 *total_out);

/**
 * @brief BleGetGattClientConditionList
 * @note Only available on [5.0.0+].
 * @param[out] list \ref BtmGattClientConditionList
 */
Result btmBleGetGattClientConditionList(BtmGattClientConditionList *list);

/**
 * @brief AcquireBlePairingEvent
 * @note Only available on [5.0.0+].
 * @note The Event must be closed by the user once finished with it.
 * @param[out] out_event Output Event with autoclear=true.
 */
Result btmAcquireBlePairingEvent(Event* out_event);

/**
 * @brief BlePairDevice
 * @note Only available on [5.1.0+].
 * @param[in] connection_handle Same as \ref btmBleDisconnect.
 * @param[in] param \ref BtdrvBleAdvertisePacketParameter
 */
Result btmBlePairDevice(u32 connection_handle, BtdrvBleAdvertisePacketParameter param);

/**
 * @brief BleUnpairDeviceOnBoth
 * @note Only available on [5.1.0+].
 * @param[in] connection_handle Same as \ref btmBleDisconnect.
 * @param[in] param \ref BtdrvBleAdvertisePacketParameter
 */
Result btmBleUnpairDeviceOnBoth(u32 connection_handle, BtdrvBleAdvertisePacketParameter param);

/**
 * @brief BleUnPairDevice
 * @note Only available on [5.1.0+].
 * @param[in] addr \ref BtdrvAddress
 * @param[in] param \ref BtdrvBleAdvertisePacketParameter
 */
Result btmBleUnPairDevice(BtdrvAddress addr, BtdrvBleAdvertisePacketParameter param);

/**
 * @brief BleGetPairedAddresses
 * @note Only available on [5.1.0+].
 * @param[in] param \ref BtdrvBleAdvertisePacketParameter
 * @param[out] addrs Output array of \ref BtdrvAddress.
 * @param[in] count Size of the addrs array in entries.
 * @param[out] total_out Total output entries. The max is 10.
 */
Result btmBleGetPairedAddresses(BtdrvBleAdvertisePacketParameter param, BtdrvAddress *addrs, u8 count, u8 *total_out);

/**
 * @brief AcquireBleServiceDiscoveryEvent
 * @note Only available on [5.1.0+].
 * @note The Event must be closed by the user once finished with it.
 * @param[out] out_event Output Event with autoclear=true.
 */
Result btmAcquireBleServiceDiscoveryEvent(Event* out_event);

/**
 * @brief GetGattServices
 * @note Only available on [5.0.0+].
 * @param[in] connection_handle Same as \ref btmBleDisconnect.
 * @param[out] services Output array of \ref BtmGattService.
 * @param[in] count Size of the services array in entries. The max is 100.
 * @param[out] total_out Total output entries.
 */
Result btmGetGattServices(u32 connection_handle, BtmGattService *services, u8 count, u8 *total_out);

/**
 * @brief Same as \ref btmGetGattServices except this only returns the \ref BtmGattService which matches the input \ref BtdrvGattAttributeUuid.
 * @note Only available on [5.0.0+].
 * @param[in] connection_handle Same as \ref btmBleDisconnect.
 * @param[in] uuid \ref BtdrvGattAttributeUuid
 * @param[out] service \ref BtmGattService
 * @param[out] flag Whether a \ref BtmGattService was returned.
 */
Result btmGetGattService(u32 connection_handle, const BtdrvGattAttributeUuid *uuid, BtmGattService *service, bool *flag);

/**
 * @brief Same as \ref btmGetGattServices except this only returns \ref BtmGattService entries where various checks pass with u16 fields.
 * @note Only available on [5.0.0+].
 * @param[in] connection_handle Same as \ref btmBleDisconnect.
 * @param[in] service_handle ServiceHandle
 * @param[out] services \ref BtmGattService
 * @param[in] count Size of the services array in entries. The max is 100.
 * @param[out] out Output value.
 */
Result btmGetGattIncludedServices(u32 connection_handle, u16 service_handle, BtmGattService *services, u8 count, u8 *out);

/**
 * @brief This is similar to \ref btmGetGattIncludedServices except this only returns 1 \ref BtmGattService.
 * @note Only available on [5.0.0+].
 * @param[in] connection_handle Same as \ref btmBleDisconnect.
 * @param[in] attribute_handle AttributeHandle
 * @param[out] service \ref BtmGattService
 * @param[out] flag Whether a \ref BtmGattService was returned.
 */
Result btmGetBelongingService(u32 connection_handle, u16 attribute_handle, BtmGattService *service, bool *flag);

/**
 * @brief GetGattCharacteristics
 * @note Only available on [5.0.0+].
 * @param[in] connection_handle Same as \ref btmBleDisconnect.
 * @param[in] service_handle This controls which \ref BtmGattCharacteristic entries to return.
 * @param[out] characteristics \ref BtmGattCharacteristic
 * @param[in] count Size of the characteristics array in entries. The max is 100.
 * @param[out] total_out Total output entries.
 */
Result btmGetGattCharacteristics(u32 connection_handle, u16 service_handle, BtmGattCharacteristic *characteristics, u8 count, u8 *total_out);

/**
 * @brief GetGattDescriptors
 * @note Only available on [5.0.0+].
 * @param[in] connection_handle Same as \ref btmBleDisconnect.
 * @param[in] char_handle Characteristic handle. This controls which \ref BtmGattDescriptor entries to return.
 * @param[out] descriptors \ref BtmGattDescriptor
 * @param[in] count Size of the descriptors array in entries. The max is 100.
 * @param[out] total_out Total output entries.
 */
Result btmGetGattDescriptors(u32 connection_handle, u16 char_handle, BtmGattDescriptor *descriptors, u8 count, u8 *total_out);

/**
 * @brief AcquireBleMtuConfigEvent
 * @note Only available on [5.0.0+].
 * @note The Event must be closed by the user once finished with it.
 * @param[out] out_event Output Event with autoclear=true.
 */
Result btmAcquireBleMtuConfigEvent(Event* out_event);

/**
 * @brief ConfigureBleMtu
 * @note Only available on [5.0.0+].
 * @param[in] connection_handle Same as \ref btmBleDisconnect.
 * @param[in] mtu MTU
 */
Result btmConfigureBleMtu(u32 connection_handle, u16 mtu);

/**
 * @brief GetBleMtu
 * @note Only available on [5.0.0+].
 * @param[in] connection_handle Same as \ref btmBleDisconnect.
 * @param[out] out Output MTU.
 */
Result btmGetBleMtu(u32 connection_handle, u16 *out);

/**
 * @brief RegisterBleGattDataPath
 * @note Only available on [5.0.0+].
 * @param[in] path \ref BtmBleDataPath
 */
Result btmRegisterBleGattDataPath(const BtmBleDataPath *path);

/**
 * @brief UnregisterBleGattDataPath
 * @note Only available on [5.0.0+].
 * @param[in] path \ref BtmBleDataPath
 */
Result btmUnregisterBleGattDataPath(const BtmBleDataPath *path);

/**
 * @brief RegisterAppletResourceUserId
 * @note Only available on [5.0.0+].
 * @param[in] AppletResourceUserId AppletResourceUserId
 * @param[in] unk Unknown
 */
Result btmRegisterAppletResourceUserId(u64 AppletResourceUserId, u32 unk);

/**
 * @brief UnregisterAppletResourceUserId
 * @note Only available on [5.0.0+].
 * @param[in] AppletResourceUserId AppletResourceUserId
 */
Result btmUnregisterAppletResourceUserId(u64 AppletResourceUserId);

/**
 * @brief SetAppletResourceUserId
 * @note Only available on [5.0.0+].
 * @param[in] AppletResourceUserId AppletResourceUserId
 */
Result btmSetAppletResourceUserId(u64 AppletResourceUserId);

