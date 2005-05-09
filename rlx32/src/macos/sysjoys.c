//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2004 - realtech VR

This file is part of No Gravity 1.9

No Gravity is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Stephane Denis
Prepared for public release: 02/24/2004 - Stephane Denis, realtech VR
*/
//-------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/errno.h>
#include <sysexits.h>
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <Kernel/IOKit/hidsystem/IOHIDUsageTables.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>

#include "_rlx32.h"
#include "_rlx.h"
#include "sysctrl.h"
#include "systools.h"

struct recElement
{
	IOHIDElementCookie cookie;				// unique value which identifies element, will NOT change
	int32_t min;								// reported min value possible
	int32_t max;								// reported max value possible

	/* runtime variables used for auto-calibration */
	int32_t minReport;							// min returned value
	int32_t maxReport;							// max returned value

	struct recElement * pNext;				// next element in list
};
typedef struct recElement recElement;

struct recDevice
{
	IOHIDDeviceInterface ** interface;		// interface to device, NULL = no interface

	char product[256];						// name of product
	int32_t usage;								// usage page from IOUSBHID Parser.h which defines general usage
	int32_t usagePage;							// usage within above page from IOUSBHID Parser.h which defines specific usage

	int32_t axes;								// number of axis (calculated, not reported by device)
	int32_t buttons;							// number of buttons (calculated, not reported by device)
	int32_t hats;								// number of hat switches (calculated, not reported by device)
	int32_t elements;							// number of total elements (shouldbe total of above) (calculated, not reported by device)
	int32_t vaxes;								// number of velocity axis (calculated, not reported by device)
	int32_t vraxes;							// number of velocity rotation axis (calculated, not reported by device)

	recElement* firstAxis;
	recElement* firstButton;
	recElement* firstHat;
	recElement* firstVAxis;
	recElement* firstVRAxis;

	struct recDevice  * pNext;				// next device
};

typedef struct recDevice recDevice;

/* Linked list of all available devices */
static struct recDevice *gpHIDDeviceList = NULL;


static void HIDReportErrorNum (char * strError, int32_t numError)
{
#ifdef _DEBUG
	SYS_Debug(strError);
#else
	UNUSED(strError);
	UNUSED(numError);
#endif
}

static void HIDGetCollectionElements (CFMutableDictionaryRef deviceProperties, recDevice *pDevice);

static SInt32 HIDGetElementValue (recDevice *pDevice, recElement *pElement)
{
	IOReturn result = kIOReturnSuccess;
	IOHIDEventStruct hidEvent;
	hidEvent.value = 0;
	
	if (NULL != pDevice && NULL != pElement && NULL != pDevice->interface)
	{
		result = (*(pDevice->interface))->getElementValue(pDevice->interface, pElement->cookie, &hidEvent);
		if (kIOReturnSuccess == result)
		{
			/* record min and max for auto calibration */
			if (hidEvent.value < pElement->minReport)
				pElement->minReport = hidEvent.value;

			if (hidEvent.value > pElement->maxReport)
				pElement->maxReport = hidEvent.value;
		}
	}

	// auto user scale
	return hidEvent.value;
}

/* similiar to HIDCalibratedValue but calibrates to an arbitrary scale instead of the elements default scale 

static SInt32 HIDScaledCalibratedValue (recDevice *pDevice, recElement *pElement, int32_t min, int32_t max)
{
	float deviceScale = max - min;
	float readScale = pElement->maxReport - pElement->minReport;
	SInt32 value = HIDGetElementValue(pDevice, pElement);
	if (readScale == 0)
		return value; // no scaling at all
	else
		return ((value - pElement->minReport) * deviceScale / readScale) + min;
}
*/

/* Create and open an interface to device, required prior to extracting values or building queues.
 * Note: appliction now owns the device and must close and release it prior to exiting
 */

static IOReturn HIDCreateOpenDeviceInterface (io_object_t hidDevice, recDevice *pDevice)
{
	IOReturn result = kIOReturnSuccess;
	HRESULT plugInResult = S_OK;
	SInt32 score = 0;
	IOCFPlugInInterface ** ppPlugInInterface = NULL;
	
	if (NULL == pDevice->interface)
	{
		result = IOCreatePlugInInterfaceForService (hidDevice, kIOHIDDeviceUserClientTypeID,
													kIOCFPlugInInterfaceID, &ppPlugInInterface, &score);
		if (kIOReturnSuccess == result)
		{
			// Call a method of the intermediate plug-in to create the device interface
			plugInResult = (*ppPlugInInterface)->QueryInterface (ppPlugInInterface,
								CFUUIDGetUUIDBytes (kIOHIDDeviceInterfaceID), (void *) &(pDevice->interface));
			if (S_OK != plugInResult)
				HIDReportErrorNum ("Couldnt query HID class device interface from plugInInterface", plugInResult);
			(*ppPlugInInterface)->Release (ppPlugInInterface);
		}
		else
			HIDReportErrorNum ("Failed to create **plugInInterface via IOCreatePlugInInterfaceForService.", result);
	}
	if (NULL != pDevice->interface)
	{
		result = (*(pDevice->interface))->open (pDevice->interface, 0);
		if (kIOReturnSuccess != result)
			HIDReportErrorNum ("Failed to open pDevice->interface via open.", result);
	}
	return result;
}

/* Closes and releases interface to device, should be done prior to exting application
 * Note: will have no affect if device or interface do not exist
 * application will "own" the device if interface is not closed
 * (device may have to be plug and re-plugged in different location to get it working again without a restart)
 */

static IOReturn HIDCloseReleaseInterface (recDevice *pDevice)
{
	IOReturn result = kIOReturnSuccess;

	if ((NULL != pDevice) && (NULL != pDevice->interface))
	{
		// close the interface
		result = (*(pDevice->interface))->close (pDevice->interface);
		if (kIOReturnNotOpen == result)
		{
			//  do nothing as device was not opened, thus can't be closed
		}
		else if (kIOReturnSuccess != result)
			HIDReportErrorNum ("Failed to close IOHIDDeviceInterface.", result);
		//release the interface
		result = (*(pDevice->interface))->Release (pDevice->interface);
		if (kIOReturnSuccess != result)
			HIDReportErrorNum ("Failed to release IOHIDDeviceInterface.", result);
		pDevice->interface = NULL;
	}	
	return result;
}

/* extracts actual specific element information from each element CF dictionary entry */

static void HIDGetElementInfo (CFTypeRef refElement, recElement *pElement)
{
	int32_t number;
	CFTypeRef refType;

	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementCookieKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->cookie = (IOHIDElementCookie) number;
	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementMinKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->min = number;
		pElement->maxReport = pElement->min;
	refType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementMaxKey));
	if (refType && CFNumberGetValue (refType, kCFNumberLongType, &number))
		pElement->max = number;
		pElement->minReport = pElement->max;
}

/* examines CF dictionary vlaue in device element hierarchy to determine if it is element of interest or a collection of more elements
 * if element of interest allocate storage, add to list and retrieve element specific info
 * if collection then pass on to deconstruction collection into additional individual elements
 */

static void HIDAddElement (CFTypeRef refElement, recDevice* pDevice)
{
	recElement* element = NULL;
	recElement** headElement = NULL;
	int32_t elementType, usagePage, usage;
	CFTypeRef refElementType = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementTypeKey));
	CFTypeRef refUsagePage = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementUsagePageKey));
	CFTypeRef refUsage = CFDictionaryGetValue (refElement, CFSTR(kIOHIDElementUsageKey));

	if ((refElementType) && (CFNumberGetValue (refElementType, kCFNumberLongType, &elementType)))
	{
		/* look at types of interest */
		if ((elementType == kIOHIDElementTypeInput_Misc) || 
			(elementType == kIOHIDElementTypeInput_Button) ||
			(elementType == kIOHIDElementTypeInput_Axis))
		{
			if (refUsagePage && CFNumberGetValue (refUsagePage, kCFNumberLongType, &usagePage) &&
				refUsage && CFNumberGetValue (refUsage, kCFNumberLongType, &usage))
			{
				switch (usagePage) /* only interested in kHIDPage_GenericDesktop and kHIDPage_Button */
				{
					case kHIDPage_GenericDesktop:
						{
							switch (usage) /* look at usage to determine function */
							{
								// Axis
								case kHIDUsage_GD_X:
								case kHIDUsage_GD_Y:
								case kHIDUsage_GD_Z:
								case kHIDUsage_GD_Rx:
								case kHIDUsage_GD_Ry:
								case kHIDUsage_GD_Rz:
								case kHIDUsage_GD_Slider:
								case kHIDUsage_GD_Dial:
								case kHIDUsage_GD_Wheel:
									element = (recElement *) NewPtrClear (sizeof (recElement));
									if (element)
									{
										pDevice->axes++;
										headElement = &(pDevice->firstAxis);
									}
								break;
								// Hats
								case kHIDUsage_GD_Hatswitch:
									element = (recElement *) NewPtrClear (sizeof (recElement));
									if (element)
									{
										pDevice->hats++;
										headElement = &(pDevice->firstHat);
									}
								break;
								// Velocity
								case kHIDUsage_GD_Vx: 
								case kHIDUsage_GD_Vy: 
								case kHIDUsage_GD_Vz: 
									element = (recElement *) NewPtrClear (sizeof (recElement));
									if (element)
									{
										pDevice->vaxes++;
										headElement = &(pDevice->firstVAxis);
									}
								break;

								// Rotation Velocity
								case kHIDUsage_GD_Vbrx:
								case kHIDUsage_GD_Vbry:
								case kHIDUsage_GD_Vbrz:
									element = (recElement *) NewPtrClear (sizeof (recElement));
									if (element)
									{
										pDevice->vraxes++;
										headElement = &(pDevice->firstVRAxis);
									}
								break;
	
								// DPad							
								case kHIDUsage_GD_DPadUp: 
								case kHIDUsage_GD_DPadDown:
								case kHIDUsage_GD_DPadRight:
								case kHIDUsage_GD_DPadLeft:
								break;
							
							}							
						}
						break;
					case kHIDPage_Button:
						element = (recElement *) NewPtrClear (sizeof (recElement));
						if (element)
						{
							pDevice->buttons++;
							headElement = &(pDevice->firstButton);
						}
						break;
					default:
						break;
				}
			}
		}
		else if (kIOHIDElementTypeCollection == elementType)
			HIDGetCollectionElements ((CFMutableDictionaryRef) refElement, pDevice);
	}

	if (element && headElement) /* add to list */
	{
		pDevice->elements++;
		if (NULL == *headElement)
			*headElement = element;
		else
		{
			recElement *elementPrevious, *elementCurrent;
			elementCurrent = *headElement;
			while (elementCurrent)
			{
				elementPrevious = elementCurrent;
				elementCurrent = elementPrevious->pNext;
			}
			elementPrevious->pNext = element;
		}
		element->pNext = NULL;
		HIDGetElementInfo (refElement, element);
	}
}

/* collects information from each array member in device element list (each array memeber = element) */

static void HIDGetElementsCFArrayHandler (const void * value, void * parameter)
{
	if (CFGetTypeID (value) == CFDictionaryGetTypeID ())
		HIDAddElement ((CFTypeRef) value, (recDevice *) parameter);
}

/* handles retrieval of element information from arrays of elements in device IO registry information */

static void HIDGetElements (CFTypeRef refElementCurrent, recDevice *pDevice)
{
	CFTypeID type = CFGetTypeID (refElementCurrent);
	if (type == CFArrayGetTypeID()) /* if element is an array */
	{
		CFRange range = {0, CFArrayGetCount (refElementCurrent)};
		/* CountElementsCFArrayHandler called for each array member */
		CFArrayApplyFunction (refElementCurrent, range, HIDGetElementsCFArrayHandler, pDevice);
	}
}			

/* handles extracting element information from element collection CF types
   used from top level element decoding and hierarchy deconstruction to flatten device element list
 */

static void HIDGetCollectionElements (CFMutableDictionaryRef deviceProperties, recDevice *pDevice)
{
	CFTypeRef refElementTop = CFDictionaryGetValue (deviceProperties, CFSTR(kIOHIDElementKey));
	if (refElementTop)
		HIDGetElements (refElementTop, pDevice);
}

/* use top level element usage page and usage to discern device usage page
   and usage setting appropriate vlaues in device record
 */

static void HIDTopLevelElementHandler (const void * value, void * parameter)
{
	CFTypeRef refCF = 0;
	if (CFGetTypeID (value) != CFDictionaryGetTypeID ())
		return;
	refCF = CFDictionaryGetValue (value, CFSTR(kIOHIDElementUsagePageKey));
	if (!CFNumberGetValue (refCF, kCFNumberLongType, &((recDevice *) parameter)->usagePage))
		HIDReportErrorNum ("CFNumberGetValue error retrieving pDevice->usagePage.", 0);
	refCF = CFDictionaryGetValue (value, CFSTR(kIOHIDElementUsageKey));
	if (!CFNumberGetValue (refCF, kCFNumberLongType, &((recDevice *) parameter)->usage))
		HIDReportErrorNum ("CFNumberGetValue error retrieving pDevice->usage.", 0);
}

/* extracts device info from CF dictionary 
   records in IO registry
*/

static void HIDGetDeviceInfo (io_object_t hidDevice, CFMutableDictionaryRef hidProperties, recDevice *pDevice)
{
	CFMutableDictionaryRef usbProperties = 0;
	io_registry_entry_t parent1, parent2;
	
	/* Mac OS X currently is not mirroring all USB properties to HID page so need to look at USB device page also
	 * get dictionary for usb properties: step up two levels and get CF dictionary for USB properties
	 */
	if ((KERN_SUCCESS == IORegistryEntryGetParentEntry (hidDevice, kIOServicePlane, &parent1)) &&
		(KERN_SUCCESS == IORegistryEntryGetParentEntry (parent1, kIOServicePlane, &parent2)) &&
		(KERN_SUCCESS == IORegistryEntryCreateCFProperties (parent2, &usbProperties, kCFAllocatorDefault, kNilOptions)))
	{
		if (usbProperties)
		{
			CFTypeRef refCF = 0;

			/* get product name */
			refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDProductKey));
			if (!refCF)
				refCF = CFDictionaryGetValue (usbProperties, CFSTR("USB Product Name"));
			if (refCF)
			{
				if (!CFStringGetCString (refCF, pDevice->product, 256, CFStringGetSystemEncoding ()))
					HIDReportErrorNum ("CFStringGetCString error retrieving pDevice->product.", 0);
			}
			
			/* get usage page and usage */
			refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDPrimaryUsagePageKey));
			if (refCF)
			{
				if (!CFNumberGetValue (refCF, kCFNumberLongType, &pDevice->usagePage))
					HIDReportErrorNum ("CFNumberGetValue error retrieving pDevice->usagePage.", 0);
				refCF = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDPrimaryUsageKey));
				if (refCF)
					if (!CFNumberGetValue (refCF, kCFNumberLongType, &pDevice->usage))
						HIDReportErrorNum ("CFNumberGetValue error retrieving pDevice->usage.", 0);
			}

			if (NULL == refCF) /* get top level element HID usage page or usage */
			{
				/* use top level element instead */
				CFTypeRef refCFTopElement = 0;
				refCFTopElement = CFDictionaryGetValue (hidProperties, CFSTR(kIOHIDElementKey));
				{
					/* refCFTopElement points to an array of element dictionaries */
					CFRange range = {0, CFArrayGetCount (refCFTopElement)};
					CFArrayApplyFunction (refCFTopElement, range, HIDTopLevelElementHandler, pDevice);
				}
			}

			CFRelease (usbProperties);
		}
		else
			HIDReportErrorNum ("IORegistryEntryCreateCFProperties failed to create usbProperties.", 0);

		if (kIOReturnSuccess != IOObjectRelease (parent2))
			HIDReportErrorNum ("IOObjectRelease error with parent2.", 0);
		if (kIOReturnSuccess != IOObjectRelease (parent1))
			HIDReportErrorNum ("IOObjectRelease error with parent1.", 0);
	}
}

static recDevice *HIDBuildDevice (io_object_t hidDevice)
{
	recDevice *pDevice = (recDevice *) NewPtrClear (sizeof (recDevice));
	if (pDevice)
	{
		/* get dictionary for HID properties */
		CFMutableDictionaryRef hidProperties = 0;
		kern_return_t result = IORegistryEntryCreateCFProperties (hidDevice, &hidProperties, kCFAllocatorDefault, kNilOptions);
		if ((result == KERN_SUCCESS) && hidProperties)
		{
			/* create device interface */
			result = HIDCreateOpenDeviceInterface (hidDevice, pDevice);
			if (kIOReturnSuccess == result)
			{
				HIDGetDeviceInfo (hidDevice, hidProperties, pDevice); /* hidDevice used to find parents in registry tree */
				HIDGetCollectionElements (hidProperties, pDevice);
			}
			else
			{
				DisposePtr((Ptr)pDevice);
				pDevice = NULL;
			}
			CFRelease (hidProperties);
		}
		else
		{
			DisposePtr((Ptr)pDevice);
			pDevice = NULL;
		}
	}
	return pDevice;
}

/* disposes of the element list associated with a device and the memory associated with the list
 */

static void HIDDisposeElementList (recElement **elementList)
{
	recElement *pElement = *elementList;
	while (pElement)
	{
		recElement *pElementNext = pElement->pNext;
		DisposePtr ((Ptr) pElement);
		pElement = pElementNext;
	}
	*elementList = NULL;
}

/* 
   Disposes of a single device, closing and releaseing interface, freeing memory for device and elements, 
   setting device pointer to NULL
   all your device no longer belong to us... (i.e., you do not 'own' the device anymore)
 */

static recDevice *HIDDisposeDevice (recDevice **ppDevice)
{
	kern_return_t result = KERN_SUCCESS;
	recDevice *pDeviceNext = NULL;
	if (*ppDevice)
	{
		// save next device prior to disposing of this device
		pDeviceNext = (*ppDevice)->pNext;
		
		/* free element lists */
		HIDDisposeElementList (&(*ppDevice)->firstAxis);
		HIDDisposeElementList (&(*ppDevice)->firstVAxis);
		HIDDisposeElementList (&(*ppDevice)->firstVRAxis);
		HIDDisposeElementList (&(*ppDevice)->firstButton);
		HIDDisposeElementList (&(*ppDevice)->firstHat);		

		result = HIDCloseReleaseInterface (*ppDevice); /* function sanity checks interface value (now application does not own device) */
		if (kIOReturnSuccess != result)
		{
			HIDReportErrorNum ("HIDCloseReleaseInterface failed when trying to dipose device.", result);
		}
		DisposePtr ((Ptr)*ppDevice);
		*ppDevice = NULL;
	}
	return pDeviceNext;
}

static int HIDOpen(io_iterator_t *pHIDObjectIterator)
{
	IOReturn result = kIOReturnSuccess;
	mach_port_t masterPort = NULL;
	CFMutableDictionaryRef hidMatchDictionary = NULL;	

	*pHIDObjectIterator = 0;

	result = IOMasterPort (bootstrap_port, &masterPort);
	if (kIOReturnSuccess != result)
	{
#ifdef _DEBUG
		SYS_Debug(__FILE__" :IOMasterPort error with bootstrap_port.");
#endif
		return -2;
	}

	/* Set up a matching dictionary to search I/O Registry by class name for all HID class devices. */
	hidMatchDictionary = IOServiceMatching (kIOHIDDeviceKey);
	if ((hidMatchDictionary == NULL))
	{
#ifdef _DEBUG
		SYS_Debug(__FILE__" :Failed to get HID CFMutableDictionaryRef via IOServiceMatching.");
#endif
		return -3;
	}

	/*/ Now search I/O Registry for matching devices. */
	result = IOServiceGetMatchingServices (masterPort, hidMatchDictionary, pHIDObjectIterator);

	/* Check for errors */
	if (kIOReturnSuccess != result)
	{
#ifdef _DEBUG
		SYS_Debug(__FILE__" : Couldn't create a HID object iterator.");
#endif
		return -4;
	}

	if (NULL == *pHIDObjectIterator) /* there are no device */
	{
		return -5;
	}
	return 0;
}

static int HIDRefCount;

static void HIDRelease()
{
	HIDRefCount--;
	if (HIDRefCount == 0)
	{
		HIDDisposeDevice(&gpHIDDeviceList);
		gpHIDDeviceList = 0;
	}
}

static int HIDInit()
{
	if (!HIDRefCount)
	{
		int result;
		
		recDevice *device, *lastDevice;
		io_iterator_t HIDObjectIterator;
		io_object_t ioHIDDeviceObject;	
		int ret = HIDOpen(&HIDObjectIterator);
		if (ret)
			return ret;
	 	
		/* build flat linked list of devices from device iterator */

		gpHIDDeviceList = lastDevice = NULL;
		
		while ((ioHIDDeviceObject = IOIteratorNext (HIDObjectIterator)))
		{
			/* build a device record */
			device = HIDBuildDevice (ioHIDDeviceObject);
			if (!device)
				continue;

			/* dump device object, it is no longer needed */
			result = IOObjectRelease (ioHIDDeviceObject);
			
			/* Add device to the end of the list */
			if (lastDevice)
				lastDevice->pNext = device;
			else
				gpHIDDeviceList = device;
			lastDevice = device;
		}

		result = IOObjectRelease (HIDObjectIterator); /* release the iterator */
	}
	HIDRefCount++;
	return 0;
}

static int isDeviceJoystick(recDevice *device)
{
	if ((device->usagePage != kHIDPage_GenericDesktop) ||
	   ((device->usage != kHIDUsage_GD_Joystick &&
	     device->usage != kHIDUsage_GD_GamePad)) ) 
		return 0;
    return 1;
}

static int isDeviceMouse(recDevice *device)
{
	if ((device->usagePage != kHIDPage_GenericDesktop) ||
	   ((device->usage != kHIDUsage_GD_Mouse) &&
		(device->usage != kHIDUsage_GD_Pointer)))
		return 0;
	return 1;
}

/*
static int isDeviceKeyboard(recDevice *device)
{
	if ((device->usagePage != kHIDPage_GenericDesktop) ||
	   ((device->usage != kHIDUsage_GD_Keyboard))
		return 0;
	return 1;
}
*/

static int JoystickOpen(void * hwnd, int bForceFeedback)
{
	/* Count the total number of devices we found */
	int ret = HIDInit();
	recDevice *device;
	if (ret)
		return ret;
	device = gpHIDDeviceList;
	sJOY->numControllers = 0;
	sJOY->numButtons = 0;
	while (device)
	{
		if (isDeviceJoystick(device))
		{
			/* Find prefered device */
			if (sJOY->numButtons <= device->buttons)
			{
				sJOY->device = device;
	    		sJOY->numButtons = device->buttons;
				sJOY->numAxes = device->axes;
				sJOY->numPOVs = device->hats;
				printf("Found joystick %d buttons, %d axis\n", sJOY->numButtons, sJOY->numAxes);    			
			}
			sJOY->numControllers++;
		}
		device = device->pNext;
	}	

	if (sJOY->numControllers)
	{
	}
	else
		return -4;

	return 0;
}

static WindowPtr g_pWnd;
static void ConvertToLocal(Point *pos)
{
    GrafPtr saveport;
    GetPort(&saveport);
    SetPort(GetWindowPort(g_pWnd));
    GlobalToLocal(pos);
    SetPort(saveport);
}

static int MouseOpen(void * hwnd)
{
	recDevice *device;

	int ret = HIDInit();
	if (ret)
		return ret;
	/* Count the total number of devices we found */
	device = gpHIDDeviceList;
	g_pWnd = (WindowPtr)hwnd;
	sMOU->numControllers = 0;
	sMOU->numButtons = 0;
	while (device)
	{
		if (isDeviceMouse(device))
		{
			/* Find prefered device */
			if (sMOU->numButtons <= device->buttons)
			{
				sMOU->device = device;
				sMOU->numButtons = device->buttons;
				sMOU->numAxes = device->axes;
				printf("Found mouse %d buttons, %d axes\n", sMOU->numButtons, sMOU->numAxes);    			
	
			}
			sMOU->numControllers++;
		}
		device = device->pNext;
	}

	if (sMOU->numControllers)
	{    	
    	
	}
	else
		return -4;

	return 0;
}

static unsigned long MouseUpdate(void *d)

{
    recDevice *device = (recDevice*) sMOU->device; // prefered device
	recElement *element;
	int32_t	*Axis = &sMOU->lX;
	u_int8_t *Buttons = sMOU->rgbButtons;


	// Get absolute position
	Point pos;
    GetGlobalMouse(&pos);
    ConvertToLocal(&pos);
    sMOU->x = pos.h;
    sMOU->y = pos.v;
   
	if (!device)
        return 1;
		
	sysMemCpy(sMOU->steButtons, sMOU->rgbButtons, sMOU->numButtons);

	// Get axis
	element = device->firstAxis;
	while (element)
	{
		*Axis = HIDGetElementValue(device, element);
		Axis++;
		element = element->pNext;
	}

	// Get buttons
	element = device->firstButton;	
	while (element)
	{
		*Buttons = HIDGetElementValue(device, element) ? 0x80 : 0;
		Buttons++;
		element = element->pNext;
	}
	return 0;
}

static unsigned long JoystickUpdate(void *p)
{
    recDevice *device = (recDevice*) sJOY->device; // prefered device    
	recElement *element;
	int32_t	*Axis = &sJOY->lX;
	int32_t	*VAxis = &sJOY->lVX;
	int32_t	*VRAxis = &sJOY->lVRx;
	u_int8_t *Buttons = sJOY->rgbButtons;
	u_int32_t *POVs = sJOY->rgdwPOV;	

	if (!device)
        return 1;

	sysMemCpy(sJOY->steButtons, sJOY->rgbButtons, sJOY->numButtons);

	// Get axis
	element = device->firstAxis;
	while (element)
	{
		*Axis = HIDGetElementValue(device, element); // HIDScaledCalibratedValue(device, element, 0, 65535);
		Axis++;
		element = element->pNext;
	}

	if (RLX.Joy.Config  == 0)
	{
		sJOY->lX  = (sJOY->lX  - RLX.Joy.J[0].MinX) * 65535 / (RLX.Joy.J[0].MaxX - RLX.Joy.J[0].MinX);
		sJOY->lY  = (sJOY->lY  - RLX.Joy.J[0].MinY) * 65535 / (RLX.Joy.J[0].MaxY - RLX.Joy.J[0].MinY);
		sJOY->lZ  = (sJOY->lZ  - RLX.Joy.J[1].MinX) * 65535 / (RLX.Joy.J[1].MaxX - RLX.Joy.J[1].MinX);
		sJOY->lRx = (sJOY->lRx - RLX.Joy.J[1].MinY) * 65535 / (RLX.Joy.J[1].MaxY - RLX.Joy.J[1].MinY);
	}

	// Get hats
	element = device->firstHat;
	while (element)
	{
		int value = HIDGetElementValue(device, element);
		if (element->max == 3) /* 4 position hatswitch - scale up value */
			value *= 2;
		else if (element->max != 7) /* Neither a 4 nor 8 positions - fall back to default position (centered) */
			value = -1;
		
		*POVs = (value>=0 && value<=7) ? 4500 * value : 0;
		POVs++;
		element = element->pNext;
	}

	// Get buttons
	element = device->firstButton;	
	while (element)
	{
		*Buttons = HIDGetElementValue(device, element) ? 0x80 : 0;
		Buttons++;
		element = element->pNext;
	}

	// Get velocity axis
	element = device->firstVAxis;
	while (element)
	{
		*VAxis = HIDGetElementValue(device, element) * 255; // HIDScaledCalibratedValue(device, element, 0, 65535);
		VAxis++;
		element = element->pNext;
	}

	// Get velocity rotation axis
	element = device->firstVRAxis;
	while (element)
	{
		*VRAxis = HIDGetElementValue(device, element) * 255; // HIDScaledCalibratedValue(device, element, 0, 65535);
		VRAxis++;
		element = element->pNext;
	}
    return 0;
}


JOY_ClientDriver *JOY_SystemGetInterface_STD(void)
{
	static JOY_ClientDriver HIDJoystick = {
	    JoystickOpen,
	    HIDRelease,
	    JoystickUpdate
	};
    sJOY = &HIDJoystick;
    return sJOY;
}

static void SetPosition(u_int32_t x, u_int32_t y)
{
    return;
}

static void Show(void)
{
	ShowCursor();
    return;
}

static void Hide(void)
{
	HideCursor();
    return;
}

MSE_ClientDriver *MSE_SystemGetInterface_STD(void)
{
	static MSE_ClientDriver HIDMouse = {
		MouseOpen,
		HIDRelease,
		Show,
		Hide,
		SetPosition,
		MouseUpdate
	};
	sMOU = &HIDMouse;
    return sMOU; 
}
