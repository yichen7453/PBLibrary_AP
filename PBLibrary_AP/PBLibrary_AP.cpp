// PBLibrary_AP.cpp: �D�n�M���ɡC

#include "stdafx.h"
#include "PBGUI.h"


using namespace PBLibrary_AP;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{  
	// �إߥ��󱱨���e�A���ҥ� Window ����ı�ƮĪG
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	// �إߥD�����ð���
	Application::Run(gcnew PBGUI());


	//system("Pause");
	return 0;

}
