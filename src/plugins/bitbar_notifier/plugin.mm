#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../api/plugin_api.h"
#include "../../common/config/cvar.h"
#include "../../common/accessibility/application.h"

#include "constants.h"

#include "../../common/config/cvar.cpp"

#define internal static

internal chunkwm_api API;
internal const char *PluginName = "BitBarNotifier";
internal const char *PluginVersion = "0.0.1";

inline bool
StringsAreEqual(const char *A, const char *B)
{
    bool Result = (strcmp(A, B) == 0);
    return Result;
}

PLUGIN_MAIN_FUNC(PluginMain)
{
    if (StringsAreEqual(Node, "chunkwm_export_display_changed") ||
            StringsAreEqual(Node, "chunkwm_export_space_changed")) {
        char Command[120];
        // See https://github.com/matryer/bitbar/blob/master/Docs/URLScheme.md#refreshplugin
        sprintf(Command, "open -g 'bitbar://refreshPlugin?name=%s'", CVarStringValue(CVAR_BITBAR_UPDATE_SCRIPT));
        system(Command);
        return true;
    }

    return false;
}

PLUGIN_BOOL_FUNC(PluginInit)
{
    API = ChunkwmAPI;
    BeginCVars(&API);
    CreateCVar(CVAR_BITBAR_UPDATE_SCRIPT, BitBar_Default_Update_Script);
    return true;
}

PLUGIN_VOID_FUNC(PluginDeInit)
{
}

CHUNKWM_PLUGIN_VTABLE(PluginInit, PluginDeInit, PluginMain)

chunkwm_plugin_export Subscriptions[] =
{
    chunkwm_export_space_changed,
    chunkwm_export_display_changed,
};
CHUNKWM_PLUGIN_SUBSCRIBE(Subscriptions)

CHUNKWM_PLUGIN(PluginName, PluginVersion);
