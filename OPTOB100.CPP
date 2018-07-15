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
//:	OPTOB100.CPP
//:	6/30/2006
//: *************************************************************

#include "stdafx.h"
#include "optob100.h"
// --------------------------------------------------------------
optoB100::optoB100(OMYSDEV omuxdev,CSerialPort * serio,int chcfg[NUMB_CHAN])
:optoMystic(omuxdev,serio,chcfg) {
	
	if ( isValid ) {
	// setup
	}
}

// --------------------------------------------------------------
optoB100::optoB100(OMYSDEV omuxdev,int comPort,long baudRate,int chcfg[NUMB_CHAN])
:optoMystic(omuxdev,comPort,baudRate,chcfg) {
	if ( isValid ) {
	// setup
	}
}

// --------------------------------------------------------------
optoB100::~optoB100(void) {
}

// --------------------------------------------------------------
BOOL optoB100::cmd_ResetDefault(void) {
	
	SendCmd('x',0,0,0);
	return(CheckData());
}

// --------------------------------------------------------------
BOOL optoB100::cmd_ChanOn(int chan) {

	CheckChannel(& chan);
	SendCmd('d',(1<<chan),0,0);
	return(CheckData());
}

// --------------------------------------------------------------
BOOL optoB100::cmd_ChanOff(int chan) {

	CheckChannel(&chan);
	SendCmd('e',(1<<chan),0,0);
	return(CheckData());
}

// --------------------------------------------------------------
long optoB100::GetCounter32(int chan) {
	
	CheckChannel(&chan);
	int chantype = GetChanType(chan);
	if ( (chantype == optoB100::COUNTER) || (chantype == optoB100::PHASEQUAD) ) {
		return GetData(chan);
	}
	return -1L;
}

// --------------------------------------------------------------
BOOL optoB100::cmd_EnableDisableCounter(int chan, BOOL bEnable) {

	CheckChannel(&chan);
	SendCmd('b',0,chan,bEnable);
	return(CheckData());
}

// --------------------------------------------------------------
int optoB100::cmd_ReadClearCounter16(int chan) {

	CheckChannel(&chan);
	SendCmd('o',chan,0,0);
	if ( CheckData() ) { return DecodeWord(); }
	return -1;
}

// --------------------------------------------------------------
int optoB100::cmd_GroupReadClearCounters32(void) {

	int j, cfg, pos = 0;

	for ( j=0; j<NUMB_CHAN; j++ ) {
		cfg = GetChanType(j);
		if ( cfg == optoB100::COUNTER || cfg == optoB100::PHASEQUAD ) {
			pos |= (1 << j);
		}
	}
	SendCmd('U',pos,0,0);
	if ( CheckData() ) {
		DecodeIntoArray(pos,8);
	}
	return pos;
}

// --------------------------------------------------------------
int optoB100::cmd_ReadFrequency(int chan) {

	CheckChannel(&chan);
	SendCmd('t',(1 << chan),0,0);
	if ( CheckData() ) {
		return DecodeWord();
	}
	return 0;
}

// --------------------------------------------------------------
int optoB100::cmd_GetPulseComplete(void) {
	
	SendCmd('V',0,0,0);
	if ( CheckData() ) { return DecodeWord(); }
	else return 0;
}

// --------------------------------------------------------------
void optoB100::cmd_GroupResetPeriod(int mask) {

	SendCmd('X',mask,0,0);
}

// --------------------------------------------------------------
int optoB100::cmd_GroupReadRestartPeriod32(BOOL bComplete) {

	int pos = bComplete ? cmd_GetPulseComplete() : GetChannelMask(optoB100::PERIOD);
	SendCmd('X',pos,0,0);
	if ( CheckData() ) { DecodeIntoArray(pos,8); }
	return pos;
}

// --------------------------------------------------------------
int optoB100::cmd_GroupReadPeriod32(BOOL bComplete) {

	int pos = bComplete ? cmd_GetPulseComplete() : GetChannelMask(optoB100::PERIOD);
	SendCmd('W',pos,0,0);
	if ( CheckData() ) { DecodeIntoArray(pos,8); }
	return pos;
}

// --------------------------------------------------------------
long optoB100::GetPulsePeriod(int chan) {

	CheckChannel(&chan);
	return GetData(chan) * 100;
}

// --------------------------------------------------------------
int optoB100::GetChannelMask(int chancfg) {

	int mask = 0;
	for ( int j=0; j<NUMB_CHAN; j++ ) {
		if ( GetChanType(j) == chancfg ) { mask |= (1 << j); }
	}
	return mask;
}

//  ******************** end of file ****************************

