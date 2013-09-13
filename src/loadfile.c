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
 * Module: loadfile.c     Use in: Full - [X]
 *                                Lite - [X]
 *---------------------------------------------------------------------------
 */

#include "pktview.h"

/*-------------------------------------------------------------------------*/
APIRET LoadFile( CHAR file[] )
{
    APIRET          rc = NO_ERROR;
    INT             nextload;
#ifndef _LITE_
    FILEMENU        TreeMenu;

    TreeMenu.Path    = TreeMenu.truePath = NULL;
    TreeMenu.list    = NULL;
    TreeMenu.origin  = NULL;
    TreeMenu.popup   = 0;
#endif
    reload = newload = FALSE;
    
    nextload = file[0] ? FALSE : TRUE;

    while( !altx )
    {
#ifndef _LITE_
        if(( nextload && !reload ) || newload )
        {
            InitMem();
            rc = ReadDir( &TreeMenu, file );
            if( rc == ERROR_NOT_ENOUGH_MEMORY )
            {
                newload = TRUE;
                CloseFile();
                InitMem();
                ShowError( "Not enough memory." );
            }
            else
                if( rc != NO_ERROR )
                    break;
        }
#endif
        UpdateTxt = Update = reload = newload = FALSE;

        rc = ViewFile( file );
        if( rc == ERROR_NOT_ENOUGH_MEMORY )
        {
            newload = TRUE;
            CloseFile();
            InitMem();
            ShowError( "Not enough memory." );
#ifdef _LITE_
            break;
#endif
        }

        if( reload || nextload || newload )
            continue;
        break;
    }
    return( rc );
}
/*-------------------------------------------------------------------------*/
APIRET ViewFile( CHAR *filename )
{
    APIRET           rc = NO_ERROR;
    LONG             m;
    CHAR             newfile[_MAX_PATH];
#ifndef _LITE_
    CHAR             nf[_MAX_PATH];
    FILEMENU         FMenu;
    CHAR            *command;

    FMenu.Path   = FMenu.truePath = NULL;
    FMenu.list   = NULL;
    FMenu.origin = NULL;
#endif
    
    strcpy( newfile, filename );

    if(( rc = FGetFileType( newfile, &m )) != NO_ERROR )
        return( rc );

    mode = m;
    
    switch( mode )
    {
        case FILE_TYPE_MSG:
            InitMem();
            if(( rc = ReadMsgFile( newfile )) != NO_ERROR )
                break;
            rc = ViewMsg( newfile, 1 );
            FreeTxtIndex();
            break;

        case FILE_TYPE_PKT:
            InitMem();
            if(( rc = ReadPktFile( newfile )) != NO_ERROR )
                break;
            Select( newfile );
            break;

#ifndef _LITE_

        case FILE_TYPE_ZIP:
            do
            {
                arcreload = FALSE;
                if( !PktReadZip( &FMenu, newfile ))
                    ArcSelect( &FMenu );
            } while( arcreload );
            break;

        case FILE_TYPE_ARJ:
            do
            {
                arcreload = FALSE;
                if( !PktReadArj( &FMenu, newfile ))
                    ArcSelect( &FMenu );
            } while( arcreload );
            break;

        case FILE_TYPE_LZH:
            do
            {
                arcreload = FALSE;
                if( !PktReadLha( &FMenu, newfile ))
                    ArcSelect( &FMenu );
            } while( arcreload );
            break;

        case FILE_TYPE_RAR:
            do
            {
                arcreload = FALSE;
                if( !PktReadRar( &FMenu, newfile ))
                    ArcSelect( &FMenu );
            } while( arcreload );
            break;

        default:
            if( strlen( cfgCurrent.os[os].viewer ) == 0 )
                break;
            strcpy( nf, "\"" ); strcat( nf, newfile ); strcat( nf, "\"" );
            command = CreateCommand( cfgCurrent.os[os].viewer, "", nf, cfgCurrent.os[os].temp );
            if( command == NULL )
                return( ERROR_NOT_ENOUGH_MEMORY );
            strcat( command, " >nul" );
#ifdef __OS2__
            strcat( command, " 2>nul" );
#endif
            _heapmin();
            rc = system( command );
            free( command );
            break;
#endif
    }
    return( rc );
}
/*-------------------------------------------------------------------------*/
