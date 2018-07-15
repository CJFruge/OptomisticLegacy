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

#ifndef OPTOB100_H_
#define OPTOB100_H_

#include "optomys.h"

//: *************************************************************
//:   OPTOB100.H  VERSION 1.00
//:   class declarations for optomystic classes
//: *************************************************************

class optoB100:public optoMystic {

public:
	enum {
COUNTER			=	0x00,
POSPULSE		=	0x01,
NEGPULSE		=	0x02,
PERIOD			=	0x03,
FREQ				=	0x04,
PHASEQUAD		=	0x05,
ONTIME			=	0x06,
OFFTIME			=	0x07,
STDOUT			=	0x80
};

private:
	int		GetChannelMask				(int);

public:
	optoB100(OMYSDEV omuxdev,CSerialPort * serio,int chcfg[NUMB_CHAN]);
	optoB100(OMYSDEV omuxdev,int comPort,long baudRate,int chcfg[NUMB_CHAN]);
	~optoB100(void);

	// command functions
	BOOL	cmd_ResetDefault							(void);
	BOOL	cmd_ChanOn										(int);
	BOOL	cmd_ChanOff										(int);
	BOOL	cmd_EnableDisableCounter			(int,BOOL);
	int		cmd_GroupReadClearCounters32	(void);
	int		cmd_ReadClearCounter16				(int);
  int		cmd_ReadFrequency							(int);
	long	GetCounter32									(int);
	int		cmd_GetPulseComplete					(void);
	int		cmd_GroupReadRestartPeriod32	(BOOL bComplete = FALSE);
	int		cmd_GroupReadPeriod32					(BOOL bComplete = FALSE);
	long	GetPulsePeriod								(int);
	void	cmd_GroupResetPeriod					(int);

	// data accessors
};

#endif // OPTOB1_H_

//: *********************** end of file *************************

