/*
Module : SERIALPORT.H
Purpose: Declaration for an MFC wrapper class for serial ports
Created: PJN / 31-05-1999

Copyright (c) 1999 - 2006 by PJ Naughter.  

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/



///////////////////// Macros / Structs etc //////////////////////////

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#ifndef CSERIALPORT_EXT_CLASS
#define CSERIALPORT_EXT_CLASS
#endif

#ifndef CSERIALPORT_EXT_API
#define CSERIALPORT_EXT_API
#endif


/////////////////////////// Classes ///////////////////////////////////////////

///////////////// Serial port exception class /////////////////////////////////

class CSERIALPORT_EXT_CLASS CSerialException : public CException
{
public:
//Constructors / Destructors
	CSerialException(DWORD dwError);

//Methods
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL GetErrorMessage(LPTSTR lpstrError, UINT nMaxError,	PUINT pnHelpContext = NULL);
	CString GetErrorMessage();

//Data members
	DWORD m_dwError;

protected:
	DECLARE_DYNAMIC(CSerialException)
};


/////////////// The main serial port class ////////////////////////////////////

class CSERIALPORT_EXT_CLASS CSerialPort
{
public:
//Enums
  enum FlowControl
  {
    NoFlowControl,
    CtsRtsFlowControl,
    CtsDtrFlowControl,
    DsrRtsFlowControl,
    DsrDtrFlowControl,
    XonXoffFlowControl
  };

  enum Parity
  {    
    EvenParity,
    MarkParity,
    NoParity,
    OddParity,
    SpaceParity
  };

  enum StopBits
  {
    OneStopBit,
    OnePointFiveStopBits,
    TwoStopBits
  };

//Constructors / Destructors
  CSerialPort();
  virtual ~CSerialPort();

//General Methods
  void Open(int nPort, DWORD dwBaud = 9600, Parity parity = NoParity, BYTE DataBits = 8, 
            StopBits stopbits = OneStopBit, FlowControl fc = NoFlowControl, BOOL bOverlapped = FALSE);
  void Close();
  void Attach(HANDLE hComm);
  HANDLE Detach();
  operator HANDLE() const { return m_hComm; };
  BOOL IsOpen() const { return m_hComm != INVALID_HANDLE_VALUE; };
#ifdef _DEBUG
  void Dump(CDumpContext& dc) const;
#endif

//Reading / Writing Methods
  DWORD Read(void* lpBuf, DWORD dwCount);
  BOOL  Read(void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped, DWORD* pBytesRead=NULL);
  void  ReadEx(void* lpBuf, DWORD dwCount);
  DWORD Write(const void* lpBuf, DWORD dwCount);
  BOOL  Write(const void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped, DWORD* pBytesWritten=NULL);
  void  WriteEx(const void* lpBuf, DWORD dwCount);
  void  TransmitChar(char cChar);
  void  GetOverlappedResult(OVERLAPPED& overlapped, DWORD& dwBytesTransferred, BOOL bWait);
  void  CancelIo();
  DWORD BytesWaiting();
  BOOL  DataWaiting(DWORD dwTimeout);

//Configuration Methods
  void GetConfig(COMMCONFIG& config);
  static void GetDefaultConfig(int nPort, COMMCONFIG& config);
  void SetConfig(COMMCONFIG& Config);
  static void SetDefaultConfig(int nPort, COMMCONFIG& config);

//Misc RS232 Methods
  void ClearBreak();
  void SetBreak();
  void ClearError(DWORD& dwErrors);
  void GetStatus(COMSTAT& stat);
  void GetState(DCB& dcb);
  void SetState(DCB& dcb);
  void Escape(DWORD dwFunc);
  void ClearDTR();
  void ClearRTS();
  void SetDTR();
  void SetRTS();
  void SetXOFF();
  void SetXON();
  void GetProperties(COMMPROP& properties);
  void GetModemStatus(DWORD& dwModemStatus); 

//Timeouts
  void SetTimeouts(COMMTIMEOUTS& timeouts);
  void GetTimeouts(COMMTIMEOUTS& timeouts);
  void Set0Timeout();
  void Set0WriteTimeout();
  void Set0ReadTimeout();

//Event Methods
  void SetMask(DWORD dwMask);
  void GetMask(DWORD& dwMask);
  void WaitEvent(DWORD& dwMask);
  BOOL WaitEvent(DWORD& dwMask, OVERLAPPED& overlapped);
  
//Queue Methods
  void Flush();
  void Purge(DWORD dwFlags);
  void TerminateOutstandingWrites();
  void TerminateOutstandingReads();
  void ClearWriteBuffer();
  void ClearReadBuffer();
  void Setup(DWORD dwInQueue, DWORD dwOutQueue);

//Overridables
  virtual void OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped);

//Static methods
  static void ThrowSerialException(DWORD dwError = 0);

protected:
//Typedefs
  typedef BOOL (WINAPI CANCELIO)(HANDLE);
  typedef CANCELIO* LPCANCELIO;

//Static methods
  static void WINAPI _OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped); 

//Member variables
  HANDLE     m_hComm;        //Handle to the comms port
  HANDLE     m_hEvent;       //A event handle we need for internal synchronisation
  HINSTANCE  m_hKernel32;    //Kernel32 handle
  LPCANCELIO m_lpfnCancelIo; //CancelIO function pointer
};

#endif //__SERIALPORT_H__
