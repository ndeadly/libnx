#define NX_SERVICE_ASSUME_NON_DOMAIN
#include "service_guard.h"
#include "services/btmdbg.h"
#include "runtime/hosversion.h"

static Service g_btmdbgSrv;

NX_GENERATE_SERVICE_GUARD(btmdbg);

Result _btmdbgInitialize(void) {
    return smGetService(&g_btmdbgSrv, "btm:dbg");
}

void _btmdbgCleanup(void) {
    serviceClose(&g_btmdbgSrv);
}

Service* btmdbgGetServiceSession(void) {
    return &g_btmdbgSrv;
}

Result btmdbgAcquireDiscoveryEvent(Event *event, u8 *flags) {
    Handle handle = INVALID_HANDLE;
    Result rc;
    if (hosversionBefore(3, 0, 0)) {
        rc = serviceDispatch(&g_btmdbgSrv, 0,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &handle,
        );
    }
    else {
        rc = serviceDispatchOut(&g_btmdbgSrv, 0, *flags,
            .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
            .out_handles = &handle,
        );
    }

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, handle, false);

    return rc;
}

Result btmdbgStartDiscovery(void) {
    return serviceDispatch(&g_btmdbgSrv, 1);
}

Result btmdbgCancelDiscovery(void) {
        return serviceDispatch(&g_btmdbgSrv, 2);
}

//Result btmdbgGetDeviceProperty() {}

Result btmdbgCreateBond(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btmdbgSrv, 4, in);
}

Result btmdbgCancelBond(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btmdbgSrv, 5, in);
}

//Result btmdbgSetTsiMode() {}
//Result btmdbgGeneralTest() {}

Result btmdbgHidConnect(const BluetoothAddress *address) {
    const struct {
        BluetoothAddress address;
    } in = { *address };

    return serviceDispatchIn(&g_btmdbgSrv, 8, in);
}
