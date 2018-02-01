
#include <iostream>
#include <Windows.h>
#include <time.h>
#include "user_plug_prcdata.h"


typedef void (__stdcall* _plug_proc_data)(int itype, const char * datastr, int * irescode, char  * proc_msg);
typedef void (__stdcall* _init_work_mode)(int workmode /*= 0*/, const char * webserviceurl /*= "http://localhost/Ehl.Atms.Tgs.WebService/OpenInterface.asmx"*/);


HINSTANCE  hdll = NULL;
_plug_proc_data  pplug_proc_data = NULL;
_init_work_mode pinit_work_mode = NULL;
#include "../libini/ini_oper.h";
#include <string>
using namespace std;
#include "../../public.h"

class c_plug_proc_data
{
public:
	c_plug_proc_data()
	{
		pplug_proc_data = NULL;
		hdll = NULL;
		
		hdll = LoadLibrary("ehl.ftpdata.save.dll");
		if (hdll == NULL)
		{
			
			FreeLibrary(hdll);
		}
		else
		{
			pplug_proc_data = (_plug_proc_data)GetProcAddress(hdll, "plug_proc_data");
		
			if (pplug_proc_data == NULL)
			{
				if (hdll != NULL)
					FreeLibrary(hdll);
			}
			pinit_work_mode = (_init_work_mode)GetProcAddress(hdll, "init_work_mode");
			if (pinit_work_mode != NULL)
			{
				int iworkmode = get_ini_int("upload_param", "work_mode", 0, _ftp_config_file);
				string str = get_ini_str_ex("upload_param", "datawebservice", "http://localhost/Ehl.Atms.Tgs.WebService/OpenInterface.asmx", _ftp_config_file);
				pinit_work_mode(iworkmode, str.c_str());
				put_ini_int("upload_param", "work_mode", iworkmode, _ftp_config_file);
				put_ini_str("upload_param", "datawebservice", str.c_str(), _ftp_config_file);
			}
		}
	}
	~c_plug_proc_data()
	{
		pplug_proc_data = NULL;
		if (hdll != NULL)
		   FreeLibrary(hdll);
		hdll = NULL;

	}


} ;
c_plug_proc_data * p_plug_proc_data = NULL;
int UnicodeToANSI(const wchar_t * strw,char * stra)
{
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_ACP,
		0,
		strw,
		-1,
		NULL,
		0,
		NULL,
		NULL);
	::WideCharToMultiByte(CP_ACP,
		0,
		strw,
		-1,
		stra,
		iTextLen,
		NULL,
		NULL);
	return iTextLen;
};

int ANSIToUnicode(const char * stra,wchar_t * strw)
{
	int  len = 0;
	len =strlen(stra);
	int  unicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		stra,
		-1,
		NULL,
		0);
	
	::MultiByteToWideChar(CP_ACP,
		0,
		stra,
		-1,
		(LPWSTR)strw,
		unicodeLen);
	return  unicodeLen;
}




c_plug_proc_data g_plug_proc_data;


void  plug_proc_data(const char * datastr, int * iresult_code,char * proc_msg)
{
	char  datastra[512];
	char  proc_msga[1024];
	
	//UnicodeToANSI(datastr, datastra);
	//sprintf(proc_msga, datastra);
	//ANSIToUnicode(proc_msga, proc_msg);
	sprintf(datastra, datastr);
	//gmux.lock();

	bool bresult = false;
	if (pplug_proc_data != NULL)
	{
		pplug_proc_data(0, datastra, iresult_code, proc_msga);
		//ANSIToUnicode(proc_msga, proc_msg);
		sprintf(proc_msg,proc_msga);
	}
	else
	{
		if (hdll != NULL)
			//wsprintf(proc_msg, _T("load proc func is fail."));
			sprintf(proc_msg, "load proc func is fail.");
		else
			//wsprintf(proc_msg, _T("load dll is fail."));
			sprintf(proc_msg, "load dll is fail.");

	}
	

	//gmux.unlock();

};