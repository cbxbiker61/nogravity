
#include <stdio.h>
#include <stdlib.h>
#include "_rlx32.h"
#include "v3xdefs.h"
#include "v3xrend.h"
#include "v3xref.h"
#include "gx_struc.h"
#include "gx_rgb.h"
#include "fixops.h"

typedef struct {
    int32_t S, PS, CRT, E, PE;
}PasTriangle;

typedef struct {
    float S, PS, CRT, E, PE;
}PasTriangle2;

extern u_int32_t g_DiffuseTable[64];
extern u_int32_t g_MixTable[64][256];

#define GET_DIFFUSE(It) ((SHRD(It))&63)
#define GET_CONSTANT() ((*((int32_t*)fce->shade)+0) &63)

#define XCHG(a, b, typ) {typ c=a;a=b;b=c;}
#define DEFSIZE (1<<DEFSHL)
#define MAPMASK ((1L<<(MAPWIDTH+MAPWIDTH))-1L)
#define MAPADR(u, v) (((u>>16)+((v>>16)<<MAPWIDTH))&MAPMASK)

#define TRI_VAR  \
PasTriangle Xg;  \
int32_t n0=0, n1=1, n2=2, somCount=1, k, y0, yF, superDY, Pper; \
int32_t DY, y, x0; \
int32_t XL, swappy; \
V3XlPTS *pt = (V3XlPTS*)fce->dispTab; \
u_int8_t *ptr, *off_ptr;

/**/
#define VINV(x) VDIV(65535, x)

#define MASTER_1  \
MapAgain:  \
k = 1;  \
while (k != 0) \
{  \
if (pt[n1].y < pt[n0].y) XCHG (n0, n1, int32_t) \
else \
if (pt[n2].y < pt[n1].y) XCHG (n1, n2, int32_t ) \
else k = 0; \
}  \
y0 = pt[n0].y;  \
yF = pt[n2].y;  \
superDY = yF - y0; \
if (!superDY) goto NextOne; \
else  \
{  \
int32_t ax, bx; \
XL = pt[n1].y - pt[n0].y;  \
Pper = VDIV(XL, superDY); \
bx = VDIV(pt[n2].x - pt[n0].x, superDY); \
ax = SHLD(pt[n0].x); \
Xg.CRT = SHL16(pt[n1].x - (pt[n0].x + VMUL(bx, XL))); \
if (!Xg.CRT) goto NextOne; \
swappy = (Xg.CRT<0); \
if (swappy) \
{ \
Xg.E = ax;  \
Xg.PE = bx; \
} \
else \
{ \
Xg.S = ax; \
Xg.PS = bx; \
} \
}

/**/


/**/
#define MASTER_L  \
while (y0 < yF) \
{ \
int32_t ax, bx; \
DY = pt[n1].y - pt[n0].y; \
if (!DY) goto MapNextTriangle; \
ax = SHLD(pt[n0].x); \
bx = VDIV(pt[n1].x - pt[n0].x, DY) ; \
if (swappy) \
{ \
Xg.S = ax; Xg.PS = bx;  \

#define MASTER_2  \
} \
else \
{ \
Xg.E = ax; Xg.PE = bx; \
} \
off_ptr = g_pRLX->pGX->View.lpBackBuffer + y0*g_pRLX->pGX->View.lPitch; \
for (y=DY;y!=0;y--) \
{ \
x0 = SHRD(Xg.S); \
XL = SHRD(Xg.E)-x0; \

#define INNER_LOOP  \
if (XL)  \
{ \
int32_t x = XL; \
ptr = off_ptr + (x0<<DEFSHL); \
for (;x!=0;ptr+=DEFSIZE, x--)
/**/
#define FINAL  \
Xg.S += Xg.PS; \
Xg.E += Xg.PE; \
off_ptr+= g_pRLX->pGX->View.lPitch; \
} \
MapNextTriangle:  \
n0 = n1;  \
n1 = n2;  \
y0 += DY;  \
} \
NextOne: \
if (somCount+2<fce->numEdges)  \
{ \
somCount++; n0 = 0; n1 = somCount; n2 = somCount+1; goto MapAgain; \
} \


#define TSWAP_IT \
	Ig.S = SHLD(shade[n0]); \
	Ig.PS = VDIV(shade[n1] - shade[n0], DY); \


#define TSWAP_Z { \
	float iz=1.f/(float)DY; \
	Ugz.S = ZTab[n0].uow; \
	Ugz.PS = (ZTab[n1].uow - ZTab[n0].uow)*iz; \
	Vgz.S = ZTab[n0].vow; \
	Vgz.PS = (ZTab[n1].vow - ZTab[n0].vow)*iz; \
	Wgz.S = ZTab[n0].oow; \
	Wgz.PS = (ZTab[n1].oow - ZTab[n0].oow)*iz; \
}
