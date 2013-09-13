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
 * Module: export.c       Use in: Full - [X]
 *                                Lite - [-]
 *---------------------------------------------------------------------------
 */

#ifndef _LITE_

#include "pktview.h"

/*-------------------------------------------------------------------------*/
static APIRET ExportPkt         ( INT mode, CHAR *str1, CHAR *title, CHAR *netfile );
static APIRET ExportToPkt       ( CHAR *name, INT act );
static APIRET ExportToMsg       ( CHAR *name, CHAR *title );
static APIRET ExportToTxt       ( CHAR *name, INT act );
static APIRET WriteToPkt        ( INT sel );
static APIRET WriteToMsg        ( INDEXPKT *index );
static APIRET SetMsgName        ( CHAR *name, INT code );
static APIRET DeletePktMsg      ( CHAR *nane );
static APIRET AddToPkt          ( CHAR *fromPkt, CHAR *toPkt );
static INT    CalcSel           ( VOID );
static VOID   SaveSel           ( VOID );
static VOID   RestSel           ( VOID );
static APIRET InitMsgPkt        ( INDEXPKT *index );
/*-------------------------------------------------------------------------*/
static CHAR  Password[9];
static ADDR  From, To;
/*-------------------------------------------------------------------------*/
APIRET CopyPkt( CHAR *netfile )
{
    return( ExportPkt( 0, "Copy", " Copy ", netfile ));
}
/*-------------------------------------------------------------------------*/
APIRET MovePkt( CHAR *netfile )
{
    return( ExportPkt( 1, "Move", " Move ", netfile ));
}
/*-------------------------------------------------------------------------*/
static APIRET ExportPkt( INT mode, CHAR *str1, CHAR *title, CHAR *netfile )
{
    APIRET       rc = ERROR_INVALID_FUNCTION;
    INT          sel, mark, key, act = FILE_CREAT;
    CHAR         str[80], name[_MAX_PATH];
    
    SetBar( BarNull );

    sel = CalcSel();
    sprintf( str, "%s from", str1 );
    
    if( sel )
    {
        if(( mark = GetMarkedMsgs( str )) < 0 )
            return( mark );
    }
    else
        mark = SEL_CURR;

    SaveSel();

    if( mark == SEL_CURR )             // current
    {
        sprintf( str, "%s current message to", str1 );
        MsgSelect( 0, 1 );
        ViewSelCount();
        Current -> sel = '>';
        selected = 1;
    }
    
    if( mark == SEL_MARK )             // marked
    {
        if( sel == 1 )  sprintf( str, "%s %u message to",  str1, sel );
        else            sprintf( str, "%s %u messages to", str1, sel );
    }

    WPShow( popup, FALSE, FALSE, -1, -1 );
    CreateName( netfile, name );
    
    if(( key = EnterPathType( title, str, name )) >= 0 )
    {
        if( key == FILE_PKT || key == FILE_TXT )
        {
            if( !access( name, F_OK ))
            {
                act = OverWrite( key, name, title );
                if( act == -1 )
                    key = -1;
            }
        }
    }
    
    switch( key )
    {
        case FILE_PKT:
            rc = ExportToPkt( name, act );
            break;
            
        case FILE_MSG:
            rc = ExportToMsg( name, title );
            break;
            
        case FILE_TXT:
            rc = ExportToTxt( name, act );
            break;
            
        default:
            rc = ERROR_INVALID_FUNCTION;
            break;
    }
    
    if( rc == NO_ERROR && mode ) rc = DeletePktMsg( netfile );
    if( rc != NO_ERROR || mark == SEL_CURR ) RestSel();
    if( rc == NO_ERROR && mark == SEL_MARK ) MsgSelect( 0, 1 );
    
    ViewSelCount();
    CheckReview( name );

    return( rc );
}
/*-------------------------------------------------------------------------*/
APIRET DelPkt( CHAR *netfile )
{
    APIRET       rc = ERROR_INVALID_FUNCTION;
    INT          mark, sel, oper;
    CHAR         str[80];
    
    SetBar( BarNull );

    sel = CalcSel();
    if( sel )
    {
        if(( mark = GetMarkedMsgs( "Delete" )) < 0 )
            return( mark );
    }
    else
        mark = SEL_CURR;
    
    SaveSel();

    if( mark == SEL_CURR )             // current
    {
        strcpy( str, "Current message" );
        MsgSelect( 0, 1 );
        ViewSelCount();
        Current -> sel = '>';
        selected = 1;
    }
    
    if( mark == SEL_MARK )             // marked
    {
        if( sel == 1 )  sprintf( str, "Selected %hu message.",  sel );
        else            sprintf( str, "Selected %hu messages.", sel );
    }
    
    WPShow( popup, FALSE, FALSE, -1, -1 );
    
    if(( oper = GetOk( str, "Delete?" )) == 0 )
        rc = DeletePktMsg( netfile );
    
    RestSel();
    ViewSelCount();
    CheckReview( netfile );
    
    return( rc );
}
/*-------------------------------------------------------------------------*/
static INT CalcSel( VOID )
{
    INT         sel = 0;
    INDEXPKT    *p;
    
    for( p = pktIndex; p; p = p -> next )
        if( p -> sel != ' ' )
            sel++;
    return( sel );
}
/*-------------------------------------------------------------------------*/
static VOID SaveSel( VOID )
{
    INDEXPKT    *p;
    
    for( p = pktIndex; p;  p = p -> next )
        p -> seltmp = p -> sel;
}
/*-------------------------------------------------------------------------*/
static VOID RestSel( VOID )
{
    INDEXPKT    *p;
    
    for( selected = 0, p = pktIndex; p;  p = p -> next )
    {
        p -> sel = p -> seltmp;
        if( p -> sel != ' ' )
            selected++;
    }
}
/*-------------------------------------------------------------------------*/
static APIRET ExportToPkt( CHAR *name, INT act )
{
    APIRET       rc;

    if( act == FILE_CREAT || act == FILE_OWER )
    {
        if( mode == FILE_TYPE_MSG )
            memset( Password, 0, sizeof( Password ));

        if( mode == FILE_TYPE_PKT )
        {
            FidoStoreAddr( &From, &HeaderFrom );
            FidoStoreAddr( &To,   &HeaderTo   );
            strncpy  ( Password,  pkt.password, 8 );
        }
        if( GetHeaderAddr( &From, &To, Password ) < 0 )
            return( ERROR_INVALID_FUNCTION );
    }
    
    rc = WriteToPkt( 1 );

    ShowWriteCount( -1 );

    if( rc != NO_ERROR )
        return( rc );

    switch( act )
    {
        case FILE_APPEND:
            rc = AddToPkt( pkttmp, name );
            break;
            
        case FILE_CREAT: case FILE_OWER:
            if(( rc = FRenFile( pkttmp, name )) != NO_ERROR )
                ShowError( "Can't rename file '%s' to '%s'", pkttmp, ShowPath( name, 40 ));
            break;
    }
    return( rc );
}
/*-------------------------------------------------------------------------*/
static APIRET ExportToMsg( CHAR *name, CHAR *title )
{
    APIRET           rc = NO_ERROR;
    INT              act = 0, count = 0;
    INDEXPKT        *p;

    for( p = pktIndex; p; p = p -> next )
    {
        if( p -> sel == ' ' )
            continue;

        ShowWriteCount( count++ );

        if( act != FILE_ALLOWER && act != FILE_LAST )
            if( !access( name, F_OK ))
                act = OverWrite( FILE_MSG, name, title );

        if( act < 0 )
        {
			rc = act;
            break;
        }

        if( act == FILE_LAST )
            SetMsgName( name, SET_LAST );
            
        if(( rc = WriteToMsg( p )) != NO_ERROR )
            break;

        if(( rc = FRenFile( pkttmp, name )) != NO_ERROR )
        {
            ShowError( "Can't rename file '%s' to '%s'", pkttmp, ShowPath( name, 40 ));
            break;
        }
        SetMsgName( name, SET_NEXT );
    }

    ShowWriteCount( -1 );

    return( rc );
}
/*-------------------------------------------------------------------------*/
static APIRET SetMsgName( CHAR *name, INT code )
{
    INT          i, j;
    CHAR         buf[_MAX_FNAME];
    
    if( code == SET_NEXT )
    {
        _splitpath( name, drive, dir, fname, ext );

        memset( buf, ' ', _MAX_FNAME );

        for( i = strlen( fname ), j = _MAX_FNAME - 1; i >= 0; i-- )
            buf[j--] = fname[i];
        
        for( i = strlen( buf ); i; i-- )
        {
            if( !isdigit( buf[i-1] ))
                buf[i-1] = '1';
            else
            {
                buf[i-1]++;
                if( buf[i-1] == '9' + 1 )
                {
                    buf[i-1] = '0';
                    continue;
                }
            }
            break;
        }
        strcpy( fname, StrSkipSpace( buf ));

        _makepath( name, drive, dir, fname, ext );
    }

    if( code == SET_LAST )
    {
        while( !access( name, F_OK ))
            SetMsgName( name, SET_NEXT );
    }
    
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
static APIRET WriteToPkt( INT sel )
{
    APIRET           rc = NO_ERROR;
    INT              count = 0, wcount;
    SHORT            sep = 2;
    CHAR             last, *string;
    struct dosdate_t date;
    struct dostime_t time;
    PKT48            pkt2;
    PACKET           packet;
    INDEXPKT        *p;
    
    if(( newfile = _fsopen( pkttmp, "wb", SH_DENYWR )) == NULL )
    {
        ShowError( "Can't create file '%s'", ShowPath( pkttmp, 50 ));
        return( ERROR_OPEN_FAILED );
    }

    _dos_getdate( &date );
    _dos_gettime( &time );
    
    memset( &pkt2, 0, sizeof( pkt2 ));

    if( sel )
    {
        memcpy( pkt2.password, Password, 8 );
        
        pkt2.orig_node      = From.Node;
        pkt2.dest_node      = To.Node;
        pkt2.orig_net       = From.Point ? -1 : From.Net;
        pkt2.dest_net       = To.Net;
        pkt2.origZone       = From.Zone;
        pkt2.destZone       = To.Zone;
        pkt2.AuxNet         = From.Point ? From.Net : 0;
        pkt2.orig_zone      = From.Zone;
        pkt2.dest_zone      = To.Zone;
        pkt2.orig_point     = From.Point;
        pkt2.dest_point     = To.Point;

        pkt2.year           = date.year;
        pkt2.month          = date.month;
        pkt2.day            = date.day;
        pkt2.hour           = time.hour;
        pkt2.minute         = time.minute;
        pkt2.second         = time.second;
        pkt2.rate           = 0;
        pkt2.ver            = 2;
        pkt2.product_low    = 0xfe;
        pkt2.prod_rev_low   = ver_hi;
        pkt2.CapValid       = 0x0100;
        pkt2.product_hi     = 0;
        pkt2.prod_rev_hi    = ver_low;
        pkt2.CapWord        = 0x0001;
        pkt2.ProdData       = 0x544b504d;
    }
    else
        memcpy( &pkt2, &pkt, sizeof( pkt2 ));
        
    if( fwrite( &pkt2, sizeof( pkt2 ), 1, newfile ) != 1 )
    {
        fclose( newfile );
        ShowError( "Error write to file '%s'", ShowPath( pkttmp, 50 ));
        return( ERROR_WRITE_FAULT );
    }
    
    string = malloc( MAX_BUFFER );
    
    for( p = pktIndex; p; p = p -> next )
    {
        if( sel && p -> sel == ' ' )
            continue;
        if( !sel && p -> sel != ' ' )
            continue;
            
        ShowWriteCount( count++ );
        
        packet.orig_net   = p -> AddrFrom.Net;
        packet.orig_node  = p -> AddrFrom.Node;
        packet.dest_net   = p -> AddrTo.Net;
        packet.dest_node  = p -> AddrTo.Node;
        packet.attr       = p -> attr;
        packet.cost       = 0;
        strncpy( packet.date, p -> date, sizeof( packet.date ));
        
        wcount  = fwrite( &sep,      sizeof( sep ),           1, newfile );
        wcount += fwrite( &packet,   sizeof( packet ),        1, newfile );
        wcount += fwrite( p -> to,   strlen( p -> to )   + 1, 1, newfile );
        wcount += fwrite( p -> from, strlen( p -> from ) + 1, 1, newfile );
        wcount += fwrite( p -> subj, strlen( p -> subj ) + 1, 1, newfile );

		if( wcount != 5 || ferror( newfile ))
		{
			fclose( newfile ); free( string );
			ShowError( "Error write to file '%s'", ShowPath( pkttmp, 50 ));
			return( ERROR_WRITE_FAULT );
		}
		
        if(( rc = InitMsgPkt( p )) != NO_ERROR )
        {
            fclose( newfile );
            return( rc );
        }
            
        while( FidoMsgGetStr( string, MAX_BUFFER - 1, PktFile, &last ) != NULL )
        {
            strcat( string, "\r" );
            if( fputs ( string, newfile ) == EOF )
			{
				rc = ERROR_WRITE_FAULT;
				break;
            }
            if( last == 0 )
                break;
        }
        if( rc == NO_ERROR )
			fputc( 0, newfile );

        if( mode == FILE_TYPE_MSG )
            CloseFile();

		if( rc == ERROR_WRITE_FAULT || ferror( newfile ))
		{
			ShowError( "Error write to file '%s'", ShowPath( pkttmp, 50 ));
			rc = ERROR_WRITE_FAULT;
			break;
		}
    }

	sep = 0;
    
    if( rc == NO_ERROR )
		fwrite( &sep, sizeof( sep ), 1, newfile );

	fclose( newfile ); free( string );

    ShowWriteCount( -1 );

    if( mode == FILE_TYPE_MSG )
        CloseFile();
    
    return( rc );
}
/*-------------------------------------------------------------------------*/
static APIRET WriteToMsg( INDEXPKT *index )
{
    APIRET           rc = NO_ERROR;
    CHAR            *string, last;
    MSG              msg2;
    
    if(( newfile = _fsopen( pkttmp, "wb", SH_DENYWR )) == NULL )
    {
        ShowError( "Can't create file '%s'", ShowPath( pkttmp, 50 ));
        return( ERROR_OPEN_FAILED );
    }
    
    if(( rc = InitMsgPkt( index )) != NO_ERROR )
    {
        fclose( newfile );
        return( rc );
    }

    if( mode == FILE_TYPE_MSG )
        memcpy( &msg2, &msg, sizeof( msg2 ));
        
    if( mode == FILE_TYPE_PKT )
    {
        memset ( &msg2, 0, sizeof( msg ));
        
        strncpy( msg2.from, index -> from, MAX_FROM );
        strncpy( msg2.to,   index -> to,   MAX_TO   );
        strncpy( msg2.subj, index -> subj, MAX_SUBJ );
        strncpy( msg2.date, index -> date, MAX_DATE );
        
        msg2.count_read = 0;
        msg2.dest_node  = index -> AddrTo.Node;
        msg2.orig_node  = index -> AddrFrom.Node;
        msg2.cost       = 0;
        msg2.orig_net   = index -> AddrFrom.Net;
        msg2.dest_net   = index -> AddrTo.Net;
        msg2.reply      = 0;
        msg2.attrib     = index -> attr;
        msg2.next_reply = 0;

    }

    if( fwrite( &msg2, sizeof( msg2 ), 1, newfile ) != 1 )
    {
        if( mode == FILE_TYPE_MSG )
            CloseFile();
        fclose( newfile );
        ShowError( "Error write to file '%s'", ShowPath( pkttmp, 50 ));
        return( ERROR_WRITE_FAULT );
    }

    string = malloc( MAX_BUFFER );

    while( FidoMsgGetStr( string, MAX_BUFFER - 1, PktFile, &last ) != NULL )
    {
        strcat( string, "\r" );
        if( fputs ( string, newfile ) == EOF )
        {
			rc = ERROR_WRITE_FAULT;
			break;
        }
        if( last == 0 )
            break;
    }

    if( mode == FILE_TYPE_MSG )
        CloseFile();

	free( string );

	if( rc == NO_ERROR )
		fputc( 0, newfile );

	if( fclose( newfile ) || rc == ERROR_WRITE_FAULT )
		ShowError( "Error write to file '%s'", ShowPath( pkttmp, 50 ));

    return( rc );
}
/*-------------------------------------------------------------------------*/
static APIRET DeletePktMsg( CHAR *netfile )
{
    APIRET           rc;
    INT              i;
    INDEXPKT        *p, *prev, *tmp;
    
    if( mode == FILE_TYPE_MSG )
    {
        for( p = pktIndex, prev = NULL; p; )
        {
            if( p -> sel == ' ' )
            {
                prev = p;
                p = p -> next;
                continue;
            }

            chmod( p -> name, S_IWRITE );
            unlink( p -> name );

            for( i = 0; i < pktcount; i++ )
            {
                if( !stricmp( pktOrigin[i] -> name, p -> name ))
                {
                    for( ; i < pktcount - 1; i++ )
                        pktOrigin[i] = pktOrigin[i+1];
                    break;
                }
            }
            
            if( prev ) prev -> next = p -> next;
            else pktIndex = p -> next;
            tmp = p -> next;
            free( p );
            p = tmp;
            pktcount--;
        }
        return( NO_ERROR );
    }
    
    if(( rc = WriteToPkt( 0 )) != NO_ERROR )
        return( rc );
    CloseFile();
    if(( rc = FRenFile( pkttmp, netfile )) != NO_ERROR )
        ShowError( "Can't rename file '%s' to '%s'", pkttmp, ShowPath( netfile , 40 ));
    UpdateTxt = 1;
    return( rc );
}
/*-------------------------------------------------------------------------*/
static APIRET AddToPkt( CHAR *fromPkt, CHAR *toPkt )
{
    APIRET           rc = NO_ERROR;
	SHORT            sep;
    INT              wcount;
	LONG             pos;
    FILE            *dest, *sour;
    CHAR             last, *string, *s;
    PACKET           packet;
    
    if(( sour = _fsopen( fromPkt, "rb", SH_DENYWR )) == NULL )
    {
        ShowError( "Can't open file '%s'", ShowPath( fromPkt, 50 ));
        return( ERROR_OPEN_FAILED );
    }
    
    if(( dest = _fsopen( toPkt, "rb+", SH_DENYWR )) == NULL )
    {
        fclose( sour );
		ShowError( "Can't open file '%s'", ShowPath( toPkt, 50 ));
        return( ERROR_OPEN_FAILED );
    }
    
    if( fread( &pkt, sizeof( pkt ), 1, sour ) != 1 )
    {
        fclose( dest ); fclose( sour );
		ShowError( "Error read file '%s'", ShowPath( fromPkt, 50 ));
        return( ERROR_READ_FAULT );
    }
    
    if( fread( &pkt, sizeof( pkt ), 1, dest ) != 1 )
    {
        fclose( dest ); fclose( sour );
		ShowError( "Error read file '%s'", ShowPath( toPkt, 50 ));
        return( ERROR_READ_FAULT );
    }
    
	string = malloc( MAX_BUFFER );
	
    while( 1 )
    {
        sep = 0;
        
        pos = ftell( dest );
        
        if( fread( &sep, sizeof( sep ), 1, dest ) != 1 )
        {
            fseek( dest, pos, SEEK_SET );
            fputc( 0, dest ); fputc( 0, dest );
            fseek( dest, pos, SEEK_SET );
            break;
        }
        
        if( sep == 0 )
        {
            fseek( dest, pos, SEEK_SET );
            break;
        }
        
        if( fread( &packet, sizeof( packet ), 1, dest ) != 1 )
        {
            fclose( dest ); fclose( sour ); free( string );
			ShowError( "Error read file '%s'", ShowPath( toPkt, 50 ));
            return( ERROR_READ_FAULT );
        }
        
        FidoMsgGetStrSeek( dest, 0, SEEK_CUR );
        
        FidoMsgGetStr( string, MAX_BUFFER - 1, dest, &last );
        FidoMsgGetStr( string, MAX_BUFFER - 1, dest, &last );
        FidoMsgGetStr( string, MAX_BUFFER - 1, dest, &last );
        
        while(( s = FidoMsgGetStr( string, MAX_BUFFER - 1, dest, &last )) != NULL )
            if( last == 0 )
                break;
        
        if( s == NULL )
        {
            fputc( 0, dest );
            break;
        }
        fseek( dest, FidoMsgGetStrTell(), SEEK_SET );
    }
    
    while( 1 )
    {
        sep = 0;
        
        fread( &sep, sizeof( sep ), 1, sour );
        
        if( sep == 0 )
            break;
        
        if( fread( &packet, sizeof( packet ), 1, sour ) != 1 )
        {
            fclose( dest ); fclose( sour ); free( string );
			ShowError( "Error read file '%s'", ShowPath( fromPkt, 50 ));
            return( ERROR_READ_FAULT );
        }
        
        FidoMsgGetStrSeek( sour, 0, SEEK_CUR );
        
        wcount  = fwrite( &sep,    sizeof( sep ),      1, dest );
        wcount += fwrite( &packet, sizeof( packet ),   1, dest );
        
        FidoMsgGetStr( string, MAX_BUFFER - 1, sour, &last );
        wcount += fwrite( string, strlen( string ) + 1, 1, dest );
        
        FidoMsgGetStr( string, MAX_BUFFER - 1, sour, &last );
        wcount += fwrite( string, strlen( string ) + 1, 1, dest );
        
        FidoMsgGetStr( string, MAX_BUFFER - 1, sour, &last );
        wcount += fwrite( string, strlen( string ) + 1, 1, dest );

		if( wcount != 5 || ferror( dest ))
		{
			rc = ERROR_WRITE_FAULT;
			break;
		}
		
        while( FidoMsgGetStr( string, MAX_BUFFER - 1, sour, &last ) != NULL )
        {
            strcat( string, "\r" );
            if( fputs ( string, dest ) == EOF )
            {
				rc = ERROR_WRITE_FAULT;
				break;
            }
            if( last == 0 )
                break;
        }
		if( rc != NO_ERROR )
			break;
		fseek( sour, FidoMsgGetStrTell(), SEEK_SET );
        fputc( 0, dest );
    }

	sep = 0;
    
	if( rc == NO_ERROR )
		fwrite( &sep, sizeof( sep ), 1, dest );

	fclose( sour ); free( string );

	if( fclose( dest ) || rc == ERROR_WRITE_FAULT )
		ShowError( "Error write to file '%s'", ShowPath( toPkt, 50 ));
    
    return( rc );
}
/*-------------------------------------------------------------------------*/
static APIRET ExportToTxt( CHAR *name, INT act )
{
    APIRET           rc = NO_ERROR;
    INT              i, count = 0;
    CHAR             last, *string;
    CHAR             a[128], buf[64];
    INDEXPKT        *p;
    
    switch( act )
    {
        case FILE_CREAT:
            if(( newfile = _fsopen( name, "wt", SH_DENYWR )) == NULL )
            {
                ShowError( "Can't create file '%s'", ShowPath( name, 50 ));
                return( ERROR_OPEN_FAILED );
            }
            break;
            
        case FILE_APPEND:
            if(( newfile = _fsopen( name, "at+", SH_DENYWR )) == NULL )
            {
                ShowError( "Can't open file '%s'", ShowPath( name, 50 ));
                return( ERROR_OPEN_FAILED );
            }
            break;
                    
        case FILE_OWER:
            if(( newfile = _fsopen( name, "wt", SH_DENYWR )) == NULL )
            {
                ShowError( "Can't overwrite file '%s'", ShowPath( name, 50 ));
                return( ERROR_OPEN_FAILED );
            }
            break;

    }
        
    string = malloc( MAX_BUFFER );
        
    for( i = 0, p = pktIndex; i < pktcount; i++, p = p -> next )
    {
        if( p -> sel == ' ' )
            continue;
        
        ShowWriteCount( count++ );
            
        if(( rc = InitMsgPkt( p )) != NO_ERROR )
        {
            fclose( newfile ); free( string );
            return( rc );
        }

        if( p -> area )
            sprintf( a, "Ä %s ", p -> area );
        else
            sprintf( a, "Ä %s ", "Netmail" );
        memset ( &a[strlen(a)], 'Ä', 80 ); a[79] = 0;
        fputs  ( a, newfile ); fputc( '\n', newfile );

        fprintf( newfile, " From : %-36.36s%-15.15s %s\n", p -> from, FidoAddr2Str( &p -> AddrFrom, buf ), p -> date );
        fprintf( newfile, " To   : %-36.36s%-20.20s\n", p -> to, FidoAddr2Str( &p -> AddrTo, buf ));
        fprintf( newfile, " Subj : %s\n", p -> subj );

        memset ( a, 'Ä', 80 ); a[79] = 0;
        fputs  ( a, newfile ); fputc( '\n', newfile );

		if( ferror( newfile ))
		{
			rc = ERROR_WRITE_FAULT;
			break;
		}
        while( FidoMsgGetStr( string, MAX_BUFFER - 1, PktFile, &last ) != NULL )
        {
            strcat( string, "\n" );
            if( string[0] == '\001' )
                string[0] = '@';
            fputs( string, newfile );

            if( last == 0 )
                break;
        }
        fputc( '\n', newfile );

        if( mode == FILE_TYPE_MSG )
            CloseFile();

		if( ferror( newfile ))
		{
			rc = ERROR_WRITE_FAULT;
			break;
		}
    }

	free( string );
	
	ShowWriteCount( -1 );
	
	if( fclose( newfile ) || rc == ERROR_WRITE_FAULT )
		ShowError( "Error write to file '%s'", ShowPath( name, 50 ));

    return( rc );
}
/*-------------------------------------------------------------------------*/
static APIRET InitMsgPkt( INDEXPKT *index )
{
    APIRET           rc = NO_ERROR;
    
    if( mode == FILE_TYPE_MSG )
    {
        if(( rc = OpenFile( index -> name )) != NO_ERROR )
            return( rc );
        if( fread( &msg, sizeof( msg ), 1, PktFile ) != 1 )
        {
            CloseFile();
            ShowError( "Error read file '%s'", ShowPath( index -> name, 50 ));
            return( ERROR_READ_FAULT );
        }
        FidoMsgGetStrSeek( PktFile, 0, SEEK_CUR );
    }

    if( mode == FILE_TYPE_PKT )
        FidoMsgGetStrSeek( PktFile, index -> telltxt, SEEK_SET );

    return( rc );
}
/*-------------------------------------------------------------------------*/

#endif
