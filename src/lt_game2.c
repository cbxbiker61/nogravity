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
#define NG_EXPORT extern

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "_rlx32.h"
#include "_stub.h"
#include "_rlx.h"
#include "systools.h"
#include "sysresmx.h"
#include "systime.h"
#include "gx_struc.h"
#include "gx_csp.h"
#include "gx_flc.h"
#include "v3xdefs.h"
#include "v3xtrig.h"
#include "v3x_1.h"
#include "v3x_2.h"
#include "v3xcoll.h"
#include "v3xrend.h"
#include "v3xmaps.h"
#include "v3xscene.h"

#include "iss_defs.h"

//
#include "gui_os.h"
#include "gx_rgb.h"

#include "lt_struc.h"
#include "lt_data.h"
#include "lt_func.h"

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int NG_ColorToNAV(int x)
*
* DESCRIPTION :
*
*/
int NG_ColorToNAV(int x)
{
    int nv=0;
    if (x>=4)
    {
        nv=(x-4)>>2;
        if (nv>7) nv=7;
    }
    return nv;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int NG_MomentumValue(int x, int mx)
*
* DESCRIPTION :
*
*/
int NG_MomentumValue(int x, int mx)
{
    int p = (g_cTimer.iCounter>>15)<<g_pCurrentGame->ship;
    if (x>0)
    {
        x-=p;
        if (x>mx) x = mx;
        if (x<0)  x = 0;
    }
    else
    if (x<0)
    {
        x+=p;
        if (x<-mx) x =-mx;
        if (x>0) x=0;
    }
    return x;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void CALLING_C NG_DrawCircle(V3XVECTOR2 *c, int32_t r, int32_t cx, int cote)
*
* DESCRIPTION :
*
*/
void CALLING_C NG_DrawCircle(V3XVECTOR2 *c, int32_t r, int32_t cx, int cote)
{
    int i, p=4096/cote;
    V3XSCALAR r0=(V3XSCALAR)((r*GX.View.lRatio)>>4), r1=(V3XSCALAR)r;
    V3XVECTOR2 a, b;
    for (i=0;i<=4096;i+=p)
    {
        a.x = c->x + MULF32(sin16(i), r0);
        a.y = c->y + MULF32(cos16(i), r1);
        if (i>1)
        {
            GX_ClippedLine(&a, &b, cx);
        }
        b=a;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_AIRandomAim(V3XVECTOR *a, V3XVECTOR *cent, int32_t r)
*
* DESCRIPTION :
*
*/
void NG_AIRandomAim(V3XVECTOR *a, V3XVECTOR *cent, V3XSCALAR r)
{
    V3XVector_PolarSet((a), (cent), ((V3XSCALAR)r), sysRand(4096), sysRand(4096));
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_AIRandomAimEx(V3XVECTOR *a, V3XVECTOR *cent, int32_t r, int32_t al, int32_t bl)
*
* DESCRIPTION :
*
*/
void NG_AIRandomAimEx(V3XVECTOR *a, V3XVECTOR *cent, V3XSCALAR r, int32_t al, int32_t bl)
{
    V3XVector_PolarSet((a), (cent), ((V3XSCALAR)r), al, bl);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  #define COMPUTE_FOEDIST()\
*
* DESCRIPTION :
*
*/
#define COMPUTE_FOEDIST()\
if ((!(g_SGGame.Count&REFRESH_FNF))||(!data->OVItarget))\
{\
    data->OVItarget = NG_AILocateNearestEnemy(&OVI->mesh->matrix.v.Pos, &di2, &k2, camp);\
}\
else\
{\
    if (data->OVItarget)\
    {\
        V3XVector_Dif(&di2, &data->OVItarget->mesh->matrix.v.Pos, &OVI->mesh->matrix.v.Pos);\
        k2 = V3XVector_Normalize(&di2, &di2);\
    }\
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI *NG_AILocateNearestTarget(V3XVECTOR *pos, V3XVECTOR *dist, int32_t *di)
*
* DESCRIPTION :
*
*/
V3XOVI *NG_AILocateNearestTarget(V3XVECTOR *pos, V3XVECTOR *dist, V3XSCALAR *di)
{
    V3XSCALAR maxy=CST_MAX, dii;
    int32_t i;
    V3XOVI *OVI, *Gs=NULL;
    SGScript *Sif;
    V3XVECTOR *d;
    for (i=g_SGGame.Scene->numOVI, OVI=g_SGGame.Scene->OVI;i!=0;OVI++, i--)
    {
        if ((OVI->data)&&(!(OVI->state&V3XSTATE_HIDDEN)))
        {
            Sif = (SGScript*)OVI->data;
            if (Sif->ColorRadar)
            {
                d = &OVI->mesh->matrix.v.Pos;
                if ((Sif->LockTime==1)&&(Sif->Locked==0))
                {
                    V3XVector_Dif((dist), (d), (pos));
                    dii = V3XVector_LengthSq(dist);
                    if (dii<maxy)
                    {
                        maxy = dii;
                        *di = dii;
                        Gs = OVI;
                    }
                }
            }
        }
    }
    *di = sqrtf(*di);
    return Gs;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI *NG_AILocateNearestEnemy(V3XVECTOR *pos, V3XVECTOR *dist, V3XSCALAR *di, int mode)
*
* DESCRIPTION :
*
*/
V3XOVI *NG_AILocateNearestEnemy(V3XVECTOR *pos, V3XVECTOR *dist, V3XSCALAR *di, int mode)
{
    V3XSCALAR maxy=CST_MAX, dii;
    int i, b, mechant;
    int32_t minpower=0xffff, maxpower=0;
    V3XOVI *OVI, *OVI2=NULL, *TargScan;
    SGScript *Sif;
    SGActor *data;
    V3XVECTOR ds;
    TargScan =  g_pLockTarget ?  g_pLockTarget : g_pPlayer->J.OVI;
    for (i=g_SGGame.Scene->numOVI, OVI=g_SGGame.Scene->OVI;i!=0;OVI++, i--)
    {
        if ((OVI->data)&&(((OVI->state&V3XSTATE_HIDDEN)==0)))
        {
            Sif = (SGScript*)OVI->data;
            data = (SGActor*)OVI->data;
            b = 0;
            switch(mode) {
                case t_ENEMY:
                mechant = ((Sif->Type == t_FRIEND)
                ||((Sif->Type == t_PLAYER)&&(!(g_pPlayer->mode&STEALTHMODE)))
                || (Sif->Type == t_PIRAT));
                switch(g_SGObjects.CallMode[t_ENEMY]) {
                    case ORDER_TargetWeakest:
                    if ((mechant)&&(Sif->Shield<minpower))
                    {OVI2=OVI;minpower=Sif->Shield;}
                    break;
                    case ORDER_TargetStrongest:
                    if ((mechant)&&(Sif->Shield>maxpower))
                    {OVI2=OVI;maxpower=Sif->Shield;}
                    break;
                    case ORDER_Attack:
                    b = mechant;
                    break;
                    case ORDER_Retreat:
                    case ORDER_StayAlert:
                    b = 0;
                    break;
                    case ORDER_Protect:  
                    b =  mechant;
                    break;
                    case ORDER_AttackTarget:
                    b = (Sif->Type == t_PLAYER)&&(!(g_pPlayer->mode&STEALTHMODE));
                    break;
                }
                break;
                case t_FRIEND:
                mechant = ((Sif->Type == t_ENEMY)||(Sif->Type==t_PIRAT));
                switch(g_SGObjects.CallMode[t_FRIEND]) {
                    case ORDER_TargetWeakest:
                    if ((mechant)&&(Sif->Shield<minpower))
                    {OVI2=OVI;minpower=Sif->Shield;}
                    break;
                    case ORDER_TargetStrongest:
                    if ((mechant)&&(Sif->Shield>maxpower))
                    {OVI2=OVI;maxpower=Sif->Shield;}
                    break;
                    case ORDER_Attack:
                    b = ((mechant)
                    || (    (Sif->Type == t_PLAYER)
                    &&(!(g_pPlayer->mode&STEALTHMODE))
                    &&  (g_pPlayer->mode&DOOMEDMODE )
                    ));
                    break;
                    case ORDER_Retreat:
                    case ORDER_StayAlert:
                    b = 0;
                    break;
                    case ORDER_Protect:
                    b = ((mechant)&&(data->OVItarget==TargScan));
                    break;
                    case ORDER_AttackTarget:
                    b = (mechant);
                    break;
                }
                break;
                case t_PIRAT:
                mechant = ((Sif->Type == t_FRIEND)
                ||((Sif->Type == t_PLAYER)&&(!(g_pPlayer->mode&STEALTHMODE)))
                ||( Sif->Type == t_ENEMY));
                break;
                case t_SPECIAL:
                b = (Sif->Type == t_SPECIAL);
                break;
                default:
                break;
            }
            if (b)
            {
                V3XVector_Dif(&ds, &OVI->mesh->matrix.v.Pos, (pos));
                dii = V3XVector_LengthSq(&ds);
                if (dii<maxy)
                {
                    maxy = dii;
                    OVI2 = OVI;
                }
            }
        }
    }
    if (OVI2)
    {
        V3XVector_Dif((dist), &OVI2->mesh->matrix.v.Pos, (pos));
        *di = V3XVector_Normalize(dist, dist);
    }
    return OVI2;
}


static void NG_AIEnemyGeneric(V3XOVI *OVI)
{
    V3XKEYVECTOR *nm = &OVI->mesh->Tk.dinfo;
    SGActor *data = (SGActor*)OVI->data;
    SGScript *p = &data->pInf;
    V3XSCALAR k, d;

	// rot
    if (data->Mv.k > 0)
    {
        if (OVI->matrix_Method==V3XMATRIX_Vector2)
        {			
			// SpeedTurn = 1 -> 1 revolution in 1 second
			float dt = ((float)p->SpeedTurn) * g_cTimer.fCounter / 700.f;

            data->Mv.k-= dt;
            if (data->Mv.k > 0)
            {
				V3XVECTOR tmp;
                V3XVector_Slerp(&tmp, &data->Mv.NewAim, &data->Mv.OldAim, data->Mv.k);
				if (V3XVector_LengthSq(&tmp)>CST_EPSILON)
					V3XVector_Normalize(&nm->vect, &tmp);
            }
            else
            {
				if (V3XVector_LengthSq(&data->Mv.NewAim)>CST_EPSILON)
					V3XVector_Normalize(&nm->vect, &data->Mv.NewAim);
            }
        }
    }

    // vel
    {
        k = p->fSpeed * g_cTimer.fCounter * 2;
        V3XVector_Scl(&data->Mv.vel, &nm->vect, k);
    }

    // Deplacement
    V3XVector_Inc(&nm->pos, &data->Mv.vel);

    // Doppler
    if (p->Doppler)
    {
        V3XVECTOR dir;
        // Distance SGActor
        V3XVector_Dif(&dir, &nm->pos, &g_pPlayer->Mat->v.Pos);
        k = V3XVector_LengthSq(&dir);
        if (k<1000000)
        {
            // vel Relative
            V3XVector_Dif(&dir, &g_pPlayer->J.Mv.vel, &data->Mv.vel);
            d = V3XVector_LengthSq(&dir);
            // Doppler ?
            if (d>(V3XSCALAR)32)
            {
                if (!p->Doppling)
                {
                    p->Doppling = NG_Audio3DPlay(p->Doppler-1, &nm->pos, &data->Mv.vel);
                }
                else
                {
                    if (!NG_Audio3DUpdate(&nm->pos, &data->Mv.vel, p->Doppling, p->Doppler-1))
					{
						p->Doppling=0;
					}
                }
            }
			else
			{
				p->Doppling=0;
			}
        }
    }
    return;
}

static void CheckAngle(V3XVECTOR *a, V3XVECTOR *b)
{
    V3XSCALAR t=V3XVector_DotProduct(a, b);
    if (t<(-(CST_ONE/2)))
    {
        V3XVECTOR c;
        V3XVector_CrossProduct(&c, (a), (b));
        *a = c;
    }
    return;
}

static void NG_AIEnemyPatrol(V3XOVI *OVI)
{
    V3XVECTOR di;
    float k;
    SGActor *data = (SGActor*)OVI->data;
    SGScript *p=&data->pInf;
    V3XKEYVECTOR *nm = &OVI->Tk->dinfo;
	V3XVector_Dif(&di, &data->Mv.aim, &nm->pos); 
	data->Mv.Dist = V3XVector_LengthSq(&di);
    if ((data->Mv.k==-1)||(data->Mv.Dist<8)||(OVI->collisionList)||(!(g_SGGame.Count&31)))
    {
        data->Mv.k = 1;
        data->Mv.OldAim = nm->vect;
        NG_AIRandomAim(&data->Mv.aim, g_SGObjects.defTarget[p->Type==t_FRIEND ? t_ENEMY : t_FRIEND], 8178L);
        V3XVector_Dif(&data->Mv.NewAim, &data->Mv.aim, &nm->pos);
        V3XVector_Normalize(&data->Mv.NewAim, &data->Mv.NewAim);
        CheckAngle(&data->Mv.NewAim, &data->Mv.OldAim);
        k = p->fSpeedMax/2;
        p->fSpeed = k + randomf(k);
    }
    NG_AIEnemyGeneric(OVI);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : static void NG_AIEnemyPatrol(V3XOVI *OVI, SGActor *data)
*
* DESCRIPTION : t_TAC_PATROL_FIGHT1
*
*/
static void NG_AIEnemyPatrolAndFight(V3XOVI *OVI)
{
    SGActor *data = (SGActor*)OVI->data;
    V3XVECTOR di, di2, *targ=NULL;
    V3XSCALAR k=0;
    V3XSCALAR k2;
	V3XSCALAR k1=0;
    V3XOVI *oldTarg;
    V3XKEYVECTOR *nm = &OVI->Tk->dinfo;
    SGScript *p=&data->pInf;
    int32_t camp = p->Type;
    // Trouve la cible
    oldTarg = data->OVItarget;
    COMPUTE_FOEDIST();
    if (data->OVItarget)
    {
        if (data->OVItarget!=OVI) 
			targ=&data->OVItarget->mesh->matrix.v.Pos;
    }
    // Touch‚ ?
    V3XVector_Dif(&di, &data->Mv.aim, &nm->pos);
    data->Mv.Dist = V3XVector_LengthSq(&di);
    if (data->Mv.k<=0)
    {
        if ((data->Mv.Dist<8)||(oldTarg!=data->OVItarget)||((g_SGGame.Count&127)==0))
        {
            if (targ)
            {
                V3XVector_Dif(&di, targ, &nm->pos);
                k = V3XVector_LengthSq(&di);
                /*if (k<p->Sight)
                {
                    data->Mv.Ang+=1600;
                    NG_AIRandomAimEx(&data->Mv.aim, targ, p->Sight*64, data->Mv.Ang, data->Mv.Ang);
                }
                else
                */
                {
                    data->Mv.aim=*targ;
                }
                p->fSpeed = p->fSpeedMax/2;
                p->fSpeed = p->fSpeed+randomf(p->fSpeed+1);
            }
            else
            {
                NG_AIRandomAim(&data->Mv.aim, g_SGObjects.defTarget[camp], 3048L);
            }
            k1 = 1;
        }
        if (((g_SGGame.Count&31)==0)||(k1))
        {
            V3XVector_Dif(&data->Mv.NewAim, &data->Mv.aim, &nm->pos);
            V3XVector_Normalize(&data->Mv.NewAim, &data->Mv.NewAim);
            data->Mv.k = 1;
            data->Mv.OldAim = nm->vect;
            CheckAngle(&data->Mv.NewAim, &data->Mv.OldAim);
        }
    }
    if ((data->OVItarget)&&(!data->pInf.ShootOk))
    {
        V3XSCALAR kx = V3XVector_DotProduct(&nm->vect, &di2);
        if ((kx>0.8*CST_ONE)&&(k<24*p->Sight)) //40000
        {
            NG_WeaponFire(data, data->pInf.Attack, data->OVItarget);
            OVI->state|=V3XSTATE_MATRIXUPDATE;
        }
    }
    NG_AIEnemyGeneric(OVI);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_AIEnemyAttack(V3XOVI *OVI)
*
* DESCRIPTION :
*
*/
static void NG_AIEnemyAttack(V3XOVI *OVI)
{
    SGActor *data = (SGActor*)OVI->data;
    SGScript *p=&data->pInf;
    V3XVECTOR di2;
    V3XSCALAR k2 = 0;
    int32_t camp = p->Type;
    COMPUTE_FOEDIST();
    if (!data->OVItarget) data->OVItarget = g_pPlayer->J.OVI;
    if ((data->OVItarget)&&(!data->pInf.ShootOk))
    {
        V3XSCALAR kx=V3XVector_DotProduct(&OVI->mesh->matrix.v.K, &di2);
        if ((kx)&&(k2<4*p->Sight)&&sysRand(2))
        {
            NG_WeaponFire(data, data->pInf.Attack, data->OVItarget);
            OVI->state|=V3XSTATE_MATRIXUPDATE;
        }
    }
    if (p->Tactic == t_TAC_ATTACK1)
    {
        V3XKEYVECTOR *nm = &OVI->Tk->dinfo;
        if (data->Mv.k<=0)
        {
            data->Mv.aim = data->OVItarget->mesh->matrix.v.Pos;
            V3XVector_Dif(&data->Mv.NewAim, &data->Mv.aim, &nm->pos);
			V3XVector_Normalize(&data->Mv.NewAim, &data->Mv.NewAim);
            data->Mv.k = 1;
            data->Mv.OldAim = nm->vect;
        }
        NG_AIEnemyGeneric(OVI);
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_AIEnemyKamikaze(V3XOVI *OVI)
*
* DESCRIPTION :
*
*/
static void NG_AIEnemyKamikaze(V3XOVI *OVI)
{
    SGActor *data = (SGActor*)OVI->data;
    V3XVECTOR di, di2, *targ;
    SGScript *p=&data->pInf;
    int32_t camp=p->Type;
    V3XSCALAR k;
    V3XSCALAR k2 = 0;
    V3XKEYVECTOR *nm = &OVI->Tk->dinfo;
    // Trouve la cible
    COMPUTE_FOEDIST();
    targ = &data->OVItarget->mesh->matrix.v.Pos;
    // Touched ?
    V3XVector_Dif(&di, &data->Mv.aim, &nm->pos); 
	k = V3XVector_Normalize(&di, &di);
    
	// Proche, touch‚ ou fin de courbe
    if ((k<64)&&(data->Mv.k<=0))
    {
        data->Mv.k = 1;
        data->Mv.OldAim = data->Mv.aim;
		V3XVector_Normalize(&data->Mv.NewAim, targ);
        data->Mv.aim = data->Mv.NewAim;
        p->fSpeed = p->fSpeedMax/2;
        p->fSpeed = p->fSpeed+randomf(p->fSpeed+1);
    }
    if (k2<32)
    {
        if (p->Shield>0) p->Shield=-1;
    }
    NG_AIEnemyGeneric(OVI);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : static void NG_AIEnemyPatrol(V3XOVI *OVI, SGActor *data)
*
* DESCRIPTION : Mine (explose a <sight> distance)
*
*/
static void NG_AIEnemyMine(V3XOVI *OVI)
{
    SGActor *data = (SGActor*)OVI->data;
    V3XVECTOR di2;
    SGScript *p=&data->pInf;
    int32_t camp = p->Type;
    V3XSCALAR k2;
	int32_t kk = g_SGObjects.CallMode[t_ENEMY]; // findNearest
    g_SGObjects.CallMode[t_ENEMY]=ORDER_Attack;
    data->OVItarget = NG_AILocateNearestEnemy(&OVI->mesh->matrix.v.Pos, &di2, &k2, camp);
    if ((data->OVItarget)&&(k2<p->Sight))
    {
        if (p->Shield>0)
			p->Shield=-1;
    }
    g_SGObjects.CallMode[t_ENEMY] = (u_int8_t)kk;
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : static void NG_AIEnemyPatrol(V3XOVI *OVI, SGActor *data)
*
* DESCRIPTION : Track - ok
*
*/
void V3XTrack_Draw(V3XTRI *TRI, int f)
{
    int i;
    V3XVECTOR a, x;
    V3XVECTOR2 n, m;
    V3XVector_Cpy(x, TRI->keys[TRI->numFrames-1].info.pos);
    V3XVector_TransformProject_pts(&x, &a);
    m.x = a.x; m.y = a.y;
    for (i=0;i<TRI->numFrames;i++)
    {
        V3XVector_Cpy(x, TRI->keys[i].info.pos);
        if (V3XVector_TransformProject_pts(&x, &a))
        {
            n.x = a.x;
            n.y = a.y;
            GX_ClippedLine( &n, &m, i<f ? i+1 : g_SGGame.CI_WHITE);
            m=n;
        }
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_AIEnemyTrack(V3XOVI *OVI)
*
* DESCRIPTION :
*
*/
#define LASTDISTTRCK 4.
#define MINDISTTRCK  4000.
#define MAXDISTTRCK  4000.
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_AIEnemyTrack(V3XOVI *OVI)
*
* DESCRIPTION :
*
*/
static void NG_AIEnemyTrack(V3XOVI *OVI)
{
    SGActor *data = (SGActor*)OVI->data;
    V3XTVI *TVI=OVI->TVI;
    V3XTRI *TRI=TVI->TRI;
    int op=0;
    int lsttrck = TVI->frame==TRI->numFrames-1;
    V3XSCALAR k;
    SGScript *p = &data->pInf;
    V3XKEYVECTOR *nm = &OVI->Tk->dinfo;
    V3XVector_Dif(&data->Mv.NewAim, &data->Mv.aim, &nm->pos);
	k = V3XVector_Length(&data->Mv.NewAim);
	if (k<CST_EPSILON)
		data->Mv.NewAim = nm->vect;		
	
	V3XVector_Normalize(&data->Mv.NewAim, &data->Mv.NewAim);

    if (p->AnimMode&m_OFF)
		p->fSpeed = 0;
    else
    if (data->Mv.k<=0)
    {
        if (k>=MAXDISTTRCK)
        {
            nm->vect = data->Mv.NewAim;
        }
        nm->roll+= (g_cTimer.iCounter>>15);
        data->Mv.Dist = k;
        if ((k<=(lsttrck ? LASTDISTTRCK : MINDISTTRCK))||(op==4))
        {
            if (!(p->AnimMode&m_REVERSE))
            {
                TVI->frame+=1;
                op = 2;
                if (TVI->frame>=TRI->numFrames)
                {
                    op = 1;
                    if (p->AnimMode&m_LOOP)
                    {
                        TVI->frame=0;
                    }
                    else
                    {
                        p->AnimMode|=m_OFF;
                        TVI->frame = (V3XSCALAR)(TRI->numFrames-1);
                    }
                    if (p->AnimMode&m_PINGPONG)
                    {
                        p->AnimMode|=m_REVERSE;
                    }
                    if (p->EndTrack==t_ET_WARP) p->Wait=16;
                }
            }
            else
            {
                TVI->frame--;
                op = 2;
                if (TVI->frame<=0)
                {
                    op = 1;
                    if (p->AnimMode&m_LOOP)
                    {
                        TVI->frame = (V3XSCALAR)(TRI->numFrames-1);
                    } else
                    {
                        p->AnimMode|=m_OFF;
                        TVI->frame=0;
                    }
                    if (p->AnimMode&m_PINGPONG)
                    {
                        p->AnimMode|=m_REVERSE;
                    }
                }
            }
            if (op==1)
            {
                if (p->AfterTrack==t_AD_ADD_AIM)
                {
					sysConPrint("Escort completed ...");
                    g_SGGame.MaxAim[g_SGObjects.NAV]-=p->Mission;
                    g_pPlayer->Notify+=SGNET_UPDATEAIM;
                    g_SGSettings.ComNumber=COM_EscortOk; 
                    g_SGSettings.ComTime=MAX_COM_DELAY;
                    NG_DisplayWarp();
                    p->AnimMode|=m_OFF;
                }
                if (p->AfterTrack==t_AD_EXPLODE)
                {
                    if (p->Shield>=0) p->Shield=-1;
                    p->AnimMode|=m_OFF;
                }
                if (p->AfterTrack==t_AD_FAILED)
                {
                    g_SGSettings.ComNumber=COM_Failed;
                    g_SGSettings.ComTime=MAX_COM_DELAY;
                    g_SGObjects.FinCode=GAMESTATE_FAIL;
                    p->AnimMode|=m_OFF;
                }
            }
        }
        p->fSpeed = (op==4) ? p->fSpeedMax/2 : p->fSpeedMax;
    }
    else
    {
        lsttrck = 1;
    }
    if (lsttrck)
		p->fSpeed = p->fSpeedMax>4 ? p->fSpeedMax/4 : 1;

    if ( op )
    {
        p->fSpeed = (op==4) ? p->fSpeedMax/2 : p->fSpeedMax;
        data->Mv.k = 1;
        data->Mv.OldAim = nm->vect;
        data->Mv.aim = TRI->keys[(int32_t)TVI->frame].info.pos;
    }
    else NG_AIEnemyGeneric(OVI);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void NG_CheckHindMode(void)
*
* DESCRIPTION : Verifie le mode Hind
*
*/
static void NG_CheckHindMode(void)
{
    int i, j;
    switch(g_cAI.mode) 
	{
        case HIND_MODE_PHASE_0:
        g_cAI.Target = g_pPlayer->J.OVI;
        g_cAI.mode = HIND_MODE_PHASE_1;
        break;
        case HIND_MODE_PHASE_1:
        if (g_cAI.nHind)
        {
            for (j=0, i=0;i<g_cAI.nHind;i++)
            {
                V3XOVI *OVI=g_cAI.OVI[i];
                SGActor *data = (SGActor*)OVI->data;
                if (data->Mv.Reg) j++;
            }
            if (j==g_cAI.nHind)
            {
                g_cAI.mode = HIND_MODE_PHASE_2;
                g_cAI.fTime = 1000;
            }
        }
        break;
        case HIND_MODE_PHASE_2:
        g_cAI.fTime -= g_cTimer.fCounter;
        if (g_cAI.fTime < 0) 
			g_cAI.mode = HIND_MODE_PHASE_0;
        break;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  : void NG_RegisterGroupTactic(void)
*
* DESCRIPTION : Prepare les tactics de groups
*
*/
static void NG_RegisterGroupTactic(void)
{
    int i;
    V3XOVI *OVI;
    V3XORI *ORI;
    SGActor *J;
    SGScript *pInf;
    g_cAI.nHind = 0;
    for (i=g_SGGame.Scene->numOVI, OVI=g_SGGame.Scene->OVI;i!=0;OVI++, i--)
    {
        ORI = OVI->ORI;
        if ((ORI->type==V3XOBJ_MESH)&&(((OVI->state&V3XSTATE_HIDDEN)==0)))
        {
            J = (SGActor*)OVI->data;
            if (J)
            {
                pInf = &J->pInf;
                switch(pInf->Tactic) {
                    case t_TAC_HIND:
                    if ((g_cAI.nHind<HIND_MAXPLAYER)
                    &&  (g_cAI.mode == HIND_MODE_PHASE_1))
                    {
                        g_cAI.OVI[g_cAI.nHind] = OVI;
                        J->Mv.Id = g_cAI.nHind;
                        g_cAI.nHind++;
                    }
                    break;
                }
            }
        }
    }
    NG_CheckHindMode();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void NG_AIEnemyHind(V3XOVI *OVI)
*
* DESCRIPTION :
*
*/
static void NG_AIEnemyHind(V3XOVI *OVI)
{
    SGActor *data = (SGActor*)OVI->data;
    V3XVECTOR di;
    V3XVECTOR *targ;
    V3XKEYVECTOR *nm;
    SGScript *p;
    if ((!g_cAI.nHind)||(!g_cAI.Target)) 
		return;

    targ =  &g_cAI.Target->Tk->vinfo.pos;
    nm =  &OVI->Tk->dinfo;
    p = &data->pInf;

    // Selectionne le type
    data->OVItarget = g_cAI.Target;
    switch(g_cAI.mode) 
	{
        case HIND_MODE_PHASE_1:			
			if ((!data->Mv.Reg)||(!(g_SGGame.Count&255)))
			{
				data->Mv.Ang = (data->Mv.Id * 4096) / g_cAI.nHind;
				NG_AIRandomAimEx(&data->Mv.aim, targ, 
					(V3XSCALAR)(p->Sight*4), (int32_t)data->Mv.Ang, (int32_t)data->Mv.Ang);
				V3XVector_Dif(&di, &data->Mv.aim, &nm->pos);
				data->Mv.Dist = V3XVector_Length(&di);
				data->Mv.Reg = 0;
			}

			if (data->Mv.Dist < 384)
			{
				data->Mv.aim  = *targ;
				data->Mv.Reg = 1;
				p->fSpeed = 0; // Stop
			}
			else
			{
				p->fSpeed = p->fSpeedMax;
			}
        // Pas de break
        case HIND_MODE_PHASE_2:
			if (g_cAI.mode==HIND_MODE_PHASE_2) 
				p->fSpeed = p->fSpeedMax;

			if ((!data->pInf.ShootOk)&&(data->Mv.Reg))
			{
				NG_WeaponFire(data, data->pInf.Attack, data->OVItarget);
				OVI->state|=V3XSTATE_MATRIXUPDATE;
			}
			if (data->Mv.k<=0)
			{
				V3XVector_Dif(&data->Mv.NewAim, &data->Mv.aim, &nm->pos);
				V3XVector_Normalize(&data->Mv.NewAim, &data->Mv.NewAim);
				data->Mv.k = 1;
				data->Mv.OldAim = nm->vect;
			}        
        break;
    }
    NG_AIEnemyGeneric(OVI);
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_AddFaceMoreObjects(void)
*
* DESCRIPTION :
*
*/
void V3XLIGHT_AddOmni(V3XVECTOR *pos, V3XSCALAR intensity, rgb24_t *color, int type, int flags)
{
    V3XLIGHT *lite = V3X.Light.light + V3X.Light.numSource;
    if (V3X.Light.numSource<V3X.Buffer.MaxLight-1)
    {
        lite->pos = *pos;
        lite->type  = type;
        lite->flags = flags;
        lite->flags     |= V3XLIGHTCAPS_DYNAMIC;
        lite->intensity = intensity>1.f ? intensity : 1.f;
        lite->diminish =  1.f;
        lite->range = V3XLIGHTCAPS_LENZFLARE ? 20000.f : 10000.f;
        RGB_Set(lite->color, color->r, color->g, color->b);
        V3X.Light.numSource++;
    }
    return;
}
static rgb24_t ColorWeap[22]={
    // R G B
    {0, 0, 0}, 
    {128, 0, 0}, 
    {0, 128, 0}, 
    {200, 200, 100}, 
    {128, 128, 128}, 
    {0, 0, 200}, 
    {180, 180, 180}, 
    {0, 0, 0}, 
    {0, 0, 0}, 
    {0, 0, 0}, 
    {0, 128, 0}, 
    {128, 0, 0}, 
    {128, 128, 0}, 
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}, 
    {0, 0, 0},
    {0, 0, 0}, 
    {0, 0, 0},
    {0, 0, 0}, 
    {128, 128, 0}, 
{180, 180, 180}};
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_AddFaceMoreObjects(void)
*
* DESCRIPTION :  
*
*/
void NG_AddMoreLights(void)
{
	SGEffect *p;
    SGWeapon *w ;
    int i;
    for (i=g_SGGame.numWeapons, w=g_SGGame.pWea;i!=0;w++, i--)
    {
        if ((w->Available)&&(w->pInf.Code<22))
        {
            V3XLIGHT_AddOmni(&w->OVI->mesh->matrix.v.Pos, 150., ColorWeap+w->pInf.Code, 
            V3XLIGHTTYPE_OMNI, V3XLIGHTCAPS_RANGE);
        }
    }
	for (p=g_SGGame.pExpl, i=g_SGGame.numEffects;i!=0;p++, i--)
    {
        if ((p->fce.visible)&&(!p->Dispo))
        {
            // Rajoute  une light pour les explosions
            if (p->Light)
            {
                rgb24_t color={255, 255, 200};
                int flags, pas=(g_cTimer.iCounter>>15);
                if (((V3X.Setup.flags&V3XOPTION_TRUECOLOR)==0)||(g_SGSettings.VisualsFx<3))
                flags = V3XLIGHTCAPS_RANGE;
                else flags = V3XLIGHTCAPS_LENZFLARE|V3XLIGHTCAPS_RANGE;
                V3XLIGHT_AddOmni(p->pos, (V3XSCALAR)p->Light, &color, V3XLIGHTTYPE_OMNI, flags);
                if ((int)p->Light>pas) p->Light-=pas;
                else p->Light=0;
            }     
        }
    }
}


/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_HudLockTargetOff(void)
*
* DESCRIPTION :
*
*/
void NG_HudLockTargetOff(void)
{
    if (g_pLockTarget!=NULL) ((SGScript*)g_pLockTarget->data)->Locked=0;
    g_pLockTarget=NULL;
    NG_AISetTacticMode();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_HudLockTargetReset(void)
*
* DESCRIPTION :
*
*/
void NG_HudLockTargetReset(void)
{
    if (g_pLockTarget!=NULL) ((SGScript*)g_pLockTarget->data)->LockTime=0;
    NG_AISetTacticMode();
    return;
}
static unsigned LockCycle=0;

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  V3XOVI static *NG_HudCycleTarget(void)
*
* DESCRIPTION :
*
*/
V3XOVI static *NG_HudCycleTarget(void)
{
    V3XOVI *OVI, *Gs=NULL;
    SGScript *Sif;
    unsigned x;
    if (LockCycle>=g_SGGame.Scene->numOVI) LockCycle=0;
    x = LockCycle;
    do
    {
        OVI = g_SGGame.Scene->OVI + LockCycle;
        if (  (OVI->data)
        &&((OVI->state&(V3XSTATE_HIDDEN|V3XSTATE_CULLED))==0)
        )
        {
            Sif = (SGScript*)OVI->data;
            if (Sif->ColorRadar)
            {
                V3XVECTOR dir;
                V3XVector_Dif(&dir, &OVI->mesh->matrix.v.Pos, &g_pPlayer->Rot->pos);
                if (V3XVector_DotProduct(&dir, &V3X.Camera.M.v.K)<CST_ZERO) 
					Gs = OVI;
            }
        }
        LockCycle++;
        if (LockCycle>=g_SGGame.Scene->numOVI)
        {
            LockCycle=0;
        }
    }while(!((x==LockCycle)||(Gs!=NULL)));
    return Gs;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_HudLockTargetOn(void)
*
* DESCRIPTION :
*
*/
void NG_HudLockTargetOn(void)
{
    V3XOVI *Gs;
    Gs = NG_HudCycleTarget();
    if (Gs)
    {
        ((SGScript*)Gs->data)->Locked=1;
        NG_HudLockTargetOff();
        g_pLockTarget = Gs;
    } else NG_HudLockTargetOff();
    NG_AISetTacticMode();
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static int LookEventCode(u_int8_t code)
*
* DESCRIPTION :
*
*/
static int LookEventCode(u_int8_t code)
{
    int i, c=1;
    V3XSCENE *Scene = g_SGGame.Scene;
    V3XOVI *OVI;
    V3XORI    *ORI;
    SGActor *J;
    SGScript *pInf;
    for (i=Scene->numOVI, OVI=Scene->OVI;(i!=0) && (c!=0);OVI++, i--)
    {
        ORI = OVI->ORI;
        if (ORI->type==V3XOBJ_MESH)
        {
            if (((OVI->state&V3XSTATE_HIDDEN)==0))
            {
                J = (SGActor*)OVI->data;
                if (J)
                {
                    pInf = &J->pInf;
                    if ((pInf->Event==code)&&(pInf->EventCode==t_EC_ENABLE))
                    {
                        switch(pInf->EventType) {
                            case t_ET_SHOOT:
                            if (!OVI->collisionList) c = 0; // pas touch‚ : on annule
                            break;
                            case t_ET_DESTROY:
                            c = 0; // pas destruit : on annule
                            break;
                        }
                    }
                }
            }
        }
    }
    return c;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void NG_LevelUpdate(void)
*
* DESCRIPTION :
*
*/
void NG_LevelUpdate(void)
{
    int i, As_wet=1;
    V3XVECTOR dist, *pos;
    V3XSCALAR di;
    V3XSCENE *Scene = g_SGGame.Scene;
    V3XOVI *OVI, *OVIp;
    V3XORI *ORI;
    SGEffect *EI;
    SGActor *J, *Jp;
    SGScript *pInf=NULL, *PIp=NULL;

    // Reset matrix Sky
    if ((g_SGGame.Count)&&(!(g_SGGame.Count&511))) // change d'ordre Enemys
    {
        g_SGObjects.CallMode[t_ENEMY]++;
        if (g_SGObjects.CallMode[t_ENEMY]>ORDER_TargetStrongest)
            g_SGObjects.CallMode[t_ENEMY]=ORDER_AttackTarget;
        //sprintf(tex, "com0%d", g_SGObjects.CallMode[t_ENEMY]+1); NG_AudioSay(tex);
    }

    NG_RegisterGroupTactic();
    NG_CheckHindMode();

    for (i=Scene->numOVI, OVI=Scene->OVI;i!=0;OVI++, i--)
    {
        ORI = OVI->ORI;
        if (ORI->type==V3XOBJ_LIGHT)
        {
            OVI->state|=V3XSTATE_MATRIXUPDATE;
            OVI->light->intensity = 255.f;
            OVI->light->type = V3XLIGHTTYPE_DIRECTIONAL;
            OVI->light->flags    |= V3XLIGHTCAPS_INFINITE|V3XLIGHTCAPS_LENZFLARE;
            RGB_Set(OVI->light->color, 255, 255, 255);
        }
        else
        if (ORI->type==V3XOBJ_MESH)
        {
			int c = min(1, (g_cTimer.iCounter>>16));
            pos = &OVI->mesh->matrix.v.Pos;
            if (OVI->mesh->selfIllumine> c) 
				OVI->mesh->selfIllumine-= (u_int16_t)c;
            else 
				OVI->mesh->selfIllumine=0;
            J = (SGActor*)OVI->data;
            if (J)
            {
                pInf = &J->pInf;
                if (pInf->Type!=t_ACCESSOIRE)
                {
                    if (pInf->Wait>0)
                    if ((pInf->ColorNav==g_SGObjects.NAV)||(pInf->Type==t_SPECIAL))
                    {
                        pInf->Wait-=g_cTimer.iCounter>>16;
                        if (pInf->Wait<=0)
                        {
                            pInf->Wait = 0;
                            if (!pInf->Stealth)  
                               pInf->Shield= pInf->ShieldMax;
                            if (pInf->AppearMode)
                                NG_FXNew(pos, FX_WARPIN, 1, &((SGActor*)OVI->data)->pInf, 2, OVI);
                            else 
                                 OVI->state^=V3XSTATE_HIDDENDISPLAY;
                            if (pInf->Stealth)
                            {
                                OVI->state &= ~V3XSTATE_HIDDENDISPLAY;
                                pInf->Wait = pInf->Appear;
                            }
                            if ((pInf->CollisionStyle==t_CS_BONUS)&&(g_SGSettings.SerialGame))
                            {
                                pInf->Wait = 500; // Reapparition des bonuses en multijoueur
                            }
                        }
                    }
                    if ((pInf->Appear>0)&&(!pInf->Stealth))
					{
						if (pInf->ColorNav==g_SGObjects.NAV)
						{
							OVIp = NG_AILocateNearestEnemy(pos, &dist, &di, t_FRIEND);
							if ((OVIp!=OVI)&&(OVIp!=NULL)&&(di<pInf->Appear))
							{
								pInf->Appear = 0;
								NG_FXNew(pos, FX_WARPIN, 1, &((SGActor*)OVI->data)->pInf, 2, OVI);
							}
						}
					}
                    switch(pInf->EventCode) {
                        case 0:
                        break;
                        case t_EC_SHIELDOFF:
                        if (!LookEventCode(pInf->Event)) pInf->Shield=pInf->ShieldMax;
                        break;
                        case t_EC_SHIELDOFFDIE:
                        if (!LookEventCode(pInf->Event))
                        {
                            pInf->Shield=pInf->ShieldMax;
                        }
                        else pInf->Shield=0;
                        break;
                        case t_EC_APPEAR:
                        if (LookEventCode(pInf->Event)) NG_FXNew(pos, FX_WARPIN, 1, &((SGActor*)OVI->data)->pInf, 2, OVI);
                        break;
                        case t_EC_TRACK_TOGGLE:
                        if (LookEventCode(pInf->Event))
                        {
                            if (OVI->state&V3XSTATE_HIDDEN) NG_FXNew(pos, FX_WARPIN, 1, &((SGActor*)OVI->data)->pInf, 2, OVI);
                            pInf->AnimMode^=m_OFF;
                        }
                        break;
                        case t_EC_MORPHLOOP:
                        case t_EC_MORPHON:
                        case t_EC_MORPHOFF:
                        break;
                    }
                    // Destruction hiearchie
                    if (((OVI->state&V3XSTATE_HIDDEN)==0))
                    {
                        OVIp = OVI->parent;
                        if (OVIp)
                        {
                            Jp = (SGActor*)OVIp->data;
                            if (Jp)
                            {
                                PIp = &Jp->pInf;
                                if ((OVIp==g_SGObjects.Sky)&&(OVI->mesh))
									OVI->mesh->flags|=V3XMESH_EXCPTDYNLIGHT;

                                if (OVIp->state&V3XSTATE_HIDDEN)           // Le parent est cach‚
                                {
                                    if (PIp->Shield<=0)   // ... et plus d'energie
                                    {
                                        pInf->Shield =0;  // NG_FXNew(pos, FX_CUSTOM, 1, &((SGActor*)OVIp->data)->pInf, 2, OVI);
                                    }
                                }
                                else
                                if (PIp->Shield>0) OVI->state&=~V3XSTATE_HIDDEN;
                            }
                        }
                    }
                    if (((OVI->state&V3XSTATE_HIDDEN)==0))
                    {
                        if (pInf->AmbiantSound)
						{
							if ((As_wet)&&(!(OVI->state&V3XSTATE_CULLEDOFR)))
							{
								if (pInf->AmbiantChannel<=0)
								{
									pInf->AmbiantChannel = NG_Audio3DPlay(pInf->AmbiantSound-1, pos, NULL);
								}
								else
								if (!NG_Audio3DUpdate(pos, NULL, pInf->AmbiantChannel, pInf->AmbiantSound-1))
								pInf->AmbiantChannel=0;
								As_wet=0;
							}
						}

						if (pInf->ShootOk>0) 
							pInf->ShootOk-=g_cTimer.iCounter>>16; 
						else 
							pInf->ShootOk=0;
                        //
                        //  GESTION DESTRUCTION OBJET
                        //
                        if (pInf->AfterDeath)
                        {
                            pInf->AfterDeath-=g_cTimer.iCounter>>16;
                            pInf->fFrozen = 1;
                            pInf->Shield = 0;
                            if (pInf->AfterDeath<=0)
                            {
                                pInf->Stealth = 0;
                                pInf->Wait = 0;
                                pInf->Appear = 0;
                                switch(pInf->AfterDestroy) 
                                {
                                    case t_AD_ADD_AIM:
                                    sysConPrint("Completed an objective ..");
                                    g_SGGame.MaxAim[g_SGObjects.NAV]-=pInf->Mission;
                                    if (g_SGGame.MaxAim[g_SGObjects.NAV]<=0)
                                    {
                                        g_SGGame.WarpOk=0;
                                        g_SGGame.MaxAim[g_SGObjects.NAV]=0;
                                    }
                                    if (g_SGGame.IsHost)
                                       g_pPlayer->Notify+=SGNET_UPDATEAIM;
                                    NG_DisplayWarp();
                                    break;
                                    case t_AD_FAILED:
                                    sysConPrint("Objective failed");
                                    g_SGGame.MaxProtect[g_SGObjects.NAV]-=pInf->Protect;
                                    if (g_SGGame.MaxProtect[g_SGObjects.NAV]==0)
                                    {
                                        g_SGSettings.ComNumber=COM_Failed;
                                        g_SGSettings.ComTime=MAX_COM_DELAY;
                                        g_SGObjects.FinCode=GAMESTATE_FAIL;
                                    }
                                    break;
                                    case t_AD_SUCCESS:
                                    sysConPrint("Objective success");
                                    g_SGObjects.FinCode = GAMESTATE_WON;
                                    break;
                                }
                                EI = NG_FXNew(pos, FX_CUSTOM, 1, pInf, 1, OVI);

                                if (EI)
                                {
                                    EI->Size = OVI->ORI->global_rayon * 4;
                                    g_SGGame.FlashColor.r=255;
                                    g_SGGame.FlashColor.g=255;
                                    g_SGGame.FlashColor.b=0;
                                    g_SGGame.FlashAlpha=16;
                                }
                                if (pInf->DebrisMode) 
                                   NG_FXDebris(pInf, OVI);

                                OVI->state |= V3XSTATE_HIDDEN;
                                if ((g_SGSettings.Difficulty==2)&&((pInf->Type==t_ENEMY)||(pInf->Type==t_PIRAT)))
                                {
                                    pInf->Wait = 1000;
                                    pInf->Mission = 0;
                                    pInf->Shield = pInf->ShieldMax;
                                    pInf->ColorRadar = 7;
                                }

                                pInf->AfterDeath=0;

                                if (pInf->Replace)
                                {
                                    char *g=strstr(OVI->ORI->name, "_");
                                    if (g)
                                    {
										
										char tex[256];
                                        sprintf(tex, "%s%s", g_pPlayerInfo[pInf->Replace].Basename, g);
                                        OVIp = V3XScene_OVI_GetByName(g_SGGame.Scene, tex);
                                        if (OVIp)
                                        {
                                            OVIp->state &= ~V3XSTATE_HIDDEN;
                                            *(OVIp->Tk) = *(OVI->Tk);
                                            pInf->Replace = 0;
                                        }
                                    }
                                }
                            }
                        }
                        //
                        //  GESTION SHIELD
                        //
                        if (pInf->Shield<=0)
                        {
                            if ((g_SGObjects.FinCode!=GAMESTATE_DEAD)&&(OVI==g_pPlayer->J.OVI))
                            {
                                J->pInf.AfterDeath = 168;
                                J->pInf.fSpeed = 0;
                                g_pPlayer->Mv.inert = 0;
                                NG_HudLockTargetOff();
                                g_SGObjects.FinCode=GAMESTATE_DEAD;
                                g_SGGame.CameraMode=CAMERA_DEATH;
                                g_pPlayer->J.pInf.Shield=0;
                                g_SGSettings.ComNumber = COM_Failed;
                                g_SGSettings.ComTime = MAX_COM_DELAY;
                                NG_AudioStopSound(g_cFXTable.Engine);
                                NG_AudioStopSound(g_cFXTable.Alarm );
                                if (g_SGSettings.DemoMode) 
                                   g_SGObjects.FinCode = GAMESTATE_ABORT;
                                g_pPlayer->Notify+=SGNET_HASDIE;
                            }

                            if (!pInf->AfterDeath)
                            {
                                pInf->Shield = 0;
                                pInf->fFrozen = 1;
                                if (pInf->Type==t_FRIEND)
                                {
                                    g_SGSettings.ComNumber=COM_FriendDead;
									g_SGSettings.ComTime=MAX_COM_DELAY;
                                }
                                if (pInf->BigExplode)
                                {
                                    NG_FXBlast(pInf, OVI);
                                    pInf->AfterDeath = 168;
                                }
                                else
                                {
                                    pInf->AfterDeath = 1;
                                }
                                g_SGGame.FlashColor.r=200;
                                g_SGGame.FlashColor.g=160;
                                g_SGGame.FlashColor.b=40;
                                g_SGGame.FlashAlpha=16;
                            }
                        }
                        else
                        {
                            if ((pInf->SoundExplode)&&
								(pInf->Shield<pInf->ShieldMax/4)&&
								((g_SGGame.Count&1024)==0)&&((OVI!=g_pPlayer->J.OVI)||(g_SGGame.CameraMode)))
									NG_FXNew(&OVI->mesh->matrix.v.Pos, FX_SMOKE, 1, &J->pInf, 1, OVI);
                        }
                        //
                        //   GESTION MOuVEMENT / TACTIC
                        //
                        if (!pInf->fFrozen)
                        {
                            switch(pInf->Animation)
							{
                                case t_FIXE:
									if( OVI->parent) 
										OVI->state|=V3XSTATE_MATRIXUPDATE;
                                break;
                                case t_GIANT:
                                if ((!g_SGObjects.Nav->Skybox)&&(OVI->state&V3XSTATE_HIDDEN))
                                    OVI->state|=V3XSTATE_HIDDEN;
                                OVI->state|=V3XSTATE_MATRIXUPDATE;
                                break;
                                case t_TURN_AROUND_X:
									OVI->Tk->info.angle.x+=pInf->fSpeedMax*g_cTimer.fCounter;
									OVI->state|=V3XSTATE_MATRIXUPDATE;
                                break;
                                case t_TURN_AROUND_Y:
									OVI->Tk->info.angle.y+=pInf->fSpeedMax*g_cTimer.fCounter;
									OVI->state|=V3XSTATE_MATRIXUPDATE;
                                break;
                                case t_TURN_AROUND_Z:
									OVI->Tk->info.angle.z+=pInf->fSpeedMax*g_cTimer.fCounter;
									OVI->state|=V3XSTATE_MATRIXUPDATE;
                                break;
                                case t_TURN_RANDOM:
                                if (i&1) 
									OVI->Tk->info.angle.x+=pInf->fSpeedMax*g_cTimer.fCounter;
                                else 
									OVI->Tk->info.angle.y+=pInf->fSpeedMax*g_cTimer.fCounter;
                                OVI->Tk->info.angle.z+=pInf->fSpeedMax*g_cTimer.fCounter;
                                OVI->state|=V3XSTATE_MATRIXUPDATE;
                                break;
                                case t_MPLAYER:
                                OVI->state|=V3XSTATE_MATRIXUPDATE;
                                break;
                            }
                            if ((!g_SGGame.IsHost)||(((g_SGSettings.DemoMode>=2)&&(g_pRecordData.version == 3)))) OVI->state|=V3XSTATE_MATRIXUPDATE;
                            else
                            {
                                g_pPlayer->Notify|=SGNET_SHIP;
                                switch(pInf->Tactic) {
                                    case 0:
                                    break;
                                    case t_TAC_DROID:
                                    NG_AIEnemyKamikaze(OVI);
                                    OVI->state|=V3XSTATE_MATRIXUPDATE;
                                    break;
                                    case t_TAC_MINE:
                                    NG_AIEnemyMine(OVI);
                                    OVI->state|=V3XSTATE_MATRIXUPDATE;
                                    break;
                                    case t_TAC_ATTACK1:
                                    OVI->state|=V3XSTATE_MATRIXUPDATE;
                                    case t_TAC_ATTACK0:
                                    NG_AIEnemyAttack(OVI);
                                    break;
                                    case t_TAC_RANDOM_PATROL:
                                    NG_AIEnemyPatrol(OVI);
                                    OVI->state|=V3XSTATE_MATRIXUPDATE;
                                    break;
                                    case t_TAC_PATROL_FIGHT1:
                                    NG_AIEnemyPatrolAndFight(OVI);
                                    OVI->state|=V3XSTATE_MATRIXUPDATE;
                                    break;
                                    case t_TAC_TRACKFULL:
                                    OVI->state|=V3XSTATE_MATRIXUPDATE;
                                    break;
                                    case t_TAC_TRACK:
                                    NG_AIEnemyTrack(OVI);
                                    OVI->state|=V3XSTATE_MATRIXUPDATE;
                                    break;
                                    case t_TAC_HIND:
                                    NG_AIEnemyHind(OVI);
                                    OVI->state|=V3XSTATE_MATRIXUPDATE;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            pInf->fFrozen-=g_cTimer.fCounter;
                            if (pInf->fFrozen<0) 
								pInf->fFrozen=0;
                        }
                    }
                }
            }
            if ((OVI->state&V3XSTATE_HIDDEN)||(OVI->state&V3XSTATE_HIDDENDISPLAY)||(OVI==g_SGObjects.Shield))
            {
                pInf->ModeX &= ~1;
            }
            else
            {
                pInf->ModeX |= 1;
            }
        }
    }
    g_pPlayer->J.pInf.ModeX = 0;
    return;
}

