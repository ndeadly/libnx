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
    BluetoothEvent_DeviceFound              = 3,
    BluetoothEvent_DiscoveryStateChanged,
    BluetoothEvent_PinRequest,
    BluetoothEvent_SspRequest,
    BluetoothEvent_BondStateChanged
} BluetoothEventType;

typedef enum  {
    BluetoothProperty_BdName = 0x1,
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
    BluetoothProperty_RemoteDeviceTimestamp = 0xFF,
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
    HidEvent_Unknown05,
    HidEvent_Unknown06,
    HidEvent_Unknown07,
    HidEvent_Unknown08,
    HidEvent_Unknown09,
} HidEventType;

typedef enum {
    HidReportType_InputReport  = 1,
    HidReportType_OutputReport,
    HidReportType_FeatureReport
} HidReportType;

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
} HidStatus;

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
} HidConnectionState;

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

    // Pad out to 0x200. Think this is what HOS uses
    u8                    _pad[0xd0];
} BluetoothDevice;

typedef struct {
    u16 length;
    u8  data[0x280];
} BluetoothHidData;

typedef struct {
	u8                  type;
    u8                  _unk2;
    u8                  id; 
	u8					data[0x27f]; // need to revise if this is real size
} HidReport;

typedef union {
    // Pre 9.0.0
    struct {
        u16                 size;
        u8                  _unk0;      //possibly controller id?
        BluetoothAddress    address;
        u8                  _unk1[3];
        HidReport	        report;
    };

    // 9.0.0+
    struct {
        u8                  _unk0[5];
        BluetoothAddress    address;
        u8                  _unk1;
        HidReport	        report;
    } v2;

} HidReportData;

typedef union {

    struct {
        BluetoothAddress    address;
        HidConnectionState  state;
    } connectionState;

    struct {
        BluetoothAddress    address;
        HidStatus           status;
        uint32_t 	        report_length;
        HidReportData		report_data;
    } getReport;

} HidEventData;

//nn::bluetooth::CircularBuffer
typedef struct {
    u64     _unk0;
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

	struct {
        u8 type;            //+0x00
        //u8 _pad0[3];
        u32 _unk0;          //+0x04
        u64 timestamp;      //+0x08
        u64 size;           //+0x10
    } header;

    HidReportData data;

} HidReportDataPacket;

/// Initialize btdrv.
Result btdrvInitialize(void);

/// Exit btdrv.
void btdrvExit(void);

/// Gets the Service object for the actual btdrv service session.
Service* btdrvGetServiceSession(void);

/// Gets the address of the btdrv SharedMemory.
void* btdrvGetSharedmemAddr(void);

void btdrvInitializeBluetoothDriver(void);

// Bluetooth core interface
Result btdrvInitializeBluetooth(Event *event);
Result btdrvEnableBluetooth(void);
Result btdrvDisableBluetooth(void);  
Result btdrvCleanupBluetooth(void); 
Result btdrvGetAdapterProperties(BluetoothAdapterProperty *property);
Result btdrvGetAdapterProperty(BluetoothPropertyType type, u8 *value, u16 size);
Result btdrvSetAdapterProperty(BluetoothPropertyType type, const u8 *value, u16 size);
Result btdrvStartDiscovery(void);
Result btdrvCancelDiscovery(void);
Result btdrvCreateBond(const BluetoothAddress *address, BluetoothTransport transport);
Result btdrvRemoveBond(const BluetoothAddress *address);
Result btdrvCancelBond(const BluetoothAddress *address);
Result btdrvPinReply(const BluetoothAddress *address, bool accept, const BluetoothPinCode *pincode, u8 length);
Result btdrvSspReply(const BluetoothAddress *address, BluetoothSspVariant variant, bool accept, u32 passkey);
Result btdrvGetEventInfo(BluetoothEventType *type, u8 *buffer, u16 length);

// HID Host interface
Result btdrvInitializeHid(Event *event, u16 version);
Result btdrvHidConnect(const BluetoothAddress *address);
Result btdrvHidDisconnect(const BluetoothAddress *address);
Result btdrvHidSendData(const BluetoothAddress *address, const BluetoothHidData *data);
Result btdrvHidSendData2(const BluetoothAddress *address, const u8 *buffer, u16 length);
Result btdrvHidSetReport(const BluetoothAddress *address, HidReportType type, const BluetoothHidData *data);
Result btdrvHidGetReport(const BluetoothAddress *address, HidReportType type, u8 id);
Result btdrvHidWakeController(const BluetoothAddress *address);
Result btdrvHidAddPairedDevice(const BluetoothDevice *device);
Result btdrvHidGetPairedDevice(const BluetoothAddress *address, BluetoothDevice *device);
Result btdrvCleanupHid(void);
Result btdrvHidGetEventInfo(HidEventType *type, u8 *buffer, u16 length);
Result btdrvRegisterHidReportEvent(Event *event);
Result btdrvHidGetReportEventInfo(HidEventType *type, u8 *buffer, u16 length);

// nn::bluetooth::CircularBuffer::Read
u8 *ReadBuffer(BluetoothCircularBuffer *buffer);
// nn::bluetooth::CircularBuffer::Free
u64 FreeBuffer(BluetoothCircularBuffer *buffer);
