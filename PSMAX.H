/****************************************************************
*****************************************************************
**
**  Copyright © 1994
**  ----------------------------------
**  P E T R O S O F T, INC.
**  7994 Locke Lee No. 39	
**  Houston, TX 77063-3053
**  713-266-8840
**  ----------------------------------
**  by:
**  CJ Fruge
**
**  All rights reserved.
**
**  Software & systems engineering by CJ Fruge
**
**  06/19/93
**
*****************************************************************
****************************************************************/

//: *************************************************************
//:   PSMAX.H
//:   macro defs
//: *************************************************************

#ifndef PSMAX_H_
#define PSMAX_H_

// --------------------------------------------------------------
// -- macros ----------------------------------------------------
// --------------------------------------------------------------

#define	invert (val, wt)	(int) ((1L << wt)-val-1)
//#define CONV_CPS_2_TICKS	55L
#define	CONV_S_2_EPH			3600.0f
#define	CONV_S_2_EPM			60.0f
//#define	CONV_CPS_2_EPM		(CLOCKS_PER_SEC * CONV_S_2_EPM)
//#define	CONV_CPS_2_EPH		(CLOCKS_PER_SEC * CONV_S_2_EPH)
#define	TSEX(t)						((clock_t)(t * 1000L))
#define	TOGGLE(a)		 			(a = !a)
#define LONG2UBYTE(var)		(var & 0xFF)
#define LONG2UINT(var)		(var & 0xFFFF)
#define FLOAT2UBYTE(var)	((long)var & 0xFF)
#define FLOAT2UINT(var)		((long)var & 0xFFFF)

#define	lpszDebug(str)	printf(#str " = %s\n",str)
#define	wDebug(val)			printf(#val " = %d\n",val)
#define	hwDebug(val)	 	printf(#val " = %04x\n",val)
#define	hlDebug(val)	 	printf(#val " = %08lx\n",val)
#define	lDebug(val)			printf(#val " = %ld\n",val)
#define	fDebug(val)			printf(#val " = %f\n",val)

//#define	EnableDlgItem(hWin,id,flag)	EnableWindow(GetDlgItem(hWin,id),flag)
//#define	SetDlgItemFocus(hWin,id)		SetFocus(GetDlgItem(hWin,id))

//#define	DEBUG_TW_DISP		(s)\
//_settextwindow( scr.rows-9, 82-scr.cols, scr.rows-1, (scr.cols/2)-1 );\
//_settextposition( 9, 28 );\
//_outtext(s)

//#define WAIT_CURSOR			SetCursor(LoadCursor(NULL,IDC_WAIT))

#endif // PSMAX_H_
//  ********************* end of file ***************************


