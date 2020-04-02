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
static BluetoothCircularBuffer *g_hidCircularBuffer = NULL;

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
        eventLoadRemote(event, handle, false);

    return rc;
}

Result btdrvEnableBluetooth(void) {
    return serviceDispatch(&g_btdrvSrv, 2);
}

Result btdrvDisableBluetooth(void) {
    return serviceDispatch(&g_btdrvSrv, 3);
}

Result btdrvCleanupBluetooth(void) {
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

Result btdrvStartDiscovery(void) {
    return serviceDispatch(&g_btdrvSrv, 8);
}

Result btdrvCancelDiscovery(void) {
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
            u16                  _pad;
            BluetoothTransport  transport;
        } in = { *address, 0, transport };
        
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

Result btdrvPinReply(const BluetoothAddress *address, bool accept, const BluetoothPinCode *pincode, u8 length) {
    const struct {
        BluetoothAddress    address;
        bool                accept;
        u8                  length;
        BluetoothPinCode    pincode;
    } in = { *address, accept, length, *pincode };

    return serviceDispatchIn(&g_btdrvSrv, 13, in);
}

Result btdrvSspReply(const BluetoothAddress *address, BluetoothSspVariant variant, bool accept, u32 passkey) {
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

Result btdrvHidConnect(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 17, in);
}

Result btdrvHidDisconnect(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 18, in);
}

Result btdrvHidSendData(const BluetoothAddress *address, const BluetoothHidData *data) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 19, in,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, sizeof(BluetoothHidData)} }
    );
}

Result btdrvHidSendData2(const BluetoothAddress *address, const u8 *buffer, u16 length) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 20, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {buffer, length} }
    );
}

Result btdrvHidSetReport(const BluetoothAddress *address, HidReportType type, const BluetoothHidData *data) {
    const struct {
        BluetoothAddress address;
        HidReportType type;
        // Maybe 2 bytes of padding?
    } in = { *address, type};

    return serviceDispatchIn(&g_btdrvSrv, 21, in,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, sizeof(BluetoothHidData)} }
    );
}

Result btdrvHidGetReport(const BluetoothAddress *address, HidReportType type, u8 id) {
    const struct {
        BluetoothAddress address;
        HidReportType type;
        u8 id;
    } in = { *address, type, id };

    return serviceDispatchIn(&g_btdrvSrv, 22, in);
}

Result btdrvHidWakeController(const BluetoothAddress *address)
{
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 23, in);
}

Result btdrvHidAddPairedDevice(const BluetoothDevice *device) {
    return serviceDispatch(&g_btdrvSrv, 24,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {device, sizeof(BluetoothDevice)} }
    );
}

Result btdrvHidGetPairedDevice(const BluetoothAddress *address, BluetoothDevice *device) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btdrvSrv, 25, in,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {device, sizeof(BluetoothDevice)} }
    );
}

Result btdrvCleanupHid(void) {
    return serviceDispatch(&g_btdrvSrv, 26);
}

Result btdrvHidGetEventInfo(HidEventType *type, u8 *buffer, u16 length) {
    return serviceDispatchOut(&g_btdrvSrv, 27, *type,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {buffer, length} }
    );
}

Result btdrvRegisterHidReportEvent(Event *event)
{
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
                    g_hidCircularBuffer = (BluetoothCircularBuffer *)btdrvGetSharedmemAddr();
                    g_sharedmemInitialised = true;
                }
            }
        }
    }

    return rc;
}

Result btdrvHidGetReportEventInfo(HidEventType *type, u8 *buffer, u16 length)
{
    if (hosversionBefore(7, 0, 0)) {
        return serviceDispatchOut(&g_btdrvSrv, hosversionBefore(4, 0, 0) ? 37 : 38, *type,
            .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
            .buffers = { {buffer, length} }
        );
    }
    else {
        u32 eventType;
        HidEventData *eventData = (HidEventData *)buffer; 
        HidReportDataPacket *packet = (HidReportDataPacket *)ReadBuffer(g_hidCircularBuffer);
              
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
            
            FreeBuffer(g_hidCircularBuffer);
            packet = (HidReportDataPacket *)ReadBuffer(g_hidCircularBuffer);
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
                        hosversionAtLeast(9, 0, 0) ? &packet->data.v2.report : &packet->data.report, 
                        sizeof(HidReport));
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
        
        FreeBuffer(g_hidCircularBuffer);
        return 0;
    }
}

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
                newOffset = currentReadOffset + buffer->data[currentReadOffset + 16] + 24;
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
            newOffset = currentReadOffset + buffer->data[currentReadOffset + 16] + 24;
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
