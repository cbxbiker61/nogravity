//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2005 - realtech VR

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
#ifndef __V3X_2H
#define __V3X_2H

__extern_c

    // Kernel allocation
_RLXEXPORTFUNC    int     RLXAPI  V3XKernel_Alloc(void);
_RLXEXPORTFUNC    void    RLXAPI  V3XKernel_Release(void);
void V3XKernel_CRC(int code);
_RLXEXPORTFUNC    void    RLXAPI  V3XKernel_RenderClass(void);
_RLXEXPORTFUNC    void    RLXAPI  V3XKernel_PushList(V3XBUFFER *pipe);
_RLXEXPORTFUNC    void    RLXAPI  V3XKernel_PopList(V3XBUFFER *pipe);
_RLXEXPORTFUNC    void    RLXAPI  V3XKernel_PopAddList(V3XBUFFER *pipe);

_RLXEXPORTFUNC    void    RLXAPI  V3XPlugIn_Add( int id, void (* __plug)(const void *) );

    // Meshes
_RLXEXPORTFUNC    V3XMESH RLXAPI *V3XMesh_New(int numVerts, int numFaces, int numMaterial, int numEdgesPerPoly);
_RLXEXPORTFUNC    void    RLXAPI  V3XMesh_Release(V3XMESH *obj);
_RLXEXPORTFUNC    void    RLXAPI V3XMesh_ReleaseDup(V3XMESH *obj);
_RLXEXPORTFUNC    void    RLXAPI  V3XMesh_Duplicate(V3XMESH *mesh1, V3XMESH *mesh2);
_RLXEXPORTFUNC    void    RLXAPI  V3XMesh_NormalizePoly(V3XMESH *Ob);
_RLXEXPORTFUNC    void    RLXAPI  V3XMesh_NormalizeEdges(V3XMESH *Ob);

    // Polygons
_RLXEXPORTFUNC    void     RLXAPI  V3XPoly_Alloc(V3XPOLY *f, int som);
_RLXEXPORTFUNC    void     RLXAPI  V3XPoly_Release(V3XPOLY *f);
_RLXEXPORTFUNC    V3XPOLY  RLXAPI *V3XPoly_XYClipping(V3XPOLY *clip);
_RLXEXPORTFUNC    V3XPOLY  RLXAPI *V3XPoly_ZClipNear(V3XPOLY *polygon);
_RLXEXPORTFUNC    V3XPOLY  RLXAPI *V3XPoly_ZClipFar(V3XPOLY *polygon);
_RLXEXPORTFUNC    V3XPOLY  RLXAPI *V3XPoly_Duplicate(V3XPOLY *clip);
_RLXEXPORTFUNC    void     RLXAPI  V3XPoly_SpriteZoom(V3XPOLY *f, GXSPRITE *sp, V3XVECTOR *pos, V3XSCALAR lx, V3XSCALAR ly, int option);
_RLXEXPORTFUNC    V3XPOLY  RLXAPI *V3XPoly_AddToPipeline(V3XPOLY *fce, unsigned options);
_RLXEXPORTFUNC    V3XPOLY  RLXAPI *V3XPoly_NewFromPipeline(void);

    // Memory
    #define V3X_CALLOC(a, b) (b*)MM_heap.malloc((a)*sizeof(b))
    #define V3XPoly_QAddPipeline(fi) { if (V3X.Buffer.MaxFaces<V3X.Buffer.MaxFacesDisplay)  V3X.Buffer.RenderedFaces[V3X.Buffer.MaxFaces++] = fi; }
    #define V3XPoly_QAlloc() V3X.Buffer.ClippedFaces + V3X.Buffer.MaxClipped
    #define V3XPoly_QTemp()  V3X.Buffer.ClippedFaces + V3X.Buffer.MaxClippedFaces
    #define V3XPoly_QExhausted() (V3X.Buffer.MaxClipped>=V3X.Buffer.MaxClippedFaces)
    #define V3XPoly_QAllocated()  V3X.Buffer.MaxClipped++
    
    #define V3XMaterial_QAlloc() V3X.Buffer.Mat + V3X.Buffer.MaxMat
	#define V3XMaterial_QExhausted() (V3X.Buffer.MaxMat>= V3X.Buffer.MaxTmpMaterials)
    #define V3XMaterial_QAllocated()  V3X.Buffer.MaxMat++
    
__end_extern_c

#endif
