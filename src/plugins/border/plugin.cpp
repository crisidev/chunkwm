#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../api/plugin_api.h"
#include "../../common/accessibility/application.h"
#include "../../common/accessibility/element.h"
#include "../../common/accessibility/observer.h"
#include "../../common/dispatch/carbon.h"
#include "../../common/dispatch/workspace.h"
#include "overlay.h"

#define internal static

internal ax_application *Application;
internal pid_t LastLaunchedPID;

internal
OBSERVER_CALLBACK(Callback)
{
    // ax_application *Application = (ax_application *) Reference;

    if(CFEqual(Notification, kAXFocusedWindowChangedNotification))
    {
        CGPoint Position = AXLibGetWindowPosition(Element);
        CGSize Size = AXLibGetWindowSize(Element);
        UpdateBorder(Position.x, Position.y, Size.width, Size.height);
    }
    else if(CFEqual(Notification, kAXWindowMovedNotification))
    {
        AXUIElementRef WindowRef = (AXUIElementRef) AXLibGetWindowProperty(Application->Ref, kAXFocusedWindowAttribute);
        if(CFEqual(WindowRef, Element))
        {
            CGPoint Position = AXLibGetWindowPosition(WindowRef);
            CGSize Size = AXLibGetWindowSize(WindowRef);
            UpdateBorder(Position.x, Position.y, Size.width, Size.height);
        }
        CFRelease(WindowRef);
    }
    else if(CFEqual(Notification, kAXWindowResizedNotification))
    {
        AXUIElementRef WindowRef = (AXUIElementRef) AXLibGetWindowProperty(Application->Ref, kAXFocusedWindowAttribute);
        if(CFEqual(WindowRef, Element))
        {
            CGPoint Position = AXLibGetWindowPosition(WindowRef);
            CGSize Size = AXLibGetWindowSize(WindowRef);
            UpdateBorder(Position.x, Position.y, Size.width, Size.height);
        }
        CFRelease(WindowRef);
    }
    /*
    else if(CFEqual(Notification, kAXWindowMiniaturizedNotification))
    {
        printf("kAXWindowMiniaturizedNotification\n");
    }
    else if(CFEqual(Notification, kAXWindowDeminiaturizedNotification))
    {
        printf("kAXWindowDeminiaturizedNotification\n");
    }
    */
}

internal void
UpdateBorderHelper(ax_application *Application)
{
    AXUIElementRef WindowRef = (AXUIElementRef) AXLibGetWindowProperty(Application->Ref, kAXFocusedWindowAttribute);
    if(WindowRef)
    {
        CGPoint Position = AXLibGetWindowPosition(WindowRef);
        CGSize Size = AXLibGetWindowSize(WindowRef);
        UpdateBorder(Position.x, Position.y, Size.width, Size.height);
        CFRelease(WindowRef);
    }
    else
    {
        UpdateBorder(0, 0, 0, 0);
    }

    if(AXLibAddApplicationObserver(Application, Callback))
    {
        printf("    plugin: subscribed to '%s' notifications\n", Application->Name);
    }
}

ax_application *FrontApplication()
{
    pid_t PID;
    ProcessSerialNumber PSN;
    GetFrontProcess(&PSN);
    GetProcessPID(&PSN, &PID);

    CFStringRef ProcessName = NULL;
    CopyProcessName(&PSN, &ProcessName);

    char *Name = CopyCFStringToC(ProcessName, true);
    if(!Name)
        Name = CopyCFStringToC(ProcessName, false);

    ax_application *Result = AXLibConstructApplication(PSN, PID, Name);
    if(Result)
    {
        if(LastLaunchedPID == PID)
        {
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.5 * NSEC_PER_SEC), dispatch_get_main_queue(),
            ^{
                UpdateBorderHelper(Result);
            });
            LastLaunchedPID = 0;
        }
        else
        {
            UpdateBorderHelper(Result);
        }
    }
    else
    {
        UpdateBorder(0, 0, 0, 0);
    }
    return Result;
}

void ApplicationLaunchedHandler(const char *Data, unsigned int DataSize)
{
    carbon_application_details *Info =
        (carbon_application_details *) Data;
    LastLaunchedPID = Info->PID;
}

void ApplicationActivatedHandler(const char *Data, unsigned int DataSize)
{
    /*workspace_application_details *Info =
        (workspace_application_details *) Data;*/

    if(Application)
    {
        AXLibDestroyApplication(Application);
        Application = NULL;
    }

    Application = FrontApplication();
}

inline bool
StringsAreEqual(const char *A, const char *B)
{
    bool Result = (strcmp(A, B) == 0);
    return Result;
}

/*
 * NOTE(koekeishiya): Function parameters
 * plugin *Plugin
 * const char *Node
 * const char *Data
 * unsigned int DataSize
 *
 * return: bool
 * */
PLUGIN_MAIN_FUNC(PluginMain)
{
    if(Node)
    {
        if(StringsAreEqual(Node, "chunkwm_export_application_activated"))
        {
            ApplicationActivatedHandler(Data, DataSize);
            return true;
        }
        else if(StringsAreEqual(Node, "chunkwm_export_application_launched"))
        {
            ApplicationLaunchedHandler(Data, DataSize);
            return true;
        }
    }

    return false;
}

/*
 * NOTE(koekeishiya):
 * param: plugin *Plugin
 * return: bool -> true if startup succeeded
 */
PLUGIN_BOOL_FUNC(PluginInit)
{
    printf("Plugin Init!\n");

    CreateBorder(0, 0, 0, 0);
    Application = FrontApplication();
    return true;
}

/*
 * NOTE(koekeishiya):
 * param: plugin *Plugin
 * return: void
 */
PLUGIN_VOID_FUNC(PluginDeInit)
{
    printf("Plugin DeInit!\n");
}

// NOTE(koekeishiya): Enable to manually trigger ABI mismatch
#if 0
#undef PLUGIN_API_VERSION
#define PLUGIN_API_VERSION 0
#endif

// NOTE(koekeishiya): Initialize plugin function pointers.
CHUNKWM_PLUGIN_VTABLE(PluginInit, PluginDeInit, PluginMain)

// NOTE(koekeishiya): Subscribe to ChunkWM events!
chunkwm_plugin_export Subscriptions[] =
{
    chunkwm_export_application_activated,
    chunkwm_export_application_launched,
};
CHUNKWM_PLUGIN_SUBSCRIBE(Subscriptions)

// NOTE(koekeishiya): Generate plugin
CHUNKWM_PLUGIN("Border", "0.0.1")