/**
 * @file btdrv.h
 * @brief Bluetooth (btdrv) service IPC wrapper.
 * @author ndeadly
 * @copyright libnx Authors
 */

#pragma once
#include "../types.h"
#include "../kernel/event.h"

#define BLUETOOTH_SHAREDMEM_SIZE 0x3000
#define BLUETOOTH_CIRCBUFFER_SIZE 10000

typedef enum  {
    BluetoothEvent_AdapterStateChanged,
    BluetoothEvent_AdapterProperties,
    BluetoothEvent_RemoteDeviceProperties,
    BluetoothEvent_DeviceFound,
    BluetoothEvent_DiscoveryStateChanged,
    BluetoothEvent_PinRequest,
    BluetoothEvent_SspRequest,
    BluetoothEvent_BondStateChanged,
    BluetoothEvent_AclStateChanged,
    BluetoothEvent_ThreadEvent,
    BluetoothEvent_DutModeReceive,
    BluetoothEvent_LeTestMode,
    BluetoothEvent_EnergyInfo
} BluetoothEventType;

typedef enum  {
    BluetoothStatus_Success,
    BluetoothStatus_Fail,
    BluetoothStatus_NotReady,
    BluetoothStatus_NoMemory,
    BluetoothStatus_Busy,
    BluetoothStatus_Done,
    BluetoothStatus_Unsupported,
    BluetoothStatus_ParameterInvalid,
    BluetoothStatus_Unhandled,
    BluetoothStatus_AuthenticationFailure,
    BluetoothStatus_RemoteDeviceDown,
    BluetoothStatus_AuthenticationRejected,
    BluetoothStatus_JniEnvironmentError,
    BluetoothStatus_JniThreadAttachError,
    BluetoothStatus_WakelockError
} BluetoothStatus;

typedef enum {
    BluetoothDiscoveryState_Stopped,
    BluetoothDiscoveryState_Started
} BluetoothDiscoveryState;

typedef enum {
    BluetoothBondState_None,
    BluetoothBondState_Bonding,
    BluetoothBondState_Bonded
} BluetoothBondState;

typedef enum  {
    BluetoothProperty_BdName                    = 0x1,
    BluetoothProperty_BdAddress,
    BluetoothProperty_Uuids,
    BluetoothProperty_DeviceClass,
    BluetoothProperty_DeviceType,
    BluetoothProperty_ServiceRecord,
    BluetoothProperty_AdapterScanMode,
    BluetoothProperty_AdapterBondedDevices,
    BluetoothProperty_AdapterDiscoveryTimeout,
    BluetoothProperty_RemoteFriendlyName,
    BluetoothProperty_RemoteRssi,
    BluetoothProperty_RemoteVersionInfo,
    BluetoothProperty_LocalLeFeatures,
    BluetoothProperty_RemoteDeviceTimestamp     = 0xFF,
} BluetoothPropertyType;

typedef enum  {
    BluetoothTransport_Auto,
    BluetoothTransport_BREDR,
    BluetoothTransport_LE
} BluetoothTransport;

typedef enum  {
    BluetoothSspVariant_PasskeyConfirmation,
    BluetoothSspVariant_PasskeyEntry,
    BluetoothSspVariant_Consent,
    BluetoothSspVariant_PasskeyNotification
} BluetoothSspVariant;

typedef enum  {
    HidEvent_ConnectionState,
    HidEvent_HidInfo,
    HidEvent_ProtocolMode,
    HidEvent_IdleTime,
    HidEvent_GetReport,
    HidEvent_VirtualUnplug,
    HidEvent_Handshake,
    HidEvent_Unknown07,
    HidEvent_Unknown08,
    HidEvent_Unknown09,
} BluetoothHidEventType;

/*
typedef enum  {
    HidHostEvent_Open,
    HidHostEvent_Close,
    HidHostEvent_MipStart,
    HidHostEvent_MipStop,
    HidHostEvent_GetReport,
    HidHostEvent_SetReport,
    HidHostEvent_GetProtocol,
    HidHostEvent_SetProtocol,
    HidHostEvent_GetIdle,
    HidHostEvent_SetIdle,
    HidHostEvent_GetDscpInfo,
    HidHostEvent_VcUnplug
} BluetoothHidEventType;
*/

typedef enum {
    HidReportType_InputReport  = 0x1,
    HidReportType_OutputReport,
    HidReportType_FeatureReport
} BluetoothHhReportType;

typedef enum {
    HidStatus_Ok                            = 0,
    HidStatus_HandshakeHidNotReady,
    HidStatus_HandshakeInvalidReportId,
    HidStatus_HandshakeTransactionNotSpt,
    HidStatus_HandshakeInvalidParameter,
    HidStatus_HandshakeError,
    HidStatus_Error,
    HidStatus_ErrorSdp,
    HidStatus_ErrorProtocol,
    HidStatus_ErrorDatabaseFull,
    HidStatus_ErrorDeviceTypeUnsupported,
    HidStatus_ErrorNoResources,
    HidStatus_ErrorAuthenicationFailed,
    HidStatus_ErrorHdl
} BluetoothHidStatus;

typedef enum {
    HidConnectionState_Connected                    = 0,
    HidConnectionState_Connecting,
    HidConnectionState_Disconnected,
    HidConnectionState_Disconnecting,
    HidConnectionState_FailedMouseFromHost,
    HidConnectionState_FailedKeyboardFromHost,
    HidConnectionState_FailedTooManyDevices,
    HidConnectionState_FailedNoBluetoothHidDriver,
    HidConnectionState_FailedGeneric,
    HidConnectionState_Unknown
} BluetoothHidConnectionState;

typedef enum {
    BleEventType_Unknown
} BluetoothBleEventType;

typedef enum {
    BleHidventType_Unknown
} BluetoothBleHidventType;

typedef char BluetoothName[0xf9];
typedef char BluetoothLocalName[0x20];
typedef char BluetoothRemoteName[0x15];
typedef struct { u8 address[0x6]; } BluetoothAddress;
typedef struct { u8 cod[0x3]; } BluetoothDeviceClass;
typedef struct { u8 pincode[0x10]; } BluetoothPinCode;
typedef struct { u8 key[0x10]; } BluetoothLinkKey;

typedef struct {
    BluetoothAddress        address;
    BluetoothDeviceClass    device_class;
    BluetoothName           name;
    u8                      _unk;
} BluetoothAdapterProperty;

typedef struct {
    BluetoothAddress      address;
    BluetoothLocalName    name;
    BluetoothDeviceClass  device_class;
    BluetoothLinkKey      link_key;
    bool                  link_key_present;
    u16                   version;
    u32                   trusted_services;
    u16                   vid;
    u16                   pid;

    // Don't know how much of this is actually correct
    u8                    ble_addr_type;
    u8                    device_type;
    u8                    _unk0[130];
    u8                    key_type;
    u8                    _unk1[100];

    // Pad out to 0x200. This is the size HOS uses
    u8                    _pad[0xd0];
} BluetoothDevicesSettings;

typedef struct {
    u16 size;
    u8  data[0x280];
} BluetoothHidReport;

typedef struct {
    union {
        // Pre 9.0.0
        struct {
            u16                 size;
            u8                  _unk0;      //possibly controller id?
            BluetoothAddress    address;
            u8                  _unk1[3];
        };
        // 9.0.0+
        struct {
            u8                  _unk0[5];
            BluetoothAddress    address;
            u8                  _unk1;
        } v2;
    };
    
    BluetoothHidReport	report;
} BluetoothHidReportData;

typedef union {
    struct __attribute__ ((__packed__)) {
        BluetoothName           name;
        BluetoothAddress        address;
        u8                      uuid[0x10];
        BluetoothDeviceClass    cod;
        /* + more items we don't care about */
        u8  _unk0;
        u8  _unk1[0x252];
        u32 _unk2;
    } deviceFound;
    
    struct {
        BluetoothDiscoveryState state;
    } discoveryState;
    
    struct {
        BluetoothAddress        address;
        BluetoothName           name;
        BluetoothDeviceClass    cod;
    } pinReply;
    
    struct {
        BluetoothAddress        address;
        BluetoothName           name;
        BluetoothDeviceClass    cod;
        BluetoothSspVariant     variant;
        u32                     passkey;
    } sspReply;
    
    union {
        struct {
            BluetoothAddress        address;
            BluetoothStatus         status;
            BluetoothBondState      state;
        };
        struct {
            BluetoothStatus         status;
            BluetoothAddress        address;
            BluetoothBondState      state;
        } v2;
    } bondState;
} BluetoothEventData;

typedef union {
    struct {
        BluetoothAddress            address;
        BluetoothHidConnectionState state;
    } connectionState;

    struct {
        BluetoothAddress        address;
        BluetoothHidStatus      status;
        u32 	                report_length;
        BluetoothHidReportData	report_data;
    } getReport;

    union {
        struct {
            BluetoothAddress    address;
            u32                 _unk0;
            u32                 _unk1;                 
        };
        struct {
            u32                 _unk0;
            u32                 _unk1;
            BluetoothAddress    address;                 
        } v2;
    } unknown07;

} BluetoothHidEventData;

typedef struct {
    u8 unk[0xa4];
} PlrList;

typedef struct {
    u8 unk[0x88];
} ChannelMapList;

typedef struct {
    u8 unk[0x14];
} BleConnectionParameter;

typedef struct {
    u8 unk[0xcc];
} BleAdvertisePacketData;

typedef struct {
    u8 unk[0x3e];
} BleAdvertiseFilter;

typedef struct {
    u32 size;
    u8 uuid[0x10];
} GattAttributeUuid; 

typedef struct {
    u8 instance_id;
    u8 pad[3];
    GattAttributeUuid uuid;
} GattId;

typedef struct {

} BluetoothBleEventData;

/// Initialize btdrv.
Result btdrvInitialize(void);
/// Exit btdrv.
void btdrvExit(void);
/// Gets the Service object for the actual btdrv service session.
Service* btdrvGetServiceSession(void);
/// Gets the address of the btdrv SharedMemory.
void* btdrvGetSharedmemAddr(void);

/// Initialize the bluetooth driver.
void btdrvInitializeBluetoothDriver(void);

// Bluetooth core interface
/// Initialize the bluetooth core interface.
Result btdrvInitializeBluetooth(Event *event);
/// Enable bluetooth.
Result btdrvEnableBluetooth(void);
/// Disable bluetooth.
Result btdrvDisableBluetooth(void);
/// Clean up bluetooth core interface.  
Result btdrvFinalizeBluetooth(void);
/// Get properties of host bluetooth adapter.
Result btdrvGetAdapterProperties(BluetoothAdapterProperty *property);
/// Get bluetooth adapter property.
Result btdrvGetAdapterProperty(BluetoothPropertyType type, u8 *value, u16 size);
/// Set bluetooth adapter property.
Result btdrvSetAdapterProperty(BluetoothPropertyType type, const u8 *value, u16 size);
/// Start device discovery.
Result btdrvStartInquiry(void);
/// Cancel device discovery.
Result btdrvStopInquiry(void);
/// Pair bluetooth device with host.
Result btdrvCreateBond(const BluetoothAddress *address, BluetoothTransport transport);
/// Remove bonded device.
Result btdrvRemoveBond(const BluetoothAddress *address);
/// Cancel bonding operation.
Result btdrvCancelBond(const BluetoothAddress *address);
/// Send response to bluetooth legacy pairing request.
Result btdrvRespondToPinRequest(const BluetoothAddress *address, bool accept, const BluetoothPinCode *pincode, u8 length);
/// Send response to bluetooth SSP pairing request.
Result btdrvRespondToSspRequest(const BluetoothAddress *address, BluetoothSspVariant variant, bool accept, u32 passkey);
/// Get bluetooth event type and data.
Result btdrvGetEventInfo(BluetoothEventType *type, u8 *buffer, u16 length);

// Bluetooth HID Host interface
/// Initialize HID host interface.
Result btdrvInitializeHid(Event *event, u16 version);
/// Connect bluetooth device.
Result btdrvOpenHidConnection(const BluetoothAddress *address);
/// Disconnect bluetooth device.
Result btdrvCloseHidConnection(const BluetoothAddress *address);
/// Send raw data to device.
Result btdrvWriteHidData(const BluetoothAddress *address, const BluetoothHidReport *data);
/// Send raw data to device.
Result btdrvWriteHidData2(const BluetoothAddress *address, const u8 *buffer, u16 length);
/// Send set report to device.
Result btdrvSetHidReport(const BluetoothAddress *address, BluetoothHhReportType type, const BluetoothHidReport *data);
/// Send get report to device.
Result btdrvGetHidReport(const BluetoothAddress *address, BluetoothHhReportType type, u8 id);
/// Wake up bluetooth device.
Result btdrvTriggerConnection(const BluetoothAddress *address, u16 unknown);
/// Register paired device with bluetooth driver.
Result btdrvAddPairedDeviceInfo(const BluetoothDevicesSettings *device);
/// Retrieve device registered with bluetooth driver.
Result btdrvGetPairedDeviceInfo(const BluetoothAddress *address, BluetoothDevicesSettings *device);
/// Clean up HID host interface.
Result btdrvFinalizeHid(void);
/// Get HID event data.
Result btdrvGetHidEventInfo(BluetoothHidEventType *type, u8 *buffer, u16 length);

/// Set TSI for a device.
Result btdrvSetTsi(const BluetoothAddress *address, u8 tsi);
/// Enable burst mode for a device.
Result btdrvEnableBurstMode(const BluetoothAddress *address, bool burst);
/// Set zero retransmission for a device.
Result btdrvSetZeroRetransmission(const BluetoothAddress *address, u8 *ids, u8 num);
/// Enable/disable MC mode.
Result btdrvEnableMcMode(bool enable);
/// Enable LLR scan.
Result btdrvEnableLlrScan(void);
/// Disable LLR scan.
Result btdrvDisableLlrScan(void);
/// Enable/disable blueooth radio.
Result btdrvEnableRadio(bool enable);
/// Set bluetooth discoverability and connectability.
Result btdrvSetVisibility(bool discoverable, bool connectable);
/// Enable/disable TBFC (Triggered Broadcom Fast Connect) scan.
Result btdrvEnableTbfcScan(bool enable);

/// Register HID report event.
Result btdrvRegisterHidReportEvent(Event *event);
/// Get HID report event type and data.
Result btdrvGetHidReportEventInfo(BluetoothHidEventType *type, u8 *buffer, u16 length);
/// Get latest PLR.
Result btdrvGetLatestPlr(PlrList *plr);
//(3.0.0+)
/// Get the number of pending connections.
Result btdrvGetPendingConnections(void);
/// Get current AFH channel map.
Result btdrvGetChannelMap(ChannelMapList *map);
/// Enable/disable transmit power boost setting.
Result btdrvEnableTxPowerBoostSetting(bool enable);
/// Check whether transmit power boost settings is enabled.
Result btdrvIsTxPowerBoostSettingEnabled(bool *enabled);
/// Enable/disable AFH setting.
Result btdrvEnableAfhSetting(bool enable);
/// Check whether AFh setting is enabled
Result btdrvIsIsAfhSettingEnabled(bool *enabled);

// Bluetooth BLE interface (5.0.0+)
/// Initialize bluetooth BLE interface.
Result btdrvInitializeBle(Event *event);
/// Enable bluetooth BLE interface.
Result btdrvEnableBle(void);
/// Disable bluetooth BLE interface.
Result btdrvDisableBle(void);
/// Clean up bluetooth BLE interface.
Result btdrvFinalizeBle(void);
/// Set BLE discoverability and connectability
Result btdrvSetBleVisibility(bool discoverable, bool connectable);
/// Set BLE connection parameter.
Result btdrvSetBleConnectionParameter(const BleConnectionParameter *param);
/// Set the default BLE connection parameter.
Result btdrvSetBleDefaultConnectionParameter(const BleConnectionParameter *param);
/// Set BLE advertise data.
Result btdrvSetBleAdvertiseData(const BleAdvertisePacketData *advertise);
/// Set BLE advertise parameter.
Result btdrvSetLeAdvertiseParameter(const BluetoothAddress *address, u16 min, u16 max);
/// Start BLE scan.
Result btdrvStartBleScan(void);
/// Stop BLE scan.
Result btdrvStopBleScan(void);
/// Add BLE scan filter condition.
Result btdrvAddBleScanFilterCondition(const BleAdvertiseFilter *filter);
/// Delete BLE scan filter condition.
Result btdrvDeleteBleScanFilterCondition(const BleAdvertiseFilter *filter);
/// Delete BLE scan filter.
Result btdrvDeleteBleScanFilter(u8 index);
/// Clear all BLE scan filters.
Result btdrvClearBleScanFilters(void);
/// Enable BLE scan filter.
Result btdrvEnableBleScanFilter(bool enable);
/// Register GATT client.
Result btdrvRegisterGattClient(const GattAttributeUuid *uuid);
/// Unregister GATT client.
Result btdrvUnregisterGattClient(u8 index);
/// Unregister all GATT clients
Result btdrvUnregisterAllGattClients(void);
/// Connect to GATT server.
Result btdrvConnectGattServer(u64 id, u8 iface, const BluetoothAddress *address, bool direct);

/// Cancel connection to GATT server.
Result btdrvCancelConnectGattServer(u8 iface, const BluetoothAddress *address, bool unk);
/// Disconnect from GATT server.
Result btdrvDisconnectGattServer(u32 unk);
/// Get GATT attribute of device.
Result btdrvGetGattAttribute(u32 unk, const BluetoothAddress *address);


Result btdrvGetGattService(u32 connId, const GattAttributeUuid *filter);
/// Configure GATT Maximum Transmission Unit.
Result btdrvConfigureAttMtu(u32 connId, u16 mtu);
/// Register GATT server.
Result btdrvRegisterGattServer(const GattAttributeUuid *uuid);
/// Unregister GATT server.
Result btdrvUnregisterGattServer(u8 iface);
/// Connect GATT client.
Result btdrvConnectGattClient(u8 unk1, const BluetoothAddress *address, bool unk2);
/// Disconnect GATT client.
Result btdrvDisconnectGattClient(u8 unk);
/// Add GATT service.
Result btdrvAddGattService(u8 iface, const GattAttributeUuid *uuid, u8 handle, bool primary);
/// Enable GATT service
Result btdrvEnableGattService(u8 iface, const GattAttributeUuid *uuid);
/// Add GATT characteristic to a service.
Result btdrvAddGattCharacteristic(u8 iface, const GattAttributeUuid *svcUuid, const GattAttributeUuid *charUuid, u16 permissions, u8 properties);
/// Add GATT descriptor to a service.
Result btdrvAddGattDescriptor(u8 iface, const GattAttributeUuid *svcUuid, const GattAttributeUuid *charUuid, u16 permissions);
/// Get event information for BLE managed event.
Result btdrvGetBleManagedEventInfo(BluetoothBleEventType *type, u8 *buffer, u16 length);
/// Get the first GATT characteristic from a service.
Result btdrvGetGattFirstCharacteristic(GattId *charOut, u8 *property, u32 connId, const GattId *svcId, bool primary, const GattAttributeUuid *charFilter);
/// Get the next GATT characteristic from a service.
Result btdrvGetGattNextCharacteristic(GattId *charOut, u8 *property, u32 connId, const GattId *svcId, bool primary, const GattId *charIn, const GattAttributeUuid *charFilter);
/// Get the first GATT descriptor from a service.
Result btdrvGetGattFirstDescriptor(GattId *descrOut, u32 connId, const GattId *svcId, bool primary, const GattId *charIn, const GattAttributeUuid *descrFilter);
/// Get the next GATT descriptor from a service.
Result btdrvGetGattNextDescriptor(GattId *descrOut, u32 connId, const GattId *svcId, bool primary, const GattId *charIn, const GattId *descrIn, const GattAttributeUuid *descrFilter);
/// Register GATT managed data path.
Result btdrvRegisterGattManagedDataPath(const GattAttributeUuid *uuid);
/// Unregister GATT managed data path.
Result btdrvUnregisterGattManagedDataPath(const GattAttributeUuid *uuid);
/// Register GATT HID data path.
Result btdrvRegisterGattHidDataPath(const GattAttributeUuid *uuid);
/// Unregister GATT HID data path.
Result btdrvUnregisterGattHidDataPath(const GattAttributeUuid *uuid);
/// Register GATT data path.
Result btdrvRegisterGattDataPath(const GattAttributeUuid *uuid);
/// Unregister GATT data path.
Result btdrvUnregisterGattDataPath(const GattAttributeUuid *uuid);
/// Read GATT charateristic.
Result btdrvReadGattCharacteristic(u32 connId, const GattId *svcId, bool primary, const GattId *charId, u8 auth);
/// Read GATT descriptor.
Result btdrvReadGattDescriptor(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const GattId *descrId, u8 auth);
/// Write GATT Characteristic.
Result btdrvWriteGattCharacteristic(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const u8 *data, u16 length, u8 auth, bool response);
/// Write GATT descriptor.
Result btdrvWriteGattDescriptor(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const GattId *descrId, const u8 *data, u16 length, u8 auth);
/// Register GATT notification.
Result btdrvRegisterGattNotification(u32 connId, const GattId *svcId, bool primary, const GattId *charId);
/// Unregister GATT notification.
Result btdrvUnregisterGattNotification(u32 connId, const GattId *svcId, bool primary, const GattId *charId);
/// Get event information for BLE HID event.
Result btdrvGetLeHidEventInfo(BluetoothBleHidventType *type, u8 *buffer, u16 length);
/// Register for BLE HID event notifications.
Result btdrvRegisterBleHidEvent(Event *event);
/// Set BLE scan parameter.
Result btdrvSetBleScanParameter(u16 interval, u16 window);

// Other
/// Move device to secondary piconet.
Result btdrvMoveToSecondaryPiconet(const BluetoothAddress *address);
/// Check whether bluetooth service is running in manufacturing mode
Result btdrvIsManufacturingMode(bool *mfmode);
/// Emulate bluetooth crash.
Result btdrvEmulateBluetoothCrash(u32 unk);
/// Get BLE channel map.
//Result btdrvGetBleChannelMap();

// nn::bluetooth::CircularBuffer
typedef struct {
    u64     sdk_mutex;
    Event   *event;
    u8      data[BLUETOOTH_CIRCBUFFER_SIZE];
    u32     writeOffset;
    u32     readOffset;
    s64 	size;
    char 	name[16];
    u8      _unk1;
    bool 	isInitialized;
    u8      _unk2[6];
    u32 	id;
    u8      _unk3[4];
} BluetoothCircularBuffer;

typedef struct {
    u8 type;            //+0x00
    u64 timestamp;      //+0x08
    u64 size;           //+0x10
} BufferPacketHeader;

typedef struct {
    BufferPacketHeader header;
    BluetoothHidReportData data;
} BufferPacket;

// Format of packets written to nn::bluetooth::CircularBuffer
typedef struct {
	struct {
        u8 type;            //+0x00
        //u8 _pad0[3];
        //u32 _unk0;          //+0x04
        u64 timestamp;      //+0x08
        u64 size;           //+0x10
    } header;

    BluetoothHidReportData data;
} HidReportDataPacket;

// nn::bluetooth::CircularBuffer::CircularBuffer
//void CreateBuffer(BluetoothCircularBuffer *buffer);
// nn::bluetooth::CircularBuffer::Initialize
//u64 InitializeBuffer(BluetoothCircularBuffer *buffer, const char*name); // pre 10.0.0 takes an event too
// nn::bluetooth::CircularBuffer::IsInitialized
//bool IsInitialized(BluetoothCircularBuffer *buffer);
// nn::bluetooth::CircularBuffer::GetWriteableSize
//u64 GetWriteableSize(BluetoothCircularBuffer *buffer);
// nn::bluetooth::CircularBuffer::Write
//u64 WriteBuffer(BluetoothCircularBuffer *buffer, void *data, size_t size);
// nn::bluetoothCirculabuffer::AttachEvent  <-- madeup name
//void AttachEvent(BluetoothCircularBuffer *buffer, Event *event);
// nn::bluetooth::CircularBuffer::Read
u8 *ReadBuffer(BluetoothCircularBuffer *buffer);
// nn::bluetooth::CircularBuffer::Free
u64 FreeBuffer(BluetoothCircularBuffer *buffer);
