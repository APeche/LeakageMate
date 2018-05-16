#include <string>
#include <vector>
#pragma once


class CPipeException
{
public:
	CPipeException(int nummer) {Fehlernummer = nummer;}
	~CPipeException() {};
	int Fehlernummer;
};

class CPipe
{
public:
	//----------------------------
// Functions 
	void PrintProgramInfo();
	bool checkIfInputFilesExist(bool);
	void readNamedPipeNameForHECoupling();
	void ReadStaticVariables();
	//----------------------------

	CPipe(void);
	~CPipe(void);
	//----------------------------
	// Variables
	std::string Name_Pipe_Read;
	std::string Name_Pipe_Write;
	int NumberPipes;
	double ColThickness;
	double BackfillPerm;
	double DisconnectionPressure;
	double FluidDensity;
	double FluidViscosity;
	double a00;
	double a10;
	double a01;
	double a20;
	double a11;
	double a02;
	double a30;
	double a21;
	double a12;
	double a03;
	double a40;
	double a31;
	double a22;
	double a13;
	int PipeInteger;
	int couplingstatus;
	int ExchangeTimeStep;
	int outputSteps;
	std::string outputFormat;
	double LastTimeStep;
	std::vector<int> PipeNameVector;
	std::vector<double> DownstreamManholeVector;
	std::vector<double> UpstreamManholeVector;
	std::vector<double> PipeLengthsVector;
	std::vector<double> DefectAreaVector;
	std::vector<double> MeanWaterPressureVector;
	std::vector <double> PipeDatumn;
	std::vector <std::string> HEPipeNames;
	std::vector <std::string> HEManholeNames;
	std::vector <double> PipeWaterLevelDatumn;
	std::vector <double> PipeWaterLevel;
	std::vector <double> PipeWaterLevel_C;
	std::vector<double> Leakage;
	//----------------------------
	bool OGSHE;
	bool VerbindePipes(LPCTSTR einName, LPCTSTR ausName);
	void GetI(long int &value);
	void GetI2(short &value);
	void GetF(float &value);
	void GetD(double &value);
	void PutI(long int value);
	void PutI2(short value);
	void PutF(float value);
	void PutD(double value);
	void PutS(char *value);
	void GetS(char *value);
	void PutTime(COleDateTime zeit);
	void GetTime(COleDateTime &zeit);
	HANDLE ein;
	HANDLE aus;
	virtual bool Exit();
	CString m_Fehlertext;
private:
	HANDLE ConnectPipe(LPCTSTR name, DWORD access);
	void GetBytes(void *buffer, DWORD length);
	void PutBytes(void *buffer, DWORD length);
};
