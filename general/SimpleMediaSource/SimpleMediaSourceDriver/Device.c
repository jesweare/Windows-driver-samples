/*++

Module Name:

    device.c - Device handling events for example driver.

Abstract:

   This file contains the device entry points and callbacks.

Environment:

    User-mode Driver Framework 2

--*/

#include "driver.h"
#include "device.tmh"

GUID CAMERA_CATEGORY = { STATIC_KSCATEGORY_VIDEO_CAMERA };
GUID CAPTURE_CATEGORY = { STATIC_KSCATEGORY_CAPTURE };
GUID VIDEO_CATEGORY = { STATIC_KSCATEGORY_VIDEO };

NTSTATUS
SimpleMediaSourceDriverCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Worker routine called to create a device and its software resources.

Arguments:

    DeviceInit - Pointer to an opaque init structure. Memory for this
                    structure will be freed by the framework when the WdfDeviceCreate
                    succeeds. So don't access the structure after that point.

Return Value:

    NTSTATUS

--*/
{
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    PDEVICE_CONTEXT deviceContext;
    WDFDEVICE device;
    NTSTATUS status;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    if (NT_SUCCESS(status)) {
        //
        // Get a pointer to the device context structure that we just associated
        // with the device object. We define this structure in the device.h
        // header file. DeviceGetContext is an inline function generated by
        // using the WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro in device.h.
        // This function will do the type checking and return the device context.
        // If you pass a wrong object handle it will return NULL and assert if
        // run under framework verifier mode.
        //
        deviceContext = DeviceGetContext(device);

        //
        // Initialize the context.
        //
        deviceContext->PrivateDeviceData = 0;

        //
        // Create a device interface so that applications can find and talk
        // to us.
        //
        status = WdfDeviceCreateDeviceInterface(
            device,
            &GUID_DEVINTERFACE_SimpleMediaSourceDriver,
            NULL // ReferenceString
            );

        if (NT_SUCCESS(status)) {
            //
            // Create a device interface so that application can find and talk
            // to us.
            //
            status = WdfDeviceCreateDeviceInterface(
                device,
                &CAPTURE_CATEGORY,
                NULL // ReferenceString
            );
        }

        if (NT_SUCCESS(status)) {
            //
            // Create a device interface so that application can find and talk
            // to us.
            //
            status = WdfDeviceCreateDeviceInterface(
                device,
                &VIDEO_CATEGORY,
                NULL // ReferenceString
            );
        }

        if (NT_SUCCESS(status)) {
            //
            // Initialize the I/O Package and any Queues
            //
            status = SimpleMediaSourceDriverQueueInitialize(device);
        }
    }

    return status;
}
