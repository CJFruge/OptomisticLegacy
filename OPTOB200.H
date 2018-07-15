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
//:   OPTOB200.H  VERSION 1.00
//:   class declarations for optomystic classes
//: *************************************************************

#ifndef	OPTOB200_H_
#define	OPTOB200_H_

#include "optomys.h"

// empty channel
#define	EMPTY_CHAN		0x8000

// input module ranges
#define B200_GENERIC_INPUT	0

#define	FOUR_20_MA	 	100
#define	ZERO_5_VOLT	 	101
#define	ZERO_10_VOLT 	102

// input modules
#define	TYPE_AD3			10
#define	TYPE_AD6			11
#define	TYPE_AD5			12
#define	TYPE_AD8			13
#define	TYPE_AD17			14
#define	TYPE_AD18			15
#define	TYPE_AD7			16
#define	TYPE_AD9			17
#define	TYPE_AD11			18
#define	TYPE_AD12			19
#define	TYPE_AD13			20
#define	TYPE_AD10T2		21
#define	TYPE_AD16T		22
#define	TYPE_AD4			23
#define	TYPE_AD15			24
#define	TYEP_AD20			25

// output modules
#define	TYPE_DA3			40
#define	TYPE_DA4			41
#define	TYPE_DA5			42
#define	TYPE_DA6			43
#define	TYPE_DA7			44
#define	TYPE_DA8			45

// --------------------------------------------------------------
//		optob200 class
// --------------------------------------------------------------


class idString {
private:
	const enum { ID_SIZE = 12 };
	char  * eString [ID_SIZE];
public:
	const char * GetIDString(int);

	idString(void);
	~idString(void) { }
};

class optoB200 : public optoMystic {

private:
	typedef struct tagACHAN {
		WORD		range;
		float		maxaval;
		float		minaval;
		float		gain;
		float		offset;
		float		cc;

		int			min;
		int			max;
		int			hi;
		int			lo;
		int			stat;
		int			type;
	} ACHAN;

	typedef struct tagACHANSTRUCT {
		int		 	cfg;
		int			samples;
		int			avgpos;
		ACHAN		ach[optoMystic::NUMB_CHAN];
	} ACHANSTRUCT;

	ACHANSTRUCT	b200cfg;
	ACHANSTRUCT	* achcfg;
	void InitAnalogStructs(ACHANSTRUCT * ach,int nsamp,int avgpos);

protected:
	void	MassageAnalog		(int);	
public:
	optoB200(OMYSDEV,
			ACHANSTRUCT *,
			CSerialPort * serio,
			int chcfg[NUMB_CHAN],
			int avgpos=0xFFFF,
			int nsamples = 6);

	optoB200(OMYSDEV,
			ACHANSTRUCT *,
			int comPort,long baudRate,
			int chcfg[NUMB_CHAN],
			int avgpos=0xFFFF,
			int nsamples = 6);

	~optoB200(void);
	// redef
	
	// command set
	BOOL	cmd_ResetDefault();
	BOOL	cmd_ReadAnalog	(int,int);
	BOOL	cmd_ReadAnalog	(int type) {
		return cmd_ReadAnalog(achcfg->avgpos,type);
	}
	BOOL	cmd_SetInputAvgWeight	(int,int);
	BOOL	cmd_SetInputAvgweight	(void) {
		return cmd_SetInputAvgWeight(achcfg->avgpos,achcfg->samples);
	}
	BOOL	cmd_WriteOutputs	(int,int);

	// accessor
	const int		GetAchanStat	(int);
	const long GetAchan(int);
	const ACHAN	*GetAchanInfo	(int);
};

#endif	// OPTOB2_H_

//: *********************** end of file *************************


