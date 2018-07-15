/****************************************************************
*****************************************************************
**
**  Copyright © 2006
**  ----------------------------------
**  P E T R O S O F T, INC.
**  9955 Kempwood No. 804
**  Houston, TX 77080
**  713.460.8722
**  ----------------------------------
**  by:
**  CJ Fruge
**
**  All rights reserved.
**
**  Software & systems engineering by CJ Fruge
**
**  06/30/2006
**
*****************************************************************
****************************************************************/

//: *************************************************************
//:   OPTOMYS.H  VERSION 1.00
//:   class declarations for optomystic classes
//: *************************************************************

// --------------------------------------------------------------
// -- includes --------------------------------------------------
// --------------------------------------------------------------

#ifndef	OPTOMYS_H_
#define	OPTOMYS_H_

//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <windows.h>
//#include "compplib.hpp"
#include "stdafx.h"
#include "psmax.h"
#include "serialport.h"

#pragma warning(disable : 4995)

typedef double long 	DLONG;
typedef	unsigned char	OMYSDEV;

// --------------------------------------------------------------
// -- error string class ----------------------------------------
// --------------------------------------------------------------

class errString {
private:
	const enum { EC_SIZE = 12 };
	char  * eString [EC_SIZE];
public:
	const char * GetErrorString(int);

	errString(void);
	~errString(void) { }
};
	
// --------------------------------------------------------------
// -- optomys class ---------------------------------------------
// --------------------------------------------------------------

class optoMystic {

protected:
	const enum {
	NUMB_CHAN	=	16,
	BUFF_SIZE =	256,
	TMP_SIZE = 128,
	EC_SIZE = 12,
	GENERAL_TIMEOUT = 200
	};
private:
	char				cmd[BUFF_SIZE];
	char				* pcmd;
	CSerialPort	* sio;
	char				rbuff[BUFF_SIZE];
	char				tmp[TMP_SIZE];
	char				* prbuff;
	COMMTIMEOUTS		comtmo;
	errString		* eS;
	bool				bCreated;
	// class data
	OMYSDEV				dev;
	int					rxlen;
	int					txlen;
	int					lastErr;
	long				errCount;
	int					chcfg[NUMB_CHAN];
	long				data[NUMB_CHAN];

protected:
	BOOL				isValid;
protected:
	const char*	GetReceiveBuffer(void);
	BOOL	cmd_PUClear					(void);
	virtual BOOL	ResetDefault		(void) { return FALSE; };
	void	DecodeIntoArray	(int,char);
	int		DecodeWord			(void);
	long	DecodeLong			(void);
	void	CheckChannel		(int * chan);
	void	SetChanConfig		(int chconfig,int chan);
	// string handling command functions
	void	SendCmd			(char,int,int,int);
	BOOL	CheckData		(void);
	
	// shared optomux funcs
public:
		void	ResetConfig				(BOOL forced = FALSE);
		BOOL 	cmd_SetChanConfig			();
		BOOL	cmd_GetStatus					(void);
		BOOL	cmd_Reset							(void);
		void  GetChanType				(int chtype[NUMB_CHAN]);
		const int	GetChanType		(int);
		BOOL	cmd_ResponseDelay			(char);
		BOOL	cmd_StoreConfig				(void);
		BOOL	cmd_Identify					(void);
		BOOL	cmd_GetConfig					(void);
		BOOL	cmd_ReadModule				(void);
		BOOL	IsValidDevice			(void) { return isValid; };
		const char * ErrorMessage(void);
		const long GetErrCount	(void) { return errCount; };
		const long GetData			(int ch);
		void	SetData						(int ch, long val);
// constructor,destructor
protected:
	optoMystic			(OMYSDEV devNumber,CSerialPort * serio,int chcfg[NUMB_CHAN],bool bCSPCreated = FALSE);
	optoMystic			(OMYSDEV devNumber,int comPort,long baudRate,int chcfg[NUMB_CHAN]);
	~optoMystic			(void);
};
	
#endif	// OPTOCOM_H_
//  ************************ end of file ************************

