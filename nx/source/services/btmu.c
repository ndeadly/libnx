#include "service_guard.h"
#include "services/btmu.h"
#include "runtime/hosversion.h"

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
