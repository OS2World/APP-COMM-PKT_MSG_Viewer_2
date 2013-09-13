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
 * Module: select.c       Use in: Full - [X]
 *                                Lite - [-]
 *---------------------------------------------------------------------------
 */

#ifndef _LITE_

#include "pktview.h"

/*-------------------------------------------------------------------------*/
#define SEL_FROM        1
#define SEL_TO          2
#define SEL_ADDRFROM    4
#define SEL_ADDRTO      8
#define SEL_SUBJ        16
#define SEL_AREA        32
/*-------------------------------------------------------------------------*/
static APIRET InitSelect        ( VOID );
static BOOL   SearchList        ( MLIST  *list, CHAR *str );
static VOID   BeforeSelect      ( FIELD field );
static KEY    FilterSelect      ( FIELD field, KEY key, BOOL *redisp );
static APIRET CreateListFrom    ( MLIST **list );
static APIRET CreateListAddrFrom( MLIST **list );
static APIRET CreateListTo      ( MLIST **list );
static APIRET CreateListAddrTo  ( MLIST **list );
static APIRET CreateListSubj    ( MLIST **list );
static APIRET CreateListArea    ( MLIST **list );
/*-------------------------------------------------------------------------*/
static BOOL     init;
static WINDOW   wsel;
static FORM     fsel;
/*-------------------------------------------------------------------------*/
APIRET PktSelect( VOID )
{
    APIRET       rc;
    
    WPShow( popup, 1, 0, -1, -1 );

    SetBar( BarNull );

    rc = InitSelect();
    WSetTitle( wsel, " Select " );
    rc = WFormGet( fsel );
    WDeactive( wsel );
    if( rc != NO_ERROR ) return( rc );
    MsgSelect( 1, 0 );
    
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
APIRET PktUnSelect( VOID )
{
    APIRET        rc;
    
    WPShow( popup, 1, 0, -1, -1 );

    SetBar( BarNull );

    rc = InitSelect();
    WSetTitle( wsel, " Unselect " );
    rc = WFormGet( fsel );
    WDeactive( wsel );
    if( rc != NO_ERROR ) return( rc );
    MsgSelect( 0, 0 );
    
    return( rc );
}
/*-------------------------------------------------------------------------*/
VOID MsgInvert( VOID )
{
    INDEXPKT    *p;
    
    selected = 0;

    for( p = pktIndex; p; p = p -> next )
    {
        if( p -> sel == ' ' )
        {
            p -> sel = '*';
            selected++;
        }
        else
            p -> sel = ' ';
    }
}
/*-------------------------------------------------------------------------*/
VOID MsgSelect( INT code, INT all )
{
    INDEXPKT    *p;
    INT          mode = 0;
    ADDR         addr;
    INT (*func)( CHAR *, CHAR *);
    
    if( pktsel.all || all )
    {
        if( code )
        {
            for( p = pktIndex; p; p = p -> next ) p -> sel = '*';
            selected = pktcount;
        }
        else
        {
            for( p = pktIndex; p; p = p -> next ) p -> sel = ' ';
            selected = 0;
        }
    }
    else
    {
        if( pktsel.sensit )
            func = strcmp;
        else
            func = stricmp;
        
        if( pktsel.from && pktsel.from[0] )
            mode += SEL_FROM;
        if( pktsel.to && pktsel.to[0] )
            mode += SEL_TO;
        if( pktsel.addrfrom && pktsel.addrfrom[0] )
            mode += SEL_ADDRFROM;
        if( pktsel.addrto && pktsel.addrto[0] )
            mode += SEL_ADDRTO;
        if( pktsel.subj && pktsel.subj[0] )
            mode += SEL_SUBJ;
        
        if( !mode )
            return;
        
        for( p = pktIndex; p; p = p -> next )
        {
            if( mode & SEL_FROM && func( p -> from, pktsel.from ))
                continue;
            if( mode & SEL_TO && func( p -> to, pktsel.to ))
                continue;
            if( mode & SEL_AREA && func( p -> area, pktsel.area ))
                continue;
            if( mode & SEL_SUBJ && func( p -> subj, pktsel.subj ))
                continue;
            if( mode & SEL_ADDRFROM )
            {
                if( FidoStr2Addr( pktsel.addrfrom, &addr, NULL ) == FALSE )
                    continue;
                if( FidoCmpAddr( &p -> AddrFrom, &addr ))
                    continue;
            }
            if( mode & SEL_ADDRTO )
            {
                if( FidoStr2Addr( pktsel.addrto, &addr, NULL ) == FALSE )
                    continue;
                if( FidoCmpAddr( &p -> AddrTo, &addr ))
                    continue;
            }
            if( code )
            {
                if( p -> sel != ' ' )
                    continue;
                p -> sel = '*';
                if( selected < pktcount )
                    selected++;
            }
            else
            {
                if( p -> sel == ' ' )
                    continue;
                p -> sel = ' ';
                if( selected )
                    selected--;
            }
        }
    }
}
/*-------------------------------------------------------------------------*/
static APIRET InitSelect( VOID )
{
    FIELD        f;
    
    if( init ) return( NO_ERROR );
    
    init = TRUE;
    
    pktsel.all      = 1;
    pktsel.from     = calloc( MAX_FROM, 1 );
    pktsel.to       = calloc( MAX_TO,   1 );
    pktsel.addrfrom = calloc( 30,       1 );
    pktsel.addrto   = calloc( 30,       1 );
    pktsel.subj     = calloc( MAX_SUBJ, 1 );
    pktsel.area     = calloc( MAX_AREA, 1 );
    
    wsel = WDefine(  x/2-36, y/2-5, x/2+35, y/2+4, 11, W_SHADOW, BORDER4, NULL );
    fsel = WFormDefine( wsel );
    
    f = WFormCheck( fsel, 1, 1, "All", &pktsel.all, 0, 0 );
    WFormString ( fsel,  5, 2, "From:", pktsel.from,     NULL, MAX_FROM, 30, NULL, f, INV_CONTROL );
    WFormString ( fsel, 41, 2, "Addr:", pktsel.addrfrom, NULL, MAX_FROM, 23, NULL, f, INV_CONTROL );
    WFormString ( fsel,  5, 3, "To  :", pktsel.to,       NULL, MAX_TO,   30, NULL, f, INV_CONTROL );
    WFormString ( fsel, 41, 3, "Addr:", pktsel.addrto,   NULL, MAX_TO,   23, NULL, f, INV_CONTROL );
    WFormString ( fsel,  5, 4, "Subj:", pktsel.subj,     NULL, MAX_SUBJ, 59, NULL, f, INV_CONTROL );
    WFormString ( fsel,  5, 5, "Area:", pktsel.area,     NULL, MAX_AREA, 30, NULL, f, INV_CONTROL );
    WFormCheck  ( fsel,  1, 6, "Case sensitive", &pktsel.sensit, f, INV_CONTROL );
    WFormSetFunc( fsel,  BeforeSelect, NULL, FilterSelect );
    
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
static KEY FilterSelect( FIELD field, KEY k, BOOL *redisp )
{
    INT          line;
    POPUP        popup;
    MLIST       *list = NULL, *l;
    
    if( !field )
        return( k );
    
    switch( k.key )
    {
        case KEY_F3:
            switch( field )
            {
                case 1: CreateListFrom    ( &list ); break;
                case 2: CreateListAddrFrom( &list ); break;
                case 3: CreateListTo      ( &list ); break;
                case 4: CreateListAddrTo  ( &list ); break;
                case 5: CreateListSubj    ( &list ); break;
                case 6: CreateListArea    ( &list ); break;
            }
            popup = WPDefine( wsells, list, NULL );
            line  = WPActive( popup );
            if( line != -1 )
            {
                l = GetCurrentList( list, line );
                
                switch( field )
                {
                    case 1: strcpy( pktsel.from,     l -> str ); break;
                    case 2: strcpy( pktsel.addrfrom, l -> str ); break;
                    case 3: strcpy( pktsel.to,       l -> str ); break;
                    case 4: strcpy( pktsel.addrto,   l -> str ); break;
                    case 5: strcpy( pktsel.subj,     l -> str ); break;
                    case 6: strcpy( pktsel.area,     l -> str ); break;
                }
                k.key = KEY_TAB;
                *redisp = TRUE;
            }
            else
                k.key = 0;
            WPRelease( popup, TRUE );
            MListFree( &list );
            break;
    }
    return( k );
}
/*-------------------------------------------------------------------------*/
static VOID BeforeSelect( FIELD field )
{
    if( !field )
        SetBar( BarNull );
    else
        SetBar( BarSel );
}
/*-------------------------------------------------------------------------*/
static APIRET CreateListFrom( MLIST **list )
{
    INDEXPKT    *index;
    
    for( MListFree( list ), index = pktIndex; index; index = index -> next )
    {
        if( SearchList( *list, index -> from ))
            continue;
        MListAdd( list, 0, index -> from, NULL );
    }
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
static APIRET CreateListAddrFrom( MLIST **list )
{
    INDEXPKT    *index;
    CHAR         buf[64];
    
    for( MListFree( list ), index = pktIndex; index; index = index -> next )
    {
        if( SearchList( *list, FidoAddr2Str( &index -> AddrFrom, buf )))
            continue;
        MListAdd( list, 0, FidoAddr2Str( &index -> AddrFrom, buf ), NULL );
    }
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
static APIRET CreateListTo( MLIST **list )
{
    INDEXPKT    *index;
    
    for( MListFree( list ), index = pktIndex; index; index = index -> next )
    {
        if( SearchList( *list, index -> to ))
            continue;
        MListAdd( list, 0, index -> to, NULL );
    }
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
static APIRET CreateListAddrTo( MLIST **list )
{
    INDEXPKT    *index;
    CHAR         buf[64];
    
    for( MListFree( list ), index = pktIndex; index; index = index -> next )
    {
        if( SearchList( *list, FidoAddr2Str( &index -> AddrTo, buf )))
            continue;
        MListAdd( list, 0, FidoAddr2Str( &index -> AddrTo, buf ), NULL );
    }
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
static APIRET CreateListSubj( MLIST **list )
{
    INDEXPKT    *index;
    
    for( MListFree( list ), index = pktIndex; index; index = index -> next )
    {
        if( SearchList( *list, index -> subj ))
            continue;
        MListAdd( list, 0, index -> subj, NULL );
    }
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
static APIRET CreateListArea( MLIST **list )
{
    INDEXPKT    *index;
    
    for( MListFree( list ), index = pktIndex; index; index = index -> next )
    {
        if( SearchList( *list, index -> area ? index -> area : "Netmail" ))
            continue;
        MListAdd( list, 0, index -> area ? index -> area : "Netmail", NULL );
    }
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
static BOOL SearchList( MLIST *list, CHAR *str )
{
    MLIST       *l;
    
    for( l = list; l; l = l -> next )
        if( !stricmp( l -> str, str ))
            return( TRUE );
    return( FALSE );
}
/*-------------------------------------------------------------------------*/

#endif
