#pragma once
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
#ifndef __V3X98COLLISION
#define __V3X98COLLISION

enum {
	V3XCTYPE_NONE = 0x0, // non-registered item
	V3XCTYPE_SPHERE = 0x1, // sphere
	V3XCTYPE_AXISBOX = 0x2, // axis aligned bounding box
	V3XCTYPE_FREEAXISBOX = 0x3, // free axis bounding box (not implemented)
	V3XCTYPE_MESH = 0x4 // mesh
};

enum {
	V3XCMODE_DONOTTESTSAMEID = 0x8 , // Can't check with same ID
	V3XCMODE_INVERTCONDITION = 0x10, // Invert condition (invert vector)
	V3XCMODE_NOFEEDBACK = 0x20, // do no smooth replacement.
	V3XCMODE_DONTMOVE = 0x40, // reference mesh can't move
	V3XCMODE_HIT  = 0x80, // Was hit
	V3XCMODE_DISPLAY = 0x100, // Draw collision mesh
	V3XCMODE_INVALIDATE = 0x200, // Disable collision system (not treated)
	V3XCMODE_CALLBACKONLY = 0x400, // If collision, call callback, don't compute displacement
	V3XCMODE_REFLEX = 0x800, // If collision, call callback, don't compute displacement
	V3XCMODE_REPLACEONPRJ = 0x1000
};

typedef struct _v3xcl_faceitem {
	uint8_t type;
	uint8_t numEdges;
	uint16_t sectorID;
	V3XVECTOR normal; // face normal
	V3XVECTOR *edges; // edges position (unXformed)
	union {
		struct _v3xcl_faceitem *next;
		uint32_t offset;
	};
} V3XCL_FACE;

typedef struct _v3xcl_mesh_disk { // 64b
	int32_t type; // type (first field)
	V3XVECTOR center; // center
	V3XVECTOR prev_center; // center after XForming (very rare)
	uint32_t numFaces; // number of faces
	uint32_t face; // face array
	uint32_t sectorList; // Sector list
	uint32_t maxsectors; // maxSectors
	uint32_t mesh_ref;
	uint32_t pad[4];
} V3XCL_MESHDISK;

typedef struct _v3xcl_mesh { // 64b
	int32_t type; // type (first field)
	V3XVECTOR center; // center
	V3XVECTOR prev_center; // center after XForming (very rare)
	uint32_t numFaces; // number of faces
	V3XCL_FACE *face; // face array
	uint16_t *sectorList; // Sector list
	uint32_t maxsectors; // maxSectors
	V3XMESH *mesh_ref;
	uint32_t pad[4];
} V3XCL_MESH;

static inline void CollMeshDiskToMem(const V3XCL_MESHDISK *d, V3XCL_MESH *m)
{
	m->type = d->type;
	m->center = d->center;
	m->prev_center = d->prev_center;
	m->numFaces = d->numFaces;
	m->maxsectors = d->maxsectors;
	m->pad[0] = d->pad[0];
	m->pad[1] = d->pad[1];
	m->pad[2] = d->pad[2];
	m->pad[3] = d->pad[3];
}

typedef struct _v3x_cl_sphere { // 64b
	int32_t type; // type (first field)
	V3XVECTOR center; // center
	V3XVECTOR _center; // center after XForming
	V3XSCALAR radius; // radius
	uint32_t pad;
} V3XCL_SPHERE;

typedef V3XCL_SPHERE V3XCL_SPHEREDISK;

static inline void SphereDiskToMem(const V3XCL_SPHEREDISK *d, V3XCL_SPHERE *m)
{
	m->type = d->type;
	m->center = d->center;
	m->_center = d->_center;
	m->radius = d->radius;
	m->pad = d->pad;
}

typedef struct _v3x_cl_box { // 64b
	int32_t type; // type (first field)
	V3XVECTOR center; // center
	V3XVECTOR _center; // center after XForming
	uint32_t pad[3]; // pad
	V3XVECTOR min; // min coordinates
	V3XVECTOR max; // max coordinates
} V3XCL_BOX;

typedef V3XCL_BOX V3XCL_BOXDISK;

static inline void BoxDiskToMem(const V3XCL_BOXDISK *d, V3XCL_BOX *m)
{
	m->type = d->type;
	m->center = d->center;
	m->_center = d->_center;
	m->pad[0] = d->pad[0];
	m->pad[1] = d->pad[1];
	m->pad[2] = d->pad[2];
	m->min = d->min;
	m->max = d->max;
}

typedef union _v3x_cl_item {
	int32_t type; // type (first field) or
	V3XCL_SPHERE sh; // ... sphere
	V3XCL_BOX box; // ... bounding box
	V3XCL_MESH mesh; // ... mesh
} V3XCL_ITEM;

typedef struct _v3x_cl_disk {
	uint32_t numItem; // numbers of items
	uint32_t weight; // weight (for collision inertia)
	uint32_t item; // item array
	V3XCL_SPHERE global; // global sphere
	V3XVECTOR velocity; // velocity vector
	uint32_t mesh_ref; // reference mesh
	uint32_t flags; // flags
	uint32_t ID; // Collision ID
	union {
		uint32_t callback; // callback when hitted
		uint8_t old[4]; // compatibility with V3X'97 (old[0]) Cs->flags|=V3XCMODE_INVERTCONDITION;
	};
	uint16_t hitCount;
	uint16_t hitID;
	V3XSCALAR reaction;
	uint32_t last_hit;
} V3XCLDISK;

typedef struct _v3x_cl {
	uint32_t numItem; // numbers of items
	uint32_t weight; // weight (for collision inertia)
	V3XCL_ITEM *item; // item array
	V3XCL_SPHERE global; // global sphere
	V3XVECTOR velocity; // velocity vector
	V3XMESH *mesh_ref; // reference mesh
	uint32_t flags; // flags
	uint32_t ID; // Collision ID
	union {
		uint32_t (*callback)(void *context); // callback when hitted
		uint8_t old[4]; // compatibility with V3X'97 (old[0]) Cs->flags|=V3XCMODE_INVERTCONDITION;
	};
	uint16_t hitCount;
	uint16_t hitID;
	V3XSCALAR reaction;
	V3XCL_ITEM *last_hit;
} V3XCL;

static inline void ClDiskToMem(const V3XCLDISK *d, V3XCL *m)
{
	m->numItem = d->numItem;
	m->weight = d->weight;
	SphereDiskToMem(&d->global, &m->global);
	m->velocity = d->velocity;
	m->flags = d->flags;
	m->ID = d->ID;
	m->old[0] = d->old[0];
	m->old[1] = d->old[1];
	m->old[2] = d->old[2];
	m->old[3] = d->old[3];
	m->hitCount = d->hitCount;
	m->hitID = d->hitID;
	m->reaction = d->reaction;
}

enum {
   V3XCL_MESHOPT_PORTAL = 0x1, // Optimize collision mesh with portal;
   V3XCL_MESHOPT_HIDDENOBJ = 0x2 // Do no treat hidden object
};

__extern_c

_RLXEXPORTFUNC    void   RLXAPI  V3XPlug_CollisionMove(const void *OVI); // plug in for V3XSCENE
//
_RLXEXPORTFUNC    V3XCL  RLXAPI *V3XCL_New(int numItems);
_RLXEXPORTFUNC    V3XCL  RLXAPI *V3XCL_NewFromMesh(V3XMESH *obj, int mode);
_RLXEXPORTFUNC    V3XCL  RLXAPI *V3XCL_NewFromDummy(V3XSCALAR radius);
_RLXEXPORTFUNC    void   RLXAPI  V3XCL_findGlobalSphere(V3XMESH *obj, V3XCL_SPHERE *Cs);
_RLXEXPORTFUNC    void   RLXAPI  V3XCL_Release(V3XCL *Cs);
//
_RLXEXPORTFUNC    void   RLXAPI  V3XCL_Xform(V3XCL *Cs);
_RLXEXPORTFUNC    void   RLXAPI  V3XCL_XformNoRef(V3XCL *Cs, V3XVECTOR *pos);
_RLXEXPORTFUNC    int    RLXAPI  V3XCL_Test(V3XCL *a, V3XCL *b);
_RLXEXPORTFUNC    void   RLXAPI  V3XCL_Draw(V3XCL *cs);

__end_extern_c

#endif

