/* Copyright (c) 2001, Nokia. All rights reserved */

#ifndef __SOCKETS_PAN__
#define __SOCKETS_PAN__

#include <e32std.h>

_LIT(KPanicSockets, "Sockets");
_LIT(KPanicSocketsEngine, "SocketsEngine");
_LIT(KPanicSocketsEngineWrite, "SocketsEngineWrite");
_LIT(KPanicSocketsEngineRead, "SocketsEngineRead");

/** Sockets application panic codes */
enum TSocketsPanics 
    {
    ESocketsBasicUi = 1,
    ESocketsBadStatus,
    ESocketsAppView,
    ESocketsBadState
    };

#endif // __SOCKETS_PAN__
