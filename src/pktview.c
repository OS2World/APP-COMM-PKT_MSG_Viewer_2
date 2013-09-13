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
 * Module: pktview.c      Use in: Full - [X]
 *                                Lite - [X]
 *---------------------------------------------------------------------------
 */

#include "pktview.h"

/*-------------------------------------------------------------------------*/
WINDOW wscan,  wpkt,   wlist,  wsubj, wquick, wbar;
WINDOW wmain,  wtext;
WINDOW wdel,   werror, wpath,  wproc;
WINDOW wtinfo, wttitl, wtree,  wtname, wdrive,  wover;
WINDOW wsells, winfo;
/*-------------------------------------------------------------------------*/
PKT48       pkt;
MSG         msg;
INT         ver_hi  = 0;              // version
INT         ver_low = 24;             // version
INT         os;                       // os = 0 - OS/2, os = 1 - DOS
INT         mode;                     // mode = 0 - .msg; mode = 1 - .pkt
BOOL        xMem;                     // uses xMem (EMS/XMS/...)
BOOL        BadPkt;                   // bad format of pkt
BOOL        ReadOnly;                 // file is read only
BOOL        review;                   // review directory
CHAR        ConfFile[_MAX_PATH];      // path to 'pktview.cfg'
CHAR        netfile [_MAX_PATH+1];
CHAR        drive   [_MAX_DRIVE], dir[_MAX_DIR];
CHAR        fname   [_MAX_FNAME], ext[_MAX_EXT];
INT         pktcount;                 // count of messafes in pkt file
INT         txtcount;                 // count of line's in pkt file
INDEXPKT   *pktIndex, *Current, **pktOrigin;
INDEXTXT   *txtIndex;
FILE       *PktFile,  *newfile;
BOOL        reload;                   // wanted reload current file
BOOL        newload;                  // wanted load new file
SHORT       x, y;
SHORT       maxXlist, maxYlist;       // max size X,Y of window <wlist>
SHORT       maxXquick,maxYquick;      // max size X,Y of window <wquick>
SHORT       maxXtext, maxYtext;       // max size X,Y of window <wtext>
SHORT       maxXmain, maxYmain;       // max size X,Y of window <wmain>
SHORT       maxXdel,  maxYdel;        // max size X,Y of window <wdel>
SHORT       maxXpath, maxYpath;       // max size X,Y of window <wpath>
SHORT       maxXproc, maxYproc;       // max size X,Y of window <wproc>
SHORT       maxXtree, maxYtree;       // max size X,Y of window <wtree>
SHORT       maxXsubj, maxYsubj;       // max size X,Y of window <wsubj>
SHORT       maxXpkt,  maxYpkt;        // max size X,Y of window <wpkt>
SHORT       maxXerr,  maxYerr;        // max size X,Y of window <werror>
BOOL        Quick = TRUE;             // quick view of message
BOOL        altx;                     // Alt-X present
BOOL        autoView, autoList;
BOOL        PktUpdate;                // pkt is changing
BOOL        Update;                   // if need update header *.pkt
BOOL        UpdateTxt;                // need update text in *.pkt
INT         selected;                 // count selected of massages
TYPESORT    typesort;
SELECT      pktsel;
POPUP       popup;
#ifndef _LITE_
  CHAR      FtsCFile[_MAX_PATH];
  CHAR      pkttmp  [_MAX_PATH];
  CHAR      HelpFile[_MAX_PATH];
  CHAR      CurPath [_MAX_PATH];
  CHAR      LastArch[_MAX_PATH];
  CHAR     *CurDir;
  UINT      CurDrive;
  BOOL      arcreload;
#endif
/*-------------------------------------------------------------------------*/
const PCHAR config = "pktview.cfg";
/*-------------------------------------------------------------------------*/
#ifndef _LITE_
BAR BarList = {  1, "Help  ", 0, "Save  ", 1, "Load  ", 1, "Edit  ", 1, "Copy  ",
                 1, "Move  ", 0,  NULL,    1, "Delete", 1, "Setup ", 1, "Exit  " };
BAR BarEdit = {  1, "Help  ", 1, "Store ", 0,  NULL,    0,  NULL,    0,  NULL,
                 0,  NULL,    0,  NULL,    0,  NULL,    0,  NULL,    1, "Exit  " };
BAR BarView = {  1, "Help  ", 0,  NULL,    1, "Load  ", 0, "Edit  ", 1, "Copy  ",
                 1, "Move  ", 0,  NULL,    1, "Delete", 1, "Setup ", 1, "Exit  " };
BAR BarDir  = {  1, "Help  ", 1,  "Drive", 1, "View  ", 0,  NULL,    0,  NULL,
                 0,  NULL,    0,  NULL,    0, "Delete", 1, "Setup ", 1, "Exit  " };
BAR BarLoad = {  1, "Help  ", 0,  NULL,    1, "View  ", 1, "Unpack", 0,  NULL,
                 0,  NULL,    0,  NULL,    1, "Delete", 1, "Setup ", 1, "Exit  " };
BAR BarSel  = {  0,  NULL,    0,  NULL,    1, "List  ", 0,  NULL,    0,  NULL,
                 0,  NULL,    0,  NULL,    0,  NULL,    0,  NULL,    0,  NULL    };
BAR BarSet  = {  1,  "Help ", 0,  NULL,    0,  NULL,    0,  NULL,    0,  NULL,
                 0,  NULL,    0,  NULL,    0,  NULL,    0,  NULL,    1,  "Exit " };
#endif
const BAR BarNull = {  0,  NULL,    0,  NULL,    0,  NULL,    0,  NULL,    0,  NULL,
                       0,  NULL,    0,  NULL,    0,  NULL,    0,  NULL,    0,  NULL    };
/*-------------------------------------------------------------------------*/
ADDR  HeaderFrom, HeaderTo;
/*-------------------------------------------------------------------------*/
const PCHAR  month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"  };
/*-------------------------------------------------------------------------*/
#ifndef _LITE_
const PCHAR comment  = "   Original     Packet  Ratio   Date      Time    Name";
const PCHAR format   = "%s %s  %3d%%  %02d-%02d-%02d  %02d:%02d:%02d  %-29.29s";
#endif
CHAR *iobuf;
/*-------------------------------------------------------------------------*/
PKTMISC cfgCurrent;
const PKTMISC cfgDefault =
{
    1, 1, 0, 0, 0,
    {
        /* --- wmain --- */
        { YELLOW|B_, YELLOW|GB_, WHITE|B_, YELLOW|B_, LIGHTRED|B_, YELLOW|B_, LIGHTMAGENTA|B_, LIGHTGRAY|B_, YELLOW|RB_, YELLOW|GB_, YELLOW|B_, YELLOW|B_, YELLOW|B_  },
        /* --- wtext --- */
        { YELLOW, YELLOW, 0, 0, RED, GREEN, 0, CYAN, 0, WHITE, YELLOW, LIGHTGRAY },
        /* --- wtinfo, wttitle, wtree --- */
        { YELLOW|B_, BLACK|GB_, WHITE|B_, 0, LIGHTCYAN|B_, YELLOW|B_, YELLOW|GB_, LIGHTGREEN|B_ },
        /* --- wpkt --- */
        { YELLOW|B_, BLACK|GB_, WHITE|B_, YELLOW|B_, LIGHTRED|B_, YELLOW|B_, LIGHTMAGENTA|B_, LIGHTGRAY|B_, YELLOW|RB_, BLACK|GB_, YELLOW|B_, YELLOW|B_, YELLOW|B_  },
        /* --- wbar --- */
        { BLACK|BG_, LIGHTGRAY, 0, 0, 0, BLACK|BG_, 0, DARKGRAY|BG_ },
        /* --- werror, wover --- */
        { WHITE|R_, BLACK|W_, YELLOW|R_, WHITE|R_, WHITE|R_, YELLOW|R_, BLACK|W_ },
        /* --- wlist --- */
        { YELLOW|B_, BLACK|GB_, WHITE|B_, 0, LIGHTCYAN|B_, YELLOW|B_, YELLOW|GB_, LIGHTGREEN|B_ },
        /* --- wscan, wproc --- */
        { BLACK|W_, BLACK|GB_, BLACK|W_, BLACK|W_, BLACK|W_, 0, 0, DARKGRAY | W_, 0, BLACK|GB_ },
        /* --- wtree --- */
        { YELLOW|B_, BLACK|GB_, WHITE|B_, 0, LIGHTCYAN|B_, LIGHTGREEN|B_, BLACK|G_, LIGHTGREEN|B_, 0, YELLOW|B_, YELLOW|GB_ },
        /* --- whelp --- */
        { BLACK|W_, BLACK|GB_, BLACK|W_, BLACK|W_, BLACK|W_, 0, 0, DARKGRAY | W_, 0, BLACK|GB_ },
        /* --- wpath, wdel --- */
        { BLACK|W_, BLACK|GB_, BLACK|W_, BLACK|W_, BLACK|W_, BLUE|W_, BLACK|GB_, DARKGRAY | W_, 0, BLACK|GB_ },
        /* --- wsells, wsel, wsort --- */
        { BLACK|W_, BLACK|GB_, BLACK|W_, BLACK|W_, BLACK|W_, 0, 0, DARKGRAY | W_, 0, BLACK|GB_ },
        /* --- wattr --- */
        { YELLOW|B_, YELLOW|GB_, WHITE|B_, YELLOW|B_, LIGHTRED|B_, YELLOW|B_, LIGHTMAGENTA|B_, LIGHTGRAY|B_, YELLOW|RB_, YELLOW|GB_, YELLOW|B_, YELLOW|B_, YELLOW|B_  },
        /* --- wquick --- */
        { YELLOW, YELLOW, 0, 0, RED, GREEN, 0, CYAN, 0, WHITE, YELLOW, LIGHTGRAY },
        /* --- wsubj --- */
        { YELLOW|B_, YELLOW|GB_, WHITE|B_, YELLOW|B_, LIGHTRED|B_, YELLOW|B_, LIGHTMAGENTA|B_, LIGHTGRAY|B_, YELLOW|RB_, YELLOW|GB_, YELLOW|B_, YELLOW|B_, YELLOW|B_  },
        /* --- wdisk --- */
        { BLACK|W_, BLACK|GB_, BLACK|W_, BLACK|W_, BLACK|W_, BLACK|W_, BLACK|GB_, DARKGRAY | W_, 0, BLACK|GB_ },
        /* --- winfo --- */
        { BLACK|W_, BLACK|GB_, BLACK|W_, BLACK|W_, BLACK|W_, 0, 0, DARKGRAY | W_, 0, BLACK|GB_ },
        /* --- wconf --- */
        { BLACK|W_, BLACK|GB_, BLACK|W_, BLACK|W_, BLACK|W_, 0, 0, DARKGRAY | W_, 0, BLACK|GB_ },
    },

#ifndef _LITE_

    {
        /* ---=== OS/2 ===--- */
        {
            "C:\\SPOOL", "hiew %f",
            {
                {   /* --- Zip --- */
                    "unzip -ojC %a %f -d %t",           // view
                    "unzip -o %a %f -d %d",             // extract
                    "zip -d %a %f",                     // del
                    "",                                 // add "zip -d %a %f"
                    "",                                 // test
                    "",                                 // password
                    "",                                 // list
                    "",                                 // reserve
                },
                {   /* --- Arj --- */
                    "unarj e %a %t\\%N",
                    "unarj x %a %d\\%N",
                    "arj d -y %a %f",
                    "",                                 // "arj a -y %a %f"
                    "",                                 // test
                    "",                                 // password
                    "",                                 // list
                    "",                                 // reserve
                },
                {   /* --- Lha --- */
                    "lh2 x %a %t %f /o /i",
                    "lh2 x %a %d %f /o /s /e /i",
                    "lh2 d %a %f",
                    "",                                 // "lh2 a %a %f"
                    "",                                 // test
                    "",                                 // password
                    "",                                 // list
                    "",                                 // reserve
                },
                {   /* --- Rar --- */
                    "rar e -std -y -c- -av- %a %f %t\\",
                    "rar x -std -y -c- -av- %a %f %d\\",
                    "rar d -std -y -c- -av- %a %f",
                    "",                                 // "rar a -std -y -c- -av- %a %f"
                    "",                                 // test
                    "",                                 // password
                    "",                                 // list
                    "",                                 // reserve
                },
            },
        },

        /* ---=== DOS ===--- */
        {
            "C:\\TMP", "hiew %f",
            {
                {   /* --- Zip --- */
                    "pkunzip -o %a %f %t",
                    "pkunzip -od %a %f %d",
                    "pkzip -d %a %f",
                    "",                                 // "pkzip %a %f"
                    "",                                 // test
                    "",                                 // password
                    "",                                 // list
                    "",                                 // reserve
                },
                {   /* --- Arj --- */
                    "arj e -y -ha %a %t %N",
                    "arj x -y %a %t %N",
                    "arj d -y %a %t",
                    "",                                 // "arj a -y %a %t"
                    "",                                 // test
                    "",                                 // password
                    "",                                 // list
                    "",                                 // reserve
                },
                {   /* --- Lha --- */
                    "lha e %a %t\ %f",
                    "lha x %a %d\ %f",
                    "lha d %a %f",
                    "",                                 // "lha a %a %f"
                    "",                                 // test
                    "",                                 // password
                    "",                                 // list
                    "",                                 // reserve
                },
                {   /* --- Rar --- */
                    "rar e -std -y -c- -av- %a %f %t\\",
                    "rar x -std -y -c- -av- %a %f %d\\",
                    "rar d -std -y -c- -av- %a %f",
                    "",                                 // "rar a -std -y -c- -av- %a %f"
                    "",                                 // test
                    "",                                 // password
                    "",                                 // list
                    "",                                 // reserve
                },
            },
        },
    },

#endif

};
/*-------------------------------------------------------------------------*/
#pragma off (unreferenced);
VOID Critical( INT code )
#pragma on (unreferenced);
{
    RestoreInit();
    exit( 3 );
}
/*-------------------------------------------------------------------------*/
#ifdef __DOS__
#pragma off (unreferenced);
INT CriticalError( unsigned deverr, unsigned errcode, unsigned far *devhdr )
#pragma on (unreferenced);
{ 
    return( _HARDERR_IGNORE );
} 
#endif
/*-------------------------------------------------------------------------*/
INT main( INT argc, char *argv[] )
{
    INT      i;
    CHAR     netfile[_MAX_PATH], *name, *version;
#ifndef _LITE_
    INT      conf = 0;
    UINT     total;
#endif
    
#ifdef __OS2__
    os = 0;
    name = "PKT/MSG Viewer/2";
#else
    os = 1;
    name = "PKT/MSG Viewer";
#endif

#ifndef _LITE_
    version = "";
#else
    version = " [lite]";
#endif
    
    printf( "\n%s v%hu.%hu.pre-release%s (C) 1995-97 Moonlit Software/2\n",
           name, ver_hi, ver_low, version );

    netfile [0] = 0;
    ConfFile[0] = 0;

    Update      =  0;
    Quick       =  1;
    altx        =  0;
    reload      =  1;

    for( i = 1; i < argc; i++ )
    {
        if( argv[i][0] == '?' )
        {
            ShortHelp();
            return( 0 );
        }

        if( argv[i][0] == '-' || argv[i][0] == '/' )
        {
            switch( argv[i][1] )
            {
                case 'q': case 'Q':
                    Quick = 0;
                    break;

                case 'a': case 'A':
                    autoView = 1;
                    break;
                    
                case 'h': case 'H': case '?':
                    ShortHelp();
                    return( 0 );
#ifndef _LITE_
                case 'c': case 'C':
                    strcpy( ConfFile, &argv[i][2] );
                    conf = 1;
                    break;

                case 's': case 'S':
                    CreateConfig( config );
                    return( 0 );

                case 'l': case 'L':
                    autoList = 1;
                    break;
                    
#endif
            }
        }
        else
            if( netfile[0] == 0 )
                strcpy( netfile, argv[i] );
    }

#ifdef _LITE_
    if( netfile[0] == 0 )
    {
        ShortHelp();
        return( 0 );
    }
#endif
    
    tzset();

    _splitpath( argv[0], drive, dir, NULL, NULL );

#ifndef _LITE_

    strcpy( pkttmp,   drive ); strcat( pkttmp,   dir ); strcat( pkttmp,   "pktview.tmp" );
    strcpy( HelpFile, drive ); strcat( HelpFile, dir ); strcat( HelpFile, "pktview.hlp" );
    strcpy( FtsCFile, drive ); strcat( FtsCFile, dir ); strcat( FtsCFile, "ftscprod.fe" );

    if( !conf )
    {
        if( !access( config, F_OK ))
        {
            getcwd( CurPath, _MAX_PATH - 1 );
            StrAddSlash( CurPath );
            _splitpath( CurPath, drive, dir, NULL, NULL );
            strcpy( ConfFile, drive );
            strcat( ConfFile, dir );
            strcat( ConfFile, config );
        }
        else
        {
            _splitpath( argv[0], drive, dir, fname, ext );
            strcpy( ConfFile, drive );
            strcat( ConfFile, dir );
            strcat( ConfFile, config );
        }
    }
#else
    strcpy( ConfFile, drive ); strcat( ConfFile, dir ); strcat( ConfFile, config );
#endif

#ifdef __OS2__
    DosError( 0L );
#else
    _harderr( CriticalError );
#endif
    
#ifndef _LITE_
    _dos_getdrive( &CurDrive );
    getcwd( CurPath, _MAX_PATH - 1 );
#endif
    
    SetDefault( &cfgCurrent, &cfgDefault );

    if( ConfFile[0] )
        LoadConfig( ConfFile );

#ifndef _LITE_

    switch( cfgCurrent.psort )
    {
        case 1:  typesort.from   = 1; break;
        case 2:  typesort.to     = 1; break;
        case 3:  typesort.area   = 1; break;
        case 4:  typesort.subj   = 1; break;
        case 5:  typesort.date   = 1; break;
        case 6:  typesort.size   = 1; break;
        default: typesort.unsort = 1;
    }

#endif

    InitVideo();

    signal( SIGINT,  Critical );
    signal( SIGABRT, Critical );

    iobuf = malloc( SIZEIOBUF );

    LoadFile( netfile );

    RestoreInit();

#ifndef _LITE_
    _dos_setdrive( CurDrive, &total );
    chdir( CurPath );
#endif
    
#ifdef __OS2__
    DosError( 1L );
#endif

    return( 0 );
}
/*-------------------------------------------------------------------------*/
VOID ShowError( CHAR *str, ... )
{
    INT          i, j, line = 0, start = 0, delta = 1;
    va_list      argptr;
    CHAR         string[256], *s;

    va_start( argptr, str );
    vsprintf( string, str, argptr );
    va_end  ( argptr );

    for( j = 0, s = string; *s; s++, j++ )
    {
        if( *s == '\n' || j >= maxXerr)
        {
            line++;
            j = 0;
        }
    }
    line++;

    switch( line )
    {
        case 1:
            start = 2;
            delta = 1;
            break;

        case 2:
            start = 1;
            delta = 2;
            break;

        case 3: case 4:
            start = 1;
            delta = 1;
            break;
    }

    WClear ( werror );
    WActive( werror );

    for( i = 0, s = string; *s; i++ )
    {
        for( j = 0; *s && *s != '\n' && j < maxXerr; s++, j++ )
            string[j] = *s;

        if( *s == '\n' )
            s++;

        string[j] = 0;

        WCPuts( werror, i * delta + start, string );
    }

    MGetKey( FALSE );

    WDeactive( werror );
}
/*-------------------------------------------------------------------------*/
CHAR *ShowPath( CHAR *path, INT len )
{
    INT              i, j, count = 1;
    static CHAR      str[_MAX_PATH];
    CHAR             tmp[_MAX_PATH];

    strncpy( tmp, path, sizeof( tmp ) - 1 );

    while( strlen( tmp ) > len )
    {
        for( i = 0; tmp[i] != '\\' && tmp[i] != '/'; i++ )
            str[i] = tmp[i];
        str[i] = 0;
        strcat ( str, "\\...\\" );
        for( i++, j = count; j; i++ )
            if( tmp[i] == '\\' || tmp[i] == '/' )
                j--;
        strcat( str, &tmp[i] );
        strcpy( tmp, str );
        count = 2;
    }
    strcpy( str, tmp );

    return( str );
}
/*-------------------------------------------------------------------------*/
#ifndef _LITE_
APIRET CheckUpdatePkt( CHAR *netfile )
{
    if( !Update )
        return( NO_ERROR );

    if( !GetOk( "PKT header is changed.", "Update?" ))
        return( UpdatePkt( netfile ));
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
APIRET UpdatePkt( CHAR *netfile )
{
    fseek( PktFile, 0, SEEK_SET );

    if( fwrite( &pkt, sizeof( pkt ), 1, PktFile ) != 1 )
    {
        ShowError( "Error write to file '%s'", ShowPath( netfile, 50 ));
        return( ERROR_WRITE_FAULT );
    }

    Update = 0;
    ViewUpdate();

    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
CHAR *CreateName( CHAR *netfile, CHAR name[] )
{
#ifdef __OS2__
    CHAR  *form = "%08x.pkt";
#else
    CHAR  *form = "%08lx.pkt";
#endif
    CHAR  drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
    struct       dostime_t t;
    
    if( !netfile )
    {
        getcwd( name, _MAX_PATH - 1 );
        StrAddSlash( name );
    }
    else
    {
        _splitpath( netfile, drive, dir, fname, ext  );
        _makepath ( name, drive, dir, NULL, NULL );
    }
    
    _dos_gettime( &t );
    sprintf( &name[strlen(name)], form, ( time(NULL) << 8 ) + t.hsecond );
    
    return( name );
}
/*-------------------------------------------------------------------------*/
CHAR *CreateCommand( CHAR *str, CHAR *arch, CHAR *list, CHAR *dirtmp )
{
    INT          i;
    CHAR        *command;
    
    command = calloc( _MAX_PATH * 2, 1 );
    _splitpath( list, NULL, NULL, fname, ext );
    
    for( i = 0; str && *str; str++ )
    {
        if( *str != '%' )
        {
            command[i++] = *str;
            continue;
        }
        switch( *++str )
        {
            case '%':
                command[i++] = '%';
                break;
                
            case 'a': case 'A':
                strcat( command, arch );
                i = strlen( command );
                break;
                
            case 'f': case 'F':
                strcat( command, list );
                i = strlen( command );
                break;
                
            case 'N':
                strcat( command, fname );
                strcat( command, ext );
                i = strlen( command );
                break;
                
            case 'n':
                strcat( command, fname );
                i = strlen( command );
                break;
                
            case 'e':
                strcat( command, ext );
                i = strlen( command );
                break;
                
            case 't': case 'T':
            case 'd': case 'D':
                strcat( command, dirtmp );
                i = strlen( command );
                break;
                
            default:
                command[i++] = *str;
        }
    }
    
    return( command );
}
#endif
/*-------------------------------------------------------------------------*/
VOID InitVideo( VOID )
{
    INT          i, m;

    WInit( V_CURRENT );
/*
    i = WGetSesType();
*/
    x = WScrMaxX(); y = WScrMaxY();

    WEnableCursor();

    for( i = 0; i < MAX_SCHEME; i++ )
        WSetScheme( i, &cfgCurrent.color[i] );

    werror= WDefine(  x/2-36, y/2-5, x/2+34, y/2+1, 5, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER1, " Error massage " );
    wdel  = WDefine(  x/2-25, y/2-5, x/2+25, y/2,  10, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER4, NULL );
    wproc = WDefine(  x/2-25, y/2-5, x/2+25, y/2+1, 7, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER4, NULL );

    wscan = WDefine(  x/2-17, y/2-5, x/2+17, y/2+1, 7, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER4, " Please Wait " );
    wpkt  = WDefine(  0,      0,     x-1,    3,     3, W_NOSCROLL | W_NOCURSOR, " Ä   Í  ", NULL );

    wpath = WDefine(  x/2-36, y/2-5, x/2+34, y/2,  10, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER4, NULL );
    wover = WDefine(  x/2-25, y/2-5, x/2+25, y/2+1, 5, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER4, NULL );

    if( Quick )
    {
        wlist = WDefine(  0,  4,  x-1, y-10, 6, W_NOSCROLL | W_NOCURSOR, NULL, NULL );
        wsubj = WDefine(  0, y-9, x-1, y-7, 14, W_NOSCROLL | W_NOCURSOR, " Í   Ä  ", NULL );
    }
    else
    {
        wlist = WDefine(  0,  4,  x-1, y-5,  6, W_NOSCROLL | W_NOCURSOR, NULL, NULL );
        wsubj = WDefine(  0, y-4, x-1, y-2, 14, W_NOSCROLL | W_NOCURSOR, " Í   Ä  ", NULL );
    }

    wquick= WDefine(  0, y-6, x-1, y-2, 13, W_NOSCROLL | W_NOCURSOR, NULL, NULL );
    wbar  = WDefine(  0, y-1, x-1, y-1,  4, W_NOSCROLL | W_NOCURSOR, NULL, NULL );

    wmain = WDefine( 0,  0, x-1,   5, 0, W_NOSCROLL | W_NOCURSOR, " Ä   Í  ", NULL );
    wtext = WDefine( 0,  6, x-1, y-2, 1, W_NOSCROLL | W_NOCURSOR, NULL,    NULL );

    /*-----------------------------------------------------------------------*/

    wtinfo = WDefine( 0,  0,  x-1,   2, 2, W_NOSCROLL | W_NOCURSOR, " Ä   Í  ", NULL );
    wttitl = WDefine( 0,  3,  x-1,   3, 2, W_NOSCROLL | W_NOCURSOR, NULL,       NULL );
    wtree  = WDefine( 0,  4,  x-1, y-5, 8, W_NOSCROLL | W_NOCURSOR, NULL,       NULL );
    wtname = WDefine( 0, y-4, x-1, y-2, 2, W_NOSCROLL | W_NOCURSOR, " Í   Ä  ", NULL );

    /*-----------------------------------------------------------------------*/

    WGetSize( wquick, &maxXquick, &maxYquick );
    WGetSize( wlist,  &maxXlist,  &maxYlist  );
    WGetSize( wtext,  &maxXtext,  &maxYtext  );
    WGetSize( wmain,  &maxXmain,  &maxYmain  );
    WGetSize( wdel,   &maxXdel,   &maxYdel   );
    WGetSize( wpath,  &maxXpath,  &maxYpath  );
    WGetSize( wproc,  &maxXproc,  &maxYproc  );
    WGetSize( wtree,  &maxXtree,  &maxYtree  );
    WGetSize( wsubj,  &maxXsubj,  &maxYsubj  );
    WGetSize( wpkt,   &maxXpkt,   &maxYpkt   );
    WGetSize( werror, &maxXerr,   &maxYerr   );

    /*-----------------------------------------------------------------------*/

    WPutbs  ( werror, 2, 48, WGetSchColor( werror, C_TITLE ), " Press any key ... " );
    WPrints ( wscan,  8,  2, "Scanning:" );

    WSetCurColor( wbar, C_SELECT );
    WPrints( wbar,  0,  0, "1" );
    for( i = 1; i < 9; i++ )
    {
        WSetXY ( wbar,  i * 8 - 1,  0 );
        WPrintf( wbar, "%2d", i + 1 );
    }
    WPrints  ( wbar,  71,  0, " 10" );

    if(( m = maxXpkt - 32 ) > 64 )
        m = 64;

    WSetCurColor( wpkt, C_HIDE );
    WPrints     ( wpkt,  0,  0, "Packet from:" );
    WPrints     ( wpkt,  0,  1, "Packet to  :" );
    WPrints     ( wpkt,  m,  0, "Data/Time:" );
    WPrints     ( wpkt,  m,  1, "Password :" );
    WSetCurColor( wpkt, C_HOT );

    WSetCurColor( wsubj, C_HIDE );
    WPrints     ( wsubj, 0,  0, "Subj:" );
    WSetCurColor( wsubj, C_HOT );

    /*-----------------------------------------------------------------------*/

    WSetCurColor( wmain, C_HIDE );
    WPrints     ( wmain, 0, 0, "Msg :" );
    WPrints     ( wmain, 0, 1, "From:" );
    WPrints     ( wmain, 0, 2, "To  :" );
    WPrints     ( wmain, 0, 3, "Subj:" );
    WPrints     ( wmain, m, 0, "Data/Time:" );
    WSetCurColor( wmain, C_HOT );

    /*-----------------------------------------------------------------------*/
    WCPuts ( wover, 0, "The following file exist" );
    WCPuts ( wover, 2, "Do you wish to write over the old file?" );
    /*-----------------------------------------------------------------------*/
    wsells = WDefine(  x/2, y/2-7, x/2+35, y/2+5, 11, W_SHADOW | W_NOCURSOR, BORDER4, NULL );

}
/*-------------------------------------------------------------------------*/
VOID RestoreInit( VOID )
{
    WDeactive ( wtree  );
    WDeactive ( wpkt   );
    WDeactive ( wlist  );
    WDeactive ( wsubj  );
    WDeactive ( wquick );
    WDeactive ( wbar   );
    WDeactive ( wmain  );
    WDeactive ( wtext  );

    WRestore  ( FALSE );
}
/*-------------------------------------------------------------------------*/
VOID InitMem( VOID )
{
    BadPkt    = FALSE;
    ReadOnly  = FALSE;
    reload    = FALSE;
    selected  = 0;
    PktUpdate = FALSE;
    UpdateTxt = FALSE;

    FreePktIndex();

    WSetCurColor( wlist, C_MORE );
    WClear      ( wlist );

    WSetCurColor( wquick, C_USER0 );
    WClear      ( wquick );

    WSetCurColor( wsubj, C_TITLE );
    WSetXY      ( wsubj, 6, 0 );
    WPrintf     ( wsubj, "%-*.*s", maxXsubj - 6, maxXsubj - 6, " " );
}
/*-------------------------------------------------------------------------*/
#ifndef _LITE_
VOID CheckReview( CHAR *file )
{
    CHAR         name[_MAX_PATH];

    _splitpath( file, drive, dir, NULL, NULL );
    strcpy( name, drive ); strcat( name, dir ); StrDelSlash( name );
    if( name[0] == 0 || ( CurDir && !stricmp( name, CurDir )))
        review = 1;
}
#endif
/*-------------------------------------------------------------------------*/
INDEXPKT *GetCurrentPkt( INT number )
{
    INDEXPKT    *p;

    for( p = pktIndex; number && p; p = p -> next, number-- );

    if( p == NULL ) p = pktIndex;
    
    return( p );
}
/*-------------------------------------------------------------------------*/
INDEXTXT *GetCurrentTxt( INT number )
{
    INDEXTXT    *p;

    for( p = txtIndex; number && p; p = p -> next, number-- );

    return( p );
}
/*-------------------------------------------------------------------------*/
VOID FreePktIndex( VOID )
{
    INT          i;
    INDEXPKT    *p = pktIndex, *p1;

    if( pktOrigin )
    {
        free( pktOrigin );
        pktOrigin = NULL;
    }

    while( p )
    {
        if( p -> to   ) free( p -> to   );
        if( p -> from ) free( p -> from );
        if( p -> subj ) free( p -> subj );
        if( p -> date ) free( p -> date );
        if( p -> area ) free( p -> area );
        if( mode == FILE_TYPE_MSG && p -> name ) free( p -> name );
        for( i = 0; i < MAX_LINE; i++ )
            if( p -> quick[i] )
                free( p -> quick[i] );
        p1 = p -> next;
        free( p );
        p = p1;
    }
    pktIndex = NULL;
    pktcount = 0;
}
/*-------------------------------------------------------------------------*/
VOID FreeTxtIndex( VOID )
{
    INDEXTXT    *p = txtIndex, *p1;

    while( p )
    {
        if( p -> str ) free( p -> str );
        p1 = p -> next;
        free( p );
        p = p1;
    }
    txtIndex = NULL;
    txtcount = 0;
}
/*-------------------------------------------------------------------------*/
MLIST *GetCurrentList( MLIST *list, INT line )
{
    MLIST       *l;
    
    for( l = list; line && l; line--, l = l -> next );
    
    return( l );
}
/*-------------------------------------------------------------------------*/
VOID SetBar( BAR bar )
{
    INT          i;
    SHORT        barX[]  = {  1,  9, 17, 25, 33, 41, 49, 57, 65, 74 };

#ifdef _LITE_
    bar = BarNull;
#endif

    if( bar == NULL )
        bar = BarNull;
    
    for( i = 0; i < 10; i++ )
    {
        if( bar[i].str == NULL || ( bar[i].str && *bar[i].str == 0 ))
        {
            WSetCurColor( wbar, C_HOT );
            WPrints     ( wbar, barX[i], 0, "      " );
        }
        else
        {
            if( bar[i].status )
                WSetCurColor( wbar, C_HOT );
            else
                WSetCurColor( wbar, C_HIDE );
            WPrints( wbar, barX[i], 0, bar[i].str );
        }
    }
}
/*-------------------------------------------------------------------------*/
#ifndef _LITE_
CHAR *ViewNum( LONG number, INT count )
{
    INT          i, len;
    static CHAR  str[80];

    len = count + ( count - 1 ) / 3;
    memset( str, ' ', len );
    str[len] = 0;

    for( i = 0, len--; len; i++, len-- )
    {
        if( i && !( i % 3 ))
            str[len--] = '.';

        str[len] = number % 10 + '0';

        number /= 10;

        if( !number )
            break;
    }

    return( str );
}
#endif
/*-------------------------------------------------------------------------*/
VOID ShortHelp( VOID )
{
    printf(
    "\n   Created by:  Oleg Milaenko    AKA 2:468/2@fidonet AKA Oleg/2"
    "\n   Assistance:  Sergey Radochin  AKA 2:468/7@fidonet"
    "\n"
#ifndef _LITE_
    "\nPKTView.exe [<filename.ext>] [[-switch][data]]"
    "\n   <switch>"
    "\n      h: help"
    "\n      q: no quick view window"
    "\n      a: auto view message"
    "\n      c: config file"
    "\n      s: create config file"
#else
    "\nPKTVlite.exe <filename.ext> [-switch]"
    "\n   <switch>"
    "\n      h: help"
    "\n      q: no quick view window"
    "\n      a: auto view message"
#endif
    "\n" );
}
/*-------------------------------------------------------------------------*/
