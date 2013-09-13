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
 * Module: read.c         Use in: Full - [X]
 *                                Lite - [X]
 *---------------------------------------------------------------------------
 */

#include "pktview.h"

#define CH_TEARLINE     0
#define CH_ORIGIN       1

/*-------------------------------------------------------------------------*/
static VOID  CheckString   ( CHAR *str, INT line, INDEXPKT *top );
static INT   GetLineColor  ( INT line, CHAR *string );
static INT   CheckNextLine ( INT check );
static CHAR *StrChar       ( CHAR *str, INT len, CHAR sim );
static INT   SortNumber    ( VOID *s1, VOID *s2 );
/*-------------------------------------------------------------------------*/
static INT       intl, origin, id;
static ADDR      addrTo, addrFrom, addrOrigin, MsgID;
static CHAR     *filename;
/*-------------------------------------------------------------------------*/
APIRET OpenFile( CHAR *netfile )
{
    ReadOnly = FALSE;

    if(( PktFile = _fsopen( netfile, "rb+", SH_DENYWR )) == NULL )
    {
        if(( PktFile = _fsopen( netfile, "rb", SH_DENYWR )) == NULL )
        {
            ShowError( "Can't open file '%s'", ShowPath( netfile, 50 ));
            return( ERROR_OPEN_FAILED );
        }
        ReadOnly = TRUE;
    }
    
    setvbuf( PktFile, iobuf, _IOFBF, SIZEIOBUF );
    
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
VOID CloseFile( VOID )
{
    if( PktFile )
    {
        fclose( PktFile );
        PktFile = NULL;
    }
}
/*-------------------------------------------------------------------------*/
APIRET ReadMsgFile( CHAR *netfile )
{
    APIRET           rc;
    CHAR            *string;
    INDEXPKT        *top;

    if(( rc = OpenFile( netfile )) != 0 )
        return( rc );
    
    if( fread( &msg, sizeof( msg ), 1, PktFile ) != 1 )
    {
        CloseFile();
        ShowError( "Error read file '%s'", ShowPath( netfile, 50 ));
        return( ERROR_READ_FAULT );
    }

    filename = netfile;

    string = malloc( MAX_BUFFER );

    if( string == NULL )
        return( ERROR_NOT_ENOUGH_MEMORY );

    pktcount = 1;

    AddNextStruct( pktIndex, top );

    if( top == NULL )
    {
        free( string );
        return( ERROR_NOT_ENOUGH_MEMORY );
    }

    Current = top;

    top -> AddrFrom.Zone      = 0;
    top -> AddrFrom.Net       = msg.orig_net;
    top -> AddrFrom.Node      = msg.orig_node;
    top -> AddrFrom.Point     = 0;
    top -> AddrFrom.Domain[0] = 0;

    top -> AddrTo.Zone        = 0;
    top -> AddrTo.Net         = msg.dest_net;
    top -> AddrTo.Node        = msg.dest_node;
    top -> AddrTo.Point       = 0;
    top -> AddrTo.Domain[0]   = 0;

    top -> attr               = msg.attrib;

    strcpy( string, msg.to   ); string[ MAX_TO ]   = 0; top -> to   = strdup( string );
    strcpy( string, msg.from ); string[ MAX_FROM ] = 0; top -> from = strdup( string );
    strcpy( string, msg.subj ); string[ MAX_SUBJ ] = 0; top -> subj = strdup( string );
    strcpy( string, msg.date ); string[ MAX_DATE ] = 0; top -> date = strdup( string );

    free( string );

    top -> area    = NULL;
    top -> sel     = ' ';
    top -> change  = 0;
    top -> size    = filelength( fileno( PktFile ));
    top -> name    = strdup( netfile );
    
    CloseFile();

    rc = ReadText( 1 );

    return( rc  );
}
/*-------------------------------------------------------------------------*/
#ifndef _LITE_
APIRET CreateMsgList( CHAR *path )
{
    INT           i, done, line;
    INT           attr = _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH;
    static CHAR   File[_MAX_PATH];
    CHAR         *string, last;
    struct find_t find;
    INDEXPKT     *top, *index, **inew;
    
    InitMem();

    mode = FILE_TYPE_MSG;

    string = malloc( MAX_BUFFER );

    if( string == NULL )
        return( ERROR_NOT_ENOUGH_MEMORY );

    _splitpath( path, drive, dir, NULL, NULL );
    strcpy( File, drive ); strcat( File, dir ); StrAddSlash( File ); strcat( File, "*.msg" );
    
    WActive( wscan );

    for( done=_dos_findfirst(File,attr,&find); !done; done=_dos_findnext(&find))
    {
        _splitpath( find.name, NULL, NULL, fname, NULL );
        strcpy( File, path ); StrAddSlash( File ); strcat( File, find.name );

        if( OpenFile( File ))
            continue;
        
        if( !( pktcount % 5 ))
        {
            WSetXY ( wscan, 19, 2 );
            if( pktcount == 0 )
                WPrintf( wscan, "%4u", 1 );
            else
                WPrintf( wscan, "%4u", pktcount );
        }

        fread( &msg, sizeof( msg ), 1, PktFile );
        
        AddNextStruct( pktIndex, top );

        if( top == NULL )
        {
            free( string );
            return( ERROR_NOT_ENOUGH_MEMORY );
        }
        
        top -> size = filelength( fileno( PktFile ));
        
        strcpy( string, msg.to   ); string[ MAX_TO ]   = 0; top -> to   = strdup( string );
        strcpy( string, msg.from ); string[ MAX_FROM ] = 0; top -> from = strdup( string );
        strcpy( string, msg.subj ); string[ MAX_SUBJ ] = 0; top -> subj = strdup( string );
        strcpy( string, msg.date ); string[ MAX_DATE ] = 0; top -> date = strdup( string );
        
        top -> AddrFrom.Zone      = 0;
        top -> AddrFrom.Net       = msg.orig_net;
        top -> AddrFrom.Node      = msg.orig_node;
        top -> AddrFrom.Point     = 0;
        top -> AddrFrom.Domain[0] = 0;
        
        top -> AddrTo.Zone        = 0;
        top -> AddrTo.Net         = msg.dest_net;
        top -> AddrTo.Node        = msg.dest_node;
        top -> AddrTo.Point       = 0;
        top -> AddrTo.Domain[0]   = 0;
        
        top -> attr               = msg.attrib;
        top -> number             = atol( fname );
        top -> name               = strdup( find.name );
        top -> area               = NULL;
        top -> sel                = ' ';
        top -> change             = 0;

        FidoMsgGetStrSeek( PktFile, 0, SEEK_CUR );

        i = line = 0;
        
        while( FidoMsgGetStr( string, MAX_BUFFER - 1, PktFile, &last ) != NULL )
        {
            CheckString( string, i, top );
            
			if( !Quick )
				break;
			
			if( i && strncmp( string, "AREA:", 5 ))
            {
                if( line < MAX_LINE && string[0] != '\001' && StrSkipSpace( string ))
                {
                    top -> quick[line] = strdup( string );
                    top -> color[line] = GetLineColor( i, string );
                    line++;
                }
            }
            if( last == 0 || line >= MAX_LINE )
                break;
            i++;
        }
        
        CloseFile();
        CheckString( NULL, 0, top );
        
        pktcount++;
    }
    
    _dos_findclose( &find );
    free( string );

    if( pktcount == 0 )
    {
        WDeactive( wscan );
        return( 0 );
    }

    if( cfgCurrent.sort )
    {
        inew = calloc( pktcount, sizeof( *inew ));

        if( inew == NULL )
            return( ERROR_NOT_ENOUGH_MEMORY );

        for( i = 0, index = pktIndex; index; index = index -> next, i++ )
            inew[i] = index;

        MShellSort( inew, pktcount, sizeof( *inew ), SortNumber );
    
        pktIndex = inew[0];
    
        for( i = 1; i < pktcount; i++ )
            inew[i-1] -> next = inew[i];
        inew[i-1] -> next = NULL;

        free( inew );

        pktOrigin = malloc( pktcount * sizeof( *pktOrigin ));

        if( pktOrigin == NULL )
            return( ERROR_NOT_ENOUGH_MEMORY );
    
        for( i = 0, index = pktIndex; index; index = index -> next, i++ )
            pktOrigin[i] = index;

    }

    WDeactive( wscan );

    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
static INT SortNumber( VOID *s1, VOID *s2 )
{
    INDEXPKT    **i1 = s1, **i2 = s2;
    
    if( (*i1) -> number > (*i2) -> number )
        return( 1 );
    if( (*i1) -> number < (*i2) -> number )
        return( -1 );
    return( 0 );
}
#endif
/*-------------------------------------------------------------------------*/
APIRET ReadPktFile( CHAR *netfile )
{
    APIRET           rc;
    INT              i, line;
    SHORT            sep;
    CHAR             last, *string;
    LONG             tell;
    PACKET           packet;
    INDEXPKT        *top;
#ifndef _LITE_
    INDEXPKT        *index, **inew;
    INT            (*func)() = NULL;
#endif

    if(( rc = OpenFile( netfile )) != 0 )
        return( rc );

    if( fread( &pkt, sizeof( pkt ), 1, PktFile ) != 1 )
    {
        CloseFile();
        ShowError( "Error read file '%s'", ShowPath( netfile, 50 ));
        return( ERROR_READ_FAULT );
    }
    if( pkt.ver != 2 )
    {
        CloseFile();
        ShowError( "File '%s' is't PKT", ShowPath( netfile, 50 ));
        return( ERROR_BAD_FORMAT );
    }

    string = malloc( MAX_BUFFER );

    if( string == NULL )
        return( ERROR_NOT_ENOUGH_MEMORY );

    WActive( wscan );

    while( 1 )
    {
        sep = 0;

        if( fread( &sep, sizeof( sep ), 1, PktFile ) != 1 )
        {
            ShowError( "Error structure in '%s'\nIncomplete pkt. Rest of packet ignored.", ShowPath( netfile, 50 ));
            BadPkt = 1;
            break;
        }

        tell = ftell( PktFile );
        
        if( sep == 0 )
        {
            if( fread( &sep, 1, 1, PktFile ) == 1 )
            {
                ShowError( "Error structure in '%s'\nFound more data, offset: 0x%08lX. Rest of packet ignored.", ShowPath( netfile, 50 ), tell );
                BadPkt = 1;
            }
            break;
        }

        if( sep != 2 )
        {
            ShowError( "Error structure in '%s'\nError separator: 0x%02hX, offset: 0x%08lX. Rest of packet ignored.", ShowPath( netfile, 50 ), sep, tell - 2 );
            BadPkt = 1;
            break;
        }

        if( fread( &packet, sizeof( packet ), 1, PktFile ) != 1 )
        {
            ShowError( "Error structure in '%s'\nIncomplete pkt. Rest of packet ignored.", ShowPath( netfile, 50 ));
            BadPkt = 1;
            break;
        }

        AddNextStruct( pktIndex, top );

        if( top == NULL )
        {
            free( string );
            WDeactive( wscan );
            return( ERROR_NOT_ENOUGH_MEMORY );
        }

        top -> tell = tell;

        if( !( pktcount % 5 ))
        {
            WSetXY ( wscan, 19, 2 );
            if( pktcount == 0 )
                WPrintf( wscan, "%4u", 1 );
            else
                WPrintf( wscan, "%4u", pktcount );
        }

        top -> AddrFrom.Zone      = 0;
        top -> AddrFrom.Net       = packet.orig_net;
        top -> AddrFrom.Node      = packet.orig_node;
        top -> AddrFrom.Point     = 0;
        top -> AddrFrom.Domain[0] = 0;

        top -> AddrTo.Zone        = 0;
        top -> AddrTo.Net         = packet.dest_net;
        top -> AddrTo.Node        = packet.dest_node;
        top -> AddrTo.Point       = 0;
        top -> AddrTo.Domain[0]   = 0;

        top -> attr               = packet.attr;

        strncpy( string, packet.date, sizeof( packet.date ));
        string[sizeof(packet.date)] = 0; top -> date = strdup( string );

        FidoMsgGetStrSeek( PktFile, 0, SEEK_CUR );

        FidoMsgGetStr( string, MAX_BUFFER - 1, PktFile, &last );
        string[ MAX_TO ] = 0; top -> to = strdup( string );

        FidoMsgGetStr( string, MAX_BUFFER - 1, PktFile, &last );
        string[ MAX_FROM ] = 0; top -> from = strdup( string );

        FidoMsgGetStr( string, MAX_BUFFER - 1, PktFile, &last );
        string[ MAX_SUBJ ] = 0; top -> subj = strdup( string );

        top -> area   = NULL;
        top -> sel    = ' ';
        top -> change = 0;

        for( i = 0; i < MAX_LINE; i++ )
            top -> quick[i] = NULL;

        top -> telltxt = FidoMsgGetStrTell();
        line = i = 0;

        while( FidoMsgGetStr( string, MAX_BUFFER - 1, PktFile, &last ) != NULL )
        {
            CheckString( string, i, top );

            if( i && strncmp( string, "AREA:", 5 ))
            {
                if( Quick && line < MAX_LINE && string[0] != '\001' && StrSkipSpace( string ))
                {
                    top -> quick[line] = strdup( string );
                    top -> color[line] = GetLineColor( i, string );
                    line++;
                }
            }
            if( last == 0 )
                break;
            i++;
        }

        CheckString( NULL, 0, top );

        top -> size = FidoMsgGetStrTell() - top -> telltxt - 1;
        fseek( PktFile, FidoMsgGetStrTell(), SEEK_SET );
        pktcount++;
    }

    free( string );

#ifndef _LITE_

    if( cfgCurrent.sort && pktcount )
    {
        pktOrigin = malloc( pktcount * sizeof( *pktOrigin ));
        if( pktOrigin )
            for( i = 0, index = pktIndex; index; index = index -> next, i++ )
                pktOrigin[i] = index;

        if( cfgCurrent.psort && ( inew = calloc( pktcount, sizeof( *inew ))) != NULL )
        {
            for( i = 0, index = pktIndex; index; index = index -> next, i++ )
                inew[i] = index;

            if( typesort.from ) func = SortFrom;
            if( typesort.to   ) func = SortTo;
            if( typesort.area ) func = SortArea;
            if( typesort.subj ) func = SortSubj;
            if( typesort.date ) func = SortDate;
            if( typesort.size ) func = SortSize;

            if( func )
                MShellSort( inew, pktcount, sizeof( *inew ), func );

            pktIndex = inew[0];
        
            for( i = 1; i < pktcount; i++ )
                inew[i-1] -> next = inew[i];
            inew[i-1] -> next = NULL;

            free( inew );
        }
    }

#endif
    
    WDeactive( wscan );

    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
APIRET ReadText( INT check )
{
    APIRET           rc;
    INT              i = 0, len = 0;
    INT              x = maxXtext - 1;
    CHAR             last = '\n', *s, *string;
    INDEXTXT        *top, *start;

    FreeTxtIndex();

    string = malloc( MAX_BUFFER );

    if( string == NULL )
        return( ERROR_NOT_ENOUGH_MEMORY );

    if( mode == FILE_TYPE_PKT )
        FidoMsgGetStrSeek( PktFile, Current -> telltxt, SEEK_SET );

    if( mode == FILE_TYPE_MSG )
    {
        if(( rc = OpenFile( Current -> name )) != NO_ERROR )
            return( rc );
        if( fread( &msg, sizeof( msg ), 1, PktFile ) != 1 )
        {
            free( string );
            CloseFile();
            ShowError( "Error read file '%s'", ShowPath( Current -> name, 50 ));
            return( ERROR_READ_FAULT );
        }
        FidoMsgGetStrSeek( PktFile, 0, SEEK_CUR );
    }
        
    intl = origin = id = 0;
    MsgID.Zone = -1;
    
    while( last && FidoMsgGetStr( string, MAX_BUFFER - 1, PktFile, &last ) != NULL )
    {
        if( check )
            CheckString( string, i, Current );

        start = NULL;

        while( 1 )
        {
            len = strlen( string );
            s = &string[ len ];

            if( len > x )
            {
                if(( s = StrChar( string, x, ' ' )) == NULL )
                    if(( s = StrChar( string, x, ',' )) == NULL )
                        if(( s = StrChar( string, x, '.' )) == NULL )
                            if(( s = StrChar( string, x, ';' )) == NULL )
                                s = StrChar( string, x, ':' );
                if( s == NULL )
                {
                    len = x;
                    s = &string[len];
                }
                else
                {
                    len = s - string + 1;
                    s++;
                }
            }

            AddNextStruct( txtIndex, top );

            if( top == NULL )
            {
                free( string );
                return( ERROR_NOT_ENOUGH_MEMORY );
            }

            top -> str = malloc( len + 1 );
            memcpy( top -> str, string, len + 1 );
            top -> str[len] = 0;

            if( !start )
            {
                start = top;
                start -> color = GetLineColor( i, string );
            }
            else
                top -> color = start -> color;

            memcpy( string, s, strlen( s ) + 1 );

            txtcount++;

            if( string[0] == 0 )
                break;
        }
        i++;
        s = &start -> str[1];
    }

    free( string );
    
    if( check )
        CheckString( NULL, 0, Current );
    
    if( mode == FILE_TYPE_MSG )
        CloseFile();

    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
static VOID CheckString( CHAR *str, INT line, INDEXPKT *top )
{
    CHAR        *s;
    
    if( str == NULL )
    {
        if( !intl )
        {
            if( MsgID.Zone != -1 && id )
            {
                FidoStoreAddr( &top -> AddrFrom, &MsgID );

                if( top -> area == NULL )
                {
                    top -> AddrTo.Zone = MsgID.Zone;
                    strcpy( top -> AddrTo.Domain, MsgID.Domain );
                }
            }
            else
            {
                if( origin )
                    FidoStoreAddr( &top -> AddrFrom, &addrOrigin );
            }
            if( top -> area && mode == FILE_TYPE_PKT )
            {
                if( top -> AddrFrom.Zone == 0 )
                    top -> AddrFrom.Zone = pkt.orig_zone;
                if( top -> AddrTo.Zone == 0 )
                    top -> AddrTo.Zone = pkt.dest_zone;
                top -> AddrTo.Point = pkt.dest_point;
            }
        }
        else
        {
            if( MsgID.Zone != -1 && id )
                FidoStoreAddr( &top -> AddrFrom, &MsgID );
        }
        intl = origin = id = 0;
        MsgID.Zone  = -1;
        return;
    }

    if( line == 0 && !strncmp( str, "AREA:", 5 ))
    {
        str[ 5 + MAX_AREA ] = 0;
        top -> area = strdup( StrSkipSpace( &str[5] ));
        return;
    }
    if( str && *str != '\001' )
    {
        if( intl || id )
            return;
        if( strncmp( str, " * Origin:", 10 ))
            return;
        if(( str = strrchr( str, '(' )) == NULL )
            return;
        if(( str = StrSkipNonNum( str )) == NULL )
            return;
        if(( s = strchr( str, ')' )) == NULL )
            return;
        *s = 0;
        if( FidoStr2Addr( str, &addrOrigin, NULL ) == TRUE )
            origin = 1;
    }

    str++;

    if( !strncmp( str, "MSGID:", 6 ))
    {
        id = 1;
        FidoStr2Addr( StrSkipSpace( &str[6] ), &MsgID, NULL );
        return;
    }
    if( !strncmp( str, "FMPT ", 5 ))
    {
        top -> AddrFrom.Point = atoi( StrSkipSpace( &str[5] ));
        return;
    }
    if( !strncmp( str, "TOPT ", 5 ))
    {
        top -> AddrTo.Point = atoi( StrSkipSpace( &str[5] ));
        return;
    }
    if( !strncmp( str, "INTL ", 5 ))
    {
        intl = 1;
        FidoStr2Addr( StrSkipSpace( &str[5] ), &addrTo, NULL );
        FidoStr2Addr( StrSkipSpace( StrSkipChar( StrSkipSpace( &str[5] ))), &addrFrom, NULL );
        top -> AddrTo.Zone   = addrTo.Zone;
        top -> AddrFrom.Zone = addrFrom.Zone;
        return;
    }
}
/*-------------------------------------------------------------------------*/
INT GetLineColor( INT line, CHAR *string )
{
    INT              i;

    if( string[0] == '\001' )
        return( C_USER2 );

    if( !line && !strncmp( string, "AREA:", 5 ))
        return( C_MORE );

    if( !strncmp( string, "---", 3 ) &&
                        ( strlen( string ) == 3 || string[3] == ' ' ))
    {
        if( CheckNextLine( CH_TEARLINE ))
            return( C_HOT );
        return( C_USER0 );
    }

    if( !strncmp( string, " * Origin:", 10 ))
    {
        if( CheckNextLine( CH_ORIGIN ))
            return( C_HOT );
        return( C_USER0 );
    }

    if( !strncmp( string, "SEEN-BY: ", 9 ))
        return( C_HIDE );

    for( i = 0; string && (string[i]==' '||string[i]=='\t'||string[i]==':') && i < 7; i++ )
    {   if( string[i] == ':' )
            return( C_USER1 );
    }

    string = StrSkipSpace( string );

    for( i = 0; string && string[i] && i < 5; i++ )
    {
        if( string[i] == '>' )
            return( C_USER1 );
    }

    return( C_USER0 );
}
/*-------------------------------------------------------------------------*/
INT CheckNextLine( INT check )
{
    INT          code = 0;
    CHAR        *string, *buf, last;
    LONG         tell;
    
    string = malloc( MAX_BUFFER );

    if( string == NULL )
        return( ERROR_NOT_ENOUGH_MEMORY );

    tell = FidoMsgGetStrTell();
    
    FidoMsgGetStrSeek( PktFile, tell, SEEK_SET );
    buf = FidoMsgGetStr( string, MAX_BUFFER - 1, PktFile, &last );

    switch( check )
    {
        case CH_TEARLINE:
            if( !buf || !last || *string == '\001' )
            {
                code = 1;
                break;
            }
            if( !strncmp( string, " * Origin:", 10 ))
            {
                code = CheckNextLine( CH_ORIGIN );
                break;
            }
            break;

        case CH_ORIGIN:
            if( !buf || !last || *string == '\001' || !strncmp( string, "SEEN-BY: ", 9 ))
                code = 1;
            break;
    }
    
    free( string );
    FidoMsgGetStrSeek( PktFile, tell, SEEK_SET );

    return( code );
}
/*-------------------------------------------------------------------------*/
CHAR *StrChar( CHAR *str, INT len, CHAR sim )
{
    CHAR        *s;
    
    for( s = &str[ len ]; len && *s != sim; s--, len-- );
    
    if( len )
        return( s );
    return( NULL );
}
/*-------------------------------------------------------------------------*/
