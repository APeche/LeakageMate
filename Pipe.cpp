#include "stdafx.h"
#include <Windows.h>
#include ".\pipe.h"

CPipe::CPipe(void)
{
	ein = INVALID_HANDLE_VALUE;
	aus = INVALID_HANDLE_VALUE;
}

CPipe::~CPipe(void)
{
	Exit();
}


bool CPipe::VerbindePipes(LPCTSTR einName, LPCTSTR ausName)
{
	aus = ConnectPipe(ausName, GENERIC_WRITE);
	if (aus == INVALID_HANDLE_VALUE)
	{
		m_Fehlertext = "Fehler beim Verbinden mit der SchreibPipe";
		return false;
	}
	Sleep(1000); // Server Zeit geben, die zweite Pipe zu erzeugen
	ein = ConnectPipe(einName, GENERIC_READ);
	if (ein == INVALID_HANDLE_VALUE)
	{
		m_Fehlertext = "Fehler beim Verbinden mit der LesePipe";
		return false;
	}
	return true;
}

HANDLE CPipe::ConnectPipe(LPCTSTR name, DWORD access)
{
	 bool fertig = false;
	 DWORD fehler;
	 HANDLE hPipe;
	 while(1) // never ending loop
	 {
		printf("Connect: %s\n",name);
		hPipe = CreateFile( 
		 name,   // pipe name 
		 access, 
		 0,              // no sharing 
		 NULL,           // default security attributes
		 OPEN_EXISTING,  // opens existing pipe 
		 0,              // default attributes 
		 NULL);          // no template file 
 
		// Break if the pipe handle is valid. 
 
		if (hPipe != INVALID_HANDLE_VALUE)
			return hPipe;
 
	  // Exit if an error other than ERROR_PIPE_BUSY occurs. 
 
	  if (GetLastError() != ERROR_PIPE_BUSY) 
	  {
		  if (GetLastError() == ERROR_FILE_NOT_FOUND)
			  printf("Pipe existiert nicht.\n");
		  else
			printf( "Keine Verbindung mit der Pipe möglich. Fehler=%d\n", GetLastError() ); 
		  return INVALID_HANDLE_VALUE;
	  }
 
	  // All pipe instances are busy, so wait for 5 seconds. 
 
	  if ( ! WaitNamedPipe(name, 5000)) 
	  { 
		 printf("Die Pipe ist nicht bereit. Warte 5 Sekunden."); 
		 return INVALID_HANDLE_VALUE;
	  } 

	 } while(!fertig);
	 return false;
}

void CPipe::GetBytes(void *buffer, DWORD length)
{
	DWORD anzahl;
	if (ein == INVALID_HANDLE_VALUE)
		throw CPipeException(2);
	ReadFile(ein,buffer,length,&anzahl,NULL);
	if (anzahl != length)
		 throw CPipeException(3);
}

void CPipe::GetI(long int &value)
{
	GetBytes(&value,sizeof(long int));
}

void CPipe::GetI2(short &value)
{
	GetBytes(&value,sizeof(short));
}

void CPipe::GetF(float &value)
{
	GetBytes(&value,sizeof(float));
}

void CPipe::GetD(double &value)
{
	GetBytes(&value,sizeof(double));
}

void CPipe::PutBytes(void *buffer, DWORD length)
{
	DWORD anzahl;
	if (aus == INVALID_HANDLE_VALUE)
		throw CPipeException(2);
	if (WriteFile(aus,buffer,length,&anzahl,NULL) == 0)
		throw CPipeException(1);
	if (anzahl != length)
		 throw CPipeException(3);
}

void CPipe::PutI(long int value)
{
	PutBytes(&value,sizeof(long int));
}

void CPipe::PutI2(short value)
{
	PutBytes(&value,sizeof(short));
}

void CPipe::PutF(float value)
{
	PutBytes(&value,sizeof(float));
}

void CPipe::PutD(double value)
{
	PutBytes(&value,sizeof(double));
}

void CPipe::PutS(char *value)
{
	unsigned l;
	l = strlen(value);
	PutBytes(&l,sizeof(unsigned));
	PutBytes(value,l);
}

void CPipe::GetS(char *value)
{
	unsigned l;
	*value = 0;
	GetBytes(&l,sizeof(unsigned));
	GetBytes(value,l);
	value[l] = 0;
}

void CPipe::PutTime(COleDateTime zeit)
{	
	short wert;
	wert = zeit.GetDay();
	PutI2(wert);
	wert = zeit.GetMonth();
	PutI2(wert);
	wert = zeit.GetYear();
	PutI2(wert);
	wert = zeit.GetHour();
	PutI2(wert);
	wert = zeit.GetMinute();
	PutI2(wert);
	wert = zeit.GetSecond();
	PutI2(wert);
}

void CPipe::GetTime(COleDateTime &zeit)
{
	int erg = 0;
	short jahr,monat,tag,stunde, minute,sekunde;
	GetI2(tag);
	GetI2(monat);
	GetI2(jahr);
	GetI2(stunde);
	GetI2(minute);
	GetI2(sekunde);
	COleDateTime tempTime(jahr,monat,tag,stunde,minute,sekunde);
	zeit = tempTime;
}

bool CPipe::Exit()
{
	if (aus != INVALID_HANDLE_VALUE)
		FlushFileBuffers(aus);
	if (ein != INVALID_HANDLE_VALUE)
		DisconnectNamedPipe(ein);
	if (aus != INVALID_HANDLE_VALUE)
		DisconnectNamedPipe(aus);
	if (ein != INVALID_HANDLE_VALUE)
		CloseHandle(ein);
	if (aus != INVALID_HANDLE_VALUE)
		CloseHandle(aus);
	ein = INVALID_HANDLE_VALUE;
	aus = INVALID_HANDLE_VALUE;
	return true;
}

