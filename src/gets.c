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
 * Module: gets.c         Use in: Full - [X]
 *                                Lite - [-]
 *---------------------------------------------------------------------------
 */

#ifndef _LITE_

#include "pktview.h"

/*--------------------------------------------------------------------------*/
static VOID   pktType  ( VOID );
static VOID   msgType  ( VOID );
static VOID   txtType  ( VOID );
static VOID   formRun  ( FIELD field );
/*--------------------------------------------------------------------------*/
static FORM     ghform;
static ADDR    *From, *To;
static INT      type;
static CHAR     afrom[32], ato[32], *inputName;
/*--------------------------------------------------------------------------*/
INT EnterPathType( CHAR *title, CHAR *prompt, CHAR name[] )
{
    INT          code;
    INT          lenX = maxXpath - 2;
    SHORT        bpkt, bmsg, btext, besc;
    FORM         form;
    
    SetBar( BarNull );

    inputName = name;
    bpkt = bmsg = btext = besc = 0;
    
    WSetTitle   ( wpath, title );
    WSetCurColor( wpath, C_MORE );
    WHLine      ( wpath, 0, 2, maxXpath, 0 );
    WSetXY      ( wpath, 1, 0 );
    WPrintf     ( wpath, "%-*.*s", lenX, lenX, prompt );
    
    form = WFormDefine( wpath );
    WFormString( form, 1, 1, "", name, NULL, _MAX_PATH, lenX, NULL, NOCONTROL, 0 );
    
    WFormButton( form, 16, 3, "[ Pkt ]",      &bpkt,  pktType, NOCONTROL, BUT_OK );
    WFormButton( form, 25, 3, "[ Msg ]",      &bmsg,  msgType, NOCONTROL, BUT_OK );
    WFormButton( form, 34, 3, "[ Text ]",     &btext, txtType, NOCONTROL, BUT_OK );
    WFormButton( form, 44, 3, "[ Cancel ]",   &besc,  NULL,    NOCONTROL, BUT_CANCEL );
    
    type = 0;

    WActive( wpath );
    code = WFormGet( form );
    WFormRelease( form, 0 );
    WDeactive( wpath );
    if( code >= 0 ) return( type );
    return( code );
}
/*--------------------------------------------------------------------------*/
static VOID pktType( VOID )
{
    CHAR         ext[_MAX_EXT];

    _splitpath( inputName, NULL, NULL, NULL, ext );
    if( ext[0] == 0 ) strcat( inputName, ".pkt" );
    type = FILE_PKT;
}
/*--------------------------------------------------------------------------*/
static VOID msgType( VOID )
{
    CHAR         ext[_MAX_EXT];
    
    _splitpath( inputName, NULL, NULL, NULL, ext );
    if( ext[0] == 0 ) strcat( inputName, ".msg" );
    type = FILE_MSG;
}
/*--------------------------------------------------------------------------*/
static VOID txtType( VOID )
{
    CHAR         ext[_MAX_EXT];
    
    _splitpath( inputName, NULL, NULL, NULL, ext );
    if( ext[0] == 0 ) strcat( inputName, ".txt" );
    type = FILE_TXT;
}
/*--------------------------------------------------------------------------*/
APIRET EnterPath( CHAR *title, CHAR *prompt, CHAR name[] )
{
    APIRET       rc;
    INT          lenX = maxXpath - 2;
    SHORT        bunp, bf10, besc;
    FORM         form;
    
    SetBar( BarNull );

    bunp = bf10 = besc = 0;
    
    WSetCurColor( wpath, C_MORE );
    WHLine      ( wpath, 0, 2, maxXpath, 0 );
    WSetXY      ( wpath, 1, 0 );
    WPrintf     ( wpath, "%-*.*s", lenX, lenX, prompt );
    
    form = WFormDefine( wpath );
    WFormString( form, 1, 1, "", name, NULL, _MAX_PATH, lenX, NULL, NOCONTROL, 0 );
    
    WSetTitle( wpath, title );
    WFormButton( form, 14, 3, "[ Unpack ]",   &bunp, NULL, NOCONTROL, BUT_OK );
    WFormButton( form, 27, 3, "[ F10-Tree ]", &bf10, NULL, NOCONTROL, FLD_HIDDEN );
    WFormButton( form, 42, 3, "[ Cancel ]",   &besc, NULL, NOCONTROL, BUT_CANCEL );
    
    WActive( wpath );
    rc = WFormGet( form );
    WFormRelease( form, 0 );
    WDeactive( wpath );
    return( rc );
}
/*-------------------------------------------------------------------------*/
INT GetOk( CHAR *str1, CHAR *str2 )
{
    WMENU        menu;
    INT          code;
    
    SetBar( BarNull );

    WSetCurColor( wdel, C_MORE );
    WClear      ( wdel );
    WHLine      ( wdel, 0, 2, maxXdel, 0 );
    
    WCPuts( wdel, 0, str1 );
    WCPuts( wdel, 1, str2 );
    
    WActive( wdel );
    
    menu = WMenuDefine( wdel, M_HORIZ );
    WMenuItem( menu, 14, 3, "[ Ok ]",     'Y', 0, MB_CLOSE, NULL );
    WMenuItem( menu, 23, 3, "[ Cancel ]", 'N', 1, MB_CLOSE, NULL );
    code = WMenuGet( menu, 0 );
    WMenuRelease( menu );
    WDeactive( wdel );
    return( code );
}
/*-------------------------------------------------------------------------*/
INT OverWrite( INT key, CHAR *name, CHAR *act )
{
    INT           code, start;
    WMENU         menu;
    
    SetBar( BarNull );

    WSetCurColor ( wover, C_TEXT );
    WCPuts       ( wover, 1,  ShowPath( name, 50 ));
    WSetTitle    ( wover, act );
    WSetXY       ( wover, 0, 4 ); WClearLine( wover );
    menu = WMenuDefine( wover, M_HORIZ );

    if( key == FILE_PKT || key == FILE_TXT )
    {
        WMenuItem( menu,  8, 4, " Overwrite ", 'O',  FILE_OWER,    MB_CLOSE, NULL );
        WMenuItem( menu, 20, 4, " Append ",    'A',  FILE_APPEND,  MB_CLOSE, NULL );
        WMenuItem( menu, 29, 4, " Cancel ",    'C',  -1,           MB_CLOSE, NULL );
        start = FILE_APPEND;
    }
    if( key == FILE_MSG )
    {
        WMenuItem( menu,  6, 4, " Overwrite ", 'O',  FILE_OWER,    MB_CLOSE, NULL );
        WMenuItem( menu, 18, 4, " All ",       'A',  FILE_ALLOWER, MB_CLOSE, NULL );
        WMenuItem( menu, 24, 4, " NextFree ",  'N',  FILE_LAST,    MB_CLOSE, NULL );
        WMenuItem( menu, 35, 4, " Cancel ",    'C',  -1,           MB_CLOSE, NULL );
        start = FILE_LAST;
    }

    WActive( wover );
    code = WMenuGet( menu, start );
    WMenuRelease( menu );
    WDeactive( wover );
    return( code );
}
/*-------------------------------------------------------------------------*/
VOID ShowWaiting( INT code )
{
    static WINDOW    win;
    
    if( code )
    {
        win = WDefine( x/2-17, y/2-5, x/2+17, y/2+1, 7, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER4, NULL );
        WCPuts( win, 2, "Please wait ..." );
        WActive( win );
    }
    else
        WRelease( win );
}
/*-------------------------------------------------------------------------*/
INT SelectDrive( VOID )
{
    INT          i, j, offset = 1, disks = 0, delta, code;
    UINT         cur, total;
    INT          d[32];
    CHAR         str[_MAX_PATH], *format;
    WMENU        menu;
    
    SetBar( BarNull );

    _dos_getdrive( &cur );
    
    for( i = 1; i <= 32; i++ )
    {
        _dos_setdrive( i, &total );
        _dos_getdrive( &total );
        if( i == total )
        {
            d[i-1] = 1;
            disks++;
        }
        else
            d[i-1] = 0;
    }
    
    _dos_setdrive( cur, &total );
    
    if( disks <= 1 ) return( 1 );
    
    if( disks < 16 )
    {
        delta = 3;
        format = " %c ";
    }
    else
    {
        delta = 2;
        format = "%c ";
    }
    
    i = delta * disks + 3;
    
    if( i < 24 )
    {   i = 24;
        offset = ( i - disks * delta ) / 2;
    }
    
    wdrive = WDefine(  5, y/2-4, 5+i, y/2+2, 15, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER4, " Drive letter " );
    WCPuts ( wdrive, 1, "Choose drive:" );
    WActive( wdrive );
    menu = WMenuDefine( wdrive, M_HORIZ );

    for( i = j = 0; i < 32; i++ )
    {
        if( d[i] == 0 ) continue;
        sprintf( str, " %c ", 'A' + i );
        WMenuItem( menu, offset + j++ * delta, 3, str, 'A' + i, i + 1, MB_CLOSE, NULL );
    }

    while( 1 )
    {
        code = WMenuGet( menu, cur );

        if( code > 0 )
        {
            _dos_setdrive( code, &total );
            _dos_getdrive( &total );
            if( !getcwd( str, _MAX_PATH ))
            {
                WPut( wdrive, '\007' );
                continue;
            }
        }
        break;
    }
    
    WMenuRelease( menu );
    WRelease( wdrive );
    
    return( code );
}
/*-------------------------------------------------------------------------*/
INT GetHeaderAddr( ADDR *addrfrom, ADDR *addrto, CHAR *pass )
{
    INT          code;
    SHORT        bok = 1, besc = 0;
    CHAR         buf[64];
    WINDOW       whead;
    
    From = addrfrom; To = addrto;
    
    SetBar( BarNull );

    strcpy( afrom, FidoAddr2Str( addrfrom, buf )); strcpy( ato, FidoAddr2Str( addrto, buf ));
    
    whead = WDefine( x/2-23, y/2-5, x/2+22, y/2+2, 10, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER4, " Header " );
    ghform = WFormDefine( whead );
    WFormString( ghform,  1, 1, "From    : ", afrom, NULL, sizeof(afrom)-1, sizeof(afrom), NULL, NOCONTROL, 0 );
    WFormString( ghform,  1, 2, "To      : ", ato,   NULL, sizeof(ato)-1,   sizeof(ato),   NULL, NOCONTROL, 0 );
    WFormString( ghform,  1, 3, "Password: ", pass,  NULL, 8,               8,             NULL, NOCONTROL, 0 );
    WFormButton( ghform, 14, 5, "[ Ok ]",     &bok,  NULL, NOCONTROL, BUT_OK );
    WFormButton( ghform, 22, 5, "[ Cancel ]", &besc, NULL, NOCONTROL, BUT_CANCEL );
    WFormSetFunc( ghform, NULL, formRun, NULL );
    
    WActive( whead );
    code = WFormGet( ghform );
    WFormRelease( ghform, 0 );
    WDeactive( whead );
    
    return( code );
}
/*--------------------------------------------------------------------------*/
static VOID formRun( FIELD field )
{
    ADDR        addr;
    CHAR        buf[64];
    
    switch( field )
    {
        case 0:
            if( FidoStr2Addr( afrom, &addr, From ) == TRUE )
                FidoStoreAddr( From, &addr );
            strcpy( afrom, FidoAddr2Str( From, buf ));
            WFormShow( ghform );
            break;
            
        case 1:
            if( FidoStr2Addr( ato, &addr, To ) == TRUE )
                FidoStoreAddr( To, &addr );
            strcpy( ato, FidoAddr2Str( To, buf ));
            WFormShow( ghform );
            break;
            
    }
}
/*-------------------------------------------------------------------------*/
INT GetMarkedMsgs( CHAR *str )
{
    INT          code;
    WINDOW       wsel;
    WMENU        menu;
    SHORT        maxX;

    SetBar( BarNull );

    wsel = WDefine( x/2-28, y/2-4, x/2+28, y/2, 10, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER4, NULL );

    WGetSize( wsel, &maxX, NULL );
    WSetCurColor( wsel, C_MORE );
    WCPuts ( wsel, 0, str );
    WHLine ( wsel, 0, 1, maxX, 0 );
    WActive( wsel );
    menu = WMenuDefine( wsel, M_HORIZ );
    WMenuItem( menu,  5, 2, "[ Marked Msgs ]",  'M',  SEL_MARK, MB_CLOSE, NULL );
    WMenuItem( menu, 22, 2, "[ Current Msg ]",  'C',  SEL_CURR, MB_CLOSE, NULL );
    WMenuItem( menu, 39, 2, "[ Cancel ]",         0,  -1,       MB_CLOSE, NULL );
    code = WMenuGet( menu, SEL_MARK );
    WMenuRelease( menu );
    WRelease ( wsel );
    return( code );
}
/*-------------------------------------------------------------------------*/
INT ShowInfo( CHAR *name )
{
    CHAR        full[_MAX_PATH];
    
    SetBar( BarNull );

    winfo = WDefine( x/2-36, y/2-5, x/2+34, y/2+6, 16, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER2, " Info " );

    WPutbs( winfo, 2, 48, WGetSchColor( winfo, C_TITLE ), " Press any key ... " );
    WSetCurColor( winfo, C_HIDE );
    WPrints     ( winfo, 1, 0, "Type:" );
    WPrints     ( winfo, 1, 1, "Name:" );
    WPrints     ( winfo, 1, 2, "Size:" );

    if( mode == FILE_TYPE_MSG )
    {
        WPrints( winfo, 1, 4, "Count read :" );
        WPrints( winfo, 1, 5, "Count reply:" );
        WPrints( winfo, 1, 6, "Next reply :" );
        WPrints( winfo, 1, 7, "Cost       :" );

        WSetCurColor( winfo, C_TEXT );
        WSetXY( winfo,  7, 0 ); WPrintf( winfo, "Msg, FSC-0001 Stored Message" );
        WSetXY( winfo, 14, 4 ); WPrintf( winfo, "%u", msg.count_read );
        WSetXY( winfo, 14, 5 ); WPrintf( winfo, "%u", msg.reply );
        WSetXY( winfo, 14, 6 ); WPrintf( winfo, "%u", msg.next_reply );
        WSetXY( winfo, 14, 7 ); WPrintf( winfo, "%u", msg.cost );
        WSetXY( winfo,  7, 2 ); WPrintf( winfo, "%sb", StrSkipSpace( ViewNum( Current -> size, 9 )));
    }

    if( mode == FILE_TYPE_PKT )
    {
        INT          i;
        USHORT       id = (USHORT)(( pkt.product_hi << 8 ) | pkt.product_low );
        USHORT       CapValid = (pkt.CapValid >> 8) | (pkt.CapValid << 8);
        SHORT        code, len;
        CHAR         statusCW[32] = { "Ok" }, *fts = "Unknown", *type, name[64];
        CHAR         c[5];
        FILE        *file;
        
        if(( pkt.origZone == pkt.orig_zone && pkt.destZone == pkt.dest_zone ) ||
           pkt.orig_net == -1 )
            type = "Type 2+ (FSC-0048)";
        else
            type = "Type 2 (FSC-0039)";
            
        if(( file = _fsopen( FtsCFile, "rb", SH_DENYWR )) != NULL )
        {
            fseek( file, 2, SEEK_SET );

            while( 1 )
            {
                len = 0;
                fread( &len, 1, 1, file );
                if( len == 0 ) break;
                fread( &code, 2, 1, file ); fread( name, len - 3, 1, file );
                if( code == id )
                {
                    fts = name;
                    break;
                }
            }
        }
        else
            fts = "file 'ftscprod.fe' not found";
        
        if( pkt.CapWord != CapValid )
            sprintf( statusCW, "%04hX - Error!", CapValid );
            
        WPrints( winfo, 1, 4, "FTSC product type    :" );
        WPrints( winfo, 1, 5, "Product revision     :" );
        WPrints( winfo, 1, 6, "Capability Word      :" );
        WPrints( winfo, 1, 7, "CW Validation Copy   :" );
        WPrints( winfo, 1, 8, "Product specific data:" );

        WSetCurColor( winfo, C_TEXT );
        WSetXY( winfo,  7, 0 ); WPrintf( winfo, type );
        WSetXY( winfo, 24, 4 ); WPrintf( winfo, "%04hX, '%s'", id, fts );
        WSetXY( winfo, 24, 5 ); WPrintf( winfo, "%u.%u", pkt.prod_rev_low, pkt.prod_rev_hi );
        WSetXY( winfo, 24, 6 ); WPrintf( winfo, "%04hX", CapValid );
        WSetXY( winfo, 24, 7 ); WPrintf( winfo, "%s", statusCW );
        WSetXY( winfo, 24, 8 ); WPrintf( winfo, "%08lX", pkt.ProdData );
        c[0] =   pkt.ProdData & 0xff;
        c[1] = ( pkt.ProdData >> 8 ) & 0xff;
        c[2] = ( pkt.ProdData >> 16 ) & 0xff;
        c[3] = ( pkt.ProdData >> 24 ) & 0xff;
        c[4] = 0;
        for( i = code = 0; i < 4; i++ )
            if( !isprint( c[i] ))
                code = 1;
        if( !code ) WPrintf( winfo, ", '%s'", c );
        WSetXY( winfo, 7, 2 ); WPrintf( winfo, "%sb", StrSkipSpace( ViewNum( filelength( fileno( PktFile )), 9 )));
    }
    
    _fullpath( full, name, _MAX_PATH );
    WSetXY( winfo, 7, 1 ); WPrintf( winfo, "%s", ShowPath( full, 50 ));

    WActive( winfo );

    MGetKey( FALSE );
    WRelease( winfo );

    return( 0 );
}
/*-------------------------------------------------------------------------*/
VOID ShowWriteCount( INT count )
{
    if( !count )
    {
        WActive( wproc );
        WCPuts ( wproc, 2, "Writing message:     " );
    }

    if( count == -1 )
    {
        WDeactive( wproc );
        return;
    }

    if( !( count % 5 ))
    {
        WSetXY ( wproc, 30, 2 );
        if( !count )
            WPrintf( wproc, "%4u", 1 );
        else
            WPrintf( wproc, "%4u", count );
    }
}
/*-------------------------------------------------------------------------*/

#endif
