#include "service_guard.h"
#include "services/btmsys.h"
#include "runtime/hosversion.h"

static Service g_btmsysSrv;
static Service g_btmSystemCore;

static Result _btmsysGetCore(Service *srv, Service *srv_out);

NX_GENERATE_SERVICE_GUARD(btmsys);

Result _btmsysInitialize(void) {
    Result rc = smGetService(&g_btmsysSrv, "btm:sys");

    if (R_SUCCEEDED(rc))
        rc = _btmsysGetCore(&g_btmsysSrv, &g_btmSystemCore);

    return rc;
}

void _btmsysCleanup(void) {
    serviceClose(&g_btmSystemCore);
    serviceClose(&g_btmsysSrv);
}

Service* btmsysGetServiceSession(void) {
    return &g_btmsysSrv;
}

static Result _btmsysGetCore(Service *srv, Service *srv_out) {
    return serviceDispatch(srv, 0,
        .out_num_objects = 1,
        .out_objects = srv_out,
    );
}

Result btmsysStartGamepadPairing(void) {
    return serviceDispatch(&g_btmSystemCore, 0);
}

Result btmsysCancelGamepadPairing(void) {
    return serviceDispatch(&g_btmSystemCore, 1);
}

Result btmsysClearGamepadPairingDatabase(void) {
    return serviceDispatch(&g_btmSystemCore, 2);
}

Result btmsysGetPairedGamepadCount(u8 *count) {
    return serviceDispatchOut(&g_btmSystemCore, 3, *count);
}

Result btmsysEnableRadio(void) {
    return serviceDispatch(&g_btmSystemCore, 4);
}

Result btmsysDisableRadio(void) {
    return serviceDispatch(&g_btmSystemCore, 5);
}

Result btmsysGetRadioOnOff(bool *state) {
    return serviceDispatchOut(&g_btmSystemCore, 6, *state);
}

Result btmsysAcquireRadioEvent(Event *event, u8 *flags) {
    Result rc;
    Handle eventHandle = INVALID_HANDLE;

    rc = serviceDispatchOut(&g_btmSystemCore, 7, *flags,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &eventHandle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, eventHandle, false);

    return rc;
}

Result btmsysAcquireGamepadPairingEvent(Event *event, u8 *flags) {
    Result rc;
    Handle eventHandle = INVALID_HANDLE;

    rc = serviceDispatchOut(&g_btmSystemCore, 8, *flags,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &eventHandle,
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, eventHandle, false);

    return rc;
}

Result btmsysIsGamepadPairingStarted(bool *started) {
    return serviceDispatchOut(&g_btmSystemCore, 9, *started);
}
