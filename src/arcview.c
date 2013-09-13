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
 * Module: arcview.c      Use in: Full - [X]
 *                                Lite - [-]
 *---------------------------------------------------------------------------
 */

#ifndef _LITE_

#include "pktview.h"

/*-------------------------------------------------------------------------*/
static APIRET ArcRun            ( CHAR *run, ARC *a, MLIST *list, CHAR *toDir );
static VOID   InitArcScr        ( VOID );
static VOID   before            ( INT line );
static KEY    filter            ( KEY key );
static CHAR  *builder           ( INT line );
static ARC   *SetArc            ( FILEMENU *menu, MLIST *list );
static ARC   *RestArc           ( VOID );
static ARC   *GetArc            ( VOID );
static VOID   ShowCount         ( INT count );
static VOID   SetLastArch       ( FILEMENU *Menu, CHAR *netfile );
static INT    SortByName        ( VOID *s1, VOID *s2 );
static INT    SortByExt         ( VOID *s1, VOID *s2 );
static INT    SortByTime        ( VOID *s1, VOID *s2 );
static INT    SortBySize        ( VOID *s1, VOID *s2 );
/*-------------------------------------------------------------------------*/
static SORT sort[] =
{
    { 0, "Name",      SortByName },
    { 0, "Extension", SortByExt  },
    { 0, "Time",      SortByTime },
    { 0, "Size",      SortBySize },
    { 0, "Unsorted",  NULL       },
    { 0,  NULL,       NULL       },
};
/*-------------------------------------------------------------------------*/
static INT   fileNumber;
static BOOL  init = 0;
static SHORT maxX;
static ARC  *Arc;
/*-------------------------------------------------------------------------*/
static ARC *SetArc( FILEMENU *menu, MLIST *list )
{
    ARC     *a;

    AddNextStruct( Arc, a );

    a -> menu  = menu;
    a -> list  = list;

    return( a );
}
/*-------------------------------------------------------------------------*/
static ARC *RestArc( VOID )
{
    ARC     *a, *a1;

    if( !Arc )
        return( NULL );

    for( a = Arc, a1 = NULL; a -> next; a1 = a, a = a -> next );

    free( a );

    if( !a1 )
        Arc = NULL;
    else
        a1 -> next = NULL;

    return( a1 );
}
/*-------------------------------------------------------------------------*/
static ARC *GetArc( VOID )
{
    ARC     *a;

    for( a = Arc; a && a -> next; a = a -> next );

    return( a );
}
/*-------------------------------------------------------------------------*/
VOID ShowCount( SHORT count )
{
    WSetXY ( wscan, 19, 2 );
    WPrintf( wscan, "%u    ", count );
}
/*-------------------------------------------------------------------------*/
APIRET PktReadArj( FILEMENU *Menu, CHAR *netfile )
{
    APIRET       rc = NO_ERROR;
    MLIST       *l;

    FreeMenu( Menu );

    Menu -> TitlePath = "ARJ: ";
    Menu -> TitleList = comment;
    Menu -> info      = ARC_ARJ;
    Menu -> Count     = 0;
    Menu -> list      = NULL;
    Menu -> truePath  = strdup( netfile );
    Menu -> maxX      = maxXtree;
    Menu -> maxY      = maxYtree;

    SetLastArch( Menu, netfile );

    WActive( wscan );
    rc = FReadArj( &Menu -> list, netfile, format, ShowCount );
    WDeactive( wscan );

    if( rc )
        ShowError( "Error read file '%s'", ShowPath( netfile, 50 ));
    else
        for( l = Menu -> list; l; l = l -> next, Menu -> Count++ );
    
    return( rc );
}
/*-------------------------------------------------------------------------*/
APIRET PktReadRar( FILEMENU *Menu, CHAR *netfile )
{
    APIRET       rc = NO_ERROR;
    MLIST        *l;

    FreeMenu( Menu );

    Menu -> TitlePath = "RAR: ";
    Menu -> TitleList = comment;
    Menu -> info      = ARC_RAR;
    Menu -> Count     = 0;
    Menu -> list      = NULL;
    Menu -> truePath  = strdup( netfile );
    Menu -> maxX      = maxXtree;
    Menu -> maxY      = maxYtree;

    SetLastArch( Menu, netfile );

    WActive( wscan );
    rc = FReadRar( &Menu -> list, netfile, format, ShowCount );
    WDeactive( wscan );
    
    if( rc )
        ShowError( "Error read file '%s'", ShowPath( netfile, 50 ));
    else
        for( l = Menu -> list; l; l = l -> next, Menu -> Count++ );

    return( rc );
}
/*-------------------------------------------------------------------------*/
APIRET PktReadLha( FILEMENU *Menu, CHAR *netfile )
{
    APIRET       rc = NO_ERROR;
    MLIST        *l;
    
    FreeMenu( Menu );

    Menu -> TitlePath = "LZH: ";
    Menu -> TitleList = comment;
    Menu -> info      = ARC_LZH;
    Menu -> Count     = 0;
    Menu -> list      = NULL;
    Menu -> truePath  = strdup( netfile );
    Menu -> maxX      = maxXtree;
    Menu -> maxY      = maxYtree;

    SetLastArch( Menu, netfile );

    WActive( wscan );
    rc = FReadLha( &Menu -> list, netfile, format, ShowCount );
    WDeactive( wscan );
    
    if( rc )
        ShowError( "Error read file '%s'", ShowPath( netfile, 50 ));
    else
        for( l = Menu -> list; l; l = l -> next, Menu -> Count++ );
    
    return( rc );
}
/*-------------------------------------------------------------------------*/
APIRET PktReadZip( FILEMENU *Menu, CHAR *netfile )
{
    APIRET       rc = NO_ERROR;
    MLIST       *l;
    
    FreeMenu( Menu );

    Menu -> TitlePath = "ZIP: ";
    Menu -> TitleList = comment;
    Menu -> info      = ARC_ZIP;
    Menu -> Count     = 0;
    Menu -> list      = NULL;
    Menu -> truePath  = strdup( netfile );
    Menu -> maxX      = maxXtree;
    Menu -> maxY      = maxYtree;

    SetLastArch( Menu, netfile );

    WActive( wscan );
    rc = FReadZip( &Menu -> list, netfile, format, ShowCount );
    WDeactive( wscan );
    
    if( rc )
        ShowError( "Error read file '%s'", ShowPath( netfile, 50 ));
    else
        for( l = Menu -> list; l; l = l -> next, Menu -> Count++ );
    
    return( rc );
}
/*-------------------------------------------------------------------------*/
static VOID SetLastArch( FILEMENU *Menu, CHAR *netfile )
{
    CHAR        path[_MAX_PATH];
    
    if( strlen( LastArch ))
    {
        _splitpath( netfile, NULL, NULL, fname, ext );
        strcpy( path, LastArch );
        strcat( path, fname );
        strcat( path, ext );
        Menu -> Path = strdup( path );
    }
    else
        Menu -> Path = strdup( netfile );
}
/*-------------------------------------------------------------------------*/
APIRET ArcSelect( FILEMENU *menu )
{
    INT          i;
    PMLIST       l;
    ARC         *a;

    if( !menu -> Count )
    {
        ShowError( "Archive '%s' is empty.", ShowPath( menu -> truePath, 50 ));
        return( NO_ERROR );
    }

	if( !init )
	{
		sort[cfgCurrent.asort].type = 1;
		init = 1;
	}

	menu -> sort = &sort;

	if( cfgCurrent.sort )
	{
		menu -> origin = malloc( menu -> Count * sizeof( *menu -> origin ));
		if( menu -> origin )
			for( i = 0, l = menu -> list; l; l = l -> next, i++ )
				menu -> origin[i] = l;
		SortList( menu );
	}
	
    menu -> popup = WPDefine( wtree, menu -> list, 1 );
    WPSetFunc( menu -> popup, before, NULL, filter, builder );

    SetArc( menu, menu -> list );

    InitArcScr();

    WPActive( menu -> popup );
    WPRelease( menu -> popup, FALSE );

    a = RestArc();

    InitArcScr();

    if( a && a -> menu -> popup )
        WPShow( popup, FALSE, TRUE, -1, -1 );

    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
VOID InitArcScr( VOID )
{
    INT          len;
    ARC         *a;

    if(( a = GetArc()) == NULL )
        return;

    SetBar( BarLoad );

    WActive( wtinfo );
    WActive( wttitl );
    WActive( wtree  );
    WActive( wtname );
    WActive( wbar   );

    WSetCurColor( wttitl, C_TEXT ); WSetXY( wttitl, 0, 0 );
    WPrintf( wttitl, "%-*.*s", a -> menu -> maxX, a -> menu -> maxX, a -> menu -> TitleList );

    maxX = a -> menu -> maxX;
    len  = a -> menu -> maxX - strlen( a -> menu -> TitlePath ) - 2;

    WSetXY  ( wtinfo, 0, 0 );
    WPrintf ( wtinfo, "%s%-*.*s", a -> menu -> TitlePath,
                                len, len, ShowPath( a -> menu -> Path, len ));
}
/*-------------------------------------------------------------------------*/
static VOID before( INT line )
{
    ARC         *a;
    CHAR        *str;
    
    fileNumber = line;
    
    a = GetArc();
    a -> list = GetCurrentList( a -> menu -> list, line );
    str = a -> list -> str;
    
    WSetXY  ( wtname, 0, 0 );
    WPrintf ( wtname, "%-*.*s", maxX - 2, maxX - 2, str ? str : "" );
}
/*-------------------------------------------------------------------------*/
static CHAR *builder( INT line )
{
    return( GetCurrentList( GetArc() -> menu -> list, line ) -> ext );
}
/*-------------------------------------------------------------------------*/
static KEY filter( KEY k )
{
    APIRET       rc = NO_ERROR;
    INT          i, len, code;
    CHAR         name [_MAX_PATH],  buf[64];
    static CHAR  fname[_MAX_FNAME], ext[_MAX_EXT], toDir[_MAX_PATH];
    MLIST       *l;
    ARC         *a;

    a = GetArc();

    switch( k.key )
    {
        case KEY_F1:
            ViewHelp( "ArcView" );
            SetBar( BarLoad );
            break;
            
        case KEY_SPACE: case KEY_INS:
            if( a -> list -> sel )
                a -> list -> sel = 0;
            else
                a -> list -> sel = 2;
            WPShow( a -> menu -> popup, FALSE, TRUE, -1, -1 );
            k.key = KEY_DOWN;
            break;
            
        case KEY_F10:
            k.key = KEY_ESC;
            break;

        case KEY_s: case KEY_S: case ALT_S:
            if( cfgCurrent.sort && a -> menu -> Count  )
            {
                SortMenu( a -> menu );
                SetBar( BarLoad );
                before( fileNumber );
            }
            k.key = 0;
            break;

        case ALT_X:
            altx = 1;
            k.key = KEY_ESC;
            break;
            
        case KEY_F4: case ALT_F4:
            getcwd( toDir, _MAX_PATH - 1 );

            _splitpath( a -> list -> str, NULL, NULL, fname, ext );
            strcpy( name, "Extract " ); strcat( name, fname );
            strcat( name, ext ); strcat( name, " to" );

            rc = EnterPath( " Unpack ", name, toDir );
            SetBar( BarLoad );
            StrDelSlash( toDir );
            if( rc )
                break;
            for( l = a -> menu -> list, i = 0; l; l = l -> next )
                if( l -> sel ) i++;

            ShowWaiting( 1 );
            
            if( i )
            {
                for( l = a -> menu -> list; l; l = l -> next )
                {
                    if( l -> sel == 0 )
                        continue;
                    rc = ArcRun( cfgCurrent.os[os].arch[a->menu->info].act[ACT_EXTR], a, l, toDir );
                    l -> sel = 0;
                    WPShow( a -> menu -> popup, FALSE, TRUE, -1, -1 );
                }
            }
            else
                rc = ArcRun( cfgCurrent.os[os].arch[a->menu->info].act[ACT_EXTR], a, a -> list, toDir );

            ShowWaiting( 0 );
            CheckReview( a -> menu -> truePath );
            
            k.key = 0;
            break;
            
        case KEY_F9:
            ChangeConfig();
            SetBar( BarLoad );
            if( altx )
                k.key = KEY_ESC;
            else
                k.key = 0;
            break;
            
        case SHIFT_F9:
            SaveConfig( ConfFile );
            k.key = 0;
            break;
            
        case KEY_ENTER: case KEY_F3:

            ShowWaiting( 1 );
            if( cfgCurrent.intUnZip && a -> menu -> info == ARC_ZIP )
            {
                rc = FUnZip( a -> menu -> truePath, a -> list -> str, cfgCurrent.os[os].temp );
                if( rc == ERROR_INVALID_FUNCTION )
                {
                    rc = ArcRun( cfgCurrent.os[os].arch[a->menu->info].act[ACT_VIEW], a, a -> list, cfgCurrent.os[os].temp );
                }
            }
            else
                rc = ArcRun( cfgCurrent.os[os].arch[a->menu->info].act[ACT_VIEW], a, a -> list, cfgCurrent.os[os].temp );
            ShowWaiting( 0 );
            
            _splitpath( a -> list -> str, NULL, NULL, fname, ext );
            strcpy( name, cfgCurrent.os[os].temp ); StrAddSlash( name );
            strcat( name, fname ); strcat( name, ext );

            if( rc )
            {
                unlink( name );
                k.key = 0;
                break;
            }
            
            len = strlen( LastArch );

            if( !len )
            {
                strcat( LastArch, a -> menu -> Path );
                strcat( LastArch, "\x5c" );
            }
            else
            {
                _splitpath( a -> menu -> truePath, NULL, NULL, fname, ext );
                strcat( LastArch, fname ); strcat( LastArch, ext );
                strcat( LastArch, "\x5c" );
            }

            rc = ViewFile( name );
            if( rc == ERROR_NOT_ENOUGH_MEMORY )
            {
                CloseFile();
                InitMem();
                ShowError( "Not enough memory." );
                k.key = 0;
                break;
            }

            LastArch[ len ] = 0;

            if( PktUpdate || Update || UpdateTxt )
                ShowError( "File '%s' deleting!\nChanging is lost!", name );
            chmod( name, S_IWRITE ); unlink( name );
            InitArcScr();
            if( altx )
                k.key = KEY_ESC;
            else
                k.key = 0;
            break;

        case KEY_DEL: case KEY_F8:
            for( l = a -> menu -> list, i = 0; l; l = l -> next )
                if( l -> sel ) i++;
            if( i <= 1 )
                sprintf( buf, "Selected 1 file." );
            else
                sprintf( buf, "Selected %u files.", i );
            
            code = GetOk( buf, "Delete?" );
            SetBar( BarLoad );
            if( code )
            {
                k.key = 0;
                break;
            }
            
            ShowWaiting( 1 );
            
            if( i )
            {
                for( l = a -> menu -> list; l; l = l -> next )
                {
                    if( l -> sel == 0 )
                        continue;
                    rc = ArcRun( cfgCurrent.os[os].arch[a->menu->info].act[ACT_DEL], a, l, toDir );
                    l -> sel = 0;
                    WPShow( a -> menu -> popup, FALSE, TRUE, -1, -1 );
                }
            }
            else
                rc = ArcRun( cfgCurrent.os[os].arch[a->menu->info].act[ACT_DEL], a, a -> list, toDir );

            ShowWaiting( 0 );
            CheckReview( a -> menu -> truePath );

            arcreload = 1;
            k.key = KEY_ESC;
            break;
            
    }
    return( k );
}
/*-------------------------------------------------------------------------*/
VOID FreeMenu( FILEMENU *menu )
{
    MLIST       *l;
    
    menu -> popup = 0;
    for( l = menu -> list; l; l = l -> next )
        if( l -> ext ) free( l -> ext );
    if( menu -> Path     ){ free( menu -> Path );    menu -> Path     = NULL;}
    if( menu -> truePath ){ free( menu -> truePath );menu -> truePath = NULL;}
    if( menu -> origin   ){ free( menu -> origin );  menu -> origin   = NULL;}
    MListFree( &menu -> list );
}
/*-------------------------------------------------------------------------*/
APIRET ArcRun( CHAR *run, ARC *a, MLIST *list, CHAR *toDir )
{
    APIRET       rc;
    CHAR        *command;
    INT          old;

    command = CreateCommand( run, a -> menu -> truePath, list -> str, toDir );

    old = strlen( command );
    strcat( command, " >nul" );
#ifdef __OS2__
    strcat( command, " 2>nul" );
#endif
    _heapmin();
    rc = system( command );
    
    if( rc )
    {
        command[ old ] = 0;
        ShowError( "Unpacker return code: %ld\nCommand: '%s'", (LONG)rc, command );
    }
    free( command );

    return( rc );
}
/*-------------------------------------------------------------------------*/
static INT SortByName( VOID *s1, VOID *s2 )
{
    MLIST       **t1 = s1, **t2 = s2;
    
    return( stricmp((*t1) -> str, (*t2) -> str ));
}
/*-------------------------------------------------------------------------*/
static INT SortByExt( VOID *s1, VOID *s2 )
{
    INT          code;
    MLIST       **t1 = s1, **t2 = s2;
    CHAR         ext1[_MAX_EXT], ext2[_MAX_EXT];
    
    _splitpath((*t1) -> str, NULL, NULL, NULL, ext1 );
    _splitpath((*t2) -> str, NULL, NULL, NULL, ext2 );
    
    if(( code = stricmp( ext1, ext2 )) != 0 )
        return( code );
    return( stricmp((*t1) -> str, (*t2) -> str ));
}
/*-------------------------------------------------------------------------*/
static INT SortByTime( VOID *s1, VOID *s2 )
{
    MLIST       **t1 = s1, **t2 = s2;
    LONG         date1, date2;
    
    date1 = DataStr2Long( &((CHAR*)(*t1) -> ext)[30] );
    date2 = DataStr2Long( &((CHAR*)(*t2) -> ext)[30] );
    
    if( date1 > date2 )
        return( 1 );
    if( date1 < date2 )
        return( -1 );
    return( stricmp((*t1) -> str, (*t2) -> str ));
}
/*-------------------------------------------------------------------------*/
static INT SortBySize( VOID *s1, VOID *s2 )
{
    INT          code;
    MLIST       **t1 = s1, **t2 = s2;
    
    if(( code = strnicmp( (CHAR*)(*t1) -> ext, (CHAR*)(*t2) -> ext, 11 )) != 0 )
        return( code );
    return( stricmp((*t1) -> str, (*t2) -> str ));
}
/*-------------------------------------------------------------------------*/

#endif
