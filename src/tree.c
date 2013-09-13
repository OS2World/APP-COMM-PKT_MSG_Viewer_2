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
 * Module: tree.c         Use in: Full - [X]
 *                                Lite - [-]
 *---------------------------------------------------------------------------
 */

#ifndef _LITE_

#include "pktview.h"

#define TREE_FILE       0
#define TREE_DIR        1
#define TREE_FILE_SEL   2
#define TREE_DIR_SEL    3

/*-------------------------------------------------------------------------*/
static INT    SortByName  ( VOID *s1, VOID *s2 );
static INT    SortByExt   ( VOID *s1, VOID *s2 );
static INT    SortByTime  ( VOID *s1, VOID *s2 );
static INT    SortBySize  ( VOID *s1, VOID *s2 );
static INT    CheckDIR    ( MLIST *l1, MLIST *l2 );
static VOID   before      ( INT line );
static KEY    filter      ( KEY key );
/*-------------------------------------------------------------------------*/
static INT       fileNumber;
static BOOL      reread, init = 0;
static FILEMENU *fmenu;
static MLIST    *list, *curList;
static INT       SubDir;
static CHAR      File[_MAX_PATH];
static struct DIRINFO
{
    SHORT start, line;
} Dir[_MAX_DIR/2];

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
APIRET ReadDir( FILEMENU *TMenu, CHAR netfile[] )
{
    INT              i, code, done;
    SHORT            maxX;
    INT              attr = _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_SUBDIR | _A_ARCH;
    CHAR             a[24], s[24];
    MLIST           *l;
    struct find_t    find;
    static CHAR      str[256];

    if( !init )
    {
        sort[cfgCurrent.fsort].type = 1;
        init = 1;
    }

    WActive( wtinfo );
    WActive( wttitl );
    WActive( wtree  );
    WActive( wtname );
    WActive( wbar   );
    SetBar ( BarDir );

    WGetSize( wtinfo, &maxX, NULL );
    WPrints( wttitl, 0, 0, " Name                                    Size     Date      Time    R/O Sys Hid" );
    fmenu = TMenu;

    while( 1 )
    {
        if( review && TMenu -> popup )
        {
            WPRelease( TMenu -> popup, FALSE );
            TMenu -> popup = 0;
            FreeMenu( TMenu );
        }

        getcwd ( File, _MAX_PATH - 1 );
        CurDir = TMenu -> Path = strdup( File );
        WSetXY ( wtinfo, 0, 0 ); WClearLine( wtinfo );
        WPrints( wtinfo, 0, 0, ShowPath( TMenu -> Path, maxX - 1 ));
        
        if( TMenu -> popup == 0 )
        {
            TMenu -> Count = 0;

            strcpy( File, TMenu -> Path ); StrAddSlash( File ); strcat( File, "*.*" );

            for( done=_dos_findfirst(File,attr,&find); !done; done=_dos_findnext(&find))
            {
                if( find.attrib & _A_SUBDIR )
                {
                    if( find.name[0] == '.' && find.name[1] == 0 )
                        continue;

                    if( find.name[0] == '.' && find.name[1] == '.' &&
                       TMenu -> Path[ strlen(TMenu -> Path)-1] == '\\' )
                        continue;
                }

                strcpy( a, find.attrib & _A_RDONLY ? "R/O " : "    " );
                strcat( a, find.attrib & _A_SYSTEM ? "Sys " : "    " );
                strcat( a, find.attrib & _A_HIDDEN ? "Hid " : "    " );
                
                if( find.attrib & _A_SUBDIR )
                    strcpy( s, "   <SUB-DIR>" );
                else
                {
                    StrNum2Str( find.size, 9, s ); strcat( s, " " );
                }

                sprintf( str, " %-32.32s %s  %02d-%02d-%02d  %02d:%02d:%02d  %s",
                        find.name, s,
                        find.wr_date & 0x1f,
                        ( find.wr_date >> 5 ) & 0x0f,
                        (( find.wr_date >> 9 ) + 80 ) % 100,
                        ( find.wr_time >> 11 ) & 0x1f,
                        ( find.wr_time >>  5 ) & 0x3f,
                        ( find.wr_time <<  1 ) & 0x3f, a );

                MListAdd( &TMenu -> list, find.attrib & _A_SUBDIR ? TREE_DIR : TREE_FILE, str, strdup( find.name ));
                TMenu -> Count++;
            }

            _dos_findclose( &find );

            TMenu -> sort = &sort;

            if( cfgCurrent.sort )
            {
                TMenu -> origin = malloc( TMenu -> Count * sizeof( *TMenu -> origin ));
                if( TMenu -> origin )
                    for( i = 0, l = TMenu -> list; l; l = l -> next, i++ )
                        TMenu -> origin[i] = l;
                SortList( TMenu );
            }

            list = TMenu -> list;

            TMenu -> popup = WPDefine( wtree, TMenu -> list, 0 );
            WPSetFunc( TMenu -> popup, before, NULL, filter, NULL );
        }

        review = reread = FALSE;

        WPShow( TMenu -> popup, FALSE, TRUE, Dir[SubDir].start, Dir[SubDir].line );
        code = WPActive( TMenu -> popup );
        WPGetInfo( TMenu -> popup, &Dir[SubDir].start, &Dir[SubDir].line );

        if( code >= 0 )
        {
            for( i = code, l = TMenu -> list; i-- && l; l = l -> next );

            if( l -> sel == TREE_FILE || l -> sel == TREE_FILE_SEL )
            {
                strcpy( File, TMenu -> Path ); StrAddSlash( File );
                strcat( File, l -> ext ); strcpy( netfile, File );
                return( NO_ERROR );
            }

            if( !strcmp( l -> ext, ".." ))
            {
                if( SubDir )
                    SubDir--;
                else
                    Dir[SubDir].start = Dir[SubDir].line = 0;
            }
            else
            {
                SubDir++;
                Dir[SubDir].start = Dir[SubDir].line = 0;
            }
            chdir( l -> ext );
        }
        WPRelease( TMenu -> popup, FALSE );
        TMenu -> popup = 0;
        FreeMenu( TMenu );
        if( review )
            continue;
        if( reread )
        {
            Dir[SubDir].start = Dir[SubDir].line = 0;
            continue;
        }
        if( code < 0 )
            break;
    }
    return( ERROR_INVALID_FUNCTION );
}
/*-------------------------------------------------------------------------*/
static KEY filter( KEY k )
{
    INT          code;
    
    switch( k.key )
    {
        case KEY_F1:
            ViewHelp( "TreeView" );
            SetBar( BarDir );
            k.key = 0;
            break;
            
        case KEY_F2: case ALT_F1: case ALT_F2:
            code = SelectDrive();
            SetBar( BarDir );
            if( code < 0 )
            {
                k.key = 0;
                break;
            }
            reread = 1;
            k.key = KEY_ESC;
            break;

        case KEY_INS: case KEY_SPACE:
            if( curList -> sel == TREE_FILE )
                curList -> sel = TREE_FILE_SEL;
            else
                if( curList -> sel == TREE_FILE_SEL )
                    curList -> sel = TREE_FILE;
            WPShow( fmenu -> popup, FALSE, TRUE, -1, -1 );
            k.key = KEY_DOWN;
            break;
            
        case KEY_DEL: case KEY_F8:
            break;
            
        case KEY_L: case KEY_l: case ALT_L:
            getcwd( File, _MAX_PATH - 1 );
            StrAddSlash( File );
            CreateMsgList( File );
            if( pktcount == 0 )
                break;
            Select( NULL );

            if( altx )
            {
                k.key = KEY_ESC;
                break;
            }
            
            WActive( wtinfo );
            WActive( wttitl );
            WActive( wtree  );
            WActive( wtname );
            WActive( wbar   );
            SetBar ( BarDir );

            break;
            
        case KEY_s: case KEY_S: case ALT_S:
            if( cfgCurrent.sort && fmenu -> Count  )
            {
                SortMenu( fmenu );
                SetBar( BarDir );
                before( fileNumber );
            }
            k.key = 0;
            break;
            
        case CTRL_R:
            review = 1;
            k.key = KEY_ESC;
            break;

        case CTRL_BSLASH:
            chdir( "\\" );
            reread = 1;
            k.key = KEY_ESC;
            break;
            
        case KEY_F3:
            if( curList && ( curList -> sel == TREE_FILE || curList -> sel == TREE_FILE_SEL ))
                k.key = KEY_ENTER;
            break;
            
        case KEY_F9:
            ChangeConfig();
            SetBar( BarDir );
            if( altx )
                k.key = KEY_ESC;
            else
                k.key = 0;
            break;
            
        case SHIFT_F9:
            SaveConfig( ConfFile );
            k.key = 0;
            break;
            
        case KEY_LEFT:
            k.key = KEY_HOME;
            break;

        case KEY_RIGHT:
            k.key = KEY_END;
            break;

        case ALT_X:
            altx = 1;
            k.key = KEY_ESC;
            break;
    }
    return( k );
}
/*-------------------------------------------------------------------------*/
static VOID before( INT line )
{
    fileNumber = line;
    
    curList = GetCurrentList( list, line );

    WSetXY ( wtname, 0, 0 ); WClearLine( wtname );
    WPrints( wtname, 0, 0, curList -> ext ? (CHAR*)curList -> ext : "" );
}
/*-------------------------------------------------------------------------*/
static INT SortByName( VOID *s1, VOID *s2 )
{
    INT           code;

    if(( code = CheckDIR( *((PMLIST*)s1), *((PMLIST*)s2) )) != 0 )
        return( code );
    return( stricmp( (*((PMLIST*)s1)) -> ext, (*((PMLIST*)s2)) -> ext ));
}
/*-------------------------------------------------------------------------*/
static INT SortByExt( VOID *s1, VOID *s2 )
{
    INT           code;
    CHAR          ext1[_MAX_EXT], ext2[_MAX_EXT];
    
    if(( code = CheckDIR( *((PMLIST*)s1), *((PMLIST*)s2) )) != 0 )
        return( code );

    _splitpath( (*((PMLIST*)s1)) -> ext, NULL, NULL, NULL, ext1 );
    _splitpath( (*((PMLIST*)s2)) -> ext, NULL, NULL, NULL, ext2 );

    if(( code = stricmp( ext1, ext2 )) != 0 )
        return( code );
    return( stricmp( (*((PMLIST*)s1)) -> ext, (*((PMLIST*)s2)) -> ext ));
}
/*-------------------------------------------------------------------------*/
static INT SortByTime( VOID *s1, VOID *s2 )
{
    INT           code;
    LONG          date1, date2;
    
    if(( code = CheckDIR( *((PMLIST*)s1), *((PMLIST*)s2) )) != 0 )
        return( code );
    
    date1 = DataStr2Long( &((*((PMLIST*)s1)) -> str)[48] );
    date2 = DataStr2Long( &((*((PMLIST*)s2)) -> str)[48] );

    if( date1 > date2 ) return( 1 );
    if( date1 < date2 ) return( -1 );
    return( stricmp( (*((PMLIST*)s1)) -> ext, (*((PMLIST*)s2)) -> ext ));
}
/*-------------------------------------------------------------------------*/
static INT SortBySize( VOID *s1, VOID *s2 )
{
    INT          code;
    
    if(( code = CheckDIR( *((PMLIST*)s1), *((PMLIST*)s2) )) != 0 )
        return( code );
    if(( code = strnicmp( &((*((PMLIST*)s1)) -> str)[33], &((*((PMLIST*)s2)) -> str)[33], 11 )) != 0 )
        return( code );
    return( stricmp( (*((PMLIST*)s1)) -> ext, (*((PMLIST*)s2)) -> ext ));
}
/*-------------------------------------------------------------------------*/
INT CheckDIR( MLIST *l1, MLIST *l2 )
{
    CHAR        *str1, *str2;

    if(( l1 -> sel == TREE_DIR  || l1 -> sel == TREE_DIR_SEL ) &&
       ( l2 -> sel == TREE_FILE || l2 -> sel == TREE_FILE_SEL ))
        return( -1 );
    if(( l1 -> sel == TREE_FILE || l1 -> sel == TREE_FILE_SEL ) &&
       ( l2 -> sel == TREE_DIR  || l2 -> sel == TREE_DIR_SEL ))
        return( 1 );

    str1 = l1 -> ext;
    str2 = l2 -> ext;

    if( str1[0] == '.' && str1[1] == '.' && str1[2] == 0 )
        return( -1 );
    if( str2[0] == '.' && str2[1] == '.' && str2[2] == 0 )
        return( 1 );
    return( 0 );
}
/*-------------------------------------------------------------------------*/

#endif
