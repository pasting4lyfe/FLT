
#include <Windows.h>
#include <iostream>
#include <Interface/wdk/wdk.hxx>
#include <print>
#define DbgPrint(x) printf(x)//for the aestethic


auto main() -> int {
	DbgPrint("Entry Called\n");
    cWizardComm c;
	std::println("Bro");
	if (!c.InitializeFunction()) {
		std::cerr << "Failed to Init Handle, Last Error -> " << GetLastError() << std::endl;
	}
	c.PrintDeviceHandle();

	c.HelloDexterMorgan();

	c.IATLookUp("GetTickCount", "notepad.exe");

	getchar();
	return 0;
}