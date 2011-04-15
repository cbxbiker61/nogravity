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
//
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
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
void NG_WeaponCreate(void)
{
    int i;
    SGWeapon *W=g_SGGame.pWea;
    for (i=g_SGGame.numWeapons;i!=0;W++, i--)
    {
        W->OVI->matrix_Method = V3XMATRIX_Custom;
        W->OVI->state       |= V3XSTATE_HIDDEN;
        W->Available = 0;
    }
    return;
}

/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int NG_WeaponFire(SGActor *Jj, int type, V3XOVI *target)
*
* DESCRIPTION :
*
*/
int NG_WeaponFire(SGActor *Jj, int type, V3XOVI *target)
{
    V3XVECTOR a;
    V3XMESH *objAttacker;
    V3XOVI *OVI;
    V3XMATRIX *Mat, M0;
    int32_t i, f=0;
	float k;
    SGWeapon *W;
    objAttacker = Jj->OVI->mesh;
    if (!Jj->pInf.ShootOk)
    for (i=g_SGGame.numWeapons, W=g_SGGame.pWea;(i!=0) && (f==0);  W++,  i--)
    {
        //
        if ((!W->Available)&&(type==W->pInf.Code)&&(W->fTime==0))
        {
            OVI = W->OVI;
            W->Camera = ((W->pInf.Code==6)||(W->pInf.Code==4));
            if ((W->Camera)&&(!target))
				return 0;
            if (target)
            {
                W->TargetLock = target;
                V3XVector_Cpy(W->Target, target->Tk->vinfo.pos);
            }
            else
            {
                W->TargetLock = NULL;
            }
            if ((W->pInf.Code==10)||(W->pInf.Code==21)||(W->pInf.Code==20))
				W->Camera=2;
            W->Available = 1;
            if ((Jj->pInf.Type==t_PLAYER)&&(W->Camera==1)&&(((SGScript*)g_pLockTarget->data)->LockTime<g_SGGame.LockMAX))
            {
                W->Available=0;
                f = 2;
            }
            if (W->Available)
            {
                if ((W->Camera==1)&&(Jj->pInf.Type==t_PLAYER))
                {
                    g_SGGame.Missile = OVI;
                }
                OVI->state  |=  V3XSTATE_CULLNEVER;
                OVI->state  &= ~V3XSTATE_HIDDEN;
                OVI->collisionList = NULL;
                W->OVI->state|=V3XSTATE_MATRIXUPDATE;
                W->Attacker = Jj;
                W->Freeze =  0;
                W->pInf.Shield = W->pInf.ShieldMax;
                // Customization des tirs
                switch(W->pInf.Code) {
                    case 1:
                    W->fTime = 150;
                    Jj->pInf.ShootOk = 15;
                    break;
                    case 4:
                    case 6:
                    W->fTime = 1000;
                    Jj->pInf.ShootOk = 50;
                    break;
                    case 5:
                    W->Freeze =  1;
                    default:
                    W->fTime = 200;
                    Jj->pInf.ShootOk = 30;
                    break;
                    case 10:
                    W->fTime = 150;
                    Jj->pInf.ShootOk = 15;
                    break;
                }
                objAttacker->selfIllumine=128;
                // Matrice
                k = W->pInf.fSpeedMax;
                OVI->mesh->matrix = objAttacker->matrix;
                switch(Jj->pInf.ShootAxe&127) {
                    case 4:
                    k= -k;
                    case 0:
                    V3XMatrix_GetCol2(&a, &objAttacker->matrix);
                    break;
                    case 1:
                    V3XMatrix_GetCol1(&a, &objAttacker->matrix);
                    break;
                    case 2:
                    V3XMatrix_GetCol0(&a, &objAttacker->matrix);
                    break;
                    case 3:
                    V3XVector_Dif(&a, &Jj->OVI->ORI->mesh->matrix.v.Pos, &objAttacker->matrix.v.Pos);
                    V3XVector_Normalize(&a, &a);
                    V3XMatrix_BuildFromNVector(&M0, &a, 0);
                    Mat=&OVI->mesh->matrix;
                    V3XMatrix_Transpose(Mat->Matrix, M0.Matrix);
                    break;
                }
                g_mtxMissile = OVI->mesh->matrix;
                V3XVector_Scl(&W->vel, &a, k);
                {
                    V3XSCALAR ky= -xSHLD(Jj->OVI->ORI->global_rayon+8.f, W->Camera);
                    if (Jj!=&g_pPlayer->J) ky=-ky;
                    {
                        V3XVector_Madd(
                        &OVI->Tk->vinfo.pos,
                        &a,
                        &objAttacker->matrix.v.Pos,
                        ky);
                    }
                }
                Jj->pInf.ShootAxe|=128;
                W->TagFX = NG_Audio3DPlay(W->pInf.StartSound-1, &objAttacker->matrix.v.Pos, &W->vel);
                f = 1;
            }
        }
    }
    return f==1;
}

void NG_WeaponUpdate(void)
{
    SGWeapon *W ;
    V3XOVI *OVI, *Parent, *Shooter;
    SGActor *J, *J_Sh;
    V3XMESH *Tir;
    V3XMATRIX *Mat, M0;
    SGScript *p, *p_Sh;
    V3XVECTOR d;
    int j;
    V3XSCALAR LockDist;
    for (j=g_SGGame.numWeapons, W=g_SGGame.pWea;j!=0;W++, j--)
    {
        if (W->fTime>0)
			W->fTime-=g_cTimer.fCounter;
		else
			W->fTime=0;
        if (W->Available)
        {
            V3XVECTOR po;
            Shooter = W->Attacker->OVI;
            J_Sh  = (SGActor*)Shooter->data;
            p_Sh  = &J_Sh->pInf;
            Tir = W->OVI->mesh;
            V3XVector_Cpy(po, Tir->Tk.vinfo.pos);
            Mat = &Tir->matrix;
            if (W->fTime<=0)
            {
                W->Available = 0;
                if (W->Camera)
                {
                    NG_FXNew(&po, FX_IMPACT, 1, &W->pInf, 1, W->OVI);
                }
            }
            else
            {
                V3XSCALAR sp = g_cTimer.fCounter * 2.f;
                W->OVI->state|=V3XSTATE_MATRIXUPDATE;
                OVI = W->OVI->collisionList;
                // MisMsile Correction ou tir corrig‚
                if (W->TargetLock)
                {
                    if ((W->TargetLock->state&V3XSTATE_HIDDEN)&&(W->Camera))
						W->fTime=4;
                }
                /*
                *   MISSILE
                */
                if (W->Camera)
                {
                    V3XVector_Dif(&d, &Tir->matrix.v.Pos, &W->TargetLock->mesh->matrix.v.Pos);
                    LockDist = V3XVector_Normalize(&d, &d);
                    if (LockDist<(V3XSCALAR)64) // Tir chirugicale
                    {
                        OVI = W->TargetLock;
                        g_SGSettings.CamDisp = 0;
                        NG_HudLockTargetOff();
                    }
                    // Smoke
                    if (W->fTime>1000)
					{
						NG_FXNew(&po, FX_SMOKE2, 1, &W->pInf, 1, W->OVI);
						W->fTime = 0;
					}
                    //
                    W->vel = d;
                    switch(W->Camera)
					{
                        case 2:
                        W->Camera=0;
                        V3XVector_Neg(&W->vel);
                        case 1:
                        V3XMatrix_BuildFromVector(&M0, &W->vel, 0);
                        V3XMatrix_Transpose(Mat->Matrix, M0.Matrix);
                        // (Mat->Matrix);
                        if ((p_Sh->Type==t_PLAYER)&&(!OVI)&&(W->Camera))
                        {
                            if (g_SGGame.Missile == W->OVI)
                            {
                                V3XMatrix_Transpose(g_mtxMissile.Matrix, Mat->Matrix);
                                g_mtxMissile.v.Pos = Mat->v.Pos;
                                // (g_mtxMissile.Matrix);
                            }
                            sp*=-W->pInf.fSpeedMax;
                        }
                        else if (!W->Camera)
                        {
                            V3XVector_Scl(&W->vel, &W->vel, W->pInf.fSpeedMax);
                        }
                        break;
                    }
                }
                //
                //  DEPLACEMENT
                //
                {
                    V3XVector_Madd(&po, &W->vel, &po,  sp);
                    if ((W->TagFX>=0) && (!NG_Audio3DUpdate(&po, &W->vel, W->TagFX, W->pInf.StartSound-1)))
						W->TagFX = -1;
                    Mat->v.Pos = po; // because in ByCustom mode
                    Tir->Tk.vinfo.pos = po;
					// FIXME: Check
                    V3XMatrix_Rotate_Z_Local((int32_t)(W->fTime/(10+W->Camera)), Mat->Matrix);
                }
                //
                //   COLLISION
                //
                if (OVI)
                {
                    Parent = Shooter->parent;
                    if ((OVI!=Parent)&&(OVI!=Shooter))
                    {
                        J = (SGActor*)OVI->data;
                        W->OVI->collisionList = NULL;
                        if ((J!=W->Attacker)&&(J))
                        {
                            p = &J->pInf;
                            if (p->Type!=t_ACCESSOIRE)
                            {
                                W->Available = 0;
                                W->OVI->state |= V3XSTATE_HIDDEN;
                                if (W->Freeze)
                                {
                                    p->fFrozen=255;
                                    NG_FXNew(&po, FX_IMPACT, 1, &W->pInf, 1, W->OVI);
                                }
                                else
                                switch(p->Type) {
                                    case t_PLAYER:
                                    if (!(g_pPlayer->mode&GODMODE))
                                    {
                                        NG_FXImpact(4);
                                        p->Shield-=W->pInf.Shield;
                                        g_pPlayer->Notify+=SGNET_SHIELDCHANGE;
                                        NG_FXNew(&po, FX_IMPACT, 1, &W->pInf, 1, W->OVI);
                                        if (OVI==g_pPlayer->J.OVI)
                                        {
                                            g_SGGame.FlashColor.r=255;g_SGGame.FlashColor.g=255;g_SGGame.FlashColor.b=255;
                                            g_SGGame.FlashAlpha = 8;
                                            if (g_SGObjects.FinCode!=GAMESTATE_DEAD)
                                            {
                                                g_SGSettings.ComNumber=COM_ShipAttacked;
												g_SGSettings.ComTime=MAX_COM_DELAY;
                                            }
                                        }
                                        else
                                        {
                                            g_SGSettings.ComNumber=COM_HelpMe;
											g_SGSettings.ComTime=MAX_COM_DELAY;
                                        }
                                    }
                                    break;
                                    case t_SPECIAL:
                                    p->Shield=p->ShieldMax;
                                    break;
                                    case t_ACCESSOIRE:
                                    p->Shield=p->ShieldMax;
                                    W->Available = 1;
                                    W->OVI->state &= V3XSTATE_HIDDEN;
                                    break;
                                    case t_FRIEND:
                                    g_SGSettings.ComNumber=COM_HelpMe;g_SGSettings.ComTime=MAX_COM_DELAY;
                                    case t_ENEMY:
                                    case t_PIRAT:
                                    case t_OBSTACLE:
                                    case t_DECOR:
                                    //ne peuvent s'entretuer
                                    if (p->Shield>=0)
                                    {
                                        if (Shooter==g_pPlayer->J.OVI)
                                        {
                                            if ((p->Type==t_ENEMY)
                                            || (p->Type==t_PIRAT))
                                            g_cGameStat.hitted++;
                                            // AutoLock du vaisseau
                                            if ((g_pCurrentGame->ship==1)&&(p->ColorRadar))
                                            {
                                                if (!p->Locked)
                                                {
                                                    p->Locked=1;
                                                    NG_HudLockTargetOff();
                                                    g_pLockTarget = OVI;
                                                }
                                                if ((g_SGSettings.SerialGame)&&(p->ColorRadar==255))
                                                g_cGameStat.frag+=1;
                                            }
                                        }
                                        if (!((p_Sh->Type==t_ENEMY)&&(p->Type==t_ENEMY)))
                                        {
                                            p->Shield-=W->pInf.Shield;
                                            if (p->Shield > p->ShieldMax)
                                            {
                                                p->Shield = 0;
                                            }
                                            OVI->mesh->selfIllumine=32;
                                            if (p->Shield>=0)
                                            {
                                                NG_FXNew(&po, FX_IMPACT, 1, &W->pInf, 1, W->OVI);
                                                if ((p->Shield>0)&&(p->Shield<=(p->ShieldMax/2))) // Finish Him ?
                                                {
                                                    if ((p->FinisHim)&&(!J->pEffect))
                                                    {
                                                        J->pEffect = NG_FXNew(&OVI->mesh->matrix.v.Pos, FX_FINISH, -1, &J->pInf, 0, OVI);
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                if (p->Type==t_FRIEND) g_cGameStat.killed_amy ++;
                                                W->Attacker->pInf.Scoring+=p->Scoring;
                                                if (W->Camera)
													NG_HudLockTargetOff();
                                                if ( Shooter == g_pPlayer->J.OVI )
                                                {
                                                    if ( p->Type==t_FRIEND )
                                                    {
                                                        g_pPlayer->mode|=DOOMEDMODE;
                                                        //g_SGObjects.CallMode[t_ENEMY] = ORDER_Attack;
                                                        if (sysRand(2))
															NG_AudioSay("mes01");
                                                    }
                                                    else
                                                    if ( p->Type==t_ENEMY )
                                                    {
														char tex[256];
                                                        g_cGameStat.killed_nmy ++;
                                                        sprintf(tex, "mes0%d", sysRand(4)+5);
														if (sysRand(2)&&(p->Scoring>=100))
															NG_AudioSay(tex);
                                                    }
                                                }
                                                else
                                                {
                                                    if ( p_Sh->Type==t_FRIEND )
                                                    {
														char tex[256];
                                                        sprintf(tex, "mes0%d", sysRand(2)+6);
                                                        if (sysRand(2))
															NG_AudioSay(tex);
                                                    }
                                                    else
                                                    if ( p_Sh->Type==t_ENEMY)
                                                    {
														char tex[256];
                                                        sprintf(tex, "mes0%d", sysRand(4)+1);
                                                        if (sysRand(2))
															NG_AudioSay(tex);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    break;
                                    default:
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if ((!W->Available)&&(W->OVI==g_SGGame.Missile))
            {
                g_SGGame.Missile = NULL;
                NG_HudLockTargetOff();
            }
            V3XVector_Cpy(po, Tir->Tk.vinfo.pos);

        } else W->OVI->state |= V3XSTATE_HIDDEN;
    }
}

