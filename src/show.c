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
 * Module: show.c         Use in: Full - [X]
 *                                Lite - [X]
 *---------------------------------------------------------------------------
 */

#include "pktview.h"

/*-------------------------------------------------------------------------*/
static VOID  ViewAttr          ( VOID );
static VOID  ViewSize          ( VOID );
static VOID  ViewHeader        ( CHAR *file );
static VOID  ViewSelected      ( VOID );
static VOID  ViewPage          ( INT maxX, INT maxY, INT line );
static INT   ViewStr           ( WINDOW win, INT y, INT maxX, CHAR *str, INT color );
static INT   SetAttr           ( VOID );
static INT   ReindexPkt        ( VOID );
/*-------------------------------------------------------------------------*/
static VOID  before            ( INT line );
static KEY   filter            ( KEY key );
static CHAR *builder           ( INT line );
/*-------------------------------------------------------------------------*/
static INT       maxN,  maxA;
static INT       pktNumber;
static MLIST    *list;
static CHAR     *filename;
#ifndef _LITE_
  static WINDOW  wattr;
#endif
/*-------------------------------------------------------------------------*/
INT Select( CHAR *name )
{
    KEY          k;
    INDEXPKT    *index;

    filename = name;

    WClear  ( wtext );

    WActive ( wpkt   );
    WActive ( wlist  );
    WActive ( wsubj  );
    WActive ( wbar   );
#ifndef _LITE_
    SetBar( BarList  );
#endif
    if( Quick )
    {
        WClear  ( wquick );
        WActive ( wquick );
    }

    HeaderFrom.Zone  = pkt.orig_zone;

    if( pkt.orig_point && pkt.orig_net == -1 )
        HeaderFrom.Net = pkt.AuxNet;
    else
        HeaderFrom.Net = pkt.orig_net;

    HeaderFrom.Node  = pkt.orig_node;
    HeaderFrom.Point = pkt.orig_point;
    HeaderFrom.Domain[0] = 0;

    HeaderTo.Zone  = pkt.dest_zone;
    HeaderTo.Net   = pkt.dest_net;
    HeaderTo.Node  = pkt.dest_node;
    HeaderTo.Point = pkt.dest_point;
    HeaderTo.Domain[0] = 0;

    /* max len "From", "To", "Area" */
    if(( maxN = ( maxXlist * 25 ) / 80 ) > 40 )
        maxN = 40;
    maxA = maxXlist - 9 - 2 * maxN;

    ViewPktHeader( &HeaderFrom, &HeaderTo, pkt.password );

#ifndef _LITE_
    ViewSelCount ();
#endif
    
    for( index = pktIndex; index; index = index -> next )
        MListAdd( &list, 0, NULL, NULL );

    popup = WPDefine( wlist, list, 1 );
    WPSetFunc( popup, before, NULL, filter, builder );

    if( autoView )
    {
        k.key = KEY_ENTER;
        MPutKey( k );
    }
        
    WPActive ( popup );
    WPRelease( popup, FALSE );
    MListFree( &list );
#ifndef _LITE_
    CheckUpdatePkt( filename );
#endif
    CloseFile();

    return( KEY_ESC );
}
/*-------------------------------------------------------------------------*/
VOID ViewPktHeader( ADDR *from, ADDR *to, CHAR *pass )
{
    INT          i;
    CHAR         buf[64];
    static CHAR  str[140];

    if(( i = maxXpkt - 32 ) > 64 )
        i = 64;
    
    str[0] = 0;
    
    WSetCurColor( wpkt, C_TEXT );
    if( mode == FILE_TYPE_PKT )
        strncpy( str, FidoAddr2Str( from, buf ), i - 14 );
    WSetXY ( wpkt, 13, 0 );
    WPrintf( wpkt, "%-*.*s", i - 14, i - 14, str );
    if( mode == FILE_TYPE_PKT )
        strncpy( str, FidoAddr2Str( to, buf ), i - 14 );
    WSetXY ( wpkt, 13, 1 );
    WPrintf( wpkt, "%-*.*s", i - 14, i - 14, str );
    WSetXY ( wpkt, i + 11, 0 );

    if( mode == FILE_TYPE_PKT )
        WPrintf( wpkt, "%2u %s %04u  %02u:%02u:%02u",
                pkt.day, month[pkt.month%12], pkt.year,
                pkt.hour, pkt.minute, pkt.second );

    if( mode == FILE_TYPE_MSG )
        WPrintf( wpkt, "%-*.*s", 21, 21, str );

    if( mode == FILE_TYPE_PKT )
        strncpy( str, pass[0] ? pass : "<none>", 8 );
    WSetXY( wpkt, i + 11, 1 ); WPrintf( wpkt, "%-8.8s", str );

    if( BadPkt )
    {
        WPutbs( wpkt, 0, 2, WGetSchColor( wpkt, C_BORDER ), "´ Ã" );
        WPutbs( wpkt, 0, 3, WGetSchColor( wpkt, C_MORE   ),  "b"  );
    }
    else
        WPutbs( wpkt, 0, 2, WGetSchColor( wpkt, C_BORDER ), "ÄÄÄ" );

#ifndef _LITE_
    
    if( ReadOnly )
    {
        WPutbs( wpkt, 0, maxXpkt - 4, WGetSchColor( wpkt, C_BORDER ),  "´  Ã" );
        WPutbs( wpkt, 0, maxXpkt - 3, WGetSchColor( wpkt, C_MORE   ),   "ro" );
    }
    else
        WPutbs( wpkt, 0, maxXpkt - 4, WGetSchColor( wpkt, C_BORDER ), "ÄÄÄÄ" );
#endif
}
/*-------------------------------------------------------------------------*/
static KEY filter( KEY k )
{
    INT         code;

    if( k.scan == 9 )  k.key = KEY_ASTERISK;
    if( k.scan == 55 ) k.key = KEY_ASTERISK;

    switch( k.key )
    {
#ifndef _LITE_
        
        case KEY_F1:
            ViewHelp( "PktSelect" );
            SetBar( BarList  );
            break;
                
        case KEY_F2:
            if( !Update )
            {
                k.key = 0;
                break;
            }
            SetBar( BarNull );
            if( !UpdatePkt( filename ))
                Update = 0;
            if( PktFile == NULL )
            {
                k.key = KEY_ESC;
                break;
            }
            SetBar( BarList  );
            k.key = 0;
            break;

        case KEY_F3:
            newload = 1;
            k.key = KEY_ESC;
            break;

        case KEY_F4:
            k.key = 0;
            EditHeader();
            SetBar( BarList );
            break;

        case KEY_F5:
            k.key = 0;
            if( !pktcount )
                break;
            CheckUpdatePkt( filename );
            CopyPkt( filename );
            SetBar( BarList );
            break;

        case KEY_F6:
            k.key = 0;
            if( !pktcount )
                break;
            CheckUpdatePkt( filename );
            code = MovePkt( filename );
            SetBar( BarList );
            if( code )
                break;
            if( mode == FILE_TYPE_MSG )
                k.key = ReindexPkt();
            if( mode == FILE_TYPE_PKT )
            {
                reload = 1;
                k.key = KEY_ESC;
            }
            break;

        case KEY_F8: case KEY_DEL:
            k.key = 0;
            if( !pktcount )
                break;
            code = DelPkt( filename );
            SetBar( BarList );
            if( code )
                break;
            if( mode == FILE_TYPE_MSG )
                k.key = ReindexPkt();
            if( mode == FILE_TYPE_PKT )
            {
                reload = 1;
                k.key = KEY_ESC;
            }
            break;

        case KEY_SPACE: case KEY_INS:
            if( !pktcount )
            {
                k.key = 0;
                break;
            }
            if( Current -> sel != ' ' )
            {   Current -> sel = ' ';
                selected--;
            }
            else
            {   Current -> sel = '*';
                selected++;
            }
            ViewSelCount();
            WPShow( popup, FALSE, TRUE, -1, -1 );
            k.key = KEY_DOWN;
            break;
            
        case KEY_s: case KEY_S: case ALT_S:
            if( cfgCurrent.sort && pktcount > 1 )
            {
                PktSort();
                SetBar( BarList );
                before( pktNumber );
            }
            k.key = 0;
            break;
            
        case KEY_i: case KEY_I: case ALT_I:
            k.key = 0;
            if( mode == FILE_TYPE_MSG )
                break;
            ShowInfo( filename );
            SetBar( BarList );
            break;
            
        case KEY_PLUS:
            if( pktcount > 1 )
            {
                PktSelect();
                SetBar( BarList );
                ViewSelCount();
                WPShow( popup, FALSE, TRUE, -1, -1 );
            }
            k.key = 0;
            break;
            
        case KEY_MINUS:
            if( pktcount > 1 )
            {
                PktUnSelect();
                SetBar( BarList );
                ViewSelCount();
                WPShow( popup, FALSE, TRUE, -1, -1 );
            }
            k.key = 0;
            break;
            
        case KEY_ASTERISK:
            if( pktcount > 1 )
            {
                MsgInvert();
                ViewSelCount();
                WPShow( popup, FALSE, TRUE, -1, -1 );
            }
            k.key = 0;
            break;

        case KEY_F9:
            ChangeConfig();
            SetBar( BarList );
            if( altx )
                k.key = KEY_ESC;
            else
                k.key = 0;
            break;
            
        case SHIFT_F9:
            SaveConfig( ConfFile );
            k.key = 0;
            break;

#endif

        case KEY_ENTER: case KEY_GRENTER: case KEY_RIGHT: case KEY_LEFT:
            if( !pktcount )
            {
                k.key = 0;
                break;
            }
            if( mode == FILE_TYPE_MSG )
            {
                filename = Current -> name;
                code = 1;
            }

            if( mode == FILE_TYPE_PKT )
                code = 0;
            
            if( ReadText( code ))
            {
                k.key = KEY_ESC;
                break;
            }

            k.key = ViewMsg( filename, 1 );

            FreeTxtIndex();

            if( reload )
            {
                k.key = KEY_ESC;
                break;
            }

            switch( k.key )
            {
                case KEY_LEFT:
                    k.key = KEY_ENTER;
                    MPutKey( k );
                    k.key = KEY_UP;
                    break;

                case KEY_RIGHT:
                    k.key = KEY_ENTER;
                    MPutKey( k );
                    k.key = KEY_DOWN;
                    break;

                case KEY_F3:
                    newload = 1;
                    k.key = KEY_ESC;
                    break;

                case KEY_ESC: case KEY_F10:
#ifndef _LITE_
                    BarList[4].status = BarList[5].status = BarList[7].status= 1;
                    SetBar( BarList );
                    ViewSelCount();
#endif
                    if( altx )
                        k.key = KEY_ESC;
                    else
                        k.key = 0;
                    break;
            }
            return( k );

        case KEY_ESC: case KEY_F10:
            k.key = KEY_ESC;
            break;

        case ALT_X:
            altx = 1;
            k.key = KEY_ESC;
            break;
    }
    return( k );
}
/*-------------------------------------------------------------------------*/
#ifndef _LITE_
static INT ReindexPkt( VOID )
{
    INDEXPKT    *index;

    if( !pktcount )
    {
        reload = 1;
        return( KEY_ESC );
    }
    else
    {
        MListFree( &list );
        for( index = pktIndex; index; index = index -> next )
            MListAdd( &list, 0, NULL, NULL );
        WPRedefine( popup, list );

        before( pktNumber );
    }
    return( 0 );
}
/*-------------------------------------------------------------------------*/
VOID ViewUpdate( VOID )
{
    if( Update )
    {
        WPutbs ( wpkt, 2, 2, WGetSchColor( wpkt, C_BORDER ), "µ Æ" );
        WPutbs ( wpkt, 2, 3, WGetSchColor( wpkt, C_SELHOT ),  "u"  );
        BarList[1].status = 1;
    }
    else
    {
        WPutbs( wpkt, 2, 2, WGetSchColor( wpkt, C_BORDER ), "ÍÍÍ" );
        BarList[1].status = 0;
    }
}
/*-------------------------------------------------------------------------*/
VOID ViewSelCount( VOID )
{
    INT          i;
    static CHAR  sel[21];
    
    if( selected )
    {
        sprintf( sel, " Selected: %u ", selected );
        i = strlen( sel );
        WPutbs ( wsubj, 0, 6, WGetSchColor( wsubj, C_SELHOT ), sel );
        memset ( sel, 'Í', 20 - i ); sel[ 20 - i ] = 0;
        WPutbs ( wsubj, 0, 6 + i, WGetSchColor( wsubj, C_BORDER ), sel );
    }
    else
    {
        memset ( sel, 'Í', 20 ); sel[ 20 ] = 0;
        WPutbs ( wsubj, 0, 2, WGetSchColor( wsubj, C_BORDER ), sel );
    }
}
#endif
/*-------------------------------------------------------------------------*/
static VOID before( INT line )
{
    INT          i;
    INDEXPKT    *index;
    static CHAR  str[140], tmp[140];

    if( !pktcount )
        return;

    pktNumber = line;
    Current = index = GetCurrentPkt( line );

#ifndef _LITE_
    ViewUpdate();
#endif
    
    strncpy( tmp, index -> subj ? index -> subj : "", maxXsubj - 6 );
    WSetCurColor( wsubj, C_TITLE );
    WSetXY      ( wsubj, 6, 0 );
    sprintf     ( str, "%-*.*s", maxXsubj - 6, maxXsubj - 6, tmp );
    WPutstr     ( wsubj, str );

    for( i = 0; Quick && i < MAX_LINE; i++ )
        ViewStr( wquick, i, maxXquick, index -> quick[i], index -> color[i] );
}
/*-------------------------------------------------------------------------*/
static CHAR *builder( INT line )
{
    INDEXPKT    *index;
    MLIST       *l;
    static CHAR  from[64], to[64], area[64];
    static CHAR  str [140];

    index = GetCurrentPkt ( line );
    l     = GetCurrentList( list, line );

    if( mode == FILE_TYPE_MSG )
    {
        strcpy( area, index -> date );
        line = index -> number;
    }

    if( mode == FILE_TYPE_PKT )
    {
        strncpy( area, index -> area ? index -> area : "<Netmail>", maxA );
        line++;
    }
        
    strncpy( from, index -> from, maxN );
    strncpy( to,   index -> to,   maxN );

    sprintf( str, "%5hu %c %-*.*s %-*.*s %-*.*s",
                 line, index -> sel, maxN, maxN, from,
                                     maxN, maxN, to,
                                     maxA, maxA, area );
    if( index -> sel != ' ' )
        l -> sel = 1;
    else
        l -> sel = 0;

    return( str );
}
/*-------------------------------------------------------------------------*/
INT ViewMsg( CHAR *file, INT renew )
{
    INT              y = 0;
    KEY              k;
    INDEXTXT        *p;
#ifndef _LITE_
    INT              code;
#endif

    WActive ( wmain );
    WActive ( wtext );
    WActive ( wbar  );
#ifndef _LITE_
    SetBar  ( BarView );
#endif
    if( renew )
    {
        ViewHeader( file );
        ViewPage  ( maxXtext, maxYtext, 0 );
    }

    while( 1 )
    {
        k = MGetKey( FALSE );

        switch( k.key )
        {
#ifndef _LITE_
            
            case KEY_F1:
                ViewHelp( "PktView" );
                SetBar( BarView );
                break;

            case KEY_F5:
                CheckUpdatePkt( file );
                CopyPkt( file );
                SetBar( BarView );
                break;
                
            case KEY_F6:
                CheckUpdatePkt( file );
                code = MovePkt( file );
                SetBar( BarView );
                if( code ) break;
                if( mode == FILE_TYPE_MSG )
                {
                    ReindexPkt();
                    WDeactive( wmain );
                    WDeactive( wtext );
                    return( KEY_ESC );
                }
                if( mode == FILE_TYPE_PKT )
                {
                    reload = 1;
                    return( KEY_ESC );
                }
                return( 0 );
                
            case KEY_F8: case KEY_DEL:
                code = DelPkt( file );
                SetBar( BarView );
                if( code ) break;
                if( mode == FILE_TYPE_MSG )
                {
                    ReindexPkt();
                    WDeactive( wmain );
                    WDeactive( wtext );
                    return( KEY_ESC );
                }
                if( mode == FILE_TYPE_PKT )
                {
                    reload = 1;
                    return( KEY_ESC );
                }
                return( 0 );

            case KEY_i: case KEY_I: case ALT_I:
                ShowInfo( file );
                SetBar( BarView );
                break;
                
            case KEY_SPACE: case KEY_INS:
                if( pktIndex == NULL )
                    break;
                if( Current -> sel != ' ' )
                {
                    Current -> sel = ' ';
                    selected--;
                }
                else
                {
                    Current -> sel = '*';
                    selected++;
                }
                ViewSelected();
                break;
                
            case KEY_F3:
                return( KEY_F3 );
                
            case ALT_C:
                SetAttr();
                SetBar( BarView );
                break;
                
            case KEY_F9:
                ChangeConfig();
                SetBar( BarView );
                if( altx )
                {
                    WDeactive( wmain );
                    WDeactive( wtext );
                    return( KEY_ESC );
                }
                break;
                
            case SHIFT_F9:
                SaveConfig( ConfFile );
                break;

#endif

            case KEY_HOME:
                if( y == 0 )
                    break;
                y = 0;
                ViewPage( maxXtext, maxYtext, y );
                break;

            case KEY_DOWN:
                if( txtcount < y + maxYtext + 1 )
                    break;
                y++;
                WSetXY( wtext, 0, 0 ); WDelLine( wtext );
                p = GetCurrentTxt( y + maxYtext - 1 );
                ViewStr ( wtext, maxYtext - 1, maxXtext, p -> str, p -> color );
                break;

            case KEY_UP:
                if( y == 0 )
                    break;
                y--;
                WSetXY( wtext, 0, 0 ); WInsLine( wtext );
                p = GetCurrentTxt( y );
                ViewStr ( wtext, 0, maxXtext, p -> str, p -> color );
                break;

            case KEY_PAGEUP:
                if( y == 0 )
                    break;
                if( y < maxYtext - 1 )
                    y = 0;
                else
                    y -= maxYtext - 1;
                ViewPage( maxXtext, maxYtext, y );
                break;

            case KEY_PAGEDOWN:
                if( txtcount < maxYtext )
                    break;
                if( y + 2 * maxYtext < txtcount )
                    y += maxYtext - 1;
                else
                    y  = txtcount - maxYtext;
                ViewPage( maxXtext, maxYtext, y );
                break;

            case KEY_END:
                if( txtcount < maxYtext )
                    break;
                y = txtcount - maxYtext;
                ViewPage( maxXtext, maxYtext, y );
                break;

            case KEY_LEFT: case CTRL_ENTER: case SHIFT_TAB:
                if( pktcount > 1 && pktNumber > 0 )
                    return( KEY_LEFT );
                break;

            case KEY_RIGHT: case KEY_ENTER: case KEY_GRENTER: case KEY_TAB:
                if( pktcount > 1 && pktNumber + 1 != pktcount )
                    return( KEY_RIGHT );
                break;

            case ALT_X:
                altx = 1;
                
            case KEY_ESC: case KEY_F10: case KEY_L: case KEY_l: case ALT_L:
                WDeactive( wmain );
                WDeactive( wtext );
                return( KEY_ESC );

        }
    }
}
/*-------------------------------------------------------------------------*/
#ifndef _LITE_
INT SetAttr( VOID )
{
    INT              attr, done, code;
    KEY              k;
    PACKET           packet;
    
    SetBar( BarNull );

    wattr = WDefine( 0, 6, 32, 23, 12, W_NOSCROLL | W_NOCURSOR | W_SHADOW, BORDER2, " Attributes " );

    WPrints ( wattr, 1,  0, "Private             <Alt-P>" );
    WPrints ( wattr, 1,  1, "Crash               <Alt-C>" );
    WPrints ( wattr, 1,  2, "Received            <Alt-R>" );
    WPrints ( wattr, 1,  3, "Sent                <Alt-S>" );
    WPrints ( wattr, 1,  4, "File Attach         <Alt-A>" );
    WPrints ( wattr, 1,  5, "Transit             <Alt-J>" );
    WPrints ( wattr, 1,  6, "Orphan              <Alt-O>" );
    WPrints ( wattr, 1,  7, "Kill/Sent           <Alt-K>" );
    WPrints ( wattr, 1,  8, "Local               <Alt-W>" );
    WPrints ( wattr, 1,  9, "Hold                <Alt-H>" );
    WPrints ( wattr, 1, 10, "File Request        <Alt-F>" );
    WPrints ( wattr, 1, 11, "Return Rcpt Request <Alt-M>" );
    WPrints ( wattr, 1, 12, "Return Rcpt         <Alt-N>" );
    WPrints ( wattr, 1, 13, "Audit Request       <Alt-Q>" );
    WPrints ( wattr, 1, 14, "File Update Request <Alt-U>" );
    WPrints ( wattr, 1, 15, "Zap all attribs     <Alt-Z>" );

    WActive( wattr );
    attr = Current -> attr;
    done = 0;
    while( !done )
    {
        ViewAttr();
        
        k = MGetKey( FALSE );
        
        switch( k.key )
        {
            case ALT_P: Current -> attr ^= PKT_PVT; break;
            case ALT_C: Current -> attr ^= PKT_CRA; break;
            case ALT_R: Current -> attr ^= PKT_RCV; break;
            case ALT_S: Current -> attr ^= PKT_SNT; break;
            case ALT_A: Current -> attr ^= PKT_ATT; break;
            case ALT_J: Current -> attr ^= PKT_TRS; break;
            case ALT_O: Current -> attr ^= PKT_ORP; break;
            case ALT_K: Current -> attr ^= PKT_KS;  break;
            case ALT_W: Current -> attr ^= PKT_LOC; break;
            case ALT_H: Current -> attr ^= PKT_HLD; break;
            case ALT_F: Current -> attr ^= PKT_FRQ; break;
            case ALT_M: Current -> attr ^= PKT_RRQ; break;
            case ALT_N: Current -> attr ^= PKT_RRC; break;
            case ALT_Q: Current -> attr ^= PKT_ARQ; break;
            case ALT_U: Current -> attr ^= PKT_URQ; break;
            case ALT_Z: Current -> attr  = 0;       break;
                
            case KEY_ESC: case KEY_ENTER: case KEY_GRENTER: case KEY_F10:
                done = 1;
                break;
        }
    }
    
    WRelease( wattr );
    
    if( k.key != KEY_ESC && attr != Current -> attr )
    {
        PktUpdate = 1;
        if( mode == FILE_TYPE_PKT )
        {
            FidoMsgGetStrSeek( PktFile, Current -> tell, SEEK_SET );
            if( fread ( &packet, sizeof( packet ), 1, PktFile ) != 1 )
                ShowError( "Error read file '%s'", ShowPath( netfile, 50 ));
            else
            {
                packet.attr = Current -> attr;
                FidoMsgGetStrSeek( PktFile, Current -> tell, SEEK_SET );
                if( fwrite( &packet, sizeof( packet ), 1, PktFile ) != 1 )
                    ShowError( "Error write to file '%s'", ShowPath( netfile, 50 ));
            }
        }
        if( mode == FILE_TYPE_MSG )
        {
            if(( code = OpenFile( Current -> name )) != 0 )
                return( code );
            if( fread( &msg, sizeof( msg ), 1, PktFile ) != 1 )
                ShowError( "Error read file '%s'", ShowPath( netfile, 50 ));
            else
            {
                msg.attrib = Current -> attr;
                fseek ( PktFile, 0, SEEK_SET );
                if( fwrite( &msg, sizeof( msg ), 1, PktFile ) != 1 )
                    ShowError( "Error write to file '%s'", ShowPath( netfile, 50 ));
            }
            CloseFile();
        }
    }
    else
        Current -> attr = attr;

    ViewAttr();

    return( 0 );
}
#endif
/*-------------------------------------------------------------------------*/
VOID ViewHeader( CHAR *file )
{
    INT          i, m;
    CHAR         buf[64];
    static CHAR  str[140], tmp[140];

    _splitpath( file, NULL, NULL, fname, ext );

    if( pktcount )
        sprintf( str, "%u - %u  (%s%s)", pktNumber + 1, pktcount, fname, ext );
    else
        sprintf( str,  "1 - 1  (%s%s)", fname, ext );

    i = strlen( str );
    WSetCurColor( wmain, C_TEXT );
    WSetXY      ( wmain,  6, 0 );
    WPuts       ( wmain, str );
    memset      ( str, ' ', 30 - i ); str[ 30 - i ] = 0;
    WSetXY      ( wmain,  6 + i, 0 );
    WPuts       ( wmain, str );
    
    if(( m = maxXmain - 32 ) > 64 )
        m = 64;

    strncpy( tmp, Current -> from, m - 7 ); WSetXY( wmain, 6, 1 );
    sprintf( str,  "%-*.*s", m - 7, m - 7, tmp ); WPutstr( wmain, str );
    
    strncpy( tmp, Current -> to, m - 7 ); WSetXY( wmain, 6, 2 );
    sprintf( str,  "%-*.*s", m - 7, m - 7, tmp ); WPutstr( wmain, str );

    strncpy( tmp, Current -> subj, maxXsubj - 6 ); WSetXY( wmain, 6, 3 );
    sprintf( str,  "%-*.*s", maxXsubj - 6, maxXsubj - 6, tmp ); WPutstr( wmain, str );

    if( Current -> area )
    {
        strcpy ( str, " " );
        strncat( str, Current -> area, 60 );
        strcat ( str, " " );
    }
    else
        strcpy( str, " Netmail " );

    i = strlen( str );
    WPutbs ( wmain, 0,  6, WGetSchColor( wmain, C_MORE ), str );
    memset ( str, 'Ä', 60 - i ); str[ 60 - i ] = 0;
    WPutbs ( wmain, 0, 6 + i, WGetSchColor( wmain, C_BORDER ), str );

    if( BadPkt )
    {
        WPutbs( wmain, 0, 2, WGetSchColor( wmain, C_BORDER ), "´ Ã" );
        WPutbs( wmain, 0, 3, WGetSchColor( wmain, C_MORE   ),  "b"  );
    }
    else
        WPutbs( wmain, 0, 2, WGetSchColor( wmain, C_BORDER ), "ÄÄÄ" );
    
    if( ReadOnly )
    {
        WPutbs( wmain, 0, maxXmain - 4, WGetSchColor( wmain, C_BORDER ),  "´  Ã" );
        WPutbs( wmain, 0, maxXmain - 3, WGetSchColor( wmain, C_MORE   ),   "ro" );
    }
    else
        WPutbs( wmain, 0, maxXmain - 4, WGetSchColor( wmain, C_BORDER ), "ÄÄÄÄ" );

    WPrints ( wmain, m + 11, 0, Current -> date );

    strncpy( str, FidoAddr2Str( &Current -> AddrFrom, buf ), maxXmain - m );
    WSetXY ( wmain, m, 1 );
    WPrintf( wmain, "%-*.*s", maxXmain - m, maxXmain - m, str );

    if( Current -> area )
        WSetCurColor( wmain, C_HIDE );

    strncpy( str, FidoAddr2Str( &Current -> AddrTo, buf ), maxXmain - m );
    WSetXY( wmain, m, 2 );
    WPrintf( wmain, "%-*.*s", maxXmain - m, maxXmain - m, str );

#ifndef _LITE_
    ViewSelected();
#endif
    ViewSize();
    ViewAttr();
}
/*-------------------------------------------------------------------------*/
#ifndef _LITE_
VOID ViewSelected( VOID )
{
    if( Current -> sel != ' ' )
    {
        WPutbs ( wmain, 2,  2, WGetSchColor( wmain, C_BORDER ), "µ Æ" );
        WPutbs ( wmain, 2,  3, WGetSchColor( wmain, C_TEXT ),    "s"  );
    }
    else
        WPutbs ( wmain, 2,  2, WGetSchColor( wmain, C_BORDER ), "ÍÍÍ" );
}
#endif
/*-------------------------------------------------------------------------*/
VOID ViewAttr( VOID )
{
    INT          i;
    INT          attr = Current -> attr;
    static CHAR  attrib[140];

    if( attr )
    {
        strcpy( attrib, " " );

        if( attr & PKT_PVT ) strcat( attrib, "Pvt " );
        if( attr & PKT_CRA ) strcat( attrib, "Cra " );
        if( attr & PKT_RCV ) strcat( attrib, "Rcv " );
        if( attr & PKT_SNT ) strcat( attrib, "Snt " );
        if( attr & PKT_ATT ) strcat( attrib, "Att " );
        if( attr & PKT_TRS ) strcat( attrib, "Trs " );
        if( attr & PKT_ORP ) strcat( attrib, "Orp " );
        if( attr & PKT_KS  ) strcat( attrib, "K/S " );
        if( attr & PKT_LOC ) strcat( attrib, "Loc " );
        if( attr & PKT_HLD ) strcat( attrib, "Hld " );
        if( attr & PKT_RES ) strcat( attrib, "Res " );
        if( attr & PKT_FRQ ) strcat( attrib, "Frq " );
        if( attr & PKT_RRQ ) strcat( attrib, "Rrq " );
        if( attr & PKT_RRC ) strcat( attrib, "Rrc " );
        if( attr & PKT_ARQ ) strcat( attrib, "Arq " );
        if( attr & PKT_URQ ) strcat( attrib, "Urq " );

        i = strlen( attrib );
        WPutbs ( wmain, 2,  17, WGetSchColor( wmain, C_MORE ), attrib );
        memset ( attrib, 'Í', 62 - i ); attrib[ 62 - i ] = 0;
        WPutbs ( wmain, 2, 17 + i, WGetSchColor( wmain, C_BORDER ), attrib );
    }
    else
    {
        memset( attrib, 'Í', 62 ); attrib[62] = 0;
        WPutbs ( wmain, 2,  17, WGetSchColor( wmain, C_BORDER ), attrib );
    }
}
/*-------------------------------------------------------------------------*/
VOID ViewSize( VOID )
{
    INT         i;
    LONG        size = Current -> size;
    CHAR        str[16];

    if( size < 999999 )
    {
        if( size <= 999 )
            sprintf( str, " %lu ", size );
        else
            sprintf( str, " %lu.%03lu ", size / 1000, size % 1000 );
    }
    else
        strcpy( str, " ###### " );
    i = strlen( str );
    WPutbs ( wmain, 2,  6, WGetSchColor( wmain, C_MORE ), str );
    memset ( str, 'Í', 9 - i ); str[ 9 - i ] = 0;
    WPutbs ( wmain, 2, 6 + i, WGetSchColor( wmain, C_BORDER ), str );
}
/*-------------------------------------------------------------------------*/
VOID ViewPage( INT maxX, INT maxY, INT line )
{
    INT              i;
    INDEXTXT        *p;

    p = GetCurrentTxt( line );

    WFreeze();

    for( i = 0; i < maxY && txtcount > line + i; i++ )
    {
        ViewStr( wtext, i, maxX, p -> str, p -> color );
        p = p -> next;
    }
    for( ; i < maxY; i++ )
    {
        WSetXY( wtext, 0, i );
        WClearLine( wtext );
    }
    WUnfreeze();
}
/*-------------------------------------------------------------------------*/
INT  ViewStr( WINDOW win, INT y, INT maxX, CHAR *str, INT color )
{
    INT          i;
    CHAR        *s;
    static CHAR  string[256];

    for( i = 0; str && *str; str++ )
        if( i < maxX )
            string[i++] = *str;
    string[i] = 0;

    WSetCurColor( win, color );

    s = string[0] == '\001' ? &string[1] : string;

    WSetXY( win, 0, y );
    WClearLine( win );
    WPutstr( win, s );

    return( 0 );
}
/*-------------------------------------------------------------------------*/
