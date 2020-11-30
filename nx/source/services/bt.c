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
    const struct {
        bool    primary;
        u8      auth;
        u8 pad[2];
        u32     connId;
        GattId  svcId;
        GattId  charId;
        u64     aruid; 
    } in = { primary, auth, {0}, connId, *svcId, *charId, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_btSrv, 0, in,
        .in_send_pid = true
    );
}

Result btLeClientReadDescriptor(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const GattId *descrId, u8 auth) {
    const struct {
        bool    primary;
        u8      auth;
        u8      pad[2];
        u32     connId;
        GattId  svcId;
        GattId  charId;
        GattId  descrId;
        u64     aruid; 
    } in = { primary, auth, {0}, connId, *svcId, *charId, *descrId, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_btSrv, 1, in,
        .in_send_pid = true
    );
}

Result btLeClientWriteCharacteristic(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const void *data, u32 length, u8 auth, bool response) {
    const struct {
        bool    primary;
        u8      auth;
        bool    response;
        u8      pad;
        u32     connId;
        GattId  svcId;
        GattId  charId;
        u64     aruid;
    } in = { primary, auth, response, 0, connId, *svcId, *charId, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_btSrv, 2, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, length} },
        .in_send_pid = true
    );
}

Result btLeClientWriteDescriptor(u32 connId, const GattId *svcId, bool primary, const GattId *charId, const GattId *descrId, const void *data, u32 length, u8 auth) {
    const struct {
        bool primary;
        u8 auth;
        u8 pad[2];
        u32 connId;
        GattId svcId;
        GattId charId;
        GattId descrId;
        u64 aruid;
    } in = { primary, auth, {0}, connId, *svcId, *charId, *descrId, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_btSrv, 3, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, length} },
        .in_send_pid = true
    );
}

Result btLeClientRegisterNotification(u32 connId, const GattId *svcId, bool primary, const GattId *charId) {
    const struct {
        bool    primary;
        u8      pad[3];
        u32     connId;
        GattId  svcId;
        GattId  charId;
        u64     aruid;
    } in = { primary, {0}, connId, *svcId, *charId, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_btSrv, 4, in,
        .in_send_pid = true
    );
}

Result btLeClientDeregisterNotification(u32 connId, const GattId *svcId, bool primary, const GattId *charId) {
    const struct {
        bool    primary;
        u8      pad[3];
        u32     connId;
        GattId  svcId;
        GattId  charId;
        u64     aruid;
    } in = { primary, {0}, connId, *svcId, *charId, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_btSrv, 5, in,
        .in_send_pid = true
    );
}

Result btSetLeResponse(u8 a, const GattAttributeUuid *b, const GattAttributeUuid *c, const void *data, u32 length) {
    const struct {
        u8 a;
        u8 pad[3];
        GattAttributeUuid b;
        GattAttributeUuid c;
        u8 pad2[4];
        u64 aruid;
    } in = {  a, {0}, *b, *c, {0}, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_btSrv, 6, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, length} },
        .in_send_pid = true
    );
}

Result btLeSendIndication(u8 a, const GattAttributeUuid *b, const GattAttributeUuid *c, const void *data, u32 length, bool f) {
    const struct {
        u8 a;
        bool f;
        u8 pad[2];
        GattAttributeUuid b;
        GattAttributeUuid c;
        u8 pad2[4];
        u64 aruid;
    } in = { a, f, {0}, *b, *c, {0}, appletGetAppletResourceUserId() };

    return serviceDispatchIn(&g_btSrv, 7, in,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_In },
        .buffers = { {data, length} },
        .in_send_pid = true
    );
}

Result btGetLeEventInfo(BluetoothBleEventType *type, u8 *buffer, u16 length) {
    u64 aruid = appletGetAppletResourceUserId();

    return serviceDispatchInOut(&g_btSrv, 8, aruid, *type,
        .buffer_attrs = { SfBufferAttr_HipcPointer | SfBufferAttr_Out },
        .buffers = { {buffer, length} },
        .in_send_pid = true
    );
}

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
}
