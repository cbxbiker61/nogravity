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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "_rlx32.h"
#include "systools.h"
#include "sysresmx.h"
#include "sysini.h"
static ConfigItemData C_DATA;
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
int GetCF_uchar2(char *s, ConfigFile *ini, u_int8_t *val)
{
    ConfigItemData  *itemData;
    itemData = GetConfigItem(s, T_LONG, ini);
    if (itemData==NULL) return 0;
    *val = (u_int8_t)itemData->i_long;
    return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int GetCF_long2(char *s, ConfigFile *ini, int32_t *val)
*
* DESCRIPTION :  
*
*/
int GetCF_long2(char *s, ConfigFile *ini, int32_t *val)
{
    ConfigItemData  *itemData;
    itemData = GetConfigItem(s, T_LONG, ini);
    if (itemData==NULL) return 0;
    *val = itemData->i_long;
    return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int GetCF_hexa2(char *s, ConfigFile *ini, int32_t *val)
*
* DESCRIPTION :  
*
*/
int GetCF_hexa2(char *s, ConfigFile *ini, int32_t *val)
{
    ConfigItemData  *itemData;
    itemData = GetConfigItem(s, T_HEX, ini);
    if (itemData==NULL) return 0;
    *val = itemData->i_long;
    return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int GetCF_bool2(char *s, ConfigFile *ini, u_int8_t *val)
*
* DESCRIPTION :  
*
*/
int GetCF_bool2(char *s, ConfigFile *ini, u_int8_t *val)
{
    ConfigItemData  *itemData;
    itemData = GetConfigItem(s, T_BOOL, ini);
    if (itemData==NULL) return 0;
    *val = itemData->i_bool;
    return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int32_t GetCF_hexa(char *s, ConfigFile *ini)
*
* DESCRIPTION :  
*
*/
int32_t GetCF_hexa(char *s, ConfigFile *ini)
{
    ConfigItemData  *itemData;
    itemData = GetConfigItem(s, T_HEX, ini);
    if (itemData==NULL) return 0;
    return itemData->i_long;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int32_t GetCF_long(char *s, ConfigFile *ini)
*
* DESCRIPTION :  
*
*/
int32_t GetCF_long(char *s, ConfigFile *ini)
{
    ConfigItemData  *itemData;
    itemData = GetConfigItem(s, T_LONG, ini);
    if (itemData==NULL) return 0;
    return itemData->i_long;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int32_t GetCF_bool(char *s, ConfigFile *ini)
*
* DESCRIPTION :  
*
*/
int32_t GetCF_bool(char *s, ConfigFile *ini)
{
    ConfigItemData  *itemData;
    itemData = GetConfigItem(s, T_BOOL, ini);
    if (itemData==NULL) return 0;
    return itemData->i_bool;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  char *GetCF_str(char *s, ConfigFile *ini)
*
* DESCRIPTION :  
*
*/
char *GetCF_str(char *s, ConfigFile *ini)
{
    ConfigItemData  *itemData;
    itemData = GetConfigItem(s, T_STR, ini);
    if (itemData==NULL) return NULL;
    return itemData->i_str;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  char *GetCF_str2(char *s, ConfigFile *ini)
*
* DESCRIPTION :  
*
*/
char *GetCF_str2(char *s, ConfigFile *ini)
{
    ConfigItemData  *itemData;
    itemData = GetConfigItem(s, T_STR, ini);
    if (itemData==NULL) return NULL;
    return itemData->i_str;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
static void trimSpace(char *str)
{
    char    *orgStr = str;
    while( isspace(*str) ) str++;
    sysStrCpy(orgStr, str);
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void flip_string(char *str)
*
* DESCRIPTION :  
*
*/
static void flip_string(char *str)
{
    char str2[256];
    int i, l=strlen(str);
    if (l==0) return ;
    for (i=0;i<l;i++)  str2[i]=str[l-i-1];
    str2[l]=0;
    sysStrCpy(str, str2);
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :
*
* DESCRIPTION :
*
*/
int ReadConfig(char const *fileName, ConfigFile *c_file)
{
    SYS_FILEHANDLE         file;
    ConfigClass *C_class = NULL;
    ConfigItem  *C_item = NULL;
    char        str[256];
    char *str2;
    int         a;
    if( !c_file ) return -1;
    c_file->firstClass = NULL;
    c_file->currentClass = NULL;
    if(( file = FIO_cur->fopen(fileName, "rt")) == NULL) return 1;
    while( FIO_cur->fgets(str, 255, file))
    {
        if  (strstr(str, "@")!=NULL)
        {
            FIO_cur->fclose(file);
            return 0;
        }
        // tracking
        if( strlen(str) > 250 )         // is the line too int32_t?
        {
            FIO_cur->fclose(file);
            return -1;
        }
        trimSpace(str);
        flip_string(str);
        trimSpace(str);
        flip_string(str);
        if( strlen(str) < 3 ) continue;
        if( *str == ';' ) continue;     // Comment
        if( *str == '[' )               // New class
        {
            str2 = strchr(str+1, ']');
            if( !str2 ) continue;       // Valid class?
            if( C_class )               // Is it the first?
            {
                // No, create a new class and link it
                if((C_class->nextClass = (ConfigClass*)MM_heap.malloc(sizeof(ConfigClass))) == NULL)
                {
                    fclose(file);
                    return 2;
                }
                C_class = C_class->nextClass;
            } else                      // Yes, create and add to config file
            {
                if((C_class = (ConfigClass*)MM_heap.malloc(sizeof(ConfigClass))) == NULL)
                {
                    FIO_cur->fclose(file);
                    return 2;
                }
                c_file->firstClass = C_class;
            }
            C_item = NULL;
            C_class->nextClass = NULL;
            C_class->firstItem = NULL;
            a = ((str2-str-1 < 31) ? str2-str-1 : 31);
            if( a == 0 )
            {
                FIO_cur->fclose(file);
                return -1;
            }
            sysStrnCpy(C_class->name, str+1, a);
            C_class->name[a] = 0;       // Terminate string
            trimSpace(C_class->name);
            flip_string(C_class->name);
            trimSpace(C_class->name);
            flip_string(C_class->name);
        }
        else
        {
            if( C_class == NULL ) continue;
            str2 = strchr(str, '=');     // Seek '='
            if( str2 == NULL ) continue;
            if( C_item )
            {
                if((C_item->nextItem = (ConfigItem*)MM_heap.malloc(sizeof(ConfigItem))) == NULL)
                {
                    FIO_cur->fclose(file);
                    return 2;
                }
                C_item = C_item->nextItem;
            } else
            {
                if((C_item = (ConfigItem*)MM_heap.malloc(sizeof(ConfigItem))) == NULL)
                {
                    FIO_cur->fclose(file);
                    return 2;
                }
                C_class->firstItem = C_item;
            }
            C_item->nextItem = NULL;
            if((C_item->data = (char*)MM_heap.malloc(strlen(str2+1)+1)) == NULL)
            {
                FIO_cur->fclose(file);
                return 2;
            }
            sysStrCpy(C_item->data, str2+1);
            trimSpace(C_item->data);
            *str2 = 0;                  // Strip the data part
            sysStrnCpy(C_item->name, str, 31);
            C_item->name[31] = 0;
            flip_string(C_item->name);
            trimSpace(C_item->name);
            flip_string(C_item->name);
        }
    }
    FIO_cur->fclose(file);
    return 0;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  ConfigItemData *GetConfigItem(char const *itemName, enum ConfigDataType type, ConfigFile *c_file)
*
* DESCRIPTION :
*
*/
ConfigItemData *GetConfigItem(char const *itemName, enum ConfigDataType type, ConfigFile *c_file)
{
    ConfigClass *c = c_file->currentClass;
    ConfigItem  *i;
    if( c == NULL ) return NULL;    // Is there a current class selection?
    i = c->firstItem;
    while( i != NULL && sysStriCmp(itemName, i->name) ) // Search for itemName
    {
        i = i->nextItem;
    }
    if( i == NULL ) return NULL;
    switch( type )
    {
        case T_STR :
        C_DATA.i_str = i->data;
        break;
        case T_BOOL :
        C_DATA.i_bool = FALSE;
        if( atol(i->data) == 1 ) C_DATA.i_bool = TRUE;
        if( sysStriCmp(i->data, "yes") == 0 ||
        sysStriCmp(i->data, "ok") == 0 ||
        sysStriCmp(i->data, "y") == 0 ||
        sysStriCmp(i->data, "true") == 0) C_DATA.i_bool = TRUE;
        break;
        case T_LONG :
        C_DATA.i_long = strtol(i->data, NULL, 0);
        break;
        case T_HEX :
        sscanf(i->data, "%x", &C_DATA.i_long);
        break;
        default:
        return NULL;
    }
    return &C_DATA;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  int SelectConfigClass(char const *className, ConfigFile *c_file)
*
* DESCRIPTION :  
*
*/
int SelectConfigClass(char const *className, ConfigFile *c_file)
{
    ConfigClass *cPtr;
    if( !c_file ) return 0;
    if(( cPtr = GetConfigClass( className, c_file )) == NULL) return 0;
    c_file->currentClass = cPtr;
    return 1;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  ConfigClass *GetConfigClass(char const *className, ConfigFile *c_file)
*
* DESCRIPTION :
*
*/
ConfigClass *GetConfigClass(char const *className, ConfigFile *c_file)
{
    ConfigClass *c = c_file->firstClass;
    if( !c_file ) return NULL;
    while( c != NULL && sysStriCmp(className, c->name) )
    {
        c = c->nextClass;
    }
    return c;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  static void DestroyConfigItem(ConfigItem *q)
*
* DESCRIPTION :
*
*/
static void DestroyConfigItem(ConfigItem *q)
{
    ConfigItem *item, *item2;
    item = q;
    while(item!=NULL)
    {
        item2 = item->nextItem;
        if (item->data) MM_heap.free(item->data);
        MM_heap.free(item);
        item = item2;
    }
    return;
}
/*------------------------------------------------------------------------
*
* PROTOTYPE  :  void DestroyConfig(ConfigFile *q)
*
* DESCRIPTION :  
*
*/
void DestroyConfig(ConfigFile *q)
{
    ConfigClass *item, *item2;
    if( !q ) return;
    item = q->firstClass;
    while( item != NULL )
    {
        item2 = item->nextClass;
        if (item->firstItem) DestroyConfigItem(item->firstItem);
        MM_heap.free(item);
        item
 = item2;
    }
    return;
}
