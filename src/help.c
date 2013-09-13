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
 * Module: help.c         Use in: Full - [X]
 *                                Lite - [-]
 *---------------------------------------------------------------------------
 */

#ifndef _LITE_

#include "pktview.h"

/*-------------------------------------------------------------------------*/
static CHAR      str[128];
static MLIST    *list;
/*-------------------------------------------------------------------------*/
APIRET ViewHelp( CHAR *section )
{
    INT          count, begin = 0;
    CHAR        *buf, *title = NULL, sect[32];
    FILE        *file;
    MLIST       *l;
    WINDOW       whelp;
    POPUP        popup;
    
    MListFree( &list );
    
    if(( file = _fsopen( HelpFile, "rt", SH_DENYWR )) == NULL )
        return( ERROR_OPEN_FAILED );
    
    while(( buf = fgets( str, sizeof( str ) - 1, file )) != NULL )
    {
        StrDelCR( buf );
        
        if( begin )
        {
            if( !stricmp( buf, "End" ))
                break;
            MListAdd( &list, 0, strdup( buf ), NULL );
        }
        if( strlen( buf ) == 0 || *buf == ' ' || *buf == '\t' )
            continue;
        buf = StrGetWord( buf, sect, sizeof(sect) - 1, NULL );
        if( stricmp( sect, section ))
            continue;
        buf = StrSkipSpace( buf );
        if( buf )
        {
            title = malloc( strlen( buf ) + 3 );
            strcpy( title, " " ); strcat( title, buf ); strcat( title, " " );
        }
        begin = 1;
    }

    if( file ) fclose( file );
    if( list == NULL ) return( 0 );
        
    for( count = 0, l = list; l; count++, l = l -> next );
    
    if( count > y - 8 ) count = y - 8;
    
    SetBar( BarNull );

    whelp = WDefine( x/2-35, y/2-count/2-1, x/2+34, y/2+count/2+1, 9, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER2, title );
    popup = WPDefine( whelp, list, 1 );
    
    WPActive ( popup );
    WPRelease( popup, FALSE );
    WRelease ( whelp );
    MListFree( &list );
    if( title ) free( title );
    
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/

#endif
