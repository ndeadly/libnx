#include "service_guard.h"
#include "services/btmu.h"
#include "runtime/hosversion.h"
#include "services/applet.h"

static Service g_btmuSrv;
static Service g_btmUserCore;

static Result _btmuGetCore(Service *srv, Service *srv_out);

NX_GENERATE_SERVICE_GUARD(btmu);

Result _btmuInitialize(void) {
    Result rc = smGetService(&g_btmuSrv, "btm:u");

    if (R_SUCCEEDED(rc))
        rc = _btmuGetCore(&g_btmuSrv, &g_btmUserCore);

    return rc;
}

void _btmuCleanup(void) {
    serviceClose(&g_btmUserCore);
    serviceClose(&g_btmuSrv);
}

Service* btmuGetServiceSession(void) {
    return &g_btmuSrv;
}

static Result _btmuGetCore(Service *srv, Service *srv_out) {
    return serviceDispatch(srv, 0,
        .out_num_objects = 1,
        .out_objects = srv_out,
    );
}

Result btmuAcquireBleScanEvent(Event *event, u8 *flags) {
    Result rc;
    Handle eventHandle = INVALID_HANDLE;

    rc = serviceDispatchOut(&g_btmUserCore, 0, *flags,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &eventHandle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, eventHandle, false);

    return rc;
}

Result btmuGetBleScanFilterParameter(BleAdvertisePacketParameter *param, u16 unk) {
    return serviceDispatchInOut(&g_btmUserCore, 1, unk, *param);
}

Result btmuGetBleScanFilterParameter2(GattAttributeUuid *uuid, u16 unk) {
    return serviceDispatchInOut(&g_btmUserCore, 2, unk, *uuid);
}

Result btmuStartBleScanForGeneral(const BleAdvertisePacketParameter *param) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64 aruid;
        BleAdvertisePacketParameter param;
    } in = { aruid, *param };

    return serviceDispatchIn(&g_btmUserCore, 3, in,
        .in_send_pid = true
    );
}

Result btmuStopBleScanForGeneral(void) {
    return serviceDispatch(&g_btmUserCore, 4);
}

/*
Result btmuGetBleScanResultsForGeneral(BleScanResult *result, u8 unk) {
    u64 aruid = appletGetAppletResourceUserId();

    return serviceDispatchInOut(&g_btmUserCore, 5, aruid, , 
        .in_send_pid = true
    );
}
*/

Result btmuStartBleScanForPaired(const BleAdvertisePacketParameter *param) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64 aruid;
        BleAdvertisePacketParameter param;
    } in = { aruid, *param };

    return serviceDispatchIn(&g_btmUserCore, 6, in,
        .in_send_pid = true
    );
}

Result btmuStopBleScanForPaired(void) {
    return serviceDispatch(&g_btmUserCore, 7);
}

Result btmuStartBleScanForSmartDevice(const GattAttributeUuid *uuid) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64 aruid;
        GattAttributeUuid uuid;
    } in = { aruid, *uuid };

    return serviceDispatchIn(&g_btmUserCore, 8, in,
        .in_send_pid = true
    );
}

Result btmuStopBleScanForSmartDevice(void) {
    return serviceDispatch(&g_btmUserCore, 9);
}

/*
Result btmuGetBleScanResultsForSmartDevice(BleScanResult *results, u8 unk) {
    u64 aruid = appletGetAppletResourceUserId();



    return serviceDispatchInOut(&g_btmUserCore, 10, aruid, , 
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { {results, sizeof(BleScanResult)} }, 
        .in_send_pid = true
    );
}
*/

Result btmuAcquireBleConnectionEvent(Event *event, u8 *flags) {
    Result rc;
    Handle eventHandle = INVALID_HANDLE;

    rc = serviceDispatchOut(&g_btmUserCore, 17, *flags,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &eventHandle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, eventHandle, false);

    return rc;
}

Result btmuBleConnect(const BluetoothAddress *address) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64 aruid;
        BluetoothAddress address;
    } in = { aruid, *address };

    return serviceDispatchIn(&g_btmUserCore, 18, in,
        .in_send_pid = true
    );
}

Result btmuBleDisconnect(u32 unk) {
    return serviceDispatchIn(&g_btmUserCore, 19, unk);
}

/*
Result btmuBleGetConnectionState(BleConnectionInfo *info, u8 unk) {
    u64 aruid = appletGetAppletResourceUserId();

    return serviceDispatchInOut(&g_btmUserCore, 20, aruid, , 
        //.buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        //.buffers = { {info, sizeof(BleScanResult)} }, 
        .in_send_pid = true
    );
}
*/

Result btmuAcquireBlePairingEvent(Event *event, u8 *flags) {
    Result rc;
    Handle eventHandle = INVALID_HANDLE;

    rc = serviceDispatchOut(&g_btmUserCore, 21, *flags,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &eventHandle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, eventHandle, false);

    return rc;
}

Result btmuBlePairDevice(u32 unk, const BleAdvertisePacketParameter *param) {
    const struct {
        u32 unk;
        BleAdvertisePacketParameter param;
    } in = { unk, *param };

    return serviceDispatchIn(&g_btmUserCore, 22, in);
}

Result btmuBleUnPairDevice(u32 unk, const BleAdvertisePacketParameter *param) {
    const struct {
        u32 unk;
        BleAdvertisePacketParameter param;
    } in = { unk, *param };

    return serviceDispatchIn(&g_btmUserCore, 23, in);
}

Result btmuBleUnPairDevice2(const BluetoothAddress *address, const BleAdvertisePacketParameter *param) {
    const struct {
        BluetoothAddress address;
        BleAdvertisePacketParameter param;
    } in = { *address, *param };

    return serviceDispatchIn(&g_btmUserCore, 24, in);
}

/*
Result btmuBleGetPairedDevices(BluetoothAddress *address, u8 unk, const BleAdvertisePacketParameter *param) {

}
*/

Result btmuAcquireBleServiceDiscoveryEvent(Event *event, u8 *flags) {
    Result rc;
    Handle eventHandle = INVALID_HANDLE;

    rc = serviceDispatchOut(&g_btmUserCore, 26, *flags,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &eventHandle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, eventHandle, false);

    return rc;
}

/*
Result btmuGetGattServices(GattService *service, u8 unk1, u32 unk2) {

}

Result btmuGetGattService(GattService *service, u32 unk, const GattAttributeUuid *uuid) {

}

Result btmuGetGattIncludedServices(GattService *service, u8 unk1, u32 unk2, u16 unk3) {

}

Result btmuGetBelongingGattService(GattService *service, u32 unk1, u16 unk2) {

}

Result btmuGetGattCharacteristics(GattCharacteristic *characteristics, u8 unk1, u32 unk2, u16 unk3) {

}

Result btmuGetGattDescriptors(GattDescriptor *descriptors, u8 unk1, u32 unk2, u16 unk3) {

}
*/

Result btmuAcquireBleMtuConfigEvent(Event *event, u8 *flags) {
    Result rc;
    Handle eventHandle = INVALID_HANDLE;

    rc = serviceDispatchOut(&g_btmUserCore, 33, *flags,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &eventHandle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, eventHandle, false);

    return rc;
}

Result btmuConfigureBleMtu(u32 unk1, u16 unk2) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64 aruid;
        u32 unk1;
        u16 unk2;
    } in = { aruid, unk1, unk2 };

    return serviceDispatchIn(&g_btmUserCore, 34, in,
        .in_send_pid = true
    );
}

/*
Result btmuGetBleMtu(u32 unk) {
    u64 aruid = appletGetAppletResourceUserId();

    return serviceDispatchInOut(&g_btmUserCore, 35, in,
        .in_send_pid = true
    );
}
*/

Result btmuRegisterBleGattDataPath(const BleDataPath *datapath) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64 aruid;
        BleDataPath datapath;
    } in = { aruid, *datapath };

    return serviceDispatchIn(&g_btmUserCore, 36, in,
        .in_send_pid = true
    );
}

Result btmuUnregisterBleGattDataPath(const BleDataPath *datapath) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64 aruid;
        BleDataPath datapath;
    } in = { aruid, *datapath };

    return serviceDispatchIn(&g_btmUserCore, 37, in,
        .in_send_pid = true
    );
}
