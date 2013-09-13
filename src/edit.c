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
 * Module: edit.c         Use in: Full - [X]
 *                                Lite - [-]
 *---------------------------------------------------------------------------
 */

#ifndef _LITE_

#include "pktview.h"

/*--------------------------------------------------------------------------*/
static VOID  formRun( FIELD field );
static KEY   filter ( FIELD f, KEY key, BOOL *redisp );
/*--------------------------------------------------------------------------*/
static FORM  form;
static CHAR  addrfrom[40], addrto[40], data[22], pass[MAX_PASS+1];
static ADDR  From, To;
/*--------------------------------------------------------------------------*/
APIRET EditHeader( VOID )
{
    APIRET       rc = NO_ERROR;
    CHAR         buf[64];
    
    WPShow( popup, 1, 0, -1, -1 );

    SetBar( BarEdit );

    FidoStoreAddr( &From, &HeaderFrom );
    FidoStoreAddr( &To, &HeaderTo );

    strncpy( addrfrom, FidoAddr2Str( &HeaderFrom, buf ), sizeof(addrfrom) - 1 );
    strncpy( addrto,   FidoAddr2Str( &HeaderTo, buf ),   sizeof(addrto) - 1 );
    sprintf( data, "%2u %s %04u  %02u:%02u:%02u",
            pkt.day, month[pkt.month%12], pkt.year,
            pkt.hour, pkt.minute, pkt.second );
    strncpy( pass, pkt.password, MAX_PASS ); pass[MAX_PASS] = 0;

    form = WFormDefine( wpkt );
    WFormString ( form, 13, 0, "", addrfrom, NULL, sizeof(addrfrom) - 1, 32, NULL, NOCONTROL, 0 );
    WFormString ( form, 13, 1, "", addrto,   NULL, sizeof(addrto) - 1,   32, NULL, NOCONTROL, 0 );
    WFormString ( form, 57, 0, "", data,     NULL, 21,                   21, NULL, NOCONTROL, FLD_HIDDEN );
    WFormString ( form, 57, 1, "", pass,     NULL, MAX_PASS,       MAX_PASS, NULL, NOCONTROL, 0 );
    WFormSetFunc( form, NULL, formRun, filter );
    
    rc = WFormGet( form );
    WFormRelease( form, 0 );
    WHideCursor( wpkt );
    
    if( rc == 0 )
    {
        if( FidoCmpAddr( &HeaderFrom, &From ))
            PktUpdate = Update = 1;
        if( FidoCmpAddr( &HeaderTo, &To ))
            PktUpdate = Update = 1;
        if( strncmp( pass, pkt.password, 8 ))
            PktUpdate = Update = 1;

        FidoStoreAddr( &HeaderFrom, &From );
        FidoStoreAddr( &HeaderTo,   &To   );
        strncpy( pkt.password, pass, 8 );

        pkt.origZone   = From.Zone;
        pkt.orig_zone  = From.Zone;
        pkt.orig_net   = From.Net;
        pkt.orig_node  = From.Node;
        pkt.orig_point = From.Point;
        pkt.AuxNet     = 0;

        pkt.destZone   = To.Zone;
        pkt.dest_zone  = To.Zone;
        pkt.dest_net   = To.Net;
        pkt.dest_node  = To.Node;
        pkt.dest_point = To.Point;
    }

    ViewPktHeader( &HeaderFrom, &HeaderTo, pkt.password );
    ViewUpdate();

    return( rc );
}
/*--------------------------------------------------------------------------*/
static VOID formRun( FIELD field )
{
    ADDR        addr;
    CHAR        buf[64];
    
    switch( field )
    {
        case 0:
            if( FidoStr2Addr( addrfrom, &addr, &From ) == TRUE )
                FidoStoreAddr( &From, &addr );
            strncpy( addrfrom, FidoAddr2Str( &From, buf ), sizeof(addrfrom) - 1 );
            WFormShow( form );
            break;

        case 1:
            if( FidoStr2Addr( addrto, &addr, &To ) == TRUE )
                FidoStoreAddr( &To, &addr );
            strncpy( addrto, FidoAddr2Str( &To, buf ), sizeof(addrto) - 1 );
            WFormShow( form );
            break;

    }
}
/*--------------------------------------------------------------------------*/
#pragma off (unreferenced);
static KEY filter( FIELD f, KEY k, BOOL *redisp )
#pragma on (unreferenced);
{
    switch( k.key )
    {
        case KEY_F1:
            ViewHelp( "EditPktHeader" );
            break;
            
        case KEY_F10: case KEY_F2:
            k.key = KEY_ENTER;
            break;

        case ALT_X:
            altx = 1;
            k.key = KEY_ENTER;
            break;
    }
    return( k );
}
/*-------------------------------------------------------------------------*/
#endif
