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
//:   OPTOB200.CPP  VERSION 1.00
//:   base class methods for optoB200
//: *************************************************************

// --------------------------------------------------------------
// -- includes --------------------------------------------------
// --------------------------------------------------------------

#include "stdafx.h"
#include "optob200.h"

// --------------------------------------------------------------
optoB200::optoB200(OMYSDEV omuxdev,ACHANSTRUCT * ACScfg,\
CSerialPort * serio,int chcfg[NUMB_CHAN],int avgpos,int nsamples)
:optoMystic(omuxdev,serio,chcfg) {

	InitAnalogStructs(ACScfg,nsamples,avgpos);
}

// --------------------------------------------------------------
optoB200::optoB200(OMYSDEV omuxdev,ACHANSTRUCT * ACScfg,\
int comPort,long baudRate,int chcfg[NUMB_CHAN],int avgpos,int nsamples)
:optoMystic(omuxdev,comPort,baudRate,chcfg) {
	
	InitAnalogStructs(ACScfg,nsamples,avgpos);
}

// --------------------------------------------------------------
optoB200::~optoB200(void) {
}



/*
00 = Generic Input Module
01 = Reserved
02 = Reserved
03 = G4AD3 4 to 20 mA
04 = G4AD4 ICTD
05 = G4AD5 Type J Thermocouple
06 = G4AD6 0 to 5 VDC
07 = G4AD7 0 to 10 VDC
08 = G4AD8 Type J Thermocouple
09 = G4AD9 0 to 50 mV
0A = G4AD10 100 Ohm RTD
0B = G4AD11 -5 to +5 VDC
0C = G4AD12 -10 to +10 VDC
0D = G4AD13 0 to 100 mV
10 = G4AD16 0 to 5 Amperes
11 = G4AD17 Type R Thermocouple
12 = G4AD18 Type T Thermocouple
13 = G4AD19 Type E Thermocouple
14 = G4AD20 0 to 4095 Hz.
16 = G4AS22 0 to 1 VDC
17 = G4AD17 Type S Thermocouple
18 = G4AD24 Type B Thermocouple
19 = G4AD25 0 to 100 VAC/VDC
1A = G4AD26 -5 V/s to +5 V/s dV/dt
1E = Type N Thermocouple*
1F = Type G Thermocouple*
20 = Type C Thermocouple*
21 = Type D Thermocouple*

80 = Generic Output Module
81 = Reserved
82 = Reserved
83 = G4DA3 4 to 20 mA
84 = G4DA4 0 to 5 VDC
85 = G4DA5 0 to 10 VDC
86 = G4DA6 -5 to +5 VDC
87 = G4DA7 -10 to +10 VDC
88 = G4DA8 0 to 20 mA
89 = G4DA9 Time Proportional Output
*/
// --------------------------------------------------------------
void optoB200::InitAnalogStructs(ACHANSTRUCT * ACSconfig, int nSamples, int avgPos) {

	achcfg = &b200cfg;
	if ( !ACSconfig ) {
		for (int j=0; j<NUMB_CHAN; j++) {
			achcfg->ach[j].min 	= achcfg->ach[j].lo = 0;
			achcfg->ach[j].max 	= achcfg->ach[j].hi = 4095;
			achcfg->ach[j].stat	= TRUE;
			achcfg->ach[j].type	= TYPE_AD3;
			achcfg->ach[j].range	= FOUR_20_MA;
			achcfg->ach[j].maxaval= 20.0f;
			achcfg->ach[j].minaval= 4.0f;
			achcfg->ach[j].gain = 1.0f;
			achcfg->ach[j].cc = 1.0f;
			achcfg->ach[j].offset = 0.0f;
			SetChanConfig(0,j);
		}
		achcfg->samples = nSamples;
		achcfg->avgpos = avgPos;
	}
	else { memcpy(achcfg,ACSconfig,sizeof(ACHANSTRUCT)); }
}

// --------------------------------------------------------------
BOOL optoB200::cmd_ResetDefault(void) {
	
	SendCmd('J',0,0,0);
	return(CheckData());
}
 
// --------------------------------------------------------------
BOOL optoB200::cmd_ReadAnalog(int pos,int type) {

	SendCmd('R',pos,type,0);
	int rtn = CheckData();
	if ( rtn ) {
		DecodeIntoArray(pos,4);
	}
	MassageAnalog(pos);
	return(rtn);
}

// --------------------------------------------------------------
BOOL optoB200::cmd_SetInputAvgWeight(int pos,int avg) {

	BOOL rtn;
	int j;

	for ( j=0; j<NUMB_CHAN; j++ ) {
		if ( (1 << j) & pos ) {
			SendCmd('h',0,j,avg);
			rtn = CheckData();	
			if ( !rtn )
				break;
		}
	}
	return rtn;
}

// --------------------------------------------------------------
BOOL optoB200::cmd_WriteOutputs(int pos,int val) {

	SendCmd('X',pos,0,val);
	return(CheckData());
}

// --------------------------------------------------------------
const long optoB200::GetAchan(int chan) {

	CheckChannel(&chan);
	return GetData(chan);
}

// --------------------------------------------------------------
const int optoB200::GetAchanStat(int chan) {

	CheckChannel(&chan);
	return (achcfg->ach[chan].stat);
}

// --------------------------------------------------------------
const optoB200::ACHAN * optoB200::GetAchanInfo(int chan) {

	CheckChannel(&chan);
	return (&(achcfg->ach[chan]));
}

// --------------------------------------------------------------
void optoB200::MassageAnalog(int pos) {

	for (int j=0; j<NUMB_CHAN; j++) {
		if ( (pos & (1 << j)) ){
			if ( GetData(j) >= EMPTY_CHAN ) {
				achcfg->ach[j].stat = FALSE;
				SetData(j,0);
			}
			else {
				achcfg->ach[j].stat = TRUE;
			}
		}
	}
}

//  *********************** end of file *************************

