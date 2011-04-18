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
#ifndef __V3X_DEFINITION
#define __V3X_DEFINITION

/*
     Definitions (version and fixed mul or floating point)
 */

#define V3XVERSION     0x500

#include "v3xtypes.h"
#include "v3xmaths.h"
#define V3X_MAXTMU 2

enum {
      V3XOPTION_WARNING = 0x1, // Enable warning level
      V3XOPTION_DEBUG = 0x2, // Enable debug output
      V3XOPTION_AMBIANT = 0x4, // Enable ambiant override
      V3XOPTION_COLLISION = 0x8, // Enable collision detection
      V3XOPTION_ADDMESH = 0x10,
      V3XOPTION_DUPPOLY = 0x20,
      V3XOPTION_COLLISIONDRAW = 0x40,
      V3XOPTION_97  = 0x80,
      V3XOPTION_PUSHSCENE = 0x100,
      V3XOPTION_USESAMELUT = 0x200,
      V3XOPTION_TRUECOLOR = 0x400,
      V3XOPTION_TRACER = 0x800,
      V3XOPTION_PORTALCULL = 0x1000,
	  V3XOPTION_RAYTRACE  = 0x2000

};

enum {
      V3XWARN_NOENOUGHSurfaces = 0x2,
	  V3XWARN_MISSINGTEXTURES = 0x4
};

enum {
      V3XMATRIX_Euler = 0x0, // Matrix mode with euler angles
      V3XMATRIX_Vector = 0x1, // Matrix mode with non-normalize vector direction
      V3XMATRIX_Custom = 0x2, // Custom matrix mode
      V3XMATRIX_Euler2 = 0x3, // Matrix mode with euler angles (Lightwave)
      V3XMATRIX_Vector2 = 0x4, // Matrix mode with normalized vector
      V3XMATRIX_Quaternion = 0x5  // Quaternion mode
};

enum {
      V3XLIGHTTYPE_SPOT = 0x1, // Spot light
      V3XLIGHTTYPE_OMNI = 0x2, // Omni light
      V3XLIGHTTYPE_OMNI_Z = 0x3, // Z light
      V3XLIGHTTYPE_DIRECTIONAL = 0x4, // Directional light
      V3XLIGHTTYPE_CUSTOM = 0x10 // Custom light
};
enum {
      V3XLIGHTCAPS_LENZFLARE = 0x1, // Lenz flare (material is valid)
      V3XLIGHTCAPS_NEGATIVE = 0x2, // Negative light (substract)
      V3XLIGHTCAPS_DYNAMIC = 0x4, // Dynamic calculation
      V3XLIGHTCAPS_RANGE  = 0x8, // Has a range
      V3XLIGHTCAPS_FOGGY  = 0x10, // Create a fog if camera in the range
      V3XLIGHTCAPS_INFINITE = 0x20, // Infinite lite (no z-fogging)
      V3XLIGHTCAPS_REALLF = 0x40, // Realistic lenz flare
      V3XLIGHTCAPS_FIXEDSIZE = 0x80 // Realistic lenz flare
};
enum {
      V3XENVMAPTYPE_NONE  = 0x0, // Disable environment mapping
      V3XENVMAPTYPE_LIGHT = 0x1, // Environmnent mapping with light influence
      V3XENVMAPTYPE_CAMERA = 0x2, // Jugi' model environmnnnt mapping
      V3XENVMAPTYPE_PLANE = 0x4, // Environment mapping with coordinate mapping
      V3XENVMAPTYPE_DOUBLE = 0x8   // Double
};
enum {
      V3XBLENDMODE_NONE = 0x0, // no alphablending
      V3XBLENDMODE_SUB  = 0x1, // substractive alpha
      V3XBLENDMODE_ALPHA = 0x2, // alpha
      V3XBLENDMODE_ADD  = 0x3, // additive
      V3XBLENDMODE_STIPPLE = 0x4   // Stippled
};

enum {
      V3XMTXBLEND_ADD = 0x1, // additive
      V3XMTXBLEND_LIGHTMAP = 0x2  // light
};

enum {
      V3XFILTER_DEFAULT = 0x0, // Use default filtering mode
      V3XFILTER_NONE = 0x1  // Disable filtering
};

/*
   Polygon definitions (32 bytes size)
   Floating point and Fixed point format.
*/

struct _v3xmaterial;

typedef struct _v3xpoly_disk{
	union
	{
		uint32_t Mat; // Material structures
		int32_t matIndex; // Material index in the mesh
	};
	uint32_t faceTab; // index array to mesh vertex
	uint32_t dispTab; // projected points (on screen)
	uint32_t uvTab; // texture coordinates
	V3XSCALAR distance; // distance (for Z Sort)
	union
	{
		uint32_t shade;
		uint32_t rgb; // light intensity
	};

	uint32_t ZTab; // Homogenous coordinates (perspective)
	uint8_t numEdges; // Number of edges 1..255
	uint8_t flags;
	uint8_t visible; // reserved
	uint8_t NeedToClip; // reserved
} V3XPOLYDISK;

typedef struct _v3xpoly{
	union
	{
		struct _v3xmaterial	*Mat; // Material structures
		int32_t matIndex; // Material index in the mesh
	};
	uint32_t *faceTab; // index array to mesh vertex
	V3XPTS *dispTab; // projected points (on screen)
	V3XUV **uvTab; // texture coordinates
	V3XSCALAR distance; // distance (for Z Sort)
	union
	{
		V3XSCALAR *shade;
		rgb32_t *rgb; // light intensity
	};

	V3XWPTS *ZTab; // Homogenous coordinates (perspective)
	uint8_t numEdges; // Number of edges 1..255
	uint8_t flags;
	uint8_t visible; // reserved
	uint8_t NeedToClip; // reserved
} V3XPOLY;

static inline void PolyDiskToMem (const V3XPOLYDISK *d, V3XPOLY *m)
{
	m->matIndex = d->matIndex;
	m->distance = d->distance;
	m->numEdges = d->numEdges;
	m->flags = d->flags;
	m->visible = d->visible;
	m->NeedToClip = d->NeedToClip;
}

typedef struct _v3xpoly_l{
    union
	{
         struct _v3xmaterial        *Mat;            // Material structures
         int        matIndex;       // Material index in the mesh
    };
    uint32_t       *faceTab;
    V3XlPTS			*dispTab;
    V3XlUV			**uvTab;
    V3XSCALAR		distance;
    union {
        int32_t		*shade;
        rgb32_t		*rgb;
		void		*lightmap;
    };
    V3XWPTS			*ZTab;
    uint8_t        numEdges;
    uint8_t		flags;
    uint8_t        visible;
    uint8_t        NeedToClip;
}V3XPOLY_L;

/*
     Mesh Material definitions (128 bytes size)
*/
typedef struct _v3x_materialProperties{
	uint32_t TwoSide       : 1;  // material is 2 sided
	uint32_t Opacity       : 1;  // primary texture has a color-key
	uint32_t Perspective   : 1;  // primary texture is perspective corrected
	uint32_t Filtering     : 1;  // filtering method

	uint32_t Texturized    : 2;  // texturing mode
	uint32_t Transparency  : 2;  // blending mode
	uint32_t Shade         : 2;  // shading mode
	uint32_t Sprite        : 2;  // is a 2D sprte

	uint32_t Environment   : 4;  // texture environement model

	uint32_t Dynamic       : 1;  // texture can be modified on the fly
	uint32_t Uncompressed  : 1;  // is a non-compressed animation file
	uint32_t AlphaLight    : 1;  // alpha component = light component
	uint32_t Transparency2 : 3;  // multitexturing blending mode OR advanced blending
	uint32_t MultiPassBlend: 2;  // Blending method for multipass

	uint32_t Custom        : 1;  // reserved
	uint32_t RGB_Modulate  : 1;
	uint32_t AlphaComponent: 1;
	uint32_t HiColorTex    : 1;

	uint32_t LightMap	   : 1;
	uint32_t filler        : 3;  // reserved
} V3XMATERIALPROPERTIES;

struct _fli_struct;

typedef struct _v3xmaterial_disk
{
	union {
		uint32_t info;
		uint32_t lod;
	};

	uint8_t shift_size; // texture scale factor (2^shift_size)
	uint8_t alpha; // alpha 0..255
	uint8_t shiness; // shiness strength 0..255
	uint8_t strength; // strength 0..255

	char mat_name[16]; // material name
	char ref_name[16]; // reflection mapping filename
	char tex_name[16]; // texture mapping filename
	GXSPRITEDISK texture[2]; // GXSPRITE structure of the texture map
	uint32_t reserved[2];
	uint32_t fli;
	uint32_t render_clip; // internal

	union {
		uint32_t render_near; // internal
		uint32_t info_near;
		uint32_t lod_near;
	};

	union {
		uint32_t render_far; // internal
		uint32_t info_far;
		uint32_t lod_far;
	};

	rgb24_t ambient, diffuse, specular; // Color informations
	uint8_t  Properties; // Properties
	uint8_t RenderID; // Render Identifier
	uint8_t Render; // reserved
	uint8_t scale; // Sprite factor 0..255
	uint8_t RenderID_near;
	uint8_t filler[2];
} V3XMATERIALDISK;

typedef struct _v3xmaterial
{
	union {
		V3XMATERIALPROPERTIES info;
		uint32_t lod;
	};

	uint8_t shift_size; // texture scale factor (2^shift_size)
	uint8_t alpha; // alpha 0..255
	uint8_t shiness; // shiness strength 0..255
	uint8_t strength; // strength 0..255

	char mat_name[16]; // material name
	char ref_name[16]; // reflection mapping filename
	char tex_name[16]; // texture mapping filename
	GXSPRITE texture[2]; // GXSPRITE structure of the texture map
	void *reserved[2];
	struct _fli_struct *fli;
	void (* CALLING_C render_clip)(V3XPOLY *fce); // internal

	union {
		void (* CALLING_C render_near)(V3XPOLY *fce); // internal
		V3XMATERIALPROPERTIES info_near;
		uint32_t lod_near;
	};

	union {
		void (* CALLING_C render_far )(V3XPOLY *fce); // internal
		V3XMATERIALPROPERTIES info_far;
		uint32_t lod_far;
	};

	rgb24_t ambient, diffuse, specular; // Color informations
	uint8_t Properties; // Properties
	uint8_t RenderID; // Render Identifier
	uint8_t Render; // reserved
	uint8_t scale; // Sprite factor 0..255
	uint8_t RenderID_near;
	uint8_t filler[2];
} V3XMATERIAL;

static inline void MaterialDiskToMem(const V3XMATERIALDISK *d, V3XMATERIAL *m)
{
	m->lod = d->lod;
	m->shift_size = d->shift_size;
	m->alpha = d->alpha;
	m->shiness = d->shiness;
	m->strength = d->strength;
	memcpy(m->mat_name, d->mat_name, sizeof(m->mat_name));
	memcpy(m->ref_name, d->ref_name, sizeof(m->ref_name));
	memcpy(m->tex_name, d->tex_name, sizeof(m->tex_name));
	SpriteDiskToMem(&d->texture[0], &m->texture[0]);
	SpriteDiskToMem(&d->texture[1], &m->texture[1]);
	m->lod_near = d->lod_near;
	m->lod_far = d->lod_far;
	m->ambient = d->ambient;
	m->diffuse = d->diffuse;
	m->specular = d->specular;
	m->Properties = d->Properties;
	m->RenderID = d->RenderID;
	m->Render = d->Render;
	m->scale = d->scale;
	m->RenderID_near = d->RenderID_near;
	m->filler[0] = d->filler[0];
	m->filler[1] = d->filler[1];
}

/*
    Matrix Mesh informations
*/

typedef struct _v3x_key_euler{
    V3XVECTOR    angle;   // 3 axis angle (0..4096)
    V3XVECTOR    pos;     // position
}V3XKEYEULER;

typedef struct _v3x_key_postarget{
    V3XVECTOR    target;  // target
    V3XVECTOR    pos;     // position
    V3XSCALAR     roll;    // rol (0..4096)
}V3XKEYCAMERA;

typedef struct _v3x_key_vector{
    V3XVECTOR    vect;
    V3XVECTOR    pos;
    V3XSCALAR    roll;
}V3XKEYVECTOR;

typedef struct _v3x_key_quaternion{
    V3XSCALAR     x, y, z;
    V3XVECTOR     pos;
    V3XSCALAR     w;
}V3XKEYQUAT;

typedef union _v3x_key{
    V3XKEYQUAT      qinfo;
    V3XKEYEULER     info;
    V3XKEYCAMERA    vinfo;
    V3XKEYVECTOR    dinfo;
}V3XKEY;

typedef struct _v3x_keyEX{
    V3XKEY          k;
    V3XVECTOR       scale;
    short           frame;
    uint16_t          flags;
}V3XKEYEX;

/*
      Mesh structures
*/

enum {
    V3XMESH_FULLUPDATE = 0x1, // Compute every faces
    V3XMESH_HASDYNLIGHT = 0x2, // Require light computation
    V3XMESH_HASDYNUV = 0x4, // Require UV computation
    V3XMESH_NOZSHADING = 0x8, // no Z shading
    V3XMESH_MINZSORT = 0x10, // Min Z sort
    V3XMESH_MAXZSORT = 0x20, // Max Z sort
    V3XMESH_PRESHADED = 0x40, // Preshade : do not compute lighting
    V3XMESH_HASUV = 0x80, // Have UV mesh
    V3XMESH_HASSHADE = 0x100, // Shaded Mesh
    V3XMESH_HASNORMAL = 0x200, // Have normal edge mesh
    V3XMESH_EXCPTDYNLIGHT = 0x400, // do not shaded by dynamic light
    V3XMESH_HASSHADETABLE = 0x800, // Contains pre-shading value
    V3XMESH_NOOPTIMIZE = 0x1000,
    V3XMESH_HASMIRRORMATERIAL = 0x2000, // Is a 'mirror' object.
    V3XMESH_NOZSORT			 = 0x4000,
	V3XMESH_LOWDETAIL		 = 0x8000,
	V3XMESH_LODNEVER		 = 0x10000,
	V3XMESH_FLATSHADE		 = 0x20000

};

typedef struct _v3x_mesh_disk{
	uint32_t vertex; // Vertex array
	uint32_t face; // Face array
	uint32_t uv; // UV coordinates array (may be null even if mesh is mapped)
	uint32_t normal; // Edges normal array (may be null if mesh is not lightened)
	uint32_t normal_face; // Face normal array
	uint32_t material; // Material mesh array

	V3XMATRIX matrix; // Matrix Transformation
	V3XKEY Tk; // Keyframe informations
	uint32_t flags; // State flags
	uint16_t numVerts; // Number of vertex (0..65535)
	uint16_t numMaterial; // Number of materials (0..65535)

	uint16_t numFaces; // Number of faces (0..65535)
	uint16_t selfIllumine; // Self illumination value (0..255)

	V3XSCALAR scale; // Uniform scaling
	union
	{
		uint32_t shade;
		uint32_t rgb;
	};
	V3XSCALAR radius;
	uint32_t pad;
} V3XMESHDISK;

typedef struct _v3x_mesh{
	V3XVECTOR *vertex; // Vertex array
	V3XPOLY *face; // Face array
	V3XUV *uv; // UV coordinates array (may be null even if mesh is mapped)
	V3XVECTOR *normal; // Edges normal array (may be null if mesh is not lightened)
	V3XVECTOR *normal_face; // Face normal array
	V3XMATERIAL *material; // Material mesh array

	V3XMATRIX matrix; // Matrix Transformation
	V3XKEY Tk; // Keyframe informations
	uint32_t flags; // State flags
	uint16_t numVerts; // Number of vertex (0..65535)
	uint16_t numMaterial; // Number of materials (0..65535)

	uint16_t numFaces; // Number of faces (0..65535)
	uint16_t selfIllumine; // Self illumination value (0..255)

	V3XSCALAR scale;// Uniform scaling
	union
	{
		V3XSCALAR *shade;
		rgb32_t *rgb;
	};
	V3XSCALAR radius;
	uint32_t pad;

} V3XMESH;

static inline void MeshDiskToMem(const V3XMESHDISK *d, V3XMESH *m)
{
	uintptr_t x;
	x = d->vertex;
	m->vertex = (V3XVECTOR*)x;
	x = d->face;
	m->face = (V3XPOLY*)x;
	x = d->uv;
	m->uv = (V3XUV*)x;
	x = d->normal;
	m->normal = (V3XVECTOR*)x;
	x = d->normal_face;
	m->normal_face = (V3XVECTOR*)x;
	x = d->material;
	m->material = (V3XMATERIAL*)x;
	m->matrix = d->matrix;
	m->Tk = d->Tk;
	m->flags = d->flags;
	m->numVerts = d->numVerts;
	m->numMaterial = d->numMaterial;
	m->numFaces = d->numFaces;
	m->selfIllumine = d->selfIllumine;
	m->scale = d->scale;
	x = d->shade;
	m->shade = (V3XSCALAR*)x;
	m->radius = d->radius;
	m->pad = d->pad;
}

/*
      Light Structure
*/

struct _v3xsprite;

typedef struct _v3x_light_disk
{
	V3XVECTOR		pos;		// Light position
	V3XVECTOR		target;		// Target position
	V3XVECTOR		vector;		// Internal (vector light/target)
	V3XVECTOR		normal;		// Internal
	V3XSCALAR		intensity;	// Light multiplier (255. : normal)
	V3XSCALAR		diminish;	// Z shading diminish factor
	V3XSCALAR		range;		// Light range
	V3XSCALAR		falloff;	// Spot falloff size
	rgb32_t			color;		// RGBA color
	union {
		uint32_t material;
		uint32_t flare;
	};
	V3XSCALAR			flaresize;
	rgb32_t				specular;
	uint32_t			reserved[2];

	V3XKEY			Tk;

	uint8_t			type;
    uint8_t			flags;
    uint8_t			alpha;
    uint8_t			status;
    uint32_t		Timer, TimeOn, TimeOff;
    uint32_t		pad2[3];
}V3XLIGHTDISK;

typedef struct _v3x_light
{
    V3XVECTOR		pos;            // Light position
    V3XVECTOR		target;         // Target position
    V3XVECTOR		vector;         // Internal (vector light/target)
    V3XVECTOR		normal;         // Internal
    V3XSCALAR		intensity;      // Light multiplier (255. : normal)
    V3XSCALAR		diminish;       // Z shading diminish factor
    V3XSCALAR		range;          // Light range
    V3XSCALAR		falloff;       // Spot falloff size
    rgb32_t			color;          // RGBA color
    union {
		 V3XMATERIAL		*	material;       // Material
		struct _v3xsprite	*	flare;
    };
    V3XSCALAR			flaresize;
	rgb32_t				specular;
	void			*	reserved[2];

    V3XKEY		Tk;

    uint8_t		type;           // type
    uint8_t		flags;          // flags
    uint8_t			alpha;
    uint8_t		status;
    uint32_t			Timer, TimeOn, TimeOff;
    uint32_t	pad2[3];
}V3XLIGHT;

static inline void LightDiskToMem(const V3XLIGHTDISK *d, V3XLIGHT *m)
{
	uintptr_t x;
	m->pos = d->pos;
	m->target = d->target;
	m->vector = d->vector;
	m->normal = d->normal;
	m->intensity = d->intensity;
	m->diminish = d->diminish;
	m->range = d->range;
	m->falloff = d->falloff;
	m->color = d->color;
	x = d->material;
	m->material = (V3XMATERIAL*)x;
	m->flaresize = d->flaresize;
	m->specular = d->specular;
	m->Tk = d->Tk;
	m->type = d->type;
	m->flags = d->flags;
	m->alpha = d->alpha;
	m->status = d->status;
	m->Timer = d->Timer;
	m->TimeOn = d->TimeOn;
	m->TimeOff = d->TimeOff;
}

/*
     Camera Structures
*/
typedef struct _v3x_camera{
    uint32_t         flags;
    uint32_t         matrix_Method;
    V3XVECTOR     transposed;      // Internal
    V3XSCALAR      focal;           // focal value
    V3XMATRIX     M;               // Matrix
    V3XKEY        Tk;              // Keyframe informations 22 q
    uint32_t         pad[7];
}V3XCAMERA;

/*
      Light Sourcer Structures
*/

typedef struct _v3x_light_mgr{
    uint32_t        flags;           // flags
    void        *tables;          // mixed Tables pointer
    V3XLIGHT    *light;           // array of light
    V3XVECTOR    HVector;         // Vector H (mid between camera and light)
    V3XSCALAR     fogDistance;     // fog range
    rgb32_t    ambiant;         // ambiant color
    rgb32_t    ambiantMaterial; // ambiant material override
    uint8_t        fogFactor;       // fog factor (should obsolete)
    uint8_t        numColor;        // numbers of colors
    uint8_t        numSource;       // numbers of light sources
    uint8_t        pad;
}V3XLIGHTS;

/*
      Computation buffer structures
*/

typedef struct _v3x_line_prim_buffer{

   V3XVECTOR *lineBuffer;
   union {
     rgb32_t *lineColor;
     unsigned  *lineValue;
   };
   unsigned   nbLines;
   unsigned   maxLines;
}V3XLINEBUFFER;


typedef struct _v3x_buffer{
    V3XPOLY    **RenderedFaces;   // array of polygons to draw
    V3XPOLY     *ClippedFaces;    // array of clipped polygons
    V3XVECTOR   *rot_vertex;      // temporary array of rotated vertex of a mesh
    V3XVECTOR   *prj_vertex;      // temporary array of projected vertex of a mesh
    V3XUV       *uv_vertex;       // temporary array of UV coordinates

    union {
        V3XSCALAR    *shade;       // temporay array of lighting value
        rgb32_t    *rgb;
    };

    uint8_t       *flag;            // temporay array of flag
    uint8_t      **OVI;             // array of OVI to treat
    V3XMATERIAL *Mat;             // temporay array of material

    unsigned     MaxFaces;        // number of displayed faces
    unsigned     MaxFacesDisplay; // number of displayable face
    unsigned     MaxClipped;      // number of clipped faces
    unsigned     MaxClippedFaces; // number of clippable faces
    unsigned     MaxMat;          // number of material dynamically allocated
    unsigned     MaxTmpMaterials;     // number of dynamicallable material
    unsigned     MaxLight;        // number of available light
    unsigned     MaxObj;          // number of object
    unsigned     MaxSceneNodes;
    unsigned     MaxPointsPerMesh;// number of vertex per objects
    unsigned     MaxEdges;        // number of edges per faces
    unsigned     ObjFailed;       // number of object which weren't processed

}V3XBUFFER;

/*
     V3X Clipping Configuration, override
*/

typedef struct _v3x_clipper{

    V3XSCALAR    Far;                // Camera far clipping
    V3XSCALAR    Near;               // Camera near clipping
    V3XSCALAR    Correction;         // Switch correction to linear threshold
    uint32_t       flags;              // flags
    V3XVECTOR   normalLeft, normalRight, normalTop, normalBottom;                              // Normals for 2D culling

}V3XCLIPPER;

/*
     V3X Configuration structures
*/

typedef struct _v3x_sceneSetup {
    uint32_t       version;                // version
    uint32_t       warnings;
    uint32_t       flags;                  // flags
    char        CommonPath[256];
    unsigned    MaxStartObjet;          // object of objects by default in a scene
    unsigned    MaxExtentableObjet;     // object maximum per scene
    void      (*add_poly)(void);        // callback before object processing
	void      (*add_lights)(void);      // callback after processing lights
    void      (*pre_render)(void);      // callback before polygons processing
    void      (*post_render)(void);     // callback after polygons processing
    void      (*custom_ovi)(void *ovi); // callback when a OVI is processed
}V3XSCENESETUP;

/*
      Render Viewport definition
*/
typedef struct _v3x_viewport{
    V3XVECTOR2     center;      // Projection coordinates on screen
    V3XSCALAR      Ratio;       // display ratio
    V3XSCALAR      Focal;       // focal length
    union {
		uint32_t         backgroundColor;
		rgb32_t	  _bckgroundColor;
	};
	union {
		uint32_t         fogColor;
		rgb32_t	  _fogColor;
	};
    V3XSCALAR      zclearValue;
    V3XSCALAR      minVisibleRadius;
	V3XSCALAR      minTextureVisibleRadius;
    uint8_t         zDepth;
    uint8_t	  pad[3];
}V3XVIEWPORT;

/*
      V3X Performance stats
*/

typedef struct _v3x_profile{
    uint32_t        pps, fps, poly;
    uint32_t        time, frame;
}V3XPROFILER;


/*
      V3X Plug-in
*/


typedef struct _v3x_dynamics{
    V3XVECTOR   gravity;
    V3XSCALAR    unity, SoundSpeed, AirViscosity;
}V3XDYNAMICRULES;

typedef struct _v3x_plugin{
    void (*call)(const void *);
}V3XPlugItem;

typedef struct _v3x_plugins{
    unsigned maxPlug;
    V3XPlugItem plug[8];
}V3XPLUGOVI;

typedef struct _v3x_cache_item{
    uint16_t            flags;
    uint16_t            type;
    void             *data;        // pointer data
    char              filename[56];    // pointer to a filename identifier
}V3XRESOURCE_ITEM;

typedef struct _v3x_tx_cache_item {
    int               numItems;    // number of item
    V3XRESOURCE_ITEM   *item;        // item list
}V3XRESOURCE;

typedef struct _v3x_time {
	unsigned		ms;
}V3XTIMER;



// Texture primitives group. (non valid with HW drivers)
typedef struct {
    unsigned caps;
    // non texture primitives
     void (* CALLING_C constc)(V3XPOLY *fce);          // Constant color (diffuse)
     void (* CALLING_C flat)(V3XPOLY *fce);            // Flat
     void (* CALLING_C gouraud)(V3XPOLY *fce);         // Gouraud
     void (* CALLING_C gouraud_trsp)(V3XPOLY *fce);    // Gouraud transparency
     void (* CALLING_C const_trsp)(V3XPOLY *fce);      // Constant transparency

}V3X_GXNonTexPrimitives;


typedef struct {
    unsigned caps;
    // non-shaded texture primitives
    void (* CALLING_C tex)(V3XPOLY *fce);              // texture mapping
    void (* CALLING_C tex_rough)(V3XPOLY *fce);        // texture mapping (rough)
    void (* CALLING_C tex_2pass)(V3XPOLY *fce);        // multitexture mapping
    void (* CALLING_C tex_opacity)(V3XPOLY *fce);      // texture mapping with opacity
    void (* CALLING_C tex_trsp)(V3XPOLY *fce);         // texture mapping with transparency
    void (* CALLING_C tex_trspAdd)(V3XPOLY *fce);         // texture mapping with transparency

    // texture + shaded primtives
    void (* CALLING_C flat_tex)(V3XPOLY *fce);         // texture with flat shading.
    void (* CALLING_C gouraud_tex)(V3XPOLY *fce);      // gouraud textured
    void (* CALLING_C flat_opacity_tex)(V3XPOLY *fce); // flat opacity

}V3X_GXTexPrimitives;

// Software rasterizer primitives (non valid with HW drivers).
typedef struct {
     V3X_GXNonTexPrimitives
		    *std;
     V3X_GXTexPrimitives
		    *Linear256x256x8b,   //  256x256x8bit maps
		    *Linear128x128x8b,   //  128x128x8bit maps
		    *Corrected256x256x8b, //  256x256x8bit maps
		    *Corrected128x128x8b, //  128x128x8bit maps
		    *Linear256x256xbpp,  //  256x256 with current color depth.
		    *Corrected256x256xbpp;  //  256x256 with current color depth.
}V3X_GXRenderClass;

// Software renderer class


// Clut layer system

typedef struct _v3x_layer_ClutItem{
    union  {
          uint8_t     **table;
          rgb24_t  *lut;
    };
    char         filename[12];   // name without extensions .act for table, .png for table
}V3XLAYER_CLITEM; //SizeOf: 16b

typedef struct _v3x_layer_Clut{
    uint32_t              flags;
    V3XLAYER_CLITEM    palette;  // clut for 8bit palette
    V3XLAYER_CLITEM    gouraud;  // clut for gouraud (8bit mode)
    V3XLAYER_CLITEM    phong;    // clut for phong (8bit mode)

    V3XLAYER_CLITEM    blur;     // clut for motion blur, light blending (8bit mode)
    V3XLAYER_CLITEM    alpha50;  // clut for transparency 50% (8bit mode)
    V3XLAYER_CLITEM    additive; // clut for additive (8bit mode)
    V3XLAYER_CLITEM    bump;     // clut for bump (8bit mode)
    uint32_t              shift;

}V3XLAYER_CLUT;    //120b

typedef struct {
     V3X_GXRenderClass   *primitive;
     void        V3XAPI  (*Render)(void);
     void        V3XAPI *(*TextureDownload)(const GXSPRITE *src, const rgb24_t *colorTable, int bpp, unsigned options);
     void        V3XAPI  (*TextureFree)(void *handle);
     int         V3XAPI  (*TextureModify)(GXSPRITE *ptr, uint8_t *newBuffer, const rgb24_t *colorTable);
     int         V3XAPI  (*Setup)(void);
     void        V3XAPI  (*Shutdown)(void);
     unsigned    V3XAPI  (*SetState)(unsigned command, uint32_t value);
     unsigned    V3XAPI  (*ZbufferClear)(rgb24_t *color, V3XSCALAR z, void *bitmap);
     void        V3XAPI  (*RenderPoly)(V3XPOLY **fce, int count);
     void        V3XAPI  (*BeginList)(void);
     void        V3XAPI  (*EndList)(void);
     void        V3XAPI  (*DrawPrimitives)(V3XVECTOR *vertexes, uint16_t *indexTab, unsigned NumIndexes, unsigned NumVertexes, int option, rgb32_t *color);
     char            s_DrvName[128];
     unsigned        version;
     unsigned        Capabilities;
	 unsigned		 State;
     uint8_t        reduce;
	 uint8_t		 texMaxSize;
     uint8_t        filler[2];
}V3X_GXSystem;

/*
      V3X'98 API Class definition
*/

struct V3XSYSTEM {
    V3XCAMERA       Camera;
    V3XBUFFER       Buffer;
    V3XLINEBUFFER   Ln;
    V3XRESOURCE      Cache;
    V3XLIGHTS       Light;
    V3XVIEWPORT     ViewPort;
    V3XSCENESETUP   Setup;
    V3XCLIPPER      Clip;
    V3XDYNAMICRULES Dynamic;
    V3XPLUGOVI      Plugin;
	V3XTIMER		Time;
	V3X_GXSystem  *	Client;

};

__extern_c
_RLXEXPORTDATA    extern    struct V3XSYSTEM     V3X;
__end_extern_c

#endif

