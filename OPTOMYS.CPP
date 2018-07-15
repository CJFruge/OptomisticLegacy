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
//:   OPTOMYS.CPP  VERSION 1.00
//:   base class methods for optoMystic
//: *************************************************************

// --------------------------------------------------------------
// -- includes --------------------------------------------------
// --------------------------------------------------------------

#include "stdafx.h"
#include "optomys.h"

// --------------------------------------------------------------
// optoComm constructor
// --------------------------------------------------------------
optoMystic::optoMystic(OMYSDEV devNumber,int comPort,long baudRate,int chcfg[NUMB_CHAN]) {
	
	sio = new CSerialPort();	
	if ( sio ) {
		sio->Open(comPort,baudRate);
		if ( sio->IsOpen() ) { optoMystic(devNumber,sio,chcfg,TRUE); }
	}
}

// --------------------------------------------------------------
optoMystic::optoMystic(OMYSDEV omuxdev,CSerialPort * serio,int cfg[NUMB_CHAN],bool bCSPCreated) {

	int j;
	bCreated = bCSPCreated;
	dev = omuxdev;
	if ( !bCSPCreated ) { sio = serio; }
	pcmd	 = &cmd[0];
	prbuff = &rbuff[0];
	for (j=0; j<NUMB_CHAN; j++) {
		data[j] = 0;
	}
	for (j=0; j<NUMB_CHAN; j++) { 
		chcfg[j] = cfg[j];
	}
	errCount = 0;
	lastErr = 0;
	eS = new errString();
	if ( !cmd_GetStatus() )	{ isValid = cmd_PUClear(); }
	else								{ isValid = TRUE; }
	cmd_SetChanConfig();
	comtmo.WriteTotalTimeoutConstant = 0;
	comtmo.WriteTotalTimeoutMultiplier = 0;
	comtmo.ReadIntervalTimeout = 0;
	comtmo.ReadTotalTimeoutConstant = GENERAL_TIMEOUT;
	comtmo.ReadTotalTimeoutMultiplier = 0;
	sio->SetTimeouts(comtmo);
}

// --------------------------------------------------------------
optoMystic::~optoMystic(void) {

	if ( sio->IsOpen() ) { sio->Close(); }
	if ( bCreated ) { delete sio; }
	delete eS;
}

// --------------------------------------------------------------
errString::errString(void) {
	
	eString[0] = "00 No error.\0";
	eString[1] = "01 Undefined command.\0";
	eString[2] = "02 Checksum error.\0";
	eString[3] = "03 Buffer overrun.\0";
	eString[4] = "04 Power-up clear.\0";
	eString[5] = "05 Data field error.\0";
	eString[6] = "06 Watchdog time-out.\0";
	eString[7] = "07 Illegal data value.\0";
	eString[8] = "08 Busy error.\0";
	eString[9] = "09 Invalid module type.\0";
	eString[10] = "10 Invalid event entry.\0";
	eString[11] = "11 High res time error.\0";
}

// --------------------------------------------------------------
const char * errString::GetErrorString(int eCode) {
	
	return eString[abs(eCode % EC_SIZE)];
}

// --------------------------------------------------------------
const char * optoMystic::GetReceiveBuffer(void) {

	return prbuff;
}

// --------------------------------------------------------------
BOOL optoMystic::CheckData(void) {

	char	* psb = prbuff;
	char	* pse;
	int	cksum;
	char ck[4];

#ifdef OPTO_DEBUG
puts(prbuff);
#endif

	pse = strrchr(psb,'\r');
	if ( *pse == '\r' )	{
		pse -= 2;		// back up to beginning of cksum
		for (cksum=0; psb < pse; psb++) {
			cksum += *psb;
		}
		cksum %= 0x100;
		sprintf(ck,"%02X",cksum);
		if ( !(strncmp(ck,pse,2)) ) { 
			if ( *psb == 'N' ) {
				lastErr = DecodeWord();
				errCount++;
				return(FALSE);
			} 
			else { return (TRUE); }
		}
	}
	errCount++;
	return(FALSE);
}

// --------------------------------------------------------------
BOOL optoMystic::cmd_PUClear(void) {

	SendCmd('A',0,0,0);
	return(CheckData());
}

// --------------------------------------------------------------
BOOL optoMystic::cmd_ResponseDelay(char delay) {

	SendCmd('~',0,0,delay);
	return(CheckData());
}

// --------------------------------------------------------------
BOOL optoMystic::cmd_Identify(void) {
	
	SendCmd('F',0,0,0);
	return CheckData();
}

// --------------------------------------------------------------
BOOL optoMystic::cmd_Reset(void) {
	
	SendCmd('B',0,0,0);
	return CheckData();
}

// --------------------------------------------------------------
int optoMystic::cmd_GetConfig(void) {
	
	SendCmd('j',0,0,0);
	if ( CheckData() ) {
		return DecodeWord();
	}
	return 0;
}

// --------------------------------------------------------------
void optoMystic::GetChanType(int chtype[NUMB_CHAN]) {

	for (int j=0; j<NUMB_CHAN; j++) { 
		chtype[j] = chcfg[j];
	}
}

// --------------------------------------------------------------
const int optoMystic::GetChanType(int chan) {

	CheckChannel(&chan);
	return chcfg[chan];
}

// --------------------------------------------------------------
void optoMystic::SetChanConfig(int chconfig, int chan) {

	CheckChannel(&chan);
	chcfg[chan] = chconfig;
}

// --------------------------------------------------------------
BOOL optoMystic::cmd_SetChanConfig() {

	int j;
	BOOL rtn;
	for (j=0; j<NUMB_CHAN; j++) {
		SendCmd('a',0,j,chcfg[j]);
		rtn = CheckData();
		if ( !rtn ) break;
	}
	return rtn;
}

// --------------------------------------------------------------
BOOL optoMystic::cmd_StoreConfig(void) {

	SendCmd('E',0,0,0);
	return(CheckData());
}

// --------------------------------------------------------------
BOOL optoMystic::cmd_ReadModule(void) {

	SendCmd('Y',0,0,0);
	BOOL rtn = CheckData();
	if ( rtn ) {
		DecodeIntoArray(0xFFFF,2);
	}
	return rtn;
}
	
// --------------------------------------------------------------
void optoMystic::SendCmd(char cl,int pos,int mod,int data) {
	
	int 	len,cksum;
	char	* ps;
	BOOL	isMod,isPos,isData;
	DWORD dwBytes;

	len = sprintf(pcmd,">%02X%c",dev,cl);
	
	switch (cl) {
		case 'A':
		case 'B':
		case 'F':
		case '^':
		case 'x':
		case 'J':
		case 'V':
			isPos = FALSE;
			isMod = FALSE;
			isData = FALSE;
			break;
		case 'h':
			isPos = FALSE;
			isMod = TRUE;
			isData = TRUE;
 			break;
		case 'a':
			isPos = FALSE;
			isMod = TRUE;
			isData = TRUE;
 			break;
		case 'R':
			isPos = TRUE;
			isMod = TRUE;
			isData = FALSE;
			break;
		case 'b':
			isPos = FALSE;
			isMod = TRUE;
			isData = TRUE;
			break;
		case 'o':
			isPos = FALSE;
			isMod = TRUE;
			isData = FALSE;
			break;
		default:
			isPos = TRUE;
			isMod = FALSE;
			isData = FALSE;
			break;
	}
	
	if ( isPos || pos ) {
		pos &= 0xFFFF;
		len = sprintf(tmp,"%04X",pos);
		strncat(pcmd,tmp,len);
	}		

	if ( isMod || mod ) {
		len = sprintf(tmp,"%02X",mod);
		strncat(pcmd,tmp,len);
	}

	if ( isData || data ) {
		if ( (cl == 'a') || (cl == 'b') ) {
			len = sprintf(tmp,"%02X",data);
		}
		else {
			len = sprintf(tmp,"%04X",data);
		}
		strncat(pcmd,tmp,len);
	}
	
	for(ps=pcmd+1,cksum=0; *ps != '\0'; ps++) {
	  	cksum += *ps;
	}

	cksum %= 0x100;
	sprintf(tmp,"%02X\r",cksum);
	strncat(pcmd,tmp,2);
	
#ifdef OPTO_DEBUG
puts(pcmd);
#endif

	txlen = sio->Write((const void *)pcmd,(DWORD)strlen(pcmd));
	for ( int j=0; j<BUFF_SIZE; j++ ) { prbuff[j] = '\0'; }
	sio->DataWaiting(40);
	while ( dwBytes = sio->BytesWaiting() > 0 ) {
		rxlen = (int)sio->Read((void *)tmp,dwBytes < TMP_SIZE ? dwBytes : TMP_SIZE);
		if ( strlen(prbuff) + rxlen < BUFF_SIZE ) { strncat(prbuff,tmp,rxlen); }
	}
}	

// --------------------------------------------------------------
int optoMystic::DecodeWord(void) {
	
	char  * psb = prbuff;
	char	s[16];
	
	psb++;
	for (int j=1; j<sizeof(s); j++) s[j]='\0';
	strncpy(s,psb,4);
	return ((int) strtol(s,0,16));
}

// --------------------------------------------------------------
long optoMystic::DecodeLong(void) {
	
	char  * psb = prbuff;
	char	s[16];
	
	if ( *psb == 'N' ) {
		return(0);
	}
	psb++;
	for (int j=1; j<sizeof(s); j++) s[j]='\0';
	strncpy(s,psb,8);
	return (strtol(s,0,16));
}

// --------------------------------------------------------------
void optoMystic::ResetConfig (BOOL forced) {

	if ( !cmd_GetStatus() || forced ) {
		cmd_PUClear();
		isValid = cmd_SetChanConfig();
		errCount = 0;
	}
	else {
		isValid = FALSE;
	}
}
// --------------------------------------------------------------
BOOL optoMystic::cmd_GetStatus (void) {

	SendCmd('F',0,0,0);
	return(CheckData());
}

// --------------------------------------------------------------
void optoMystic::DecodeIntoArray(int pos,char width) {

	int 		j;
	char  	* psb = prbuff;
	char 		spos[8];

	for (j=1;j<sizeof(spos);j++) spos[j] = '\0';
	psb++;
	for (j=NUMB_CHAN-1; j>=0; j--) {
		if ( (pos & (1 << j)) ) {
			strncpy(spos,psb,width);
			psb += width;
			data[j] = strtol(spos,0,16);
		}
	}	
}

// --------------------------------------------------------------
void optoMystic::CheckChannel(int * chan) {

	*chan = abs(*chan);
	*chan %= NUMB_CHAN;
}

// --------------------------------------------------------------
const long optoMystic::GetData(int chan) {

	CheckChannel(&chan);
	return data[chan];
}

// --------------------------------------------------------------
void optoMystic::SetData(int chan, long val) {
	
	CheckChannel(&chan);
	data[chan] = val;
}

// --------------------------------------------------------------
const char * optoMystic::ErrorMessage(void) {

	return eS->GetErrorString(lastErr);
}
// *********************** end  of file *************************