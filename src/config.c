/*---------------------------------------------------------------------------
 *             PKTView/2 - FTN PKT/MSG viewer for OS/2 & DOS
 *                    [ FTS-0001, FSC-0039, FSC-0048 ]
 * 
 *  Copyright (c) 1995-1997 Moonlit Software/2 by Oleg Milaenko AKA Oleg/2
 *                  2:468/2@fidonet AKA 2:468/10@fidonet
 *
 * You may freely use this product for any non-commercial purpose.
 *   If you want to use source code for your own programs, please notice
 *   product copyright, as shown above.
 *
 * Disclaimer of warranty.
 *   This software are distributed "as is" and without warranties expressed
 *   or implied. Because of the various hardware and software environments
 *   into which this program may be put, no warranty of fitness for a
 *   particular purpose is offered.
 *
 * This program was originally compiled with Watcom C/C++ v10.0b
 *
 * Module: config.c       Use in: Full - [X]
 *                                Lite - [X]
 *---------------------------------------------------------------------------
 */

#include "pktview.h"

#define CFG_VER     2

/*-------------------------------------------------------------------------*/
static VOID GetParam     ( VOID );
static VOID SetParam     ( VOID );
static VOID confBefore   ( FIELD f );
static KEY  confFilter   ( FIELD f, KEY key, BOOL *redisp );
/*-------------------------------------------------------------------------*/
static CHAR       *signature = "PKTView/2 Config";
static CFGHEADER   cfgHeader;
#ifndef _LITE_
  static PKTMISC   cfgTemp;
  static WINDOW    wconf;
  static FORM      form;
  static INT       renew, init;
  static SHORT     typeOS[2] = { 1, 0 }, arch[4] = { 1, 0, 0, 0 };
  static CHAR      parametrs[8][_MAX_PATH];
  static CHAR      temp[_MAX_PATH], view[_MAX_PATH];
  static INT       curOS = 0, curArch = ARC_ZIP;
  static CHAR     *sPSort[] = { "Unsorted", "From", "To", "Area", "Subj", "Time", "Size", NULL };
  static CHAR     *sFSort[] = { "Name", "Extension", "Time", "Size", "Unsorted", NULL };
  static CHAR      fbuf[16], abuf[16], pbuf[16];
#endif
/*-------------------------------------------------------------------------*/
#ifndef _LITE_
APIRET ChangeConfig( VOID )
{
    APIRET       rc;
	SHORT        maxX;
	FIELD        s;
    
    if( !init )
    {
        init = 1;
        curOS = os;
        if( os )
        {
            typeOS[0] = 0; typeOS[1] = 1;
        }
    }
    
    SetDefault( &cfgTemp, &cfgCurrent );

	strcpy( fbuf, sFSort[cfgTemp.fsort] );
	strcpy( abuf, sFSort[cfgTemp.asort] );
	strcpy( pbuf, sPSort[cfgTemp.psort] );

	GetParam();

    wconf = WDefine( x/2-33, y/2-9, x/2+32, y/2+9, 17, W_SHADOW | W_NOCURSOR, BORDER4, " Setup " );

    WGetSize( wconf, &maxX, NULL );
    WHLine( wconf, 0, 4, maxX, 0 );

    form = WFormDefine( wconf );

    WFormRadio ( form,  1, 1, "OS/2", &typeOS[0], NOCONTROL, 0 );
    WFormRadio ( form,  1, 2, "DOS ", &typeOS[1], NOCONTROL, 0 );

    WFormString( form, 13, 1, "Temp    : ", temp, NULL, _MAX_PATH - 1, 40, NULL, NOCONTROL, 0 );
    WFormString( form, 13, 2, "Viewer  : ", view, NULL, _MAX_PATH - 1, 40, NULL, NOCONTROL, 0 );
    
    WFormRadio ( form,  1,  6, "Zip © ", &arch[ARC_ZIP], NOCONTROL, 0 );
    WFormRadio ( form,  1,  7, "Arj ед", &arch[ARC_ARJ], NOCONTROL, 0 );
    WFormRadio ( form,  1,  8, "Lha ╢ ", &arch[ARC_LZH], NOCONTROL, 0 );
    WFormRadio ( form,  1,  9, "Rar ы ", &arch[ARC_RAR], NOCONTROL, 0 );

    WFormString( form, 10,  6, " з Unpack  : ", parametrs[ACT_VIEW], NULL, _MAX_PATH - 1, 40, NULL, NOCONTROL, 0 );
    WFormString( form, 10,  7, "де Extract : ", parametrs[ACT_EXTR], NULL, _MAX_PATH - 1, 40, NULL, NOCONTROL, 0 );
    WFormString( form, 10,  8, " ю Delete  : ", parametrs[ACT_DEL],  NULL, _MAX_PATH - 1, 40, NULL, NOCONTROL, 0 );

	s = WFormCheck ( form,  1, 12, "Sort", &cfgTemp.sort, NOCONTROL, 0 );

	WFormString( form, 10, 11, " з Files   : ", fbuf, sFSort, 15, 9, NULL, s, 0 );
	WFormString( form, 10, 12, "де Archives: ", abuf, sFSort, 15, 9, NULL, s, 0 );
	WFormString( form, 10, 13, " ю Messages: ", pbuf, sPSort, 15, 9, NULL, s, 0 );

	WFormCheck ( form,  1, 15, "Int. UnZip", &cfgTemp.intUnZip, NOCONTROL, 0 );

	WFormSetFunc( form, confBefore, NULL, confFilter );
    
    SetBar( BarSet );
    WActive( wconf );
    rc = WFormGet( form );
    WFormRelease( form, 0 );
    WRelease( wconf );

    if( rc == 0 )
    {
        SetParam();
        SetDefault( &cfgCurrent, &cfgTemp );
    }

    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
#pragma off (unreferenced);
static VOID confBefore( FIELD f )
#pragma on (unreferenced);
{
    SetParam();
    
    if( typeOS[0] ) curOS = 0;
    if( typeOS[1] ) curOS = 1;

    if( arch[ ARC_ZIP ] ) curArch = ARC_ZIP;
    if( arch[ ARC_ARJ ] ) curArch = ARC_ARJ;
    if( arch[ ARC_LZH ] ) curArch = ARC_LZH;
    if( arch[ ARC_RAR ] ) curArch = ARC_RAR;

    GetParam();
}
/*-------------------------------------------------------------------------*/
#pragma off (unreferenced);
static KEY confFilter( FIELD f, KEY k, BOOL *redisp )
#pragma on (unreferenced);
{
    switch( k.key )
    {
        case KEY_F1:
            ViewHelp( "Setup" );
            SetBar( BarSet );
            k.key = 0;
            break;

        case SHIFT_F9:
            SetParam();
            SetDefault( &cfgCurrent, &cfgTemp );
            SaveConfig( ConfFile );
            k.key = KEY_ENTER;
            break;

        case ALT_X:
            altx = 1;
            
        case KEY_F10:
            k.key = KEY_ENTER;
            break;
    }
    return( k );
}
/*-------------------------------------------------------------------------*/
static VOID GetParam( VOID )
{
    INT          i;
    
    strcpy( temp, cfgTemp.os[curOS].temp );
    strcpy( view, cfgTemp.os[curOS].viewer );
    
    for( i = 0; i < 8; i++ )
        strcpy( parametrs[i], cfgTemp.os[curOS].arch[curArch].act[i] );
    WHideCursor( wconf );
    WFormShow( form );
    renew = 0;
}
/*-------------------------------------------------------------------------*/
static VOID SetParam( VOID )
{
    INT          i;
    
    if( strcmp( temp, cfgTemp.os[curOS].temp ))
    {
        free( cfgTemp.os[curOS].temp );
        cfgTemp.os[curOS].temp = strdup( temp );
    }
    if( strcmp( view, cfgTemp.os[curOS].viewer ))
    {
        free( cfgTemp.os[curOS].viewer );
        cfgTemp.os[curOS].viewer = strdup( view );
    }
    
    for( i = 0; i < 8; i++ )
    {
        if( strcmp( parametrs[i], cfgTemp.os[curOS].arch[curArch].act[i] ))
        {
            free( cfgTemp.os[curOS].arch[curArch].act[i] );
                cfgTemp.os[curOS].arch[curArch].act[i] = strdup( parametrs[i] );
        }
    }
	for( i = cfgTemp.fsort = 0; sFSort[i]; i++ )
	{
		if( stricmp( fbuf, sFSort[i] ))
			continue;
		cfgTemp.fsort = i;
		break;
	}
	for( i = cfgTemp.asort = 0; sFSort[i]; i++ )
	{
		if( stricmp( abuf, sFSort[i] ))
			continue;
		cfgTemp.asort = i;
		break;
	}
	for( i = cfgTemp.psort = 0; sPSort[i]; i++ )
	{
		if( stricmp( pbuf, sPSort[i] ))
			continue;
		cfgTemp.psort = i;
		break;
	}
}
/*-------------------------------------------------------------------------*/
APIRET CreateConfig( CHAR *name )
{
    SetDefault( &cfgCurrent, &cfgDefault );
    return( SaveConfig( name ));
}
/*-------------------------------------------------------------------------*/
APIRET SaveConfig( CHAR *name )
{
    INT          i, j, k;
    LONG         offset;
    FILE        *file;
    
    memcpy( cfgHeader.signature, signature, sizeof( cfgHeader.signature ));
    cfgHeader.ver  = CFG_VER;
    cfgHeader.crc  = 0;
    
    offset = 0;
    
    for( i = 0; i < 2; i++ )                    // OS/2, DOS
    {
        cfgHeader.os[i].temp = offset;
        offset += strlen( cfgCurrent.os[i].temp ) + 1;
        cfgHeader.os[i].viewer = offset;
        offset += strlen( cfgCurrent.os[i].viewer ) + 1;
        
        for( j = 0; j < 4; j++ )                // ZIP, ARJ, LHZ, RAR
        {
            for( k = 0; k < 8; k++ )            // VIEW, EXTR, DEL, ADD, ...
            {
                cfgHeader.os[i].arch[j].act[k] = offset;
                offset += strlen( cfgCurrent.os[i].arch[j].act[k] ) + 1;
            }
        }
    }
    
    cfgHeader.size = offset;
    
    cfgHeader.intUnZip = cfgCurrent.intUnZip;
	cfgHeader.sort     = cfgCurrent.sort;
	cfgHeader.fsort    = cfgCurrent.fsort;
	cfgHeader.asort    = cfgCurrent.asort;
	cfgHeader.psort    = cfgCurrent.psort;

	memcpy( cfgHeader.color, cfgCurrent.color, sizeof( cfgHeader.color ));
    
    if(( file = _fsopen( name, "wb", SH_DENYWR )) == NULL )
    {
        printf( "Ч Error create config file %s\n", name );
        return( ERROR_OPEN_FAILED );
    }
    
    fwrite( &cfgHeader, sizeof( cfgHeader ), 1, file );
    
    for( i = 0; i < 2; i++ )
    {
        fwrite( cfgCurrent.os[i].temp,   strlen( cfgCurrent.os[i].temp )   + 1, 1, file );
        fwrite( cfgCurrent.os[i].viewer, strlen( cfgCurrent.os[i].viewer ) + 1, 1, file );
        
        for( j = 0; j < 4; j++ )
            for( k = 0; k < 8; k++ )
                fwrite( cfgCurrent.os[i].arch[j].act[k],
                       strlen( cfgCurrent.os[i].arch[j].act[k] ) + 1, 1, file );
    }
    
    fclose( file );
    
    return( NO_ERROR );
}
#endif
/*-------------------------------------------------------------------------*/
VOID SetDefault( PKTMISC *dest, PKTMISC *sour )
{
#ifndef _LITE_

    INT          i, j, k;
    
    for( i = 0; i < 2; i++ )                    // OS/2, DOS
    {
        if( dest -> os[i].temp   ) free( dest -> os[i].temp   );
        if( dest -> os[i].viewer ) free( dest -> os[i].viewer );
        
        dest -> os[i].temp   = strdup( sour -> os[i].temp   );
        dest -> os[i].viewer = strdup( sour -> os[i].viewer );
        
        for( j = 0; j < 4; j++ )                // ZIP, ARJ, LHZ, RAR
        {
            for( k = 0; k < 8; k++ )            // VIEW, EXTR, DEL, ADD, ...
            {
                if( dest -> os[i].arch[j].act[k] ) free( dest -> os[i].arch[j].act[k] );
                dest -> os[i].arch[j].act[k] = strdup( sour -> os[i].arch[j].act[k] );
            }
        }
    }
#endif
    dest -> intUnZip = sour -> intUnZip;
	dest -> sort     = sour -> sort;
	dest -> fsort    = sour -> fsort;
	dest -> asort    = sour -> asort;
	dest -> psort    = sour -> psort;
    memcpy( dest -> color, sour -> color, sizeof( dest -> color ));
}
/*-------------------------------------------------------------------------*/
APIRET LoadConfig( CHAR *name )
{
#ifndef _LITE_
    INT          i, j, k;
    CHAR        *buf;
#endif
    FILE        *file;
    
    if(( file = _fsopen( name, "rb", SH_DENYWR )) == NULL )
    {
        printf( "Ч Error open config file %s\n", name );
        return( ERROR_OPEN_FAILED );
    }
    
    if( fread( &cfgHeader, sizeof(cfgHeader), 1, file ) != 1 )
    {
        printf( "Ч Error read config file %s\n", name );
        return( ERROR_READ_FAULT );
    }
    
    if( cfgHeader.ver != CFG_VER || memcmp( cfgHeader.signature, signature, sizeof( cfgHeader.signature )))
    {
        printf( "Ч Error version config file %s\n", name );
        return( ERROR_BAD_FORMAT );
    }
    
#ifndef _LITE_

    buf = malloc( cfgHeader.size );
    
    if( fread( buf, cfgHeader.size, 1, file ) != 1 )
    {
        printf( "Ч Error read config file %s\n", name );
        return( ERROR_READ_FAULT );
    }
    
    fclose( file );
    
    for( i = 0; i < 2; i++ )                    // OS/2, DOS
    {
        if( strlen( &buf[ cfgHeader.os[i].temp ] ))
        {
            if( cfgCurrent.os[i].temp ) free( cfgCurrent.os[i].temp );
            cfgCurrent.os[i].temp = strdup( &buf[ cfgHeader.os[i].temp ] );
        }
        
        if( strlen( &buf[ cfgHeader.os[i].viewer ] ))
        {
            if( cfgCurrent.os[i].viewer ) free( cfgCurrent.os[i].viewer );
            cfgCurrent.os[i].viewer = strdup( &buf[ cfgHeader.os[i].viewer ] );
        }
        
        for( j = 0; j < 4; j++ )                // ZIP, ARJ, LHZ, RAR
        {
            for( k = 0; k < 8; k++ )            // VIEW, EXTR, DEL, ADD, ...
            {
                if( strlen( &buf[ cfgHeader.os[i].arch[j].act[k] ]))
                {
                    if( cfgCurrent.os[i].arch[j].act[k] )
                        free( cfgCurrent.os[i].arch[j].act[k] );
                    cfgCurrent.os[i].arch[j].act[k] = strdup( &buf[ cfgHeader.os[i].arch[j].act[k] ]);
                }
            }
        }
    }
    free( buf );
    
#else
    fclose( file );
#endif
    
    cfgCurrent.intUnZip = cfgHeader.intUnZip;
	cfgCurrent.sort     = cfgHeader.sort;
	cfgCurrent.fsort    = cfgHeader.fsort;
	cfgCurrent.asort    = cfgHeader.asort;
	cfgCurrent.psort    = cfgHeader.psort;
    memcpy( cfgCurrent.color, cfgHeader.color, sizeof( cfgCurrent.color ));
    
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
