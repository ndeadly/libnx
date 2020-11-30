#define NX_SERVICE_ASSUME_NON_DOMAIN
#include "service_guard.h"
#include <string.h>
#include "kernel/shmem.h"
#include "services/btdrv.h"
#include "arm/counter.h"
#include "runtime/hosversion.h"

static Service g_btdrvSrv;
static SharedMemory g_btdrvSharedmem;
static bool g_sharedmemInitialised = false;
static BluetoothCircularBuffer *g_btdrvCircularBuffer = NULL;

NX_GENERATE_SERVICE_GUARD(btdrv);

Result _btdrvInitialize(void) {
    return smGetService(&g_btdrvSrv, "btdrv");
}

void _btdrvCleanup(void) {
    serviceClose(&g_btdrvSrv);
}

Service* btdrvGetServiceSession(void) {
    return &g_btdrvSrv;
}

void* btdrvGetSharedmemAddr(void) {
    return shmemGetAddr(&g_btdrvSharedmem);
}

void btdrvInitializeBluetoothDriver(void) {
    serviceDispatch(&g_btdrvSrv, 0);
} 

Result btdrvInitializeBluetooth(Event *event) {
    Handle handle = INVALID_HANDLE;
    Result rc = serviceDispatch(&g_btdrvSrv, 1,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &handle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, handle, true);

    return rc;
}

Result btdrvEnableBluetooth(void) {
    return serviceDispatch(&g_btdrvSrv, 2);
}

Result btdrvDisableBluetooth(void) {
    return serviceDispatch(&g_btdrvSrv, 3);
}

Result btdrvFinalizeBluetooth(void) {
    return serviceDispatch(&g_btdrvSrv, 4);
}

Result btdrvGetAdapterProperties(BluetoothAdapterProperty *property) {
    return serviceDispatch(&g_btdrvSrv, 5,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {property, sizeof(BluetoothAdapterProperty)} }
    );
}

Result btdrvGetAdapterProperty(BluetoothPropertyType type, u8 *value, u16 size) {
    const struct {
        BluetoothPropertyType type;
    } in = { type };

    return serviceDispatchIn(&g_btdrvSrv, 6, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {value, size} }
    );
}

Result btdrvSetAdapterProperty(BluetoothPropertyType type, const u8 *value, u16 size) {
    const struct {
        BluetoothPropertyType type;
    } in = { type };

    return serviceDispatchIn(&g_btdrvSrv, 7, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {value, size} }
    );
}

Result btdrvStartInquiry(void) {
    return serviceDispatch(&g_btdrvSrv, 8);
}

Result btdrvStopInquiry(void) {
    return serviceDispatch(&g_btdrvSrv, 9);
}

Result btdrvCreateBond(const BluetoothAddress *address, BluetoothTransport transport) {
    if (hosversionBefore(9, 0, 0)) {
        const struct {
            BluetoothAddress address;
        } in = { *address };
        
        return serviceDispatchIn(&g_btdrvSrv, 10, in,
            .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In },
            .buffers = { {&transport, sizeof(BluetoothTransport)} }
        );
    }
    else {
        const struct {
            BluetoothAddress    address;
            BluetoothTransport  transport;
        } in = { *address, transport };
        
        return serviceDispatchIn(&g_btdrvSrv, 10, in);
    }
}

Result btdrvRemoveBond(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 11, in);
}

Result btdrvCancelBond(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 12, in);
}

Result btdrvRespondToPinRequest(const BluetoothAddress *address, bool accept, const BluetoothPinCode *pincode, u8 length) {
    const struct {
        BluetoothAddress    address;
        bool                accept;
        u8                  length;
        BluetoothPinCode    pincode;
    } in = { *address, accept, length, *pincode };

    return serviceDispatchIn(&g_btdrvSrv, 13, in);
}

Result btdrvRespondToSspRequest(const BluetoothAddress *address, BluetoothSspVariant variant, bool accept, u32 passkey) {
    const struct {
        BluetoothAddress    address;
        u8                  variant;
        bool                accept;
        u32                 passkey;
    } in = { *address, variant, accept, passkey };

    return serviceDispatchIn(&g_btdrvSrv, 14, in);
}

Result btdrvGetEventInfo(BluetoothEventType *type, u8 *buffer, u16 length) {
    return serviceDispatchOut(&g_btdrvSrv, 15, *type,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {buffer, length} }
    );
}

Result btdrvInitializeHid(Event *event, u16 version) {
    Handle handle = INVALID_HANDLE;
    Result rc = serviceDispatchIn(&g_btdrvSrv, 16, version, 
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &handle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, handle, false);

    return rc;
}

Result btdrvOpenHidConnection(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 17, in);
}

Result btdrvCloseHidConnection(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 18, in);
}

Result btdrvWriteHidData(const BluetoothAddress *address, const BluetoothHidReport *data) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 19, in,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, sizeof(BluetoothHidReport)} }
    );
}

Result btdrvWriteHidData2(const BluetoothAddress *address, const u8 *buffer, u16 length) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 20, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {buffer, length} }
    );
}

Result btdrvSetHidReport(const BluetoothAddress *address, BluetoothHhReportType type, const BluetoothHidReport *data) {
    const struct {
        BluetoothAddress address;
        BluetoothHhReportType type;
    } in = { *address, type};

    return serviceDispatchIn(&g_btdrvSrv, 21, in,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, sizeof(BluetoothHidReport)} }
    );
}

Result btdrvGetHidReport(const BluetoothAddress *address, BluetoothHhReportType type, u8 id) {
    const struct {
        BluetoothAddress address;
        BluetoothHhReportType type;
        u8 id;
    } in = { *address, type, id };

    return serviceDispatchIn(&g_btdrvSrv, 22, in);
}

Result btdrvTriggerConnection(const BluetoothAddress *address, u16 unknown) {
    if (hosversionBefore(9, 0, 0)) {
        const struct {
            BluetoothAddress address;
        } in = { *address };

        return serviceDispatchIn(&g_btdrvSrv, 23, in);
    }
    else {
        const struct {
            BluetoothAddress address;
            u16 unknown;
        } in = { *address, unknown };

        return serviceDispatchIn(&g_btdrvSrv, 23, in);
    }

}

Result btdrvAddPairedDeviceInfo(const BluetoothDevicesSettings *device) {
    return serviceDispatch(&g_btdrvSrv, 24,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {device, sizeof(BluetoothDevicesSettings)} }
    );
}

Result btdrvGetPairedDeviceInfo(const BluetoothAddress *address, BluetoothDevicesSettings *device) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 25, in,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {device, sizeof(BluetoothDevicesSettings)} }
    );
}

Result btdrvFinalizeHid(void) {
    return serviceDispatch(&g_btdrvSrv, 26);
}

Result btdrvGetHidEventInfo(BluetoothHidEventType *type, u8 *buffer, u16 length) {
    return serviceDispatchOut(&g_btdrvSrv, 27, *type,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {buffer, length} }
    );
}

Result btdrvSetTsi(const BluetoothAddress *address, u8 tsi) {
    const struct {
        BluetoothAddress address;
        u8 tsi;
    } in = { *address, tsi };

    return serviceDispatchIn(&g_btdrvSrv, 28, in);
}

Result btdrvEnableBurstMode(const BluetoothAddress *address, bool burst) {
    const struct {
        BluetoothAddress address;
        bool burst;
    } in = { *address, burst };

    return serviceDispatchIn(&g_btdrvSrv, 29, in);
}

Result btdrvSetZeroRetransmission(const BluetoothAddress *address, u8 *ids, u8 num) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 30, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {ids, num} }
    );
}

Result btdrvEnableMcMode(bool enable) {
    return serviceDispatchIn(&g_btdrvSrv, 31, enable);
}

Result btdrvEnableLlrScan(void) {
    return serviceDispatch(&g_btdrvSrv, 32);
}

Result btdrvDisableLlrScan(void) {
    return serviceDispatch(&g_btdrvSrv, 33);
}

Result btdrvEnableRadio(bool enable) {
    return serviceDispatchIn(&g_btdrvSrv, 34, enable);
}

Result btdrvSetVisibility(bool discoverable, bool connectable) {
    const struct {
        bool discoverable;
        bool connectable;
    } in = { discoverable, connectable };

    return serviceDispatchIn(&g_btdrvSrv, 35, in);
}

Result btdrvEnableTbfcScan(bool enable) {
    return serviceDispatchIn(&g_btdrvSrv, 36, enable);
}

Result btdrvRegisterHidReportEvent(Event *event) {
    Result rc;
    Handle eventHandle = INVALID_HANDLE;

    rc = serviceDispatch(&g_btdrvSrv, hosversionBefore(4, 0, 0) ? 36 : 37,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &eventHandle,
    );

    if (R_SUCCEEDED(rc)) {
        eventLoadRemote(event, eventHandle, true);

        // Map btdrv shared memory on 7.0.0+
        if (hosversionAtLeast(7, 0, 0) && !g_sharedmemInitialised) {
            Handle shmemHandle = INVALID_HANDLE;

            rc = serviceDispatch(&g_btdrvSrv, 38,
                .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
                .out_handles = &shmemHandle,
            );
            
            if (R_SUCCEEDED(rc)) {
                shmemLoadRemote(&g_btdrvSharedmem, shmemHandle, BLUETOOTH_SHAREDMEM_SIZE, Perm_Rw);
                rc = shmemMap(&g_btdrvSharedmem);
                if (R_SUCCEEDED(rc)) {
                    g_btdrvCircularBuffer = (BluetoothCircularBuffer *)btdrvGetSharedmemAddr();
                    g_sharedmemInitialised = true;
                }
            }
        }
    }

    return rc;
}

Result btdrvGetHidReportEventInfo(BluetoothHidEventType *type, u8 *buffer, u16 length)
{
    if (hosversionBefore(7, 0, 0)) {
        return serviceDispatchOut(&g_btdrvSrv, hosversionBefore(4, 0, 0) ? 37 : 38, *type,
            .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
            .buffers = { {buffer, length} }
        );
    }
    else {
        u32 eventType;
        BluetoothHidEventData *eventData = (BluetoothHidEventData *)buffer; 
        HidReportDataPacket *packet = (HidReportDataPacket *)ReadBuffer(g_btdrvCircularBuffer);
              
        while (true) {
            if (packet == NULL) {
                *type = HidEvent_GetReport;
                eventData->getReport.report_length = 0;
                return 0;
            }
            
            *type = packet->header.type;
            eventType = *type;
            
            if (packet->header.type != HidEvent_GetReport) {
                break;
            }
            
            if (armTicksToNs(armGetSystemTick() - packet->header.timestamp) <= 1e8) {
                eventType = *type;
                break;
            }
            
            FreeBuffer(g_btdrvCircularBuffer);
            packet = (HidReportDataPacket *)ReadBuffer(g_btdrvCircularBuffer);
        }

        switch (eventType) {
            case HidEvent_GetReport:
                eventData->getReport.status = 0;
                eventData->getReport.report_length = (u32)packet->header.size;
                memcpy(&eventData->getReport.address, 
                        hosversionAtLeast(9, 0, 0) ? &packet->data.v2.address : &packet->data.address, 
                        sizeof(BluetoothAddress));

                //memcpy(&eventData->report_data, &packet->report_data, sizeof(HidReportData));
                eventData->getReport.report_data.size = (u16)packet->header.size;
                memcpy(&eventData->getReport.report_data.address, &eventData->getReport.address, sizeof(BluetoothAddress));
                memcpy(&eventData->getReport.report_data.report, 
                        &packet->data.report, 
                        sizeof(BluetoothHidReport));
                break;

            case HidEvent_Unknown08:
                break;
                
            case HidEvent_Unknown09:
                if (hosversionAtLeast(9, 0, 0))
                    memcpy(buffer, &packet->data, 0x2c8);   
                else
                    memcpy(buffer, &packet->data, 0x290);        
                break;
            
            default:
                // Maybe fatalThrow here?
                break;
        }
        
        FreeBuffer(g_btdrvCircularBuffer);
        return 0;
    }
}

Result btdrvGetLatestPlr(PlrList *plr) {
    return serviceDispatch(&g_btdrvSrv, hosversionBefore(4, 0, 0) ? 38 : 39,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { {plr, sizeof(PlrList) } }
    );
}

Result btdrvGetPendingConnections(void) {
    if (hosversionBefore(3, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, hosversionBefore(4, 0, 0) ? 39 : 40); 
}

Result btdrvGetChannelMap(ChannelMapList *map) {
    if (hosversionBefore(3, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, hosversionBefore(4, 0, 0) ? 40 : 41,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { {map, sizeof(ChannelMapList) } }
    );
}

Result btdrvEnableTxPowerBoostSetting(bool enable) {
    if (hosversionBefore(3, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(4, 0, 0) ? 41 : 42, enable);
}

Result btdrvIsTxPowerBoostSettingEnabled(bool *enabled) {
    if (hosversionBefore(3, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchOut(&g_btdrvSrv, hosversionBefore(4, 0, 0) ? 42 : 43, *enabled);
}

Result btdrvEnableAfhSetting(bool enable) {
    if (hosversionBefore(3, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(4, 0, 0) ? 43 : 44, enable);
}

Result btdrvIsIsAfhSettingEnabled(bool *enabled) {
    return serviceDispatchOut(&g_btdrvSrv, hosversionBefore(4, 0, 0) ? 44 : 45, *enabled);
}

Result btdrvInitializeBle(Event *event) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    Handle handle = INVALID_HANDLE;
    Result rc = serviceDispatch(&g_btdrvSrv, 46,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &handle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, handle, true);

    return rc;
}

Result btdrvEnableBle(void) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 47);
}

Result btdrvDisableBle(void) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 48);
}

Result btdrvFinalizeBle(void) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 49);
}

Result btdrvSetBleVisibility(bool discoverable, bool connectable) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 50, discoverable, connectable);
}

Result btdrvSetBleConnectionParameter(const BleConnectionParameter *param) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        BleConnectionParameter param;
    } in = { *param };
    
    return serviceDispatchIn(&g_btdrvSrv, 51, in);
}

Result btdrvSetBleDefaultConnectionParameter(const BleConnectionParameter *param) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        BleConnectionParameter param;
    } in = { *param };
    
    return serviceDispatchIn(&g_btdrvSrv, 52, in);
}

Result btdrvSetBleAdvertiseData(const BleAdvertisePacketData *advertise) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 53,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {advertise, sizeof(BleAdvertisePacketData)} },
    );
}

Result btdrvSetLeAdvertiseParameter(const BluetoothAddress *address, u16 min, u16 max) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        BluetoothAddress address;
        u16 min;
        u16 max;
    } in = { *address, min, max };

    return serviceDispatchIn(&g_btdrvSrv, 54, in);
}

Result btdrvStartBleScan(void) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 55);
}

Result btdrvStopBleScan(void) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 56);
}

Result btdrvAddBleScanFilterCondition(const BleAdvertiseFilter *filter) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 57,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {filter, sizeof(BleAdvertiseFilter)} },
    );
}

Result btdrvDeleteBleScanFilterCondition(const BleAdvertiseFilter *filter) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 58,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {filter, sizeof(BleAdvertiseFilter)} },
    );
}

Result btdrvDeleteBleScanFilter(u8 index) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 59, index);
}

Result btdrvClearBleScanFilters(void) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 60);
}

Result btdrvEnableBleScanFilter(bool enable) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 61, enable);
}

Result btdrvRegisterGattClient(const GattAttributeUuid *uuid) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        GattAttributeUuid uuid;
    } in = { *uuid };

    return serviceDispatchIn(&g_btdrvSrv, 62, in);
}

Result btdrvUnregisterGattClient(u8 index) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 63, index);
}

Result btdrvUnregisterAllGattClients(void) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 64);
}

Result btdrvConnectGattServer(u64 aruid, u8 iface, const BluetoothAddress *address, bool direct) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);    

    const struct {
        u8 iface;
        BluetoothAddress address;
        bool direct;
        u64 aruid;
    } in = { iface, *address, direct, aruid };

    return serviceDispatchIn(&g_btdrvSrv, 65, in);
}

/* These need attention to ensure correct data is sent on older fw versions */
// 
Result btdrvCancelConnectGattServer(u8 iface, const BluetoothAddress *address, bool unk) {
    if (hosversionBefore(5, 1, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u8 iface;
        BluetoothAddress address;
        bool unk;
    } in = { iface, *address, unk };

    return serviceDispatchIn(&g_btdrvSrv, 66, in);
}
//
Result btdrvDisconnectGattServer(u32 unk) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u32 unk;
    } in = { unk };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 66 : 67, in);  
}
//
Result btdrvGetGattAttribute(u32 unk, const BluetoothAddress *address) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    if (hosversionAtLeast(9, 0, 0))
        return serviceDispatchIn(&g_btdrvSrv, 68, unk); 

    const struct {
        BluetoothAddress address;
        u32 unk;
    } in = { *address, unk };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 67 : 68, in);
}

Result btdrvGetGattService(u32 connId, const GattAttributeUuid *filter) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u32 connId;
        GattAttributeUuid filter;
    } in = { connId, *filter };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 68 : 69, in);
}

Result btdrvConfigureAttMtu(u32 connId, u16 mtu) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u16 mtu;
        u8 pad[2];
        u32 connId;
    } in = { mtu, {0}, connId };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 69 : 70, in);
}

/* Need to check these four on older fw as well */
//
Result btdrvRegisterGattServer(const GattAttributeUuid *uuid) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        GattAttributeUuid uuid;
    } in = { *uuid };

    return serviceDispatchIn(&g_btdrvSrv, 71, in);
}
//
Result btdrvUnregisterGattServer(u8 iface) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 72, iface);
}
//
Result btdrvConnectGattClient(u8 unk1, const BluetoothAddress *address, bool unk2) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u8 unk1;
        BluetoothAddress address;
        bool unk2;
    } in = { unk1, *address, unk2 };

    return serviceDispatchIn(&g_btdrvSrv, 73, in);
}
//
Result btdrvDisconnectGattClient(u8 unk) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 74, unk);
}

Result btdrvAddGattService(u8 iface, const GattAttributeUuid *uuid, u8 handle, bool primary) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u8 iface;
        u8 handle;
        bool primary;
        u8 pad;
        GattAttributeUuid uuid;
    } in = { iface, handle, primary, 0, *uuid };

    return serviceDispatchIn(&g_btdrvSrv, 75, in);
}

Result btdrvEnableGattService(u8 iface, const GattAttributeUuid *uuid) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u8 iface;
        u8 pad[3];
        GattAttributeUuid uuid;
    } in = { iface, {0}, *uuid };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 74 : 76, in);
}

Result btdrvAddGattCharacteristic(u8 iface, const GattAttributeUuid *svcUuid, const GattAttributeUuid *charUuid, u16 permissions, u8 properties) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u8 iface;
        u8 properties;
        u16 permissions;
        GattAttributeUuid svcUuid;
        GattAttributeUuid charUuid;        
    } in = { iface, properties, permissions, *svcUuid, *charUuid };

    return serviceDispatchIn(&g_btdrvSrv, 77, in);
}

Result btdrvAddGattDescriptor(u8 iface, const GattAttributeUuid *svcUuid, const GattAttributeUuid *charUuid, u16 permissions) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u8 iface;
        u8 pad;
        u16 permissions;
        GattAttributeUuid svcUuid;
        GattAttributeUuid charUuid;
    } in = { iface, 0, permissions, *svcUuid, *charUuid };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 76 : 78, in);
}

Result btdrvGetBleManagedEventInfo(BluetoothBleEventType *type, u8 *buffer, u16 length) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchOut(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 78 : 79, *type,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {buffer, length} }
    );
}

Result btdrvGetGattFirstCharacteristic(GattId *charOut, u8 *propertyOut, u32 connId, const GattId *svcId, bool primary, const GattAttributeUuid *charFilter) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        bool    primary;
        u8      pad[3];
        u32     connId;
        GattId  svcId;
        GattAttributeUuid charFilter;
    } in = { primary, {0}, connId, *svcId, *charFilter };

    struct {
        u8 property;
        u8 pad[3];
        GattId characteristic;
    } out;

    Result rc =  serviceDispatchInOut(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 79 : 80, in, out);
    if (R_SUCCEEDED(rc)) {
        if (propertyOut) 
            *propertyOut = out.property;
        if (charOut) 
            *charOut = out.characteristic;
    }
    return rc;
}

Result btdrvGetGattNextCharacteristic(GattId *charOut, u8 *propertyOut, u32 connId, const GattId *svcId, bool primary, const GattId *charIn, const GattAttributeUuid *charFilter) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        bool    primary;
        u8      pad[3];
        u32     connId;
        GattId  svcId;
        GattId  charIn;
        GattAttributeUuid charFilter;
    } in = { primary, {0}, connId, *svcId, *charIn, *charFilter };

    struct {
        u8 property;
        u8 pad[3];
        GattId characteristic;
    } out;

    Result rc = serviceDispatchInOut(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 80 : 81, in, out);
    if (R_SUCCEEDED(rc)) {
        if (propertyOut) 
            *propertyOut = out.property;
        if (charOut) 
            *charOut = out.characteristic;
    }
    return rc;
}

Result btdrvGetGattFirstDescriptor(GattId *descrOut, u32 connId, const GattId *svcId, bool primary, const GattId *charIn, const GattAttributeUuid *descrFilter) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        bool    primary;
        u8      pad[3];
        u32     connId;
        GattId  svcId;
        GattId  charIn;
        GattAttributeUuid descrFilter;
    } in = { primary, {0}, connId, *svcId, *charIn, *descrFilter };

    return serviceDispatchInOut(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 81 : 82, in, *descrOut);
}

Result btdrvGetGattNextDescriptor(GattId *descrOut, u32 connId, const GattId *svcId, bool primary, const GattId *charIn, const GattId *descrIn, const GattAttributeUuid *descrFilter) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        bool    primary;
        u8      pad[3];
        u32     connId;
        GattId  svcId;
        GattId  charIn;
        GattId  descrIn;
        GattAttributeUuid descrFilter;
    } in = { primary, {0}, connId, *svcId, *charIn, *descrIn, *descrFilter };

    return serviceDispatchInOut(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 82 : 83, in, *descrOut);
}

Result btdrvRegisterGattManagedDataPath(const GattAttributeUuid *uuid) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 84, *uuid);
}

Result btdrvUnregisterGattManagedDataPath(const GattAttributeUuid *uuid) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 85, *uuid);
}

Result btdrvRegisterGattHidDataPath(const GattAttributeUuid *uuid) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 86, *uuid);
}

Result btdrvUnregisterGattHidDataPath(const GattAttributeUuid *uuid) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 87, *uuid);
}

Result btdrvRegisterGattDataPath(const GattAttributeUuid *uuid) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 88, *uuid);
}

Result btdrvUnregisterGattDataPath(const GattAttributeUuid *uuid) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 83 : 89, *uuid);
}

Result btdrvReadGattCharacteristic(u32 connId, const GattId *svcId, bool primary, const GattId *charId, u8 auth) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        bool    primary;
        u8      auth;
        u8      pad[2];
        u32     connId;
        GattId  svcId;
        GattId  charId;
    } in = { primary, auth, {0}, connId, *svcId, *charId };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 89 : 90, in);
}

Result btdrvReadGattDescriptor(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const GattId *descrId, u8 auth) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        bool    primary;
        u8      auth;
        u8      pad[2];
        u32     connId;
        GattId  svcId;
        GattId  charId;
        GattId  descrId;
    } in = { primary, auth, {0}, connId, *svcId, *charId, *descrId };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 90 : 91, in);
}

Result btdrvWriteGattCharacteristic(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const u8 *data, u16 length, u8 auth, bool response) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        bool    primary;
        u8      auth;
        bool    response;
        u8      pad;
        u32     connId;
        GattId  svcId;
        GattId  charId; 
    } in = { primary, auth, response, 0, connId, *svcId, *charId };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 91 : 92, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, length} }
    );
}

Result btdrvWriteGattDescriptor(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const GattId *descrId, const u8 *data, u16 length, u8 auth) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        bool    primary;
        u8      auth;
        u8      pad[2];
        u32     connId;
        GattId  svcId;
        GattId  charId;
        GattId  descrId;
    } in = { primary, auth, {0}, connId, *svcId, *charId, *descrId };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 92 : 93, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, length} }
    );
}

Result btdrvRegisterGattNotification(u32 connId, const GattId *svcId, bool primary, const GattId *charId) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        bool    primary;
        u8      pad[3];
        u32     connId;
        GattId  svcId;
        GattId  charId;
    } in = { primary, {0}, connId, *svcId, *charId };

    return serviceDispatchIn(&g_btdrvSrv, 94, in);
}

Result btdrvUnregisterGattNotification(u32 connId, const GattId *svcId, bool primary, const GattId *charId) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        bool    primary;
        u8      pad[3];
        u32     connId;
        GattId  svcId;
        GattId  charId;
    } in = { primary, {0}, connId, *svcId, *charId };

    return serviceDispatchIn(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 93 : 95, in);
}

Result btdrvGetLeHidEventInfo(BluetoothBleHidventType *type, u8 *buffer, u16 length) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchOut(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 95 : 96, *type,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {buffer, length} }
    );
}

Result btdrvRegisterBleHidEvent(Event *event) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    Handle handle = INVALID_HANDLE;
    Result rc = serviceDispatch(&g_btdrvSrv, hosversionBefore(5, 1, 0) ? 96 : 97,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &handle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, handle, true);

    return rc;
}

Result btdrvSetBleScanParameter(u16 interval, u16 window) {
    if (hosversionBefore(5, 1, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        u16 interval;
        u16 window;
    } in = { interval, window };

    return serviceDispatchIn(&g_btdrvSrv, 98, in);
}

Result btdrvMoveToSecondaryPiconet(const BluetoothAddress *address) {
    if (hosversionBefore(10, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 99, in);
}

Result btdrvIsManufacturingMode(bool *mfmode) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchOut(&g_btdrvSrv, 256, *mfmode);
}

Result btdrvEmulateBluetoothCrash(u32 unk) {
    if (hosversionBefore(7, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatchIn(&g_btdrvSrv, 257, unk);
}

/*
Result btdrvGetBleChannelMap() {
    if (hosversionBefore(9, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    return serviceDispatch(&g_btdrvSrv, 258, 
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { } }
    );
}
*/

/*
void CreateBuffer(BluetoothCircularBuffer *buffer) {
    buffer->readOffset = 0;
    buffer->writeOffset = 0;
    buffer->isInitialized = false;
}

u64 InitializeBuffer(BluetoothCircularBuffer *buffer, const char *name) {

    if (!name || buffer->isInitialized) {
        fatalThrow(-1);
    }

    buffer->readOffset = 0;
    buffer->writeOffset = 0;
    strncpy(buffer->name, name, 0x10);
    buffer->_unk1 = 0;
    buffer->size = BLUETOOTH_CIRCBUFFER_SIZE;
    buffer->_unk0 = 0;
    buffer->isInitialized = true;
}

bool IsInitialized(BluetoothCircularBuffer *buffer) {
    return buffer->isInitialized;
}

u64 GetWriteableSize(BluetoothCircularBuffer *buffer) {
    u32 readOffset = buffer->readOffset;
    u32 writeOffset = buffer->writeOffset;
    u64 size;

    if (!buffer->isInitialized)
        return 0;

    if (readOffset <= writeOffset)
        size = (BLUETOOTH_CIRCBUFFER_SIZE - 1) - writeOffset + readOffset;
    else
        size = readOffset + ~writeOffset; //

    if (size > BLUETOOTH_CIRCBUFFER_SIZE) 
        size = 0;

    return size;
}

void AttachEvent(BluetoothCircularBuffer *buffer, Event *event) {
    buffer->event = event;
}

u64 _write(BluetoothCircularBuffer *buffer, u8 type, void *data, size_t size) {

    HidReportDataPacket *packet = (HidReportDataPacket *)buffer->data[buffer->writeOffset];
    packet->header.type = type;
    packet->header.timestamp = armGetSystemTick();
    packet->header.size = size;

    if (type != -1) {
        if (data && (size > 0))
            memcpy(&packet->data, data, size);
        else 
            return -1;
    }

    u32 newOffset = buffer->writeOffset + size + sizeof(packet->header);
    if (newOffset >= BLUETOOTH_CIRCBUFFER_SIZE)
        return -1;

    if (newOffset == BLUETOOTH_CIRCBUFFER_SIZE)
        buffer->writeOffset = 0; 
    else
        buffer->writeOffset = newOffset;

    return 0;
}

u64 WriteBuffer(BluetoothCircularBuffer *buffer, u8 type, void *data, size_t size) {
    if (IsInitialized(buffer)) {
        os::SdkMutexType::Lock(&buffer->mutex);

        u32 writeableSize = GetWriteableSize(buffer)
        if (writeableSize < size + 24) {
            if (buffer->event)
                os::SignalEvent(&buffer->event);

            os::SdkMutexType::Unlock(&buffer->mutex);
            return -1;
        }
        else {

        }
    }
}

*/

u8 *ReadBuffer(BluetoothCircularBuffer *buffer) {
    u32 currentReadOffset;
    u32 newOffset;

	if (buffer->isInitialized) {
		do {
            currentReadOffset = buffer->readOffset;
			if (currentReadOffset == buffer->writeOffset) {
                break;
            }
			
			if (buffer->data[currentReadOffset] != -1) {
				return buffer->data + currentReadOffset;
            }
			
			if (!buffer->isInitialized) {
                break;
            }

            currentReadOffset = buffer->readOffset;
            if (currentReadOffset != buffer->writeOffset) {
                newOffset = currentReadOffset + buffer->data[currentReadOffset + 16] + sizeof(BufferPacketHeader);
                if (newOffset >= BLUETOOTH_CIRCBUFFER_SIZE) {
                    newOffset = 0;
                }

                if (newOffset >= BLUETOOTH_CIRCBUFFER_SIZE) {
                    // Maybe fatalThrow here?
                    break;
                }
                
                buffer->readOffset = newOffset;
            }
		
		} while(buffer->isInitialized);
	}
	
	return 0;
}

u64 FreeBuffer(BluetoothCircularBuffer *buffer) {
    u32 currentReadOffset;
	u32 newOffset;

	if (buffer->isInitialized) {
        currentReadOffset = buffer->readOffset;
        if (currentReadOffset != buffer->writeOffset) {
            newOffset = currentReadOffset + buffer->data[currentReadOffset + 16] + sizeof(BufferPacketHeader);
            if (newOffset >= BLUETOOTH_CIRCBUFFER_SIZE) {
                newOffset = 0;
            }
            
            if (newOffset < BLUETOOTH_CIRCBUFFER_SIZE) {
                buffer->readOffset = newOffset;
                return 0;
            }

            // Maybe fatalThrow here?
            //fatalThrow();
        }
    }
	
    return -1;
}
