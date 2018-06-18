// This is a little program I wrote to investigate Z ordering.  If you
// uncomment the WinCreateWindow and comment out the WinCreateStdWIndow,
// the z ordering doesnt work as well.  There is obviously some msg
// that I am not handling that I should.    Feel free to use what you want
// out of this code.  Paul Montgomery CIS: 71500,3525


#define INCL_WIN
#define INCL_GPI
#define INCL_DOSDATETIME
#define INCL_DOSPROCESS
#include <os2.h>

#define BUG_MOVE_START     (WM_USER + 1)
#define BUG_MOVE           (WM_USER + 2)
#define BUG_JUMP           (WM_USER + 3)
#define BUG_POINTS         (27)

LONG iconx,icony;
HAB         hab;
LONG i = 0;
HWND        hframe, hclient;
HBITMAP     hbm;
HWND        old;
BOOL        hidden;
HWND        hwndnext;

POINTL      bug[BUG_POINTS] = { 0,0,
                        8,8,
                        8,9,
                        8,8,
                        6,4,
                        7,4,
                        8,5,
                        9,5,
                        8,5,
                        7,4,
                        2,4,
                        1,5,
                        0,5,
                        1,5,
                        2,4,
                        5,1,
                        4,1,
                        3,2,
                        7,2,
                        9,0,
                        1,8,
                        1,9,
                        1,8,
                        3,6,
                        3,5,
                        3,3,
                        6,3 };

#pragma linkage ( WinProc,system )
MRESULT WinProc ( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 );

#pragma linkage ( thread,system )
VOID thread (ULONG arg)
{
   DATETIME  dt;

   while (TRUE)
      {
      DosGetDateTime ( &dt );
      DosSleep ( ((LONG)dt.hundredths) * 250L);
      WinPostMsg ( hframe, BUG_JUMP, 0, 0 );
      }
}

int main ( int argc, char *argv[] )
{
   QMSG        qmsg;
   HMQ         hmq;
   ULONG       fs;
   TID         tid;

   fs =
         FCF_ICON   |
         FCF_TASKLIST   |
         FCF_NOBYTEALIGN
         ;

   if((hab = WinInitialize(0)) == 0)
      {
      return(FALSE);
      }

   if((hmq = WinCreateMsgQueue(hab, 0)) == 0)
      {
      WinTerminate(hab);
      return(FALSE);
      }

   DosCreateThread ( &tid, (PFNTHREAD)thread, 1, 0, 8000 );

   WinRegisterClass(hab,
                    "BUG",
                    WinProc,
                    CS_SIZEREDRAW | CS_SYNCPAINT,
                    0);

   hframe = WinCreateStdWindow(HWND_DESKTOP,
           0,
           &fs,
           "BUG",
           "",
           0L,
           0L,
           1,
           &hclient);

//   hframe = WinCreateWindow ( HWND_DESKTOP,
//            "BUG",
//            NULL,
//            WS_VISIBLE,
//            0,0,
//            0,0,
//            HWND_DESKTOP,
//            HWND_BOTTOM,
//            1,
//            NULL,
//            NULL );

   WinSetWindowPos ( hframe, HWND_TOP, 300, 300,
         10, 10,
         SWP_SHOW | SWP_MOVE | SWP_ZORDER | SWP_SIZE );

   WinPostMsg ( hframe, BUG_JUMP, 0, 0 );

   while(WinGetMsg(hab, &qmsg, 0, 0, 0))
      {
      WinDispatchMsg(hab, &qmsg);
      }

   WinDestroyWindow(hframe);
   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
   return (0);

}


/* **************************************************************
 * Function Name:  WinProc
 *
 * Description: This function is the window procedure for the panel
 *              editor.
 *
 * Parameter      Description
 * --------------------------------------------------------------
 * The normal window parameters
 *
 * Returns:
 *
 * Comments:
 *
 ***************************************************************/

MRESULT WinProc ( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 )
{
   HPS   hps;
   RECTL rc;
   HENUM henum;
   SWP   swp1, swp2, swp3;
   BOOL  out;

   switch (msg)
      {
      case WM_CREATE:
         return (MPARAM)FALSE;
         break;

      case WM_BUTTON2DOWN:
         WinPostMsg ( hwnd, BUG_MOVE_START, 0, 0 );
         return (MPARAM)TRUE;
         break;

      case WM_BUTTON1DBLCLK:
         WinPostMsg ( hwnd, WM_QUIT, 0L, 0L );
         return (MRESULT) TRUE;
         break;

      case WM_ADJUSTWINDOWPOS:
         ((PSWP)mp1)->hwndInsertBehind = HWND_BOTTOM;
         return (MPARAM) NULL;
         break;

      case WM_DESTROY:
         WinPostMsg ( hwnd, WM_QUIT, 0L, 0L );
         return (MRESULT) TRUE;
         break;

      case WM_TIMER:
         if ((USHORT)mp1 == 1)
            {
            WinPostMsg ( hwnd, BUG_MOVE, (MPARAM)hwndnext, 0 );
            }
         return (MPARAM)NULL;
         break;

      case BUG_JUMP:
         WinQueryWindowPos ( hframe, &swp3 );
         if (swp3.x > 30)
            {
            swp3.x -=25;
            }
         else
            {
            swp3.x += 40;
            }

         if (swp3.y > 20)
            {
            swp3.y -=18;
            }
         else
            {
            swp3.y +=30;
            }

         swp3.fl = SWP_MOVE | SWP_SHOW;
         WinSetMultWindowPos ( hab, &swp3, 1 );
         WinPostMsg ( hwnd, BUG_MOVE_START, 0, 0 );
         return (MPARAM)TRUE;
         break;

      case BUG_MOVE_START:
         // we need to determine where to move to and then we need to move
         // there.

         henum = WinBeginEnumWindows ( HWND_DESKTOP );
         out = FALSE;
         WinQueryWindowPos ( hframe, &swp1 );
         while (!out)
            {
            hwndnext = WinGetNextWindow  ( henum );
            if (hwndnext != hframe)
               {
               if (hwndnext == NULL)
                  {
                  out = TRUE;
                  }
               else
                  {
                  WinQueryWindowPos ( hwndnext, &swp2 );
                  if ((swp1.cx <= swp2.cx) && (swp1.cy <= swp2.cy))
                     {
                     WinStartTimer ( hab, hwnd, 1, 110 );
                     out = TRUE;
                     }
                  }
               }
            }
         WinEndEnumWindows ( henum );
         return (MPARAM)TRUE;
         break;

      case BUG_MOVE:
         // mp1 = hwnd to move under
         if ( WinIsWindowShowing(hwnd) )
            {
            hidden = FALSE;
            // move under the window
            WinQueryWindowPos ( hframe, &swp1 );
            WinQueryWindowPos ( hwndnext, &swp2 );

            if ( swp1.x < swp2.x )
               {
               swp1.x += 11;
               }
            else if (swp1.x > swp2.x)
               {
               swp1.x -= 10;
               }

            if (swp1.y < swp2.y)
               {
               swp1.y += 7;
               }
            else if (swp1.y > swp2.y)
               {
               swp1.y -= 6;
               }

            swp1.hwndInsertBehind = swp2.hwnd;
            swp1.fl = SWP_MOVE | SWP_SHOW | SWP_ZORDER ;

            WinSetMultWindowPos ( hab, &swp1, 1 );
            }
         else
            {
            hidden = TRUE;
            WinStopTimer ( hab, hwnd, 1 );
            }
         return (MPARAM) TRUE;
         break;

      case WM_PAINT:
         if (hidden)
            {
            WinPostMsg ( hwnd, BUG_MOVE_START, 0, 0 );
            }

         hps = WinBeginPaint(hwnd, 0, 0);

//         GpiErase ( hps );
         WinQueryWindowRect(hwnd, &rc);

         WinFillRect(hps, &rc, CLR_BACKGROUND);

         GpiMove ( hps, (PPOINTL) &bug[0] );
         GpiPolyLine ( hps, BUG_POINTS, &bug[0] );

         WinEndPaint(hps);

         return (MPARAM)TRUE;
         break;

      }

return(WinDefWindowProc(hwnd, msg, mp1, mp2));
}
