// PBLibrary_AP.cpp: 主要專案檔。

#include "stdafx.h"
#include "PBGUI.h"


using namespace PBLibrary_AP;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{  
	// 建立任何控制項之前，先啟用 Window 的視覺化效果
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	// 建立主視窗並執行
	Application::Run(gcnew PBGUI());


	//system("Pause");
	return 0;

}
