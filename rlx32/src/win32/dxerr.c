/*==========================================================================;
 *
 *
 *  File	:   dxerr.c
 *  Content	:   DirectX Error Library Source File (support for Direct X 7 and earlier)
 *			    for DirectX 8 and later, use DXERR8.LIB instead.
 *
 *              Messages for Generic COM interface, DirectDraw, DirectInput
 *				DirectSound, DirectPlay and Direct3D for DX3 to DX7
 *
 ****************************************************************************/

#include <stdio.h>
#include <windows.h>

#ifdef DX6
#define DIRECTINPUT_VERSION 0x0600
#elif defined DX7
#define DIRECTINPUT_VERSION 0x0700
#elif defined DX8 || defined DX81 || defined DX9
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <d3d.h>
#include <ddraw.h>
#include <dplay.h>
#include <dinput.h>
#include "win32/dxerr.h"

const char*  WINAPI DXGetErrorDescriptionA(HRESULT hr)
{
	switch(hr)
	{
		// COM ------------------------------------------------------------------------------------

		case E_UNEXPECTED:
			return"Catastrophic failure";

		case E_NOTIMPL:
			return"Not implemented";

		case E_OUTOFMEMORY:
			return"Ran out of memory";

		case E_INVALIDARG:
			return"An invalid parameter was passed to thereturning function";

		case E_NOINTERFACE:
			return"Requested COM interface not available";

		case S_OK:
			return"No error occurred";

		case E_POINTER:
			return"Invalid pointer";

		case E_HANDLE:
			return"Invalid handle";

		case E_ACCESSDENIED:
			return"General access denied error";

		case E_FAIL:
			return"An undetermined error occurred";

		case E_ABORT:
			return"Operation aborted";

		// DIRECT 3D ----------------------------------------------------------------

		case D3DERR_BADMAJORVERSION:
			return"Bad MAJOR VERSION";

		case D3DERR_BADMINORVERSION:
			return"Bad MINOR VERSION";

		case D3DERR_EXECUTE_LOCKED:
			return"Execute LOCKED";

		case D3DERR_EXECUTE_NOT_LOCKED:
			return"Execute not LOCKED";

		case D3DERR_EXECUTE_CREATE_FAILED:
			return"Execute CREATE FAILED";

		case D3DERR_EXECUTE_DESTROY_FAILED:
			return"Execute DESTROY FAILED";

		case D3DERR_EXECUTE_LOCK_FAILED:
			return"Execute LOCK FAILED";

		case D3DERR_EXECUTE_UNLOCK_FAILED:
			return"Execute UNLOCK FAILED";

		case D3DERR_EXECUTE_FAILED:
			return"Execute FAILED";

		case D3DERR_EXECUTE_CLIPPED_FAILED:
			return"Execute CLIPPED FAILED";

		case D3DERR_TEXTURE_NO_SUPPORT:
			return"Texture NO SUPPORT";

		case D3DERR_TEXTURE_NOT_LOCKED:
			return"Texture not LOCKED";

		case D3DERR_TEXTURE_LOCKED:
			return"Texture LOCKED";

		case D3DERR_TEXTURE_CREATE_FAILED:
			return"Texture CREATE FAILED";

		case D3DERR_TEXTURE_DESTROY_FAILED:
			return"Texture DESTROY FAILED";

		case D3DERR_TEXTURE_LOCK_FAILED:
			return"Texture LOCK FAILED";

		case D3DERR_TEXTURE_UNLOCK_FAILED:
			return"TEXTURE_UNLOCK_FAILED";

		case D3DERR_TEXTURE_LOAD_FAILED:
			return"TEXTURE_LOAD_FAILED";

		case D3DERR_SETVIEWPORTDATA_FAILED:
			return"SETVIEWPORTDATA FAILED";

		case D3DERR_MATERIAL_CREATE_FAILED:
			return"MATERIAL CREATE FAILED";

		case D3DERR_MATERIAL_DESTROY_FAILED:
			return"MATERIAL DESTROY FAILED";

		case D3DERR_MATERIAL_SETDATA_FAILED:
			return"MATERIAL SETDATA FAILED";

		case D3DERR_LIGHT_SET_FAILED:
			return"LIGHT SET FAILED";

		case D3DERR_VIEWPORTHASNODEVICE:
			return"Viewport has no device";

#if DIRECTDRAW_VERSION > 0x0500
		case D3DERR_STENCILBUFFER_NOTPRESENT:
			return"Stencilbuffer not present";

		case D3DERR_ZBUFFER_NOTPRESENT:
			return"Zbuffer not present";

		case D3DERR_TOOMANYOPERATIONS:
			return"The application is requesting more texture-filtering operations than the device supports";

		case D3DERR_UNSUPPORTEDALPHAARG:
			return"The device does not support a specified texture-blending argument for the alpha channel";

		case D3DERR_UNSUPPORTEDALPHAOPERATION:
			return"The device does not support a specified texture-blending operation for the alpha channel";

		case D3DERR_UNSUPPORTEDCOLORARG:
			return"The device does not support a specified texture-blending argument for color values";

		case D3DERR_UNSUPPORTEDCOLOROPERATION:
			return"The device does not support a specified texture-blending operation for color values";

		case D3DERR_UNSUPPORTEDFACTORVALUE:
			return"The device does not support the specified Texture factor value";

		case D3DERR_UNSUPPORTEDTEXTUREFILTER:
			return"The device does not support the specified Texture filter";

		case D3DERR_CONFLICTINGRENDERSTATE:
			return"The currently set render states cannot be used together";

		case D3DERR_CONFLICTINGTEXTUREFILTER:
			return"The current Texture filters cannot be used together";

		case D3DERR_CONFLICTINGTEXTUREPALETTE:
			return"The current textures cannot be used simultaneously This generally occurs when a multiTexture device requires that all palletized textures simultaneously enabled also share the same palette";

		case D3DERR_WRONGTEXTUREFORMAT:
			return"The pixel format of the Texture surface is not valid";
#endif

		// DIRECT DRAW ----------------------------------------------------------------

		case DDERR_ALREADYINITIALIZED:
			return"This object is already initialized";

		case DDERR_CANNOTATTACHSURFACE:
			return"This surface can not be attached to the requested surface";

		case DDERR_CANNOTDETACHSURFACE:
			return"This surface can not be detached from the requested surface";

		case DDERR_CURRENTLYNOTAVAIL:
			return"Support is currently not available";

		case DDERR_EXCEPTION:
			return"An exception was encountered while performing the requested operation";

		case DDERR_HEIGHTALIGN:
			return"Height of rectangle provided is not a multiple of reqd alignment";

		case DDERR_INCOMPATIBLEPRIMARY:
			return"Unable to match primary surface creation request with existing primary surface";

		case DDERR_INVALIDCAPS:
			return"One or more of the caps bits passed to the callback are incorrect";

		case DDERR_INVALIDCLIPLIST:
			return"DirectDraw does not support provided Cliplist";

		case DDERR_INVALIDMODE:
			return"DirectDraw does not support the requested mode";

		case DDERR_INVALIDOBJECT:
			return"DirectDraw received a pointer that was an invalid DIRECTDRAW object";

		case DDERR_INVALIDPIXELFORMAT:
			return"pixel format was invalid as specified";

		case DDERR_INVALIDRECT:
			return"Rectangle provided was invalid";

		case DDERR_LOCKEDSURFACES:
			return"Operation could not be carried out because one or more surfaces are locked";

		case DDERR_NO3D:
			return"There is no 3D present";

		case DDERR_NOALPHAHW:
			return"Operation could not be carried out because there is no alpha accleration";

		case DDERR_NOCLIPLIST:
			return"no clip list available";

		case DDERR_NOCOLORCONVHW:
			return"Operation could not be carried out because there is no color conversion";

		case DDERR_NOCOOPERATIVELEVELSET:
			return"Create function called without DirectDraw object method SetCooperativeLevel";

		case DDERR_NOCOLORKEY:
			return"Surface doesn't currently have a color key";

		case DDERR_NOCOLORKEYHW:
			return"Operation could not be carried out because there is no hardware support";

		case DDERR_NODIRECTDRAWSUPPORT:
			return"No DirectDraw support possible with current display driver";

		case DDERR_NOEXCLUSIVEMODE:
			return"The operation requires the application to have exclusive mode, but the application does not have exclusive mode";

		case DDERR_NOFLIPHW:
			return"Flipping visible surfaces is not supported";

		case DDERR_NOGDI:
			return"There is no GDI present";

		case DDERR_NOMIRRORHW:
			return"Operation could not be carried out because there is no hardware present";

		case DDERR_NOTFOUND:
			return"Requested item was not found";

		case DDERR_NOOVERLAYHW:
			return"Operation could not be carried out because there is no overlay hardware";

		case DDERR_NORASTEROPHW:
			return"Operation could not be carried out because there is no appropriate raster";

		case DDERR_NOROTATIONHW:
			return"Operation could not be carried out because there is no rotation hardware";

		case DDERR_NOSTRETCHHW:
			return"Operation could not be carried out because there is no hardware support";

		case DDERR_NOT4BITCOLOR:
			return"DirectDrawSurface is not in 4 bit color palette and the requested operation";

		case DDERR_NOT4BITCOLORINDEX:
			return"DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette";

		case DDERR_NOT8BITCOLOR:
			return"DirectDraw Surface is not in 8 bit color mode and the requested operation";

		case DDERR_NOTEXTUREHW:
			return"Operation could not be carried out because there is no Texture mapping";

		case DDERR_NOVSYNCHW:
			return"Operation could not be carried out because there is no hardware support";

		case DDERR_NOZBUFFERHW:
			return"No zbuffer blting";

		case DDERR_NOZOVERLAYHW:
			return"Overlay surfaces could not be z layered based on their BltOrder";

		case DDERR_OUTOFCAPS:
			return"The hardware needed for the requested operation";

		case DDERR_OUTOFVIDEOMEMORY:
			return"DirectDraw does not have enough memory to perform the operation";

		case DDERR_OVERLAYCANTCLIP:
			return"Hardware does not support clipped overlays";

		case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
			return"Can only have ony color key active at one time";

		case DDERR_PALETTEBUSY:
			return"Access to this palette is being refused";

		case DDERR_COLORKEYNOTSET:
			return"No src color key specified";

		case DDERR_SURFACEALREADYATTACHED:
			return"This surface is already attached";

		case DDERR_SURFACEALREADYDEPENDENT:
			return"This surface is already a dependency of the surface";

		case DDERR_SURFACEBUSY:
			return"Access to this surface is being refused. Busy";

		case DDERR_CANTLOCKSURFACE:
			return"Access to this surface is being refused. Can't lock";

		case DDERR_SURFACEISOBSCURED:
			return"Access to Surface refused because Surface is obscured";

		case DDERR_SURFACELOST:
			return"Access to this surface is being refused because the surface is gone";

		case DDERR_SURFACENOTATTACHED:
			return"The requested surface is not attached";

		case DDERR_TOOBIGHEIGHT:
			return"Height requested by DirectDraw is too large";

		case DDERR_TOOBIGSIZE:
			return"Size requested by DirectDraw is too large";

		case DDERR_TOOBIGWIDTH:
			return"Width requested by DirectDraw is too large";

		case DDERR_UNSUPPORTEDFORMAT:
			return"FOURCC format requested is unsupported by DirectDraw";

		case DDERR_UNSUPPORTEDMASK:
			return"Bitmask in the pixel format requested is unsupported by DirectDraw";

		case DDERR_VERTICALBLANKINPROGRESS:
			return"vertical blank is in progress";

		case DDERR_WASSTILLDRAWING:
			return"Informs DirectDraw that the previous Blt which is transfering information";

		case DDERR_XALIGN:
			return"Rectangle provided was not horizontally aligned on reqd. boundary";

		case DDERR_INVALIDDIRECTDRAWGUID:
			return"The GUID passed to DirectDrawCreate is not a valid";

		case DDERR_DIRECTDRAWALREADYCREATED:
			return"A DirectDraw object representing this driver has already been created";

		case DDERR_NODIRECTDRAWHW:
			return"A hardware only DirectDraw object creation was attempted";

		case DDERR_PRIMARYSURFACEALREADYEXISTS:
			return"this process already has created a primary surface";

		case DDERR_NOEMULATION:
			return"software emulation not available";

		case DDERR_REGIONTOOSMALL:
			return"region passed to Clipper::GetClipList is too small";

		case DDERR_CLIPPERISUSINGHWND:
			return"an attempt was made to set a clip list for a clipper objec";

		case DDERR_NOCLIPPERATTACHED:
			return"No clipper object attached to surface object";

		case DDERR_NOHWND:
			return"Clipper notification requires an HWND or";

		case DDERR_HWNDSUBCLASSED:
			return"HWND used by DirectDraw CooperativeLevel has been subclassed,";

		case DDERR_HWNDALREADYSET:
			return"The CooperativeLevel HWND has already been set";

		case DDERR_NOPALETTEATTACHED:
			return"No palette object attached to this surface";

		case DDERR_NOPALETTEHW:
			return"No hardware support for 16 or 256 color palettes";

		case DDERR_BLTFASTCANTCLIP:
			return"If a clipper object is attached to the source surface passed into a";

		case DDERR_NOBLTHW:
			return"No blter";

		case DDERR_NODDROPSHW:
			return"No DirectDraw ROP hardware";

		case DDERR_OVERLAYNOTVISIBLE:
			return"returned when GetOverlayPosition is called on a hidden overlay";

		case DDERR_NOOVERLAYDEST:
			return"returned when GetOverlayPosition is called on a overlay that UpdateOverlay";

		case DDERR_INVALIDPOSITION:
			return"returned when the position of the overlay on the destionation";

		case DDERR_NOTAOVERLAYSURFACE:
			return"returned when an overlay member is called for a non-overlay";

		case DDERR_EXCLUSIVEMODEALREADYSET:
			return"An attempt was made to set the cooperative level when it was already";

		case DDERR_NOTFLIPPABLE:
			return"An attempt has been made to flip a surface that is not flippable";

		case DDERR_CANTDUPLICATE:
			return"Can't duplicate primary & 3D surfaces,";

		case DDERR_NOTLOCKED:
			return"Surface was not locked";

		case DDERR_CANTCREATEDC:
			return"Windows can not create any more DCs";

		case DDERR_NODC:
			return"No DC was ever created for this surface";

		case DDERR_WRONGMODE:
			return"This surface can not be restored because it was created in a different";

		case DDERR_IMPLICITLYCREATED:
			return"This surface can not be restored because it is an implicitly created";

		case DDERR_NOTPALETTIZED:
			return"The surface being used is not a palette-based surface";

		case DDERR_UNSUPPORTEDMODE:
			return"The display is currently in an unsupported mode";

		case DDERR_NOMIPMAPHW:
			return"Operation could not be carried out because there is no mip-map";

		case DDERR_INVALIDSURFACETYPE:
			return"The requested action could not be performed because the surface was of the wrong type";

		case DDERR_NOOPTIMIZEHW:
			return"Device does not support optimized surfaces, therefore no video memory optimized surfaces";

		case DDERR_NOTLOADED:
			return"Surface is an optimized surface, but has not yet been allocated any memory";

		case DDERR_DCALREADYCREATED:
			return"A DC has already beenreturned for this surface";

		case DDERR_NONONLOCALVIDMEM:
			return"An attempt was made to allocate non-local video memory from a device that does not support non-local video memory";

		case DDERR_CANTPAGELOCK:
			return"The attempt to page lock a surface failed";

		case DDERR_CANTPAGEUNLOCK:
			return"The attempt to page unlock a surface failed";

		case DDERR_NOTPAGELOCKED:
			return"An attempt was made to page unlock a surface with no outstanding page locks";

		case DDERR_MOREDATA:
			return"There is more data available than the specified buffer size could hold";

		case DDERR_VIDEONOTACTIVE:
			return"The video port is not active";

		case DDERR_DEVICEDOESNTOWNSURFACE:
			return"Surfaces created by one direct draw device cannot be used directly";

		// DIRECT INPUT ----------------------------------------------------------------

		case DI_NOTATTACHED:
			return"The device exists but is not currently attached";

		case DI_POLLEDDEVICE:
			return"The device is a polled device.  As a result, device buffering"
			"will not collect any data and event notifications will not be"
			"signalled until GetDeviceState is called";

		case DI_DOWNLOADSKIPPED:
			return"The parameters of the effect were successfully updated by "
			"IDirectInputEffect::SetParameters, but the effect was not "
			"downloaded because the device is not exclusively acquired "
			"or because the DIEP_NODOWNLOAD flag was passed";

		case DI_EFFECTRESTARTED:
			return"The parameters of the effect were successfully updated by "
			"IDirectInputEffect::SetParameters, but in order to change "
			"the parameters, the effect needed to be restarted ";

		case DI_TRUNCATED:
			return"The parameters of the effect were successfully updated by "
			"IDirectInputEffect::SetParameters, but some of them were "
			"beyond the capabilities of the device and were truncated ";

		case DI_TRUNCATEDANDRESTARTED:
			return"Equal to DI_EFFECTRESTARTED | DI_TRUNCATED";

		case DIERR_OLDDIRECTINPUTVERSION:
			return"The application requires a newer version of DirectInput";

		case DIERR_BETADIRECTINPUTVERSION:
			return "The application was written for an unsupported prerelease version of DirectInput";

		case DIERR_BADDRIVERVER:
			return "The object could not be created due to an incompatible driver version or mismatched or incomplete driver components";

		case DIERR_DEVICENOTREG:
			return "The device or device instance or effect is not registered with DirectInput";

		case DIERR_NOTFOUND:
			return "The requested object does not exist";

		case DIERR_NOTINITIALIZED:
			return "This object has not been initialized";

		case DIERR_ALREADYINITIALIZED:
			return "This object is already initialized";

		case DIERR_NOAGGREGATION:
			return "This object does not support aggregation";

		case DIERR_INPUTLOST:
			return "Access to the device has been lost.  It must be re-acquired";

		case DIERR_NOTACQUIRED:
			return "The operation cannot be performed unless the device is acquired";

		case DIERR_INSUFFICIENTPRIVS:
			return "Unable to IDirectInputJoyConfig_Acquire because the user does not have sufficient privileges to change the joystick configuration";

		case DIERR_DEVICEFULL:
			return "The device is full";

		case DIERR_MOREDATA:
			return "Not all the requested information fit into the buffer";

		case DIERR_NOTDOWNLOADED:
			return "The effect is not downloaded";

		case DIERR_HASEFFECTS:
			return "The device cannot be reinitialized because there are still effects attached to it";

		case DIERR_NOTEXCLUSIVEACQUIRED:
			return "The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode";

		case DIERR_INCOMPLETEEFFECT:
			return "The effect could not be downloaded because essential information "
			"is missing.  For example, no axes have been associated with the "
			"effect, or no type-specific information has been created";

		case DIERR_NOTBUFFERED:
			return "Attempted to read buffered device data from a device that is not buffered";

		case DIERR_EFFECTPLAYING:
			return "An attempt was made to modify parameters of an effect while it is "
			"playing.  not all hardware devices support altering the parameters of an effect while it is playing";

#if DIRECTINPUT_VERSION > 0x600
		case DIERR_UNPLUGGED:
			return "The operation could not be completed because the device is not plugged in";

		case DIERR_REPORTFULL:
			return "SendDeviceData failed because more information was requested to be sent than can be sent to the device. "
			"Some devices have restrictions on how much data can be sent to them.  (For example, "
			"there might be a limit on the number of buttons that can be pressed at once.)";
#endif

		// DIRECTPLAY -------------------------------------------------------------------------------

		case DPERR_ACTIVEPLAYERS:
			return"The requested operation cannot be performed because there are existing active players";

		case DPERR_ALREADYINITIALIZED:
			return"This object is already initialized";

		case DPERR_APPNOTSTARTED:
			return"The application has not been started yet";

		case DPERR_AUTHENTICATIONFAILED:
			return"The password or credentials supplied could not be authenticated";

		case DPERR_BUFFERTOOLARGE:
			return"The data buffer is too large to store";

		case DPERR_BUSY:
			return"A message cannot be sent because the transmission medium is busy";

		case DPERR_BUFFERTOOSMALL:
			return"The supplied buffer is not large enough to contain the requested data";

		case DPERR_CANTADDPLAYER:
			return"The player cannot be added to the session";

		case DPERR_CANTCREATEGROUP:
			return"A new group cannot be created";

		case DPERR_CANTCREATEPLAYER:
			return"A new player cannot be created";

		case DPERR_CANTCREATEPROCESS:
			return"cannot start the application";

		case DPERR_CANTCREATESESSION:
			return"A new session cannot be created";

		case DPERR_CANTLOADSSPI:
			return"No credentials were supplied and the software security package (SSPI) that will prompt for credentials cannot be loaded";

		case DPERR_CAPSNOTAVAILABLEYET:
			return"The capabilities of the DirectPlay object have not been determined yet. This error will occur if the DirectPlay object is implemented on a connectivity solution that requires polling..";

		case DPERR_CONNECTING:
			return"The method is in the process of connecting to the network. The application should keep calling the method until itreturns DP_OK, indicating successful completion, or itreturns a different error";

		case DPERR_ENCRYPTIONFAILED:
			return"The requested information could not be digitally encrypted. Encryption is used for message privacy. This error is only relevant in a secure session";

		case DPERR_EXCEPTION:
			return"An exception occurred when processing the request";

		case DPERR_INVALIDFLAGS:
			return"The flags passed to this function are invalid";

		case DPERR_INVALIDGROUP:
			return"The group ID is not recognized as a valid group ID for this game session";

		case DPERR_INVALIDINTERFACE:
			return"The interface parameter is invalid";

		case DPERR_INVALIDOBJECT:
			return"The DirectPlay object pointer is invalid";

		case DPERR_INVALIDPASSWORD:
			return"An invalid password was supplied when attempting to join a session that requires a password";

		case DPERR_INVALIDPLAYER:
			return"The player ID is not recognized as a valid player ID for this game session";

		case DPERR_NOCAPS:
			return"The communication link that DirectPlay is attempting to use is not capable of this function";

		case DPERR_NOCONNECTION:
			return"No communication link was established";

		case DPERR_NOMESSAGES:
			return"There are no messages in the receive queue";

		case DPERR_NONAMESERVERFOUND:
			return"No name server (host) could be found or created. A host must exist to create a player";

		case DPERR_NONEWPLAYERS:
			return"The session is not accepting any new players";

		case DPERR_NOPLAYERS:
			return"There are no active players in the session";

		case DPERR_NOSESSIONS:
			return"There are no existing sessions for this game";

		case DPERR_NOTLOBBIED:
			return"Returned by the IDirectPlayLobby2::Connect method if the application was not started by using the IDirectPlayLobby2::RunApplication";

		case DPERR_PLAYERLOST:
			return"A player has lost the connection to the session";

		case DPERR_SENDTOOBIG:
			return"The message being sent by the IDirectPlay4::Send method is too large";

		case DPERR_SESSIONLOST:
			return"The connection to the session has been lost";

		case DPERR_SIGNFAILED:
			return"The requested information could not be digitally signed"
				"Digital signatures are used to establish the authenticity of messages";

		case DPERR_TIMEOUT:
			return"The operation could not be completed in the specified time";

		case DPERR_UNAVAILABLE:
			return"The requested function is not available at this time";

		case DPERR_UNINITIALIZED:
			return"The requested object has not been initialized";

		case DPERR_UNKNOWNAPPLICATION:
			return"An unknown application was specified";

		case DPERR_USERCANCEL:
			return"Can bereturned in two ways"
				  "1) The user canceled the connection process during a call to the IDirectPlay4::Open method"
				  "2) The user pressed Cancel";
	}

	return"Unidentified";
}

HRESULT WINAPI DXTraceA( char* strFile, DWORD dwLine, HRESULT hr, char* strMsg, BOOL bPopMsgBox )
{
	char strDbg[1024];
	if (strMsg)
	{
		sprintf(strDbg,"%s: %d: %s ( %s )", strFile, dwLine, DXGetErrorDescriptionA(hr), strMsg);
	}
	else
	{
		sprintf(strDbg,"%s: %d: %s", strFile, dwLine, DXGetErrorDescriptionA(hr));
	}
	OutputDebugString(strDbg);
	if (bPopMsgBox)
	{
		MessageBox(0, strDbg, "DirectX", (FAILED(hr) ? MB_ICONERROR: 0) |  MB_OK);
	}
	return hr;
}

