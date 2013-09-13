/*----------------------------------------------------------------------------
 * DOS:  Moonlit     v3.00 beta 07 (c) 1992-93,96,97 Moonlit Software/2.
 * OS/2: Moonlit/2   v3.00 beta 07 (c) 1995-97       Moonlit Software/2.
 * REXX: RxMoonlit/2 v1.00 beta 07 (c) 1996,97       Moonlit Software/2.
 *
 * Created by Oleg Milaenko AKA Oleg/2 AKA Moon Devil/2, 2:468/10@fidonet
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
 *----------------------------------------------------------------------------
 */

#ifdef __OS2__
  #define INCL_NOPMAPI
  #define INCL_SUB
  #define INCL_ERRORS
  #define INCL_DOSPROCESS
  #include <os2.h>
#endif

#include <malloc.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <direct.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <dos.h>
#include <mem.h>
#include <time.h>
#include <sys\stat.h>
#include <share.h>

#ifndef __OS2__

    #define  NO_ERROR                    0
    #define  ERROR_INVALID_FUNCTION      1
    #define  ERROR_FILE_NOT_FOUND        2
    #define  ERROR_NOT_ENOUGH_MEMORY     8
    #define  ERROR_BAD_FORMAT           11
    #define  ERROR_INVALID_ACCESS       12
    #define  ERROR_WRITE_FAULT          29
    #define  ERROR_READ_FAULT           30
    #define  ERROR_GEN_FAILURE          31
    #define  ERROR_OPEN_FAILED         110
    #define  ERROR_VIO_INVALID_PARMS   421
    #define  ERROR_VIO_INVALID_HANDLE  436

    #define  APIENTRY        _Cdecl

    typedef  unsigned long    APIRET;
    typedef  void             VOID;
    typedef  void           * PVOID;
    typedef  char             CHAR;
    typedef  char           * PCH;
	typedef  char           * PCHAR;
    typedef  unsigned char    UCHAR;
    typedef  unsigned char  * PUCH;
    typedef  unsigned char    BYTE;
    typedef  unsigned char  * PBYTE;
    typedef  short            SHORT;
    typedef  short          * PSHORT;
    typedef  unsigned short   USHORT;
    typedef  unsigned short * PUSHORT;
    typedef  int              INT;
    typedef  int            * PINT;
    typedef  unsigned int     UINT;
    typedef  unsigned int   * PUINT;
    typedef  long             LONG;
    typedef  long           * PLONG;
    typedef  unsigned long    ULONG;
    typedef  unsigned long  * PULONG;
    typedef  unsigned long    BOOL;
    typedef  unsigned long  * PBOOL;

     #ifndef   FALSE
        #define FALSE         0
    #endif

    #ifndef   TRUE
        #define TRUE          1
    #endif
#endif

/* ----=== Key codes ===---- */
#define KEY_A           'A'
#define KEY_a           'a'
#define KEY_B           'B'
#define KEY_b           'b'
#define KEY_C           'C'
#define KEY_c           'c'
#define KEY_D           'D'
#define KEY_d           'd'
#define KEY_E           'E'
#define KEY_e           'e'
#define KEY_F           'F'
#define KEY_f           'f'
#define KEY_G           'G'
#define KEY_g           'g'
#define KEY_H           'H'
#define KEY_h           'h'
#define KEY_I           'I'
#define KEY_i           'i'
#define KEY_J           'J'
#define KEY_j           'j'
#define KEY_K           'K'
#define KEY_k           'k'
#define KEY_L           'L'
#define KEY_l           'l'
#define KEY_M           'M'
#define KEY_m           'm'
#define KEY_N           'N'
#define KEY_n           'n'
#define KEY_O           'O'
#define KEY_o           'o'
#define KEY_P           'P'
#define KEY_p           'p'
#define KEY_Q           'Q'
#define KEY_q           'q'
#define KEY_R           'R'
#define KEY_r           'r'
#define KEY_S           'S'
#define KEY_s           's'
#define KEY_T           'T'
#define KEY_t           't'
#define KEY_U           'U'
#define KEY_u           'u'
#define KEY_V           'V'
#define KEY_v           'v'
#define KEY_W           'W'
#define KEY_w           'w'
#define KEY_X           'X'
#define KEY_x           'x'
#define KEY_Y           'Y'
#define KEY_y           'y'
#define KEY_Z           'Z'
#define KEY_z           'z'
#define KEY_1           '1'
#define KEY_2           '2'
#define KEY_3           '3'
#define KEY_4           '4'
#define KEY_5           '5'
#define KEY_6           '6'
#define KEY_7           '7'
#define KEY_8           '8'
#define KEY_9           '9'
#define KEY_0           '0'
#define KEY_ESC         0x1b
#define KEY_GRAVE       '`'
#define KEY_MINUS       '-'
#define KEY_EQUALS      '='
#define KEY_BSLASH      '\\'
#define KEY_BSPACE      0x08
#define KEY_TAB         0x09
#define KEY_LBRAKET     '['
#define KEY_RBRAKET     ']'
#define KEY_SEMI        ';'
#define KEY_APOS        '\''
#define KEY_ENTER       0x0d
#define KEY_GRENTER     0xe00d
#define KEY_COMMA       ','
#define KEY_PERIOD      '.'
#define KEY_SLASH       '/'
#define KEY_GRSLASH     0xe02f
#define KEY_SPACE       ' '
#define KEY_TILDE       '~'
#define KEY_XCLAM       '!'
#define KEY_ATSIGN      '@'
#define KEY_NUMSIGN     '#'
#define KEY_DOLLAR      '$'
#define KEY_PERCENT     '%'
#define KEY_CARET       '^'
#define KEY_AMP         '&'
#define KEY_ASTERISK    '*'
#define KEY_LPAREN      '('
#define KEY_RPAREN      ')'
#define KEY_US          '_'
#define KEY_PLUS        '+'
#define KEY_SPLITVBAR   '|'
#define KEY_LBRACE      '{'
#define KEY_RBRACE      '}'
#define KEY_COLON       ':'
#define KEY_CDQ         '"'
#define KEY_LTSYM       '<'
#define KEY_GTSYM       '>'
#define KEY_QUEST       '?'
#define KEY_F1          0x3b00
#define KEY_F2          0x3c00
#define KEY_F3          0x3d00
#define KEY_F4          0x3e00
#define KEY_F5          0x3f00
#define KEY_F6          0x4000
#define KEY_F7          0x4100
#define KEY_F8          0x4200
#define KEY_F9          0x4300
#define KEY_F10         0x4400
#define KEY_F11         0x8500
#define KEY_F12         0x8600
#define KEY_INS         0x5200
#define KEY_DEL         0x5300
#define KEY_HOME        0x4700
#define KEY_END         0x4f00
#define KEY_PAGEUP      0x4900
#define KEY_PAGEDOWN    0x5100
#define KEY_LEFT        0x4b00
#define KEY_RIGHT       0x4d00
#define KEY_UP          0x4800
#define KEY_DOWN        0x5000

/* ----=== Shift codes ===---- */
#define SHIFT_TAB       0x0f00
#define SHIFT_F1        0x5400
#define SHIFT_F2        0x5500
#define SHIFT_F3        0x5600
#define SHIFT_F4        0x5700
#define SHIFT_F5        0x5800
#define SHIFT_F6        0x5900
#define SHIFT_F7        0x5a00
#define SHIFT_F8        0x5b00
#define SHIFT_F9        0x5c00
#define SHIFT_F10       0x5d00
#define SHIFT_F11       0x8700
#define SHIFT_F12       0x8800

/* ------=== Control codes ===------ */
#define CTRL_A          (KEY_A-'@')
#define CTRL_B          (KEY_B-'@')
#define CTRL_C          (KEY_C-'@')
#define CTRL_D          (KEY_D-'@')
#define CTRL_E          (KEY_E-'@')
#define CTRL_F          (KEY_F-'@')
#define CTRL_G          (KEY_G-'@')
#define CTRL_H          (KEY_H-'@')
#define CTRL_I          (KEY_I-'@')
#define CTRL_J          (KEY_J-'@')
#define CTRL_K          (KEY_K-'@')
#define CTRL_L          (KEY_L-'@')
#define CTRL_M          (KEY_M-'@')
#define CTRL_N          (KEY_N-'@')
#define CTRL_O          (KEY_O-'@')
#define CTRL_P          (KEY_P-'@')
#define CTRL_Q          (KEY_Q-'@')
#define CTRL_R          (KEY_R-'@')
#define CTRL_S          (KEY_S-'@')
#define CTRL_T          (KEY_T-'@')
#define CTRL_U          (KEY_U-'@')
#define CTRL_V          (KEY_V-'@')
#define CTRL_W          (KEY_W-'@')
#define CTRL_X          (KEY_X-'@')
#define CTRL_Y          (KEY_Y-'@')
#define CTRL_Z          (KEY_Z-'@')
#define CTRL_2          0x0300
#define CTRL_6          0x001e
#define CTRL_MINUS      0x001f
#define CTRL_BSLASH     0x001c
#define CTRL_BSPACE     0x007f
#define CTRL_TAB        0x9400
#define CTRL_LBRAKET    (KEY_LBRAKET-'@')
#define CTRL_RBRAKET    (KEY_RBRAKET-'@')
#define CTRL_ENTER      0x000a
#define CTRL_F1         0x5e00
#define CTRL_F2         0x5f00
#define CTRL_F3         0x6000
#define CTRL_F4         0x6100
#define CTRL_F5         0x6200
#define CTRL_F6         0x6300
#define CTRL_F7         0x6400
#define CTRL_F8         0x6500
#define CTRL_F9         0x6600
#define CTRL_F10        0x6700
#define CTRL_F11        0x8900
#define CTRL_F12        0x8a00
#define CTRL_INS        0x9200
#define CTRL_DEL        0x9300
#define CTRL_HOME       0x7700
#define CTRL_END        0x7500
#define CTRL_PAGEUP     0x8400
#define CTRL_PAGEDOWN   0x7600
#define CTRL_LEFT       0x7300
#define CTRL_RIGHT      0x7400
#define CTRL_UP         0x8d00
#define CTRL_DOWN       0x9100

/* -----=== Alt codes ===----- */
#define ALT_A           0x1e00
#define ALT_B           0x3000
#define ALT_C           0x2e00
#define ALT_D           0x2000
#define ALT_E           0x1200
#define ALT_F           0x2100
#define ALT_G           0x2200
#define ALT_H           0x2300
#define ALT_I           0x1700
#define ALT_J           0x2400
#define ALT_K           0x2500
#define ALT_L           0x2600
#define ALT_M           0x3200
#define ALT_N           0x3100
#define ALT_O           0x1800
#define ALT_P           0x1900
#define ALT_Q           0x1000
#define ALT_R           0x1300
#define ALT_S           0x1f00
#define ALT_T           0x1400
#define ALT_U           0x1600
#define ALT_V           0x2f00
#define ALT_W           0x1100
#define ALT_X           0x2d00
#define ALT_Y           0x1500
#define ALT_Z           0x2c00
#define ALT_1           0x7800
#define ALT_2           0x7900
#define ALT_3           0x7a00
#define ALT_4           0x7b00
#define ALT_5           0x7c00
#define ALT_6           0x7d00
#define ALT_7           0x7e00
#define ALT_8           0x7f00
#define ALT_9           0x8000
#define ALT_0           0x8100
#define ALT_GRAVE       0x2900
#define ALT_MINUS       0x8200
#define ALT_EQUALS      0x8300
#define ALT_BSLASH      0x2b00
#define ALT_BSPACE      0x0e00
#define ALT_TAB         0xa500
#define ALT_LBRAKET     0x1a00
#define ALT_RBRAKET     0x1b00
#define ALT_SEMI        0x2700
#define ALT_APOS        0x2800
#define ALT_ENTER       0x1c00
#define ALT_COMMA       0x3300
#define ALT_PERIOD      0x3400
#define ALT_SLASH       0x3500
#define ALT_F1          0x6800
#define ALT_F2          0x6900
#define ALT_F3          0x6a00
#define ALT_F4          0x6b00
#define ALT_F5          0x6c00
#define ALT_F6          0x6d00
#define ALT_F7          0x6e00
#define ALT_F8          0x6f00
#define ALT_F9          0x7000
#define ALT_F10         0x7100
#define ALT_F11         0x8b00
#define ALT_F12         0x8c00
#define ALT_INS         0xa200
#define ALT_DEL         0xa300
#define ALT_END         0x9f00
#define ALT_PAGEUP      0x9900
#define ALT_PAGEDOWN    0xa100
#define ALT_LEFT        0x9b00
#define ALT_RIGHT       0x7400
#define ALT_UP          0x8d00
#define ALT_DOWN        0x9100
/*--------------------------------------------------------------------------*/
#define AddNextStruct( start, n ) \
    if( start == NULL ) \
        n = start = calloc( sizeof( *n ), 1 ); \
    else \
    {   for( n = start; n -> next; n = n -> next ); \
        n -> next = calloc( sizeof( *n ), 1 ); \
        n = n -> next; \
    }
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/*------------------ Functions for Window library -------------------------*/

    typedef ULONG     WINDOW;

    /*----------------------------------------------------------------------*/

    APIRET  APIENTRY  WInit ( SHORT Vmode, ... );

    /* Vmode */
    
    #define V_CURRENT       0x0000
    #define V_WINDOWED      0x0001
    #define V_80x25         0x0002
    #define V_80x28         0x0003
    #define V_80x30         0x0004
    #define V_80x33         0x0005
    #define V_80x43         0x0006
    #define V_80x50         0x0007
    #define V_80x60         0x0008
    #define V_132x25        0x0009
    #define V_132x43        0x000a
    #define V_132x50        0x000b

    /*----------------------------------------------------------------------*/

    VOID    APIENTRY  WRestore    ( BOOL restMode );

    /*----------------------------------------------------------------------*/
    
    LONG    APIENTRY  WGetSesType ( VOID );
    
    /* return session type */

    #define SES_TEXT        0
	#define SES_DOS         1
    #define SES_WINDOWED    2
    #define SES_PM          3
    #define SES_DETACHED    4

    /*----------------------------------------------------------------------*/

    WINDOW  APIENTRY  WDefine       ( SHORT x0, SHORT y0, SHORT x1, SHORT y1,
                                      SHORT scheme, SHORT status, UCHAR *border,
                                      UCHAR *title, ... );
    SHORT   APIENTRY  WGetStatus    ( WINDOW handle );

    /* set status */
    
    #define W_DEFAULT       0x0000
    #define W_NOSCROLL      0x0001
    #define W_NOCURSOR      0x0002
    #define W_INWINDOW      0x0040
    #define W_SHADOW        0x0080

    /* get status */
    
    #define W_ACTIV         0x0100
    #define W_SHOW          0x0200
    #define W_NO_USES       0x0400

    /* border */

    #define BORDER0         "⁄ƒø≥Ÿƒ¿≥"
    #define BORDER1         "…Õª∫ºÕ»∫"
    #define BORDER2         "⁄ƒ∑∫ºÕ‘≥"
    #define BORDER3         "…Õ∏≥Ÿƒ”∫"
    #define BORDER4         "’Õ∏≥æÕ‘≥"
    #define BORDER5         "÷ƒ∑∫Ωƒ”∫"
    #define BORDER6         "÷ƒ∑∫ºÕ»∫"
    #define BORDER7         "’Õ∏≥Ÿƒ¿≥"
    #define BORDER8         "€ﬂ€€€‹€€"

    /*----------------------------------------------------------------------*/

    APIRET  APIENTRY   WActive         ( WINDOW handle );
    APIRET  APIENTRY   WDeactive       ( WINDOW handle );
    APIRET  APIENTRY   WShow           ( WINDOW handle );
    APIRET  APIENTRY   WHide           ( WINDOW handle );
    APIRET  APIENTRY   WRelease        ( WINDOW handle );
    APIRET  APIENTRY   WMove           ( WINDOW handle, SHORT dx, SHORT dy );
    APIRET  APIENTRY   WClear          ( WINDOW handle );
    APIRET  APIENTRY   WGetLocation    ( WINDOW handle, SHORT *x, SHORT *y );
    APIRET  APIENTRY   WGetSize        ( WINDOW handle, SHORT *x, SHORT *y );

    /*----------------------------------------------------------------------*/

    APIRET  APIENTRY   WPuts           ( WINDOW handle, UCHAR *string );
    APIRET  APIENTRY   WPut            ( WINDOW handle, UCHAR c );
    APIRET  APIENTRY   WPutch          ( WINDOW handle, UCHAR c );       /* w/o control */
    APIRET  APIENTRY   WCPuts          ( WINDOW handle, SHORT y, UCHAR *string );
    APIRET  APIENTRY   WPrints         ( WINDOW handle, SHORT x, SHORT y, UCHAR *str );
    APIRET  APIENTRY   WPrintf         ( WINDOW handle, CHAR *format, ... );
    APIRET  APIENTRY   WPutstr         ( WINDOW handle, UCHAR *string );    /* w/o control */
    APIRET  APIENTRY   WPutb           ( WINDOW handle, SHORT line, SHORT offset, SHORT color, UCHAR c );
    APIRET  APIENTRY   WPutbs          ( WINDOW handle, SHORT line, SHORT offset, SHORT color, UCHAR *str );

    /*----------------------------------------------------------------------*/

    APIRET  APIENTRY   WBox            ( WINDOW handle, SHORT x0, SHORT y0, SHORT x1, SHORT y1,
                                        SHORT color, UCHAR *box );
    APIRET  APIENTRY   WLine           ( WINDOW handle, SHORT x0, SHORT y0,
                                        SHORT x1, SHORT y1, SHORT code );
    APIRET  APIENTRY   WHLine          ( WINDOW handle, SHORT x, SHORT y, SHORT len, SHORT code );
    APIRET  APIENTRY   WVLine          ( WINDOW handle, SHORT x, SHORT y, SHORT len, SHORT code );

    /*----------------------------------------------------------------------*/

    APIRET  APIENTRY   WGetXY          ( WINDOW handle, SHORT *x, SHORT *y );
    APIRET  APIENTRY   WSetXY          ( WINDOW handle, SHORT  x, SHORT  y );
    APIRET  APIENTRY   WShowCursor     ( WINDOW handle );
    APIRET  APIENTRY   WHideCursor     ( WINDOW handle );
    APIRET  APIENTRY   WGetCursor      ( WINDOW handle, SHORT *start, SHORT *end );
    APIRET  APIENTRY   WSetCursor      ( WINDOW handle, SHORT  start, SHORT  end );
    APIRET  APIENTRY   WUpCursor       ( WINDOW handle );
    APIRET  APIENTRY   WDownCursor     ( WINDOW handle );
    APIRET  APIENTRY   WLeftCursor     ( WINDOW handle );
    APIRET  APIENTRY   WRightCursor    ( WINDOW handle );

    APIRET  APIENTRY   WEnableCursor   ( VOID );
    APIRET  APIENTRY   WDisableCursor  ( VOID );

    /*----------------------------------------------------------------------*/

    APIRET  APIENTRY   WInsLine        ( WINDOW handle );
    APIRET  APIENTRY   WDelLine        ( WINDOW handle );
    APIRET  APIENTRY   WClearLine      ( WINDOW handle );

    /*----------------------------------------------------------------------*/

    APIRET  APIENTRY   WChangeColor    ( WINDOW handle, SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT color );
    SHORT   APIENTRY   WGetCurColor    ( WINDOW handle );
    APIRET  APIENTRY   WSetCurColor    ( WINDOW handle, SHORT number );
    SHORT   APIENTRY   WGetSchColor    ( WINDOW handle, SHORT number );
    APIRET  APIENTRY   WSetSchColor    ( WINDOW handle, SHORT number, SHORT color );

    /*----------------------------------------------------------------------*/
    
    UCHAR * APIENTRY   WGetBorder      ( WINDOW handle );
    APIRET  APIENTRY   WSetBorder      ( WINDOW handle, UCHAR *border );
    UCHAR * APIENTRY   WGetTitle       ( WINDOW handle );
    APIRET  APIENTRY   WSetTitle       ( WINDOW handle, UCHAR *title );

    /*----------------------------------------------------------------------*/

    #define MAX_SCHEME      24
    #define MAX_COLOR       17
    
    typedef struct _SCHEME
    {
        UCHAR color[ MAX_COLOR ];
    } SCHEME, * PSCHEME;

    SCHEME * APIENTRY   WGetScheme  ( SHORT number );
    APIRET   APIENTRY   WSetScheme  ( SHORT number, SCHEME *Scheme );

    /* ------ SCHEME color[...] ------ */
    
    #define C_TEXT          0
    #define C_SELECT        1
    #define C_BORDER        2
    #define C_TITLE         3
    #define C_MORE          4
    #define C_HOT           5
    #define C_SELHOT        6
    #define C_HIDE          7
    #define C_SELHIDE       8
    #define C_USER0         9
    #define C_USER1         10
    #define C_USER2         11
    #define C_USER3         12
    #define C_USER4         13
    #define C_USER5         14
    #define C_USER6         15
    #define C_USER7         16

    /* --- Foreground Colors --- */
    
    #define N               0
    #define B               1
    #define G               2
    #define BG              3
    #define GB              3
    #define R               4
    #define RB              5
    #define BR              5
    #define RG              6
    #define GR              6
    #define W               7
    
    /* --- Background Colors --- */
    
    #define N_              0x00
    #define B_              0x10
    #define G_              0x20
    #define BG_             0x30
    #define GB_             0x30
    #define R_              0x40
    #define RB_             0x50
    #define BR_             0x50
    #define RG_             0x60
    #define GR_             0x60
    #define W_              0x70
    
    #define BT              0x08       /* Bright */
    #define BL              0x80       /* Blink  */
    
    /*---------------------------------------------------------------------*/

#if !defined(__COLORS)
#define __COLORS

    enum COLORS {
        BLACK,              /* dark colors */
        BLUE,
        GREEN,
        CYAN,
        RED,
        MAGENTA,
        BROWN,
        LIGHTGRAY,
        DARKGRAY,           /* light colors */
        LIGHTBLUE,
        LIGHTGREEN,
        LIGHTCYAN,
        LIGHTRED,
        LIGHTMAGENTA,
        YELLOW,
        WHITE
    };
#endif

    /*----------------------------------------------------------------------*/

    APIRET  APIENTRY   WFreeze         ( VOID );
    APIRET  APIENTRY   WUnfreeze       ( VOID );
    APIRET  APIENTRY   WHideWinImage   ( BOOL save, BOOL restMode  );
    APIRET  APIENTRY   WShowWinImage   ( VOID );
    APIRET  APIENTRY   WHighColor      ( VOID );
    APIRET  APIENTRY   WNormalColor    ( VOID );
    SHORT   APIENTRY   WScrMaxX        ( VOID );     // screen max X
    SHORT   APIENTRY   WScrMaxY        ( VOID );     // screen max Y

/*--------------------------------------------------------------------------*/

#ifndef __OS2__
    typedef  struct _KBDKEYINFO
    {
        UCHAR    chChar;
        UCHAR    chScan;
        UCHAR    fbStatus;
        UCHAR    bNlsShift;
        USHORT   fsState;
        ULONG    time;
    } KBDKEYINFO, * PKBDKEYINFO;
#endif

    typedef union _KEY
    {
        ULONG       code;
        struct
        {
            USHORT  key;
            UCHAR   scan;
            UCHAR   state;
        };
    } KEY, *PKEY;

    KEY     APIENTRY  MGetKey  ( SHORT mode, ... );
    APIRET  APIENTRY  MPutKey  ( KEY key );
    VOID    APIENTRY  MFreeKey ( VOID );
    
    /* mode */
    #define KBD_WAIT                    0x0000
    #define KBD_NOWAIT                  0x0001
    #define KBD_EXTINFO                 0x8000

    /* state */
#ifndef __OS2__
    #define KBDSTF_RIGHTSHIFT           0x0001
    #define KBDSTF_LEFTSHIFT            0x0002
    #define KBDSTF_CONTROL              0x0004
    #define KBDSTF_ALT                  0x0008
    #define KBDSTF_SCROLLLOCK_ON        0x0010
    #define KBDSTF_NUMLOCK_ON           0x0020
    #define KBDSTF_CAPSLOCK_ON          0x0040
    #define KBDSTF_INSERT_ON            0x0080
    
    #define KBDSTF_LEFTCONTROL          0x0100
    #define KBDSTF_LEFTALT              0x0200
    #define KBDSTF_RIGHTCONTROL         0x0400
    #define KBDSTF_RIGHTALT             0x0800
    #define KBDSTF_SCROLLLOCK           0x1000
    #define KBDSTF_NUMLOCK              0x2000
    #define KBDSTF_CAPSLOCK             0x4000
    #define KBDSTF_SYSREQ               0x8000
#endif
    
    /*----------------------------------------------------------------------*/

    typedef struct _MLIST
    {
        SHORT            sel;
        CHAR            *str;
        VOID            *ext;
        struct _MLIST   *next;
    } MLIST, *PMLIST;

    MLIST * APIENTRY  MListAdd   ( MLIST **list, SHORT sel, CHAR *str, VOID *ext );
    VOID    APIENTRY  MListFree  ( MLIST **list );

    /*----------------------------------------------------------------------*/

    VOID    APIENTRY  MShellSort ( VOID *data, LONG count, SHORT size, int (*compare)( VOID *s1, VOID *s2 ));

/*--------------------------------------------------------------------------*/

    typedef ULONG      POPUP;

    POPUP   APIENTRY   WPDefine    ( WINDOW win, MLIST *list, SHORT freez );
    POPUP   APIENTRY   WPRedefine  ( POPUP popup, MLIST *list );
    APIRET  APIENTRY   WPSetFunc   ( POPUP popup, VOID  (*before) ( SHORT line ),
                                     VOID  (*after)  ( SHORT line ),
                                     KEY   (*filter) ( KEY key ),
                                     CHAR *(*builder)( SHORT line ));
    APIRET  APIENTRY   WPActive    ( POPUP popup );
    APIRET  APIENTRY   WPShow      ( POPUP popup, BOOL show, BOOL cursor, SHORT start, SHORT line );
    APIRET  APIENTRY   WPGetInfo   ( POPUP popup, SHORT *start, SHORT *line );
    APIRET  APIENTRY   WPRelease   ( POPUP popup, BOOL hide );

/*--------------------------------------------------------------------------*/
    
    BOOL    APIENTRY  StrAddSlash   ( CHAR *str );
    BOOL    APIENTRY  StrDelSlash   ( CHAR *str );
    CHAR *  APIENTRY  StrDelCR      ( CHAR *str );

    /*----------------------------------------------------------------------*/

    CHAR *  APIENTRY  StrGetWord    ( CHAR *buf, CHAR word[], SHORT lenword, CHAR *comment );
    CHAR *  APIENTRY  StrGetString  ( CHAR *buf, CHAR str[], SHORT lenstr, CHAR *separ, CHAR *lastsim );
	CHAR *  APIENTRY  StrNum2Str    ( LONG number, INT count, CHAR *str );

    /*----------------------------------------------------------------------*/

    CHAR *  APIENTRY  StrSkipSpace  ( CHAR *str );
    CHAR *  APIENTRY  StrSkipChar   ( CHAR *str );
    CHAR *  APIENTRY  StrSkipNum    ( CHAR *str );
    CHAR *  APIENTRY  StrSkipNonNum ( CHAR *str );
    
/*--------------------------------------------------------------------------*
 *  Special for FIDONET
 */
    
    #define MAX_DOMAIN      16

    typedef struct _ADDR
    {
        SHORT  Zone;
        SHORT  Net;
        SHORT  Node;
        SHORT  Point;
        CHAR   Domain[MAX_DOMAIN];
    } ADDR, * PADDR;

    SHORT   APIENTRY   FidoCmpAddr   ( ADDR *addr1, ADDR *addr2 );
    ADDR *  APIENTRY   FidoStoreAddr ( ADDR *dest, ADDR *sour );
    BOOL    APIENTRY   FidoStr2Addr  ( CHAR *node, ADDR *addr, ADDR *def );
    CHAR *  APIENTRY   FidoAddr2Str  ( ADDR *adr, CHAR *node );

    /*----------------------------------------------------------------------*/

    CHAR *  APIENTRY   FidoDecodeStr ( CHAR *str );
    CHAR *  APIENTRY   FidoEncodeStr ( CHAR *str );

    /*----------------------------------------------------------------------*/

    APIRET  APIENTRY   FidoReadMsg    ( CHAR *filename,
                                        CHAR **from, CHAR **to,
                                        ADDR *addrFrom, ADDR *addrTo,
                                        CHAR **subj, CHAR **date,
                                        SHORT *attr, MLIST **list );
    APIRET  APIENTRY   FidoWriteMsg   ( CHAR *path,
                                        CHAR *from, CHAR *to,
                                        ADDR *addrFrom, ADDR *addrTo, CHAR *subj,
                                        SHORT attr, CHAR *tearline, CHAR *origin,
                                        MLIST *list );
    APIRET  APIENTRY   FidoGetNextMsg ( CHAR *path, CHAR *filename );

#ifndef MAX_DOMAIN
    #define MAX_DOMAIN      16
#endif
    #define MAX_TO          36
    #define MAX_FROM        36
    #define MAX_SUBJ        72
    #define MAX_DATE        20
    #define MAX_AREA        64
    #define MAX_PASS         8

    /*----------------------------------------------------------------------*/

    typedef struct _MSG
    {
        CHAR        from[ MAX_FROM ], to  [ MAX_TO ];
        CHAR        subj[ MAX_SUBJ ], date[ MAX_DATE ];
        USHORT      count_read;
        USHORT      dest_node, orig_node;
        USHORT      cost;
        USHORT      orig_net, dest_net;
        CHAR        res[8];
        USHORT      reply;
        USHORT      attrib;
        USHORT      next_reply;
    } MSG, * PMSG;

    /*----------------------------------------------------------------------*/

    typedef struct _PKT48           /* FSC-0048 packet type 2+        */
    {
        USHORT  orig_node;          /* originating node               */
        USHORT  dest_node;          /* destination node               */
        SHORT   year;               /* 0..99  when packet was created */
        SHORT   month;              /* 0..11  when packet was created */
        SHORT   day;                /* 1..31  when packet was created */
        SHORT   hour;               /* 0..23  when packet was created */
        SHORT   minute;             /* 0..59  when packet was created */
        SHORT   second;             /* 0..59  when packet was created */
        SHORT   rate;               /* destination's baud rate        */
        SHORT   ver;                /* packet version, must be 2      */
        USHORT  orig_net;           /* originating network number     */
        USHORT  dest_net;           /* destination network number     */
        BYTE    product_low;        /* FTSC product type (low byte)   */
        BYTE    prod_rev_low;       /* product rev (low byte)         */
        CHAR    password[8];        /* session/pickup password        */
        SHORT   origZone;           /* Orig Zone ( ZMailQ, QMail )    */
        SHORT   destZone;           /* Dest Zone ( ZMailQ, QMail )    */
        SHORT   AuxNet;             /* Aux Net                        */
        USHORT  CapValid;           /* CapWord with bytes swapped.    */
        BYTE    product_hi;         /* FTSC product type (high byte)  */
        BYTE    prod_rev_hi;        /* product rev (hi byte)          */
        USHORT  CapWord;            /* Capability word                */
        SHORT   orig_zone;          /* originating zone               */
        SHORT   dest_zone;          /* Destination zone               */
        SHORT   orig_point;         /* originating point              */
        SHORT   dest_point;         /* destination point              */
        LONG    ProdData;           /* Product-specific data          */
    } PKT48, *PPKT48;

    /*----------------------------------------------------------------------*/

    typedef struct _PACKET
    {
        SHORT   orig_node, dest_node;
        SHORT   orig_net,  dest_net;
        SHORT   attr,      cost;
        CHAR    date[20];
    } PACKET, * PPACKET;
    
    /* PACKET.attr / MSG.attrib */
    
    #define PKT_PVT     0x0001
    #define PKT_CRA     0x0002
    #define PKT_RCV     0x0004
    #define PKT_SNT     0x0008
    #define PKT_ATT     0x0010
    #define PKT_TRS     0x0020
    #define PKT_ORP     0x0040
    #define PKT_KS      0x0080
    #define PKT_LOC     0x0100
    #define PKT_HLD     0x0200
    #define PKT_RES     0x0400
    #define PKT_FRQ     0x0800
    #define PKT_RRQ     0x1000
    #define PKT_RRC     0x2000
    #define PKT_ARQ     0x4000
    #define PKT_URQ     0x8000

    /*----------------------------------------------------------------------*/

    /* quick read string from msg/pkt file */

    CHAR *  APIENTRY  FidoMsgGetStr     ( CHAR *str, SHORT len, FILE *file, CHAR *last );
    VOID    APIENTRY  FidoMsgGetStrSeek ( FILE *file, LONG seek, SHORT where );
    LONG    APIENTRY  FidoMsgGetStrTell ( VOID );

/*--------------------------------------------------------------------------*/

    APIRET  APIENTRY  FMakeDir     ( CHAR *str );
    APIRET  APIENTRY  FCopyFile    ( CHAR *srcFile, CHAR *destFile );
    APIRET  APIENTRY  FRenFile     ( CHAR *oldFile, CHAR *newFile );

    /*----------------------------------------------------------------------*/

	APIRET  APIENTRY  FGetFileType ( CHAR *netfile, LONG *type );

    /* return file type */

    #define FILE_TYPE_UNKNOWN   0
    #define FILE_TYPE_MSG       1
    #define FILE_TYPE_PKT       2
    #define FILE_TYPE_ZIP       3
    #define FILE_TYPE_ARJ       4
    #define FILE_TYPE_LZH       5
    #define FILE_TYPE_RAR       6

    APIRET  APIENTRY  FReadArj ( MLIST **list, CHAR *file, CHAR *format, VOID (*func)( SHORT count ));
    APIRET  APIENTRY  FReadRar ( MLIST **list, CHAR *file, CHAR *format, VOID (*func)( SHORT count ));
    APIRET  APIENTRY  FReadLha ( MLIST **list, CHAR *file, CHAR *format, VOID (*func)( SHORT count ));
    APIRET  APIENTRY  FReadZip ( MLIST **list, CHAR *file, CHAR *format, VOID (*func)( SHORT count ));

	APIRET  APIENTRY  FUnZip   ( CHAR *zipfile, CHAR *filename, CHAR *path );

/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

/*--------------------------------------------------------------------------*/
/*  Functions from Sergey Radochin AKA 2:468/7@fidonet                      */
/*--------------------------------------------------------------------------*/

// For "code" in definition of Inp, Check, Num, But & Radio

#define INV_CONTROL     1
#define FLD_HIDDEN      2
#define BUT_OK          4
#define BUT_CANCEL      8
#define BUT_NONE        16

// For "control" in definition of Inp, Check, Num, But & Radio

#define NOCONTROL       -1

// For "code" in WFormRelease

#define FR_HIDE         0
#define FR_NOHIDE       1

// For WMenuGetBar id

#define M_CURBAR        -1

// For WMenuGet id

#define M_LASTBAR       -1

// For menu status

#define M_SHADOW        1
#define M_VERT          2
#define M_HORIZ         4

// For menu bars status

#define MB_CLOSE        1       // Close menu on choosing
#define MB_HIDEALL      2       // Hide all open menus on choosing
#define MB_HIDE         4       // Hide menu on choosing
#define MB_HASPD        8       // Button has pulldown submenu

/*---------------------------------------------------------------------*/
#define ERROR_MEMALLOC    -1
#define ERROR_NOFORM      -2
#define ERROR_NOFIELDS    -3
#define ERROR_FORMHIDDEN  -4
#define ERROR_NOACTIVE    -5
#define ERROR_ESCPRESSED  -6
#define ERROR_BUFERROR    -7
#define ERROR_NOMENU      -8
#define ERROR_NOBARS      -9
/*---------------------------------------------------------------------*/
struct _EDITINFO;

typedef struct _EDITINFO    EDITINFO;
typedef SHORT               FORM;
typedef SHORT               FIELD;
typedef SHORT               WMENU;
/*---------------------------------------------------------------------*/
struct _EDITINFO
{
                                                // NECESSERY PARAMETERS:
    WINDOW     win;                             // edit window
    CHAR      *buf;                             // edit buffer address
    ULONG      buflen;                          // buffer length
                                                // UNNECESSERY PARAMETERS:
    KEY      (*filter)( KEY key );              // filter function
                                                // (if not needed - NULL)
    VOID     (*status)( EDITINFO * );           // status update function
                                                // (if not needed - NULL)
    SHORT      lx, ly, fx, fy;                  // cursor coordinates
                                                // (usually 0, 0, 0, 0)
    SHORT      tabsize;                         // TAB size
    SHORT      changes;                         // 'changes' flag (output)
    CHAR       ins;                             // insert mode flag
                                                // IGNORE THIS PARAMETERS:
    SHORT      undotype;                        // ignore this
    CHAR      *undo;                            // must be NULL on 1st call
};
/*---------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/*---------------------------------------------------------------------*/

FORM    APIENTRY    WFormDefine    ( WINDOW win );
APIRET  APIENTRY    WFormRelease   ( FORM form, SHORT code );
APIRET  APIENTRY    WFormShow      ( FORM form );
APIRET  APIENTRY    WFormHide      ( FORM form );
APIRET  APIENTRY    WFormGet       ( FORM form );
APIRET  APIENTRY    WFormSetFunc   ( FORM form, VOID (*before)( FIELD f ),
                                     VOID (*after)( FIELD f ),
                                     KEY (*filter)( FIELD f, KEY key, BOOL *redisp ));
FIELD   APIENTRY    WFormString    ( FORM form, SHORT x, SHORT y, CHAR *text, CHAR *str,
                                     CHAR *(*arr)[], SHORT len, SHORT flen,
                                     SHORT (*validate) (CHAR *), FIELD control, CHAR code );
FIELD   APIENTRY    WFormCheck     ( FORM form, SHORT x, SHORT y, CHAR *text,
                                     SHORT *data, FIELD control, CHAR code );
FIELD   APIENTRY    WFormRadio     ( FORM form, SHORT x, SHORT y, CHAR *text,
                                     SHORT *data, FIELD control, CHAR code );
FIELD   APIENTRY    WFormNumber    ( FORM form, SHORT x, SHORT y, CHAR *text,
                                     SHORT *data, SHORT max, FIELD control, CHAR code );
FIELD   APIENTRY    WFormButton    ( FORM fo, SHORT x, SHORT y, CHAR *text, SHORT *data,
                                     VOID (*func)(VOID), FIELD control, CHAR code );

/*---------------------------------------------------------------------*/

WMENU   APIENTRY    WMenuCreate    ( SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT col,
                                     SHORT status, CHAR *border, CHAR *title );
WMENU   APIENTRY    WMenuDefine    ( WINDOW win, SHORT status );
APIRET  APIENTRY    WMenuItem      ( WMENU m, SHORT x, SHORT y, UCHAR *text, CHAR hotkey,
                                     SHORT id, SHORT status, VOID (*func)(VOID) );
APIRET  APIENTRY    WMenuGet       ( WMENU menu, SHORT id );
APIRET  APIENTRY    WMenuShow      ( WMENU menu );
APIRET  APIENTRY    WMenuHide      ( WMENU menu );
APIRET  APIENTRY    WMenuSetFunc   ( WMENU menu, VOID (*before)( SHORT id ),
                                     VOID (*after)( SHORT id ),
                                     KEY (*filter)( SHORT id, KEY key ) );
APIRET  APIENTRY    WMenuRelease   ( WMENU m );

APIRET  APIENTRY    WMenuGetItem   ( WMENU m, SHORT id, UCHAR *buf, SHORT len );
APIRET  APIENTRY    WMenuPutItem   ( WMENU m, SHORT id, UCHAR *newtext );
/*---------------------------------------------------------------------*/

APIRET  APIENTRY    WEditText      ( EDITINFO *info );

/*---------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
