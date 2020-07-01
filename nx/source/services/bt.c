#define NX_SERVICE_ASSUME_NON_DOMAIN
#include "service_guard.h"
#include "services/bt.h"
#include "services/applet.h"

static Service g_btSrv;

NX_GENERATE_SERVICE_GUARD(bt);

Result _btInitialize(void) {
    return smGetService(&g_btSrv, "bt");
}

void _btCleanup(void) {
    serviceClose(&g_btSrv);
}

Service* btGetServiceSession(void) {
    return &g_btSrv;
}

Result btLeClientReadCharacteristic(u32 connId, const GattId *svcId, bool primary, const GattId *charId, u8 auth) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64     aruid; 
        u32     connId;
        GattId  svcId;
        bool    primary;
        GattId  charId;
        u8      auth;
    } in = { aruid, connId, *svcId, primary, *charId, auth };

    return serviceDispatchIn(&g_btSrv, 0, in,
        .in_send_pid = true
    );
};

Result btLeClientReadDescriptor(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const GattId *descrId, u8 auth) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64     aruid; 
        u32     connId;
        GattId  svcId;
        bool    primary;
        GattId  charId;
        GattId  descrId;
        u8      auth;
    } in = { aruid, connId, *svcId, primary, *charId, *descrId, auth };

    return serviceDispatchIn(&g_btSrv, 1, in,
        .in_send_pid = true
    );
};

Result btLeClientWriteCharacteristic(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const void *data, u32 length, u8 auth, bool response) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64     aruid;
        u32     connId;
        GattId  svcId;
        bool    primary;
        GattId  charId;
        u8      auth;
        bool    response;
    } in = { aruid, connId, *svcId, primary, *charId, auth, response };

    return serviceDispatchIn(&g_btSrv, 2, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, length} },
        .in_send_pid = true
    );
};

Result btLeClientWriteDescriptor(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const GattId *descrId, const void *data, u32 length, u8 auth) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64 aruid;
        u32 connId;
        GattId svcId;
        bool primary;
        GattId charId;
        GattId descrId;
        u8 auth;
    } in = { aruid, connId, *svcId, primary, *charId, *descrId, auth };

    return serviceDispatchIn(&g_btSrv, 3, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, length} },
        .in_send_pid = true
    );
};

Result btLeClientRegisterNotification(u32 connId, const GattId *svcId, bool primary, const GattId *charId) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64     aruid;
        u32     connId;
        GattId  svcId;
        bool    primary;
        GattId  charId;
    } in = { aruid, connId, *svcId, primary, *charId };

    return serviceDispatchIn(&g_btSrv, 4, in,
        .in_send_pid = true
    );
};

Result btLeClientDeregisterNotification(u32 connId, const GattId *svcId, bool primary, const GattId *charId) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64     aruid;
        u32     connId;
        GattId  svcId;
        bool    primary;
        GattId  charId;
    } in = { aruid, connId, *svcId, primary, *charId };

    return serviceDispatchIn(&g_btSrv, 5, in,
        .in_send_pid = true
    );
};

Result btSetLeResponse(u8 a, const GattAttributeUuid *b, const GattAttributeUuid *c, const void *data, u32 length) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64 aruid;
        u8 a;
        GattAttributeUuid b;
        GattAttributeUuid c;
    } in = { aruid, a, *b, *c };

    return serviceDispatchIn(&g_btSrv, 6, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, length} },
        .in_send_pid = true
    );
};

Result btLeSendIndication(u8 a, const GattAttributeUuid *b, const GattAttributeUuid *c, const void *data, u32 length, bool f) {
    u64 aruid = appletGetAppletResourceUserId();

    const struct {
        u64 aruid;
        u8 a;
        GattAttributeUuid b;
        GattAttributeUuid c;
        bool f;
    } in = { aruid, a, *b, *c, f };

    return serviceDispatchIn(&g_btSrv, 7, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, length} },
        .in_send_pid = true
    );
};

Result btGetLeEventInfo(BluetoothBleEventType *type, u8 *buffer, u16 length) {
    u64 aruid = appletGetAppletResourceUserId();

    return serviceDispatchInOut(&g_btSrv, 8, aruid, *type,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {buffer, length} },
        .in_send_pid = true
    );
};

Result btRegisterBleEvent(Event *event) {
    Handle handle = INVALID_HANDLE;
    u64 aruid = appletGetAppletResourceUserId();

    Result rc = serviceDispatchIn(&g_btSrv, 9, aruid,
        .out_handle_attrs = { SfOutHandleAttr_HipcCopy },
        .out_handles = &handle,
        .in_send_pid = true
    );

    if (R_SUCCEEDED(rc))
        eventLoadRemote(event, handle, true);

    return rc;
};
