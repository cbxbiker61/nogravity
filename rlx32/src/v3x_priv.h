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

#define v3x_EvalMatrixEuler(Ob)\
{\
    V3XMatrix_Rot_XYZ((Ob)->matrix.Matrix, (Ob)->Tk.info.angle);\
    V3XVector_Cpy((Ob)->matrix.v.Pos, (V3XSCALAR)(Ob)->Tk.info.pos);\
}

#define v3x_EvalMatrixRotation2(Ob)\
{\
    V3XVECTOR Dir;\
    V3XMATRIX M0=(Ob)->matrix;\
    V3XVector_Dif(&Dir, &(Ob)->Tk.vinfo.target, &(Ob)->Tk.vinfo.pos);\
    V3XMatrix_BuildFromVector(&M0, &Dir, (int)(Ob)->Tk.vinfo.roll);\
    V3XMatrix_Transpose((Ob)->matrix.Matrix, M0.Matrix);\
    V3XVector_Cpy((Ob)->matrix.v.Pos, (V3XSCALAR) (Ob)->Tk.vinfo.pos);\
}
#define v3x_EvalMatrixRotation3(Ob)\
{\
    V3XMATRIX M0=(Ob)->matrix;\
    V3XMatrix_BuildFromNVector(&M0, &(Ob)->Tk.dinfo.vect, (int)(Ob)->Tk.dinfo.roll);\
    V3XMatrix_Transpose((Ob)->matrix.Matrix, M0.Matrix);\
    V3XVector_Cpy((Ob)->matrix.v.Pos, (V3XSCALAR) (Ob)->Tk.dinfo.pos);\
}

#define QuaternionToMatrix(qinfo, Mat)\
{\
    V3XSCALAR *m=(*Mat).Matrix;\
    V3XSCALAR\
    x2=2*(MULF32_SQR((*qinfo).x)), \
    y2=2*(MULF32_SQR((*qinfo).y)), \
    z2=2*(MULF32_SQR((*qinfo).z)), \
    xy=2*(MULF32 ((*qinfo).x, (*qinfo).y)), \
    xz=2*(MULF32 ((*qinfo).x, (*qinfo).z)), \
    xw=2*(MULF32 ((*qinfo).x, (*qinfo).w)), \
    yz=2*(MULF32 ((*qinfo).y, (*qinfo).z)), \
    yw=2*(MULF32 ((*qinfo).y, (*qinfo).w)), \
    zw=2*(MULF32 ((*qinfo).z, (*qinfo).w));\
    m[0]=CST_ONE-y2-z2;     m[1]=xy+zw       ;     m[2]=xz-yw;\
    m[3]=xy-zw;            m[4]=CST_ONE-x2-z2;     m[5]=yz+xw;\
    m[6]=xz+yw;            m[7]=yz-xw;            m[8]=CST_ONE-x2-y2;\
}

#define v3x_EvalMatrixRotation_q(Ob)\
{\
    QuaternionToMatrix(&(*Ob).Tk.qinfo, &(*Ob).matrix);\
    V3XVector_Cpy((*Ob).matrix.v.Pos, (V3XSCALAR)(*Ob).Tk.qinfo.pos);\
}

#define V3XCLIP_SIDE(n) \
{\
    V3XSCALAR d = V3XVector_DotProduct(&c, n); \
    if (d>CST_ZERO) \
    { \
        flags |= (d<radius) ? V3XCLIPPED_SIDE : V3XCLIPPED_VIEW; \
    } \
    else \
    { \
		if (d<radius) flags |= V3XCLIPPED_SIDE; \
    }\
}
