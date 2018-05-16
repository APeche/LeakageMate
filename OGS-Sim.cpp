// OGS-Sim.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "Pipe.h"
#include <conio.h>


int _tmain(int argc, _TCHAR* argv[])
{
	double austauschzeitschritt = 60.0;
	long anzahlHaltungen = 0;
	long anzahlSchaechte = 0;
	COleDateTime zeitpunkt;
	char buffer[255];
	printf("Start\n");
	CPipe *pipe = new CPipe();
	printf("Verbinden\n");
	if (!pipe->VerbindePipes("\\\\.\\pipe\\GW-Schreiben","\\\\.\\pipe\\GW-Lesen"))
	{
		printf("Nicht verbunden. Programm endet nach Tastendruck.\n");
		_getch();
		delete pipe;
		return 1;
	}
	printf("Verbunden\n");
	// Initialer Datenaustausch
	pipe->PutD(austauschzeitschritt);
	pipe->GetTime(zeitpunkt);
	pipe->GetI(anzahlHaltungen);
	printf("Anzahl Haltungen: %d\n", anzahlHaltungen);
	for(long i= 0; i< anzahlHaltungen;i++)
	{
		pipe->GetS(buffer);
	}
	pipe->GetI(anzahlSchaechte);
	printf("Anzahl Schächte: %d\n", anzahlSchaechte);
	for(long i= 0; i< anzahlSchaechte;i++)
	{
		pipe->GetS(buffer);
	}
	bool Extran_ok = true;
	while (Extran_ok)
	{
		double wasserstand;
		printf("+");
		pipe->GetTime(zeitpunkt);
		for(long i= 0; i< anzahlSchaechte;i++)
		{
			pipe->GetD(wasserstand);
		}
		printf("-");
		double zufluss = 0.1;
		for(long i= 0; i< anzahlHaltungen;i++)
		{
			pipe->PutD(zufluss);
		}
		//long status = 0;
		//if (current_time < end_time) 
		//{
		//	int couplingstatus = 0;
		//}
		//else{
		//	couplingstatus = 1;
		//}
		//pipe->PutI(status);
	}
	delete pipe;
	return 0;
}

