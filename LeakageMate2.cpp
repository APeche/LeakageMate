// OGS-Sim.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include <string>
#include "stdafx.h"
#include "Pipe.h"
#include <conio.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

void PrintProgramInfo()
{
	printf("          ###################################################\n");
	printf("          ##                   LeakageMate                 ##\n");
	printf("          ##                        -                      ##\n");
	printf("          ##                  Coupled to HE                ##\n");
	printf("          ##             (Version 1.0.2 [06/18])           ##\n");
	printf("          ###################################################\n");
	printf("\n");
	printf("                                by AaRoSi\n");
	printf("\n");
	printf("          ###################################################\n");
	printf("          ##   This program calculates upscaled leakage    ##\n");
	printf("          ##        from HYSTEM-EXTRAN water levels.       ##\n");
	printf("          ###################################################\n");
}

bool checkIfInputFilesExist(bool)
{
	string filenameOGSHEXML = "OpenGeoSysKopplung.xml";
	string filenameOGSHE_HE_File = "LeakageMate.in";
	ifstream ifile(filenameOGSHEXML.c_str());
	ifstream jfile(filenameOGSHE_HE_File.c_str());
	if (!ifile.is_open() || !jfile.is_open()) {
		cerr << "HE Coupling - no such files: OpenGeoSysKopplung.xml or LeakageMate.in" << endl;
		return 1; //coupling bool 0->Coupling, 1->No Coupling
	}
	else
	{
		cerr << "Found required input files for coupling to HE. Initiate coupling." << endl;
		return 0.0;
	}
}

void readNamedPipeNameForHECoupling(string* Name_Pipe_Write, string* Name_Pipe_Read)
{
	cout << "Reading OpenGeoSysKopplung.xml..." << endl;
	string OGSHE_pre1 = "\\\\.\\pipe\\"; //path to append names of pipes with
	string OGSHE_pre2 = "\\\\.\\pipe\\"; //path to append names of pipes with
	ifstream in("OpenGeoSysKopplung.xml"); //Input OpenGeoSysKopplung.xml
	in.ignore(200, '\n');	//Jump lines to names of named pipes
	in.ignore(200, '\n');
	in.ignore(200, '\n');
	in.ignore(200, '\n');
	in.ignore(200, '\n');
	string PipeReadTemp;	//Initiate string for whole line from *.xml
	in >> PipeReadTemp;
	char R_start = PipeReadTemp.find('GW');	//Find start of NamedPipe name
	char R_end = PipeReadTemp.find('</');	//Find end of NamedPipe name
	string Name_Pipe_Read_end = PipeReadTemp.substr(R_start - 1, R_end - R_start); //Cut name (temporary - improve this)
	string NPR = OGSHE_pre1.append(Name_Pipe_Read_end);
	*Name_Pipe_Read = NPR;
	//*Name
	string PipeWriteTemp;
	in >> PipeWriteTemp;
	char W_start = PipeWriteTemp.find('GW');
	char W_end = PipeWriteTemp.find('</');
	string Name_Pipe_Write_end = PipeWriteTemp.substr(W_start - 1, W_end - W_start);
	//CP2.Name_Pipe_Write = OGSHE_pre2.append(Name_Pipe_Write_end);
	string NPW = OGSHE_pre2.append(Name_Pipe_Write_end);
	*Name_Pipe_Write = NPW;
	cout << "...done." << endl;
}

void ReadStaticVariables(string* Method, int* NumberPipes, double* ColmLayerPerm, double* ColThickness, double* BackfillPerm, double* DisconnectionPressure,
	double* FluidDensity, double* FluidViscosity, double* a00, double* a10, double* a01, double* a20,
	double* a11, double* a02, double* a30, double* a21, double* a12, double* a03, double* a40,
	double* a22, double* a13, double* a31, double* k1, double* k2, double* k3, int* ExchangeTimeStep, int* outputSteps,
	string* outputFormat, double* LastTimeStep, vector<int>* PipeNameVector, vector<double>* UpstreamManholeVector, vector<double>* DownstreamManholeVector,
	vector<double>* PipeLengthsVector, vector<double>* DefectAreaVector, vector<double>* MeanWaterPressureVector, vector<double>* PipeDatumn)
{
	// Function defines variables that are defined once at initialization

	// Create input stream.
	printf("Reading LeakageMate.in ...\n");
	vector<string> DataArray;
	ifstream myfile("LeakageMate.in");
	copy(istream_iterator<string>(myfile),
		istream_iterator<string>(),
		back_inserter(DataArray));
	printf("Initializing variables ...\n");
	*Method = DataArray.at(1).c_str();
	*NumberPipes = atoi(DataArray.at(3).c_str());
	*ColmLayerPerm = atof(DataArray.at(5).c_str());
	*ColThickness = atof(DataArray.at(7).c_str());
	*BackfillPerm = atof(DataArray.at(9).c_str());
	*DisconnectionPressure = atof(DataArray.at(11).c_str());
	*FluidDensity = atof(DataArray.at(13).c_str());
	*FluidViscosity = atof(DataArray.at(14).c_str());
	*a00 = atof(DataArray.at(16).c_str());
	*a10 = atof(DataArray.at(17).c_str());
	*a01 = atof(DataArray.at(18).c_str());
	*a20 = atof(DataArray.at(19).c_str());
	*a11 = atof(DataArray.at(20).c_str());
	*a02 = atof(DataArray.at(21).c_str());
	*a30 = atof(DataArray.at(22).c_str());
	*a21 = atof(DataArray.at(23).c_str());
	*a12 = atof(DataArray.at(24).c_str());
	*a03 = atof(DataArray.at(25).c_str());
	*a40 = atof(DataArray.at(26).c_str());
	*a31 = atof(DataArray.at(27).c_str());
	*a22 = atof(DataArray.at(28).c_str());
	*a13 = atof(DataArray.at(29).c_str());
	*ExchangeTimeStep = atoi(DataArray.at(31).c_str());
	*outputSteps = atoi(DataArray.at(32).c_str());
	*outputFormat = (DataArray.at(33).c_str());
	*LastTimeStep = atof(DataArray.at(34).c_str());
	*k1 = atof(DataArray.at(36).c_str());
	*k2 = atof(DataArray.at(37).c_str());
	*k3 = atof(DataArray.at(38).c_str());
	// Read Pipe Names, pipe lengths, defect area, and mean soil water pressure and store in vector
	vector<int> PNV; vector<double> UMV; vector<double> DMV; vector<double>PLV; vector<double> DAV;
	vector<double> MWPV; vector<double> PD;
	for (int i = 0; i < *NumberPipes; i++)
	{
		PNV.push_back(atoi(DataArray.at(40 + (i * 7)).c_str()));
		UMV.push_back(atof(DataArray.at(41 + (i * 7)).c_str()));
		DMV.push_back(atof(DataArray.at(42 + (i * 7)).c_str()));
		PLV.push_back(atof(DataArray.at(43 + (i * 7)).c_str()));
		DAV.push_back(atof(DataArray.at(44 + (i * 7)).c_str()));
		MWPV.push_back(atof(DataArray.at(45 + (i * 7)).c_str()));
		PD.push_back(atof(DataArray.at(46 + (i * 7)).c_str()));
	}
	*PipeNameVector = PNV;
	*UpstreamManholeVector = UMV;
	*DownstreamManholeVector = DMV;
	*PipeLengthsVector = PLV;
	*DefectAreaVector = DAV;
	*MeanWaterPressureVector = MWPV;
	*PipeDatumn = PD;
	printf("...done.\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	CPipe CP;
	PrintProgramInfo();
	bool cpl = 0;
	cpl = checkIfInputFilesExist(cpl);
	if (cpl == 1){
		cout << "Stopping program. Add above listed files." << endl;
		return 0;
	};
	readNamedPipeNameForHECoupling(&CP.Name_Pipe_Write, &CP.Name_Pipe_Read);
	ReadStaticVariables(&CP.Method, &CP.NumberPipes, &CP.ColmLayerPerm, &CP.ColThickness, &CP.BackfillPerm, &CP.DisconnectionPressure,
		&CP.FluidDensity, &CP.FluidViscosity, &CP.a00, &CP.a10, &CP.a01, &CP.a20,
		&CP.a11, &CP.a02, &CP.a30, &CP.a21, &CP.a12, &CP.a03, &CP.a40,
		&CP.a22, &CP.a13, &CP.a31, &CP.k1, &CP.k2, &CP.k3, &CP.ExchangeTimeStep, &CP.outputSteps,
		&CP.outputFormat, &CP.LastTimeStep, &CP.PipeNameVector, &CP.UpstreamManholeVector, &CP.DownstreamManholeVector,
		&CP.PipeLengthsVector, &CP.DefectAreaVector, &CP.MeanWaterPressureVector, &CP.PipeDatumn);
	cout << "Method: " << CP.Method << "..." << endl;
	CP.Name_Pipe_Write;
	cout << "NumberOfPipes: " << CP.NumberPipes << endl;
	long anzahlHaltungen = 0;
	long anzahlSchaechte = 0;
	COleDateTime zeitpunkt;
	char buffer[255];
	printf("Start\n");
	CPipe *pipe = new CPipe();
	printf("Verbinden\n");
	if (!pipe->VerbindePipes(CP.Name_Pipe_Write.c_str(), CP.Name_Pipe_Read.c_str()))
	{
		printf("Nicht verbunden. Programm endet nach Tastendruck.\n");
		_getch();
		delete pipe;
		return 1;
	}
	printf("Verbunden\n");
	// Initialer Datenaustausch
	pipe->PutD(CP.ExchangeTimeStep);
	pipe->GetTime(zeitpunkt);
	pipe->GetI(anzahlHaltungen);
	printf("Anzahl Haltungen: %d\n", anzahlHaltungen);
	ofstream sas("HEPipeNames.txt");
	for (long i = 0; i< anzahlHaltungen; i++)
	{
		pipe->GetS(buffer);
		CP.HEPipeNames.push_back(buffer);
		sas << i << "   " << CP.HEPipeNames[i] << endl;
	}
	pipe->GetI(anzahlSchaechte);
	printf("Anzahl Schaechte: %d\n", anzahlSchaechte);
	ofstream ss("HEManholeNames.txt");
	for (long i = 0; i< anzahlSchaechte; i++)
	{
		pipe->GetS(buffer);
		CP.HEManholeNames.push_back(buffer);
		ss << i << "   " << CP.HEManholeNames[i] << endl;
	}
	bool Extran_ok = true;
	ofstream Leakagefile;
	while (Extran_ok == true)
	{
		for (long t = 0; t <= CP.LastTimeStep; t += CP.ExchangeTimeStep)
		{
			CP.Leakage.clear();
			CP.PipeWaterLevel.clear();
			CP.PipeWaterLevel_C.clear();
			CP.PipeWaterLevelDatumn.clear();
			ofstream Leakagefile;
			double wasserstand;
			cout << "Time is: " << t << " s" << endl;
			pipe->GetTime(zeitpunkt);
			for (long j = 0; j < anzahlSchaechte; j++)
			{
				pipe->GetD(wasserstand);
				CP.PipeWaterLevelDatumn.push_back(wasserstand);
			}
			for (long l = 0; l < anzahlHaltungen; l++)
			{
				CP.PipeWaterLevel.push_back(((CP.PipeWaterLevelDatumn[CP.UpstreamManholeVector[l]] + 
					CP.PipeWaterLevelDatumn[CP.DownstreamManholeVector[l]]) / 2) - CP.PipeDatumn[l]);
			}
			printf("-");
			for (int i = 0; i < anzahlHaltungen; i++)
			{
				if (find(CP.HEPipeNames.begin(), CP.HEPipeNames.end(), to_string(CP.PipeNameVector[i])) != CP.HEPipeNames.end())
				{
					//----------------------------
					// This int is in order to choose only pipes listed in *.in file 
					CP.PipeInteger = find(CP.HEPipeNames.begin(), CP.HEPipeNames.end(), to_string(CP.PipeNameVector[i])) - CP.HEPipeNames.begin();
					CP.PipeWaterLevel_C.push_back(CP.PipeWaterLevel[CP.PipeInteger]);
					//
					if (CP.MeanWaterPressureVector[i] / (CP.FluidDensity * 9.81) < CP.PipeWaterLevel_C[i] + CP.ColThickness)
					{
						if (CP.MeanWaterPressureVector[i] < CP.DisconnectionPressure && CP.Method == "LEAKAGE_FUNCTION")
						{
							CP.MeanWaterPressureVector[i] = CP.DisconnectionPressure;
						}
						if (CP.Method == "LEAKAGE_FUNCTION")
						{
							CP.Leakage.push_back(-1 * CP.DefectAreaVector[i] * CP.ExchangeTimeStep * CP.PipeLengthsVector[i] *
								(CP.a00 + CP.a10 * CP.MeanWaterPressureVector[i] + CP.a01 * CP.PipeWaterLevel_C[i] +
								CP.a20 * pow(CP.MeanWaterPressureVector[i], 2) +
								CP.a11 * CP.MeanWaterPressureVector[i] * CP.PipeWaterLevel_C[i] + CP.a02 * pow(CP.PipeWaterLevel_C[i], 2) +
								CP.a30 * pow(CP.MeanWaterPressureVector[i], 3) + CP.a21 * pow(CP.MeanWaterPressureVector[i], 2) * CP.PipeWaterLevel_C[i] +
								CP.a12 * CP.MeanWaterPressureVector[i] * pow(CP.PipeWaterLevel_C[i], 2) + CP.a03 * pow(CP.PipeWaterLevel_C[i], 3) +
								CP.a40 * pow(CP.MeanWaterPressureVector[i], 4) + CP.a31 * pow(CP.MeanWaterPressureVector[i], 3) * CP.PipeWaterLevel_C[i] +
								CP.a22 * pow(CP.MeanWaterPressureVector[i], 2) * pow(CP.PipeWaterLevel_C[i], 2) +
								CP.a13 * CP.MeanWaterPressureVector[i] * pow(CP.PipeWaterLevel_C[i], 3)));
						}
						else if (CP.Method == "LEAKAGE_FACTOR")
						{
							CP.Leakage.push_back(-1 * CP.DefectAreaVector[i] * CP.ExchangeTimeStep * CP.PipeLengthsVector[i] *
								((9.81 * CP.FluidDensity) / CP.FluidViscosity) * CP.ColmLayerPerm * 
								(CP.PipeWaterLevel_C[i] + CP.ColThickness + (CP.MeanWaterPressureVector[i] / (CP.FluidDensity * 9.81))) / CP.ColThickness);
						}
						else if (CP.Method == "FEFLOW") //Becker et al. (2009)Eine nichtlineare Leakage-Randbedingung für die Modellierung von hochwasserbeeinflusstem Grundwasseranstieg, Gewässer
						{
							CP.Leakage.push_back(-1 * CP.DefectAreaVector[i] * CP.ExchangeTimeStep * CP.PipeLengthsVector[i] * CP.k1 *
								(1 - exp(-CP.k2 * (CP.PipeWaterLevel_C[i] - (CP.MeanWaterPressureVector[i] / (CP.FluidDensity * 9.81))))));
						}
						if (CP.PipeWaterLevel_C[i] < .0001)
						{
							CP.Leakage[i] = 0;
						}
					}
					else
					{
						if (CP.Method == "LEAKAGE_FUNCTION")
						{
							CP.Leakage.push_back(-1 * CP.DefectAreaVector[i] * CP.ExchangeTimeStep * CP.PipeLengthsVector[i] *
								CP.BackfillPerm * ((9.81 * CP.FluidDensity) / CP.FluidViscosity)  *
								((CP.PipeWaterLevel_C[i] + CP.ColThickness) - (CP.MeanWaterPressureVector[i] / (CP.FluidDensity * 9.81))) / (CP.ColThickness));
						}
						else if (CP.Method == "LEAKAGE_FACTOR")
						{
							CP.Leakage.push_back(-1 * CP.DefectAreaVector[i] * CP.ExchangeTimeStep * CP.PipeLengthsVector[i] *
								CP.BackfillPerm * ((9.81 * CP.FluidDensity) / CP.FluidViscosity) * 
								(CP.PipeWaterLevel_C[i] - (CP.MeanWaterPressureVector[i] / (CP.FluidDensity * 9.81))) / CP.ColThickness);
						}
						else if (CP.Method == "FEFLOW")
						{
							CP.Leakage.push_back(CP.DefectAreaVector[i] * CP.ExchangeTimeStep * CP.PipeLengthsVector[i] * CP.k3 *
								(exp(CP.k2 * (CP.PipeWaterLevel_C[i] - (CP.MeanWaterPressureVector[i] / (CP.FluidDensity * 9.81))))-1));
						}
					}

					if (((t % CP.outputSteps) == 0) && i == CP.NumberPipes - 1) //output every 60 (dt = 1s) or 120 (dt = 2s) sec
					{
						//Generate Output
						std::stringstream ss;	// ap initialize empty stringstream
						ss << "leakageflow_time_" << t << ".csv"; //fill stringstream
						Leakagefile.open(ss.str().c_str());	//name file after stringstream 
						Leakagefile << "ogshe_temporal_output" << endl; //temporal output file header
						Leakagefile << "Pipe_name" << " " << "Mesh_node" << "he_water_level_[m]" << " " << "st_node_pressure_[Pa]" << " " << "leakage_volume_[m^3]" << endl;
						for (int k = 0; k < CP.NumberPipes; k++)
						{
							Leakagefile << CP.PipeNameVector[k] << " " << CP.PipeWaterLevel_C[k] << " " << CP.MeanWaterPressureVector[k] << " " <<  CP.Leakage[k] << endl;
						}
					}
				}
				else
				{
					CP.Leakage[i] = 0;
				}


				//----------------------------

				pipe->PutD(CP.Leakage[i]);
			}
			if (t == CP.LastTimeStep)
			{
				Extran_ok = false;
			}

			if (t < CP.LastTimeStep)
			{
				CP.couplingstatus = 0;
			}
			else{
				CP.couplingstatus = 1;
			}
			pipe->PutI(CP.couplingstatus);
		}
	}
	printf("Calculation done!");
	delete pipe;
	return 0;
}

