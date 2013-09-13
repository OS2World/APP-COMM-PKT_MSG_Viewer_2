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
 * Module: sert.c         Use in: Full - [X]
 *                                Lite - [-]
 *---------------------------------------------------------------------------
 */

#ifndef _LITE_

#include "pktview.h"

/*-------------------------------------------------------------------------*/
static WINDOW   wsort;
static FORM     fsort;
/*-------------------------------------------------------------------------*/
APIRET SortMenu( FILEMENU *menu )
{
    INT          rc, i, x0, y0, dy0, dy1;
    WINDOW       wsort;
    FORM         fsort;
    
    for( y0 = 0; y0 < MAX_SORT && menu -> sort[y0].str; y0++ );

    for( i = x0 = 0; i < y0; i++ )
        if( x0 < strlen( menu -> sort[i].str ))
            x0 = strlen( menu -> sort[i].str );

    x0 = ( x0 + 1 ) / 2 + 3; dy0 = ( y0 + 1 ) / 2 + 1; dy1 = y0 / 2 + 2;
    
    wsort = WDefine ( x/2-x0, y/2-dy0, x/2+x0+1, y/2+dy1, 11, W_SHADOW | W_NOCURSOR, BORDER4, " Sort " );
    fsort = WFormDefine( wsort );

    for( i = 0; i < y0; i++ )
        WFormRadio( fsort, 1, i + 1, menu -> sort[i].str, &menu -> sort[i].type, NOCONTROL, 0 );
    
    SetBar( BarNull );
    
    rc = WFormGet( fsort );
    WFormRelease( fsort, 0 );
    WRelease( wsort );
    
    if( rc != NO_ERROR )
        return( rc );
    
    ShowWaiting( 1 );
    SortList   ( menu );
    ShowWaiting( 0 );
    
    WPRedefine( menu -> popup, menu -> list );
    WPShow( menu -> popup, FALSE, TRUE, -1, -1 );
    
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
APIRET SortList( FILEMENU *menu )
{
    INT           i, (*func)() = NULL;
    PMLIST       *lnew, l;
    
    if( menu -> Count <= 1 )
        return( NO_ERROR );
    
    for( i = 0; i < MAX_SORT; i++ )
    {
        if( menu -> sort[i].type == 0 )
            continue;
        func = menu -> sort[i].func;
        break;
    }
        
    lnew = malloc( menu -> Count * sizeof( *lnew ));
    
    if( lnew == NULL )
        return( ERROR_NOT_ENOUGH_MEMORY );
    
    if( menu -> origin )
        memcpy( lnew, menu -> origin, menu -> Count * sizeof( *lnew ));
    else
        for( i = 0, l = menu -> list; l; l = l -> next, i++ )
            lnew[i] = l;
    
    if( func )
        MShellSort( lnew, menu -> Count, sizeof( *lnew ), func );
    
    menu -> list = lnew[0];
    
    for( i = 1; i < menu -> Count; i++ )
        lnew[i-1] -> next = lnew[i];
    lnew[i-1] -> next = NULL;
    
    free( lnew );
    
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
APIRET PktSort( VOID )
{
    APIRET        rc;
    INT           i;
    INDEXPKT    **inew, *index;
    INT         (*func)() = NULL;
    
    WPShow( popup, 1, 0, -1, -1 );

    wsort = WDefine ( x/2-9, y/2-5, x/2+9, y/2+5, 11, W_SHADOW | W_NOCURSOR, BORDER4, " Sort " );
    fsort = WFormDefine( wsort );
    
    WFormRadio( fsort, 1, 1, "Unsorted",    &typesort.unsort, NOCONTROL, 0 );
    WFormRadio( fsort, 1, 2, "From",        &typesort.from,   NOCONTROL, 0 );
    WFormRadio( fsort, 1, 3, "To",          &typesort.to,     NOCONTROL, 0 );
    WFormRadio( fsort, 1, 4, "Area",        &typesort.area,   NOCONTROL, 0 );
    WFormRadio( fsort, 1, 5, "Subj",        &typesort.subj,   NOCONTROL, 0 );
    WFormRadio( fsort, 1, 6, "Date & Time", &typesort.date,   NOCONTROL, 0 );
    WFormRadio( fsort, 1, 7, "Size",        &typesort.size,   NOCONTROL, 0 );
    
    SetBar( BarNull );

    rc = WFormGet( fsort );
    WFormRelease( fsort, 0 );
    WRelease( wsort );
    
    if( rc != NO_ERROR ) return( rc );
    
    ShowWaiting( 1 );
    
    inew = calloc( pktcount, sizeof( *inew ));

    if( pktOrigin )
        memcpy( inew, pktOrigin, pktcount * sizeof( *inew ));
    else
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

    ShowWaiting( 0 );
    
    return( NO_ERROR );
}
/*-------------------------------------------------------------------------*/
LONG DataStr2Long( CHAR *str )
{
    SHORT        sec, min, hour, day, mon, year;
    struct  tm   tm;
    
    sscanf( str, "%2hd-%2hd-%2hd  %2hd:%2hd:%2hd", &day, &mon, &year, &hour, &min, &sec );
    
    tm.tm_sec   = sec;
    tm.tm_min   = min;     /* minutes after the hour   -- [0,59] */
    tm.tm_hour  = hour;    /* hours after midnight     -- [0,23] */
    tm.tm_mday  = day;     /* day of the month         -- [1,31] */
    tm.tm_mon   = mon - 1; /* months since January     -- [0,11] */
    tm.tm_year  = year;    /* years since 1900                   */
    tm.tm_wday  = 0;       /* days since Sunday        -- [0,6]  */
    tm.tm_yday  = 0;       /* days since January 1     -- [0,365]*/
    tm.tm_isdst = 0;       /* Daylight Savings Time flag */
    
    return( mktime( &tm ));
}
/*-------------------------------------------------------------------------*/
INT SortFrom( VOID *s1, VOID *s2 )
{
    INDEXPKT    **i1 = s1, **i2 = s2;
    
    return( stricmp( (*i1) -> from, (*i2) -> from ));
}
/*-------------------------------------------------------------------------*/
INT SortTo( VOID *s1, VOID *s2 )
{
    INDEXPKT    **i1 = s1, **i2 = s2;
    
    return( stricmp( (*i1) -> to, (*i2) -> to ));
}
/*-------------------------------------------------------------------------*/
INT SortSubj( VOID *s1, VOID *s2 )
{
    INDEXPKT    **i1 = s1, **i2 = s2;
    
    return( stricmp( (*i1) -> subj, (*i2) -> subj ));
}
/*-------------------------------------------------------------------------*/
INT SortArea( VOID *s1, VOID *s2 )
{
    INDEXPKT    **i1 = s1, **i2 = s2;
    
    if( (*i1) -> area == NULL && (*i2) -> area )
        return( -1 );
    
    if( (*i1) -> area && (*i2) -> area == NULL )
        return( 1 );
    
    if( (*i1) -> area == NULL && (*i2) -> area == NULL )
        return( 0 );
    
    return( stricmp( (*i1) -> area, (*i2) -> area ));
}
/*-------------------------------------------------------------------------*/
INT SortDate( VOID *s1, VOID *s2 )
{
    INDEXPKT    **i1 = s1, **i2 = s2;
    LONG          date1, date2;
    
    date1 = DataMsg2Long( (*i1) -> date );
    date2 = DataMsg2Long( (*i2) -> date );
    
    if( date1 > date2 )
        return( 1 );
    if( date1 < date2 )
        return( -1 );
    return( 0 );
}
/*-------------------------------------------------------------------------*/
LONG DataMsg2Long( CHAR *str )
{
    SHORT        day, moon, year, hour, min, sec;
    CHAR         mon[16];
    struct  tm   tm;
    
    sscanf( str, "%2hd %3s %2hd  %2hd:%2hd:%2hd", &day, mon, &year, &hour, &min, &sec );
    
    for( moon = 0; moon < 12 && stricmp( mon, month[moon] ); moon++ );
    
    tm.tm_sec   = sec;
    tm.tm_min   = min;     /* minutes after the hour   -- [0,59] */
    tm.tm_hour  = hour;    /* hours after midnight     -- [0,23] */
    tm.tm_mday  = day;     /* day of the month         -- [1,31] */
    tm.tm_mon   = moon;    /* months since January     -- [0,11] */
    tm.tm_year  = year;    /* years since 1900                   */
    tm.tm_wday  = 0;       /* days since Sunday        -- [0,6]  */
    tm.tm_yday  = 0;       /* days since January 1     -- [0,365]*/
    tm.tm_isdst = 0;       /* Daylight Savings Time flag */
    
    return( mktime( &tm ));
}
/*-------------------------------------------------------------------------*/
INT SortSize( VOID *s1, VOID *s2 )
{
    INDEXPKT    **i1 = s1, **i2 = s2;
    
    if( (*i1) -> size > (*i2) -> size )
        return( 1 );
    if( (*i1) -> size < (*i2) -> size )
        return( -1 );
    return( 0 );
}
/*-------------------------------------------------------------------------*/

#endif
