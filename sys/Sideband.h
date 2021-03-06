/*
MIT License

Copyright (c) 2018 Benjamin "Nefarius" H�glinger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#pragma once

#include "driver.h"

#define NTDEVICE_NAME_STRING        L"\\Device\\HidGuardian"
#define SYMBOLIC_NAME_STRING        L"\\DosDevices\\HidGuardian"

#define IOCTL_INDEX                 0x801
#define FILE_DEVICE_HIDGUARDIAN     32768U

//
// Used for inverted calls to get request information
// 
#define IOCTL_HIDGUARDIAN_GET_CREATE_REQUEST        CTL_CODE(FILE_DEVICE_HIDGUARDIAN,   \
                                                                    IOCTL_INDEX + 0x00, \
                                                                    METHOD_BUFFERED,    \
                                                                    FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//
// Used to instruct driver to allow or deny request
// 
#define IOCTL_HIDGUARDIAN_SET_CREATE_REQUEST        CTL_CODE(FILE_DEVICE_HIDGUARDIAN,   \
                                                                    IOCTL_INDEX + 0x01, \
                                                                    METHOD_BUFFERED,    \
                                                                    FILE_WRITE_ACCESS)


#include <pshpack1.h>

#pragma warning(push)
#pragma warning(disable:4200) // disable warnings for structures with zero length arrays.
typedef struct _HIDGUARDIAN_GET_CREATE_REQUEST
{
    //
    // Size of packet
    // 
    IN ULONG Size;

    //
    // Arbitrary value to match request and response
    // 
    IN ULONG RequestId;

    //
    // ID of the process this request is related to
    // 
    OUT ULONG ProcessId;

    //
    // Index of device this request belongs to (zero-based)
    // 
    OUT ULONG DeviceIndex;

    //
    // Buffer containing Hardware ID string
    // 
    OUT WCHAR HardwareIds[];

} HIDGUARDIAN_GET_CREATE_REQUEST, *PHIDGUARDIAN_GET_CREATE_REQUEST;
#pragma warning(pop)

typedef struct _HIDGUARDIAN_SET_CREATE_REQUEST
{
    //
    // Arbitrary value to match request and response
    // 
    IN ULONG RequestId;

    //
    // Index of device this request belongs to (zero-based)
    // 
    IN ULONG DeviceIndex;

    //
    // TRUE if WdfRequestTypeCreate is allowed, FALSE otherwise
    // 
    IN BOOLEAN IsAllowed;

    //
    // If TRUE, the decision will be cached in the driver for the current PID
    // 
    IN BOOLEAN IsSticky;

} HIDGUARDIAN_SET_CREATE_REQUEST, *PHIDGUARDIAN_SET_CREATE_REQUEST;

#include <poppack.h>

typedef struct _CONTROL_DEVICE_CONTEXT
{
    WDFQUEUE InvertedCallQueue;

    BOOLEAN IsServicePresent;

} CONTROL_DEVICE_CONTEXT, *PCONTROL_DEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(CONTROL_DEVICE_CONTEXT, ControlDeviceGetContext)


EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL HidGuardianSidebandIoDeviceControl;
EVT_WDF_DEVICE_FILE_CREATE  HidGuardianSidebandDeviceFileCreate;
EVT_WDF_FILE_CLEANUP HidGuardianSidebandFileCleanup;

_Must_inspect_result_
_Success_(return == STATUS_SUCCESS)
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
HidGuardianCreateControlDevice(
    WDFDEVICE Device
);

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
HidGuardianDeleteControlDevice(
    WDFDEVICE Device
);
