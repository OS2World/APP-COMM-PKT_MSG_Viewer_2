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
 * Module: pktview.h      Use in: Full - [X]
 *                                Lite - [X]
 *---------------------------------------------------------------------------
 */

#ifdef __OS2__
  #define INCL_NOPM
  #define INCL_NOPMAPI
  #define INCL_DOSFILEMGR
  #define INCL_DOSDEVICES
  #define INCL_DOSDEVIOCTL
  #define INCL_DOSSEMAPHORES
  #define INCL_DOSQUEUES
  #define INCL_DOSMEMMGR
  #define INCL_DOSDATETIME
  #define INCL_DOSPROCESS
  #define INCL_DOSERRORS
  #define INCL_DOSMISC
  #define INCL_SUB

  #include <os2.h>
  #include <bsememf.h>
  #include <signal.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <conio.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>
#include <stdarg.h>
#include <io.h>
#include <dos.h>
#include <errno.h>
#include <mem.h>
#include <direct.h>
#include <malloc.h>
#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sys\utime.h>
#include <fcntl.h>
#include <share.h>

#include "moonlit.h"

/*----- file type for copy/move --------------------------------------------*/
#define FILE_PKT        0
#define FILE_MSG        1
#define FILE_TXT        2
/*--------------------------------------------------------------------------*/
#define FILE_CREAT      0
#define FILE_APPEND     1
#define FILE_OWER       2
#define FILE_ALLOWER    3
#define FILE_LAST       4
/*--------------------------------------------------------------------------*/
#define ARC_ZIP         0
#define ARC_ARJ         1
#define ARC_LZH         2
#define ARC_RAR         3
/*--------------------------------------------------------------------------*/
#define ACT_VIEW        0
#define ACT_EXTR        1
#define ACT_DEL         2
#define ACT_ADD         3
#define ACT_TEST        4
#define ACT_PASS        5
#define ACT_LIST        6
#define ACT_RES         7
/*--------------------------------------------------------------------------*/
#define SEL_CURR        0
#define SEL_MARK        1
/*--------------------------------------------------------------------------*/
#define SET_NEXT        0
#define SET_LAST        1
/*--------------------------------------------------------------------------*/
#define MAX_SORT        10
#define MAX_BUFFER      8 * 1024
#define MAX_LINE        5
#define SIZEIOBUF       1024
/*---------------------------------------------------------------------*/
struct _INDEXPKT;
struct _INDEXTXT;
typedef struct _INDEXPKT INDEXPKT;
typedef struct _INDEXTXT INDEXTXT;
/*--------------------------------------------------------------------------*/
typedef union _STORE
{
    PCHAR        str;
    INT          mem;
} STORE;
/*--------------------------------------------------------------------------*/
struct _INDEXPKT
{
    CHAR             sel,  seltmp, change;
    SHORT            attr;
    LONG             size;
    PCHAR            from, to, subj, date, area;
    ADDR             AddrFrom, AddrTo;
    union
    {
        struct
        {
            LONG     tell, telltxt;
        };
        struct
        {
            PCHAR    name;
            LONG     number;
        };
    };
    PCHAR            quick[MAX_LINE];
    SHORT            color[MAX_LINE];
    INDEXPKT        *next;
};
/*--------------------------------------------------------------------------*/
struct _INDEXTXT
{
    CHAR             color;
    PCHAR            str;
    INDEXTXT        *next;
};
/*--------------------------------------------------------------------------*/
typedef struct _TYPESORT
{
    SHORT            unsort, from, to, area;
    SHORT            subj, date, size;
} TYPESORT;
/*--------------------------------------------------------------------------*/
typedef struct _SELECT
{
    SHORT            all, sensit, attr, cost;
    PCHAR            from, to, subj, date, area;
    PCHAR            addrfrom, addrto;
} SELECT;
/*--------------------------------------------------------------------------*/
typedef struct _BAR
{
    SHORT            status;
    PCHAR            str;
} BAR[10];
/*--------------------------------------------------------------------------*/
typedef struct _SORT
{
    SHORT            type;
    PCHAR            str;
    INT            (*func)();
} SORT;
/*--------------------------------------------------------------------------*/
typedef struct _FILEMENU
{
    INT              Count, select;
    SHORT            maxX, maxY, info;
    PCHAR            TitlePath, Path, truePath;
    PCHAR            TitleList;
    POPUP            popup;
    PMLIST           list, *origin;
    SORT            *sort;
} FILEMENU;
/*-------------------------------------------------------------------------*/
typedef struct _ARC
{
    FILEMENU        *menu;
    PMLIST           list;
    struct _ARC     *next;
} ARC;
/*-------------------------------------------------------------------------*/
typedef struct _CFGMISC
{
    SHORT           intUnZip, sort, fsort, asort, psort;
    SCHEME          color[24];
    struct
    {
        PCHAR       temp, viewer;
        struct
        {
            PCHAR   act[8];     // view, extract, del, add, test, pass, list;
        }           arch[4];    // zip, arj, lzh, rar;
    }               os[2];      // os/2, dos
} PKTMISC;
/*-------------------------------------------------------------------------*/
typedef struct _CFGHEADER
{
    CHAR            signature[16];
    LONG            ver, size, crc, res;
    SHORT           intUnZip, sort, fsort, asort, psort;
    SHORT           reserv[32];
    SCHEME          color[24];
    struct
    {
        LONG        temp, viewer;
        struct
        {
            LONG    act[8];     // view, extract, del, add, test, pass, list;
        }           arch[4];    // zip, arj, lzh, rar;
    }               os[2];      // os/2, dos
} CFGHEADER;
/*--------------------------------------------------------------------------
 --- pktview.c
 */
CHAR     *ShowPath      ( CHAR *path, INT len );
VOID      ShowError     ( CHAR *str, ... );
APIRET    CheckUpdatePkt( CHAR *name );
APIRET    UpdatePkt     ( CHAR *name );
VOID      InitVideo     ( VOID );
VOID      RestoreInit   ( VOID );
VOID      InitMem       ( VOID );
VOID      CheckReview   ( CHAR *file );
INDEXPKT *GetCurrentPkt ( INT number );
INDEXTXT *GetCurrentTxt ( INT number );
VOID      FreePktIndex  ( VOID );
VOID      FreeTxtIndex  ( VOID );
MLIST    *GetCurrentList( MLIST  *list, INT line );
CHAR     *CreateName    ( CHAR *netfile, CHAR name[] );
VOID      SetBar        ( BAR bar );
CHAR     *ViewNum       ( LONG number, INT count );
VOID      ShortHelp     ( VOID );
CHAR     *CreateCommand ( CHAR *str, CHAR *arch, CHAR *list, CHAR *dirtmp );
/*--------------------------------------------------------------------------
 --- arcview.c
 */
APIRET PktReadZip       ( FILEMENU *ZMenu, CHAR *file );
APIRET PktReadArj       ( FILEMENU *AMenu, CHAR *file );
APIRET PktReadLha       ( FILEMENU *LMenu, CHAR *file );
APIRET PktReadRar       ( FILEMENU *RMenu, CHAR *file );
VOID   FreeMenu         ( FILEMENU *menu );
APIRET ArcSelect        ( FILEMENU *Menu );
/*--------------------------------------------------------------------------
 --- tree.c
 */
APIRET ReadDir          ( FILEMENU *TMenu, CHAR file[] );
/*--------------------------------------------------------------------------
 --- loadfile.c
 */
APIRET LoadFile         ( CHAR file[] );
APIRET ViewFile         ( CHAR *file );
/*--------------------------------------------------------------------------
 --- read.c
 */
APIRET OpenFile         ( CHAR *netfile );
VOID   CloseFile        ( VOID );
APIRET ReadMsgFile      ( CHAR *file );
APIRET CreateMsgList    ( CHAR *path );
APIRET ReadPktFile      ( CHAR *file );
APIRET ReadText         ( INT  code );
/*-------------------------------------------------------------------------
 --- config.c
 */
APIRET ChangeConfig     ( VOID );
VOID   SetDefault       ( PKTMISC *dest, PKTMISC *sour );
APIRET CreateConfig     ( CHAR *name );
APIRET SaveConfig       ( CHAR *name );
APIRET LoadConfig       ( CHAR *name );
/*--------------------------------------------------------------------------
 --- gets.c
*/
INT    EnterPathType    ( CHAR *title, CHAR *prompt, CHAR name[] );
APIRET EnterPath        ( CHAR *title, CHAR *prompt, CHAR name[] );
INT    GetOk            ( CHAR *str1, CHAR *str2 );
VOID   ShowWaiting      ( INT code );
INT    OverWrite        ( INT key, CHAR *name, CHAR *act );
INT    SelectDrive      ( VOID );
INT    GetHeaderAddr    ( ADDR *from, ADDR *to, CHAR *pass );
INT    GetMarkedMsgs    ( CHAR *str );
INT    ShowInfo         ( CHAR *name );
VOID   ShowWriteCount   ( INT count );
/*--------------------------------------------------------------------------
 --- export.c
 */
APIRET CopyPkt          ( CHAR *netfile );
APIRET MovePkt          ( CHAR *netfile );
APIRET DelPkt           ( CHAR *netfile );
/*--------------------------------------------------------------------------
 --- help.c
 */
APIRET ViewHelp         ( CHAR *section );
/*--------------------------------------------------------------------------
 --- show.c
 */
INT   Select            ( CHAR *name );
INT   ViewMsg           ( CHAR *file, SHORT renew );
VOID  ViewPktHeader     ( ADDR *from, ADDR *to, CHAR *password );
VOID  ViewUpdate        ( VOID );
VOID  ViewSelCount      ( VOID );
/*--------------------------------------------------------------------------
 --- edit.c
 */
APIRET EditHeader       ( VOID );
/*--------------------------------------------------------------------------
 --- sort.c
 */
APIRET SortMenu         ( FILEMENU *menu );
APIRET SortList         ( FILEMENU *menu );
APIRET PktSort          ( VOID );
INT    SortFrom         ( VOID *s1, VOID *s2 );
INT    SortTo           ( VOID *s1, VOID *s2 );
INT    SortSubj         ( VOID *s1, VOID *s2 );
INT    SortArea         ( VOID *s1, VOID *s2 );
INT    SortSize         ( VOID *s1, VOID *s2 );
INT    SortDate         ( VOID *s1, VOID *s2 );
LONG   DataStr2Long     ( CHAR *str );
LONG   DataMsg2Long     ( CHAR *str );
/*--------------------------------------------------------------------------
 --- select.c
 */
APIRET PktSelect        ( VOID );
APIRET PktUnSelect      ( VOID );
VOID   MsgSelect        ( INT code, INT all );
VOID   MsgInvert        ( VOID );
/*-------------------------------------------------------------------------*/
extern PKT48     pkt;
extern MSG       msg;
extern INT       ver_hi, ver_low;
extern INT       os;                  // os = 0 - OS/2, os = 1 - DOS
extern INT       mode;                // mode = 0 - .msg; mode = 1 - .pkt
extern BOOL      xMem;                // uses xMem (EMS/XMS/...)
extern BOOL      BadPkt;              // bad format of pkt
extern BOOL      ReadOnly;            // file is read only
extern BOOL      review;              // review directory
extern CHAR      netfile[];
extern CHAR      pkttmp[], HelpFile[], FtsCFile[], ConfFile[];
extern CHAR      LastArch[];          // name of last archive
extern CHAR      fname[], ext[];
extern CHAR      drive[], dir[];
extern PCHAR     CurDir;
extern INT       pktcount;            // count of messafes in pkt file
extern INT       txtcount;            // count of line's in pkt file
extern INDEXPKT *pktIndex, *Current, **pktOrigin;
extern INDEXTXT *txtIndex;
extern FILE     *PktFile, *newfile;
extern BOOL      reload;              // need reload *.pkt after del ...
extern BOOL      newload;             // wanted load new file
extern INT       selected;            // count selected of massages

extern SHORT     x, y;                // max size X,Y of SCREEN
extern SHORT     maxXlist, maxYlist;  // max size X,Y of window <wlist>
extern SHORT     maxXquick,maxYquick; // max size X,Y of window <wquick>
extern SHORT     maxXtext, maxYtext;  // max size X,Y of window <wtext>
extern SHORT     maxXmain, maxYmain;  // max size X,Y of window <wmain>
extern SHORT     maxXdel,  maxYdel;   // max size X,Y of window <wdel>
extern SHORT     maxXpath, maxYpath;  // max size X,Y of window <wpath>
extern SHORT     maxXproc, maxYproc;  // max size X,Y of window <wproc>
extern SHORT     maxXtree, maxYtree;  // max size X,Y of window <wtree>
extern SHORT     maxXsubj, maxYsubj;  // max size X,Y of window <wsubj>
extern SHORT     maxXpkt,  maxYpkt;   // max size X,Y of window <wpkt>

extern BOOL      Quick;               // quick view of message
extern BOOL      PktUpdate;           // pkt is changing
extern BOOL      Update;              // need update header *.pkt
extern BOOL      UpdateTxt;           // need update text in *.pkt
extern BOOL      altx;
extern BOOL      autoView, autoList;

extern TYPESORT       typesort;
extern SELECT         pktsel;
extern POPUP          popup;

extern BAR            BarList, BarEdit, BarView, BarDir, BarLoad;
extern BAR            BarSet, BarSel;
extern const BAR      BarNull;

extern const PCHAR    comment, format;
extern PCHAR          iobuf;

extern const PCHAR    month[];
extern ADDR           HeaderFrom, HeaderTo;

extern WINDOW         wscan,  wpkt,   wlist,  wsubj, wquick, wbar;
extern WINDOW         wmain,  wtext;
extern WINDOW         wdel,   werror, wpath,  wproc;
extern WINDOW         wtinfo, wttitl, wtree,  wtname, wover;
extern WINDOW         wsells, wdrive, winfo;
extern BOOL           arcreload;
extern PKTMISC        cfgCurrent;
extern const PKTMISC  cfgDefault;
/*-------------------------------------------------------------------------*/
