#define NX_SERVICE_ASSUME_NON_DOMAIN
#include "service_guard.h"
#include "services/btm.h"
#include "runtime/hosversion.h"

static Service g_btmSrv;

NX_GENERATE_SERVICE_GUARD(btm);

Result _btmInitialize(void) {
    return smGetService(&g_btmSrv, "btm");
}

void _btmCleanup(void) {
    serviceClose(&g_btmSrv);
}

Service* btmGetServiceSession(void) {
    return &g_btmSrv;
}

Result btmGetState(u32 *state) {
    return serviceDispatchOut(&g_btmSrv, 0, *state);
}

Result btmGetHostDeviceProperty(BtmHostDeviceProperty *property) {
    return serviceDispatchOut(&g_btmSrv, 1, *property);
}

Result btmAcquireDeviceConditionEvent(Event *event, u8 *flags) {
    Handle handle = INVALID_HANDLE;
    Result rc;
    if (hosversionBefore(3, 0, 0)) {
        rc = serviceDispatch(&g_btmSrv, 2,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &handle,
        );
    }
    else {
        rc = serviceDispatchOut(&g_btmSrv, 2, *flags,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &handle,
        );
    }

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, handle, false);

    return rc;
}

Result btmGetDeviceCondition(BtmDeviceCondition *condition) {
    size_t size;
    if (hosversionBefore(5, 1, 0))
        size = sizeof(BtmDeviceConditionV100);
    else if (hosversionBefore(8, 0, 0))
        size = sizeof(BtmDeviceConditionV510);
    else if (hosversionBefore(9, 0, 0))
        size = sizeof(BtmDeviceConditionV800);
    else
        size = sizeof(BtmDeviceConditionV900);

    return serviceDispatch(&g_btmSrv, 3,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {condition, size} }
    );
}

//Result btmSetBurstMode() {}
//Result btmSetSlotMode() {}
//Result btmSetBluetoothMode() {}
//Result btmSetWlanMode() {}

Result btmAcquireDeviceInfoEvent(Event *event, u8 *flags) {
    Handle handle = INVALID_HANDLE;
    Result rc;
    if (hosversionBefore(3, 0, 0)) {
        rc = serviceDispatch(&g_btmSrv, 8,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &handle,
        );
    }
    else {
        rc = serviceDispatchOut(&g_btmSrv, 8, *flags,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &handle,
        );
    }

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, handle, false);

    return rc;
}

Result btmGetDeviceInfo(BtmDeviceInfo *devices) {
    return serviceDispatch(&g_btmSrv, 9,
        .buffer_attrs = { SfBufferAttr_FixedSize | SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {devices, sizeof(BtmDeviceInfo)} }
    );
}

Result btmAddDeviceInfo(const BtmDevice *info) {
    const struct {
        BtmDevice info;
    } in = { *info };

    return serviceDispatchIn(&g_btmSrv, 10, in);
}

Result btmRemoveDeviceInfo(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btmSrv, 11, in);
}

Result btmIncreaseDeviceInfoOrder(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btmSrv, 12, in);
}

//Result btmLlrNotify() {}

Result btmEnableRadio(void) {
    return serviceDispatch(&g_btmSrv, 14);
}

Result btmDisableRadio(void) {
    return serviceDispatch(&g_btmSrv, 15);
}

Result btmHidDisconnect(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btmSrv, 16, in);
}

//Result btmHidSetRetransmissionMode() {}

Result btmAcquireAwakeReqEvent(Event *event, u8 *flags) {
    Handle handle = INVALID_HANDLE;
    Result rc;
    if (hosversionBefore(3, 0, 0)) {
        rc = serviceDispatch(&g_btmSrv, 18,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &handle,
        );
    }
    else {
        rc = serviceDispatchOut(&g_btmSrv, 18, *flags,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &handle,
        );
    }

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, handle, false);

    return rc;
}

Result btmProtectDeviceInfo(const BluetoothAddress *address, u8 unk) {
    if (hosversionBefore(5, 0, 0))
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);

    const struct {
        BluetoothAddress address;
        u8               unk;
    } in = { *address, unk };

    return serviceDispatchIn(&g_btmSrv, 22, in);
}
