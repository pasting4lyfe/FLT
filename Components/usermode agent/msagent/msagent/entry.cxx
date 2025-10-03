
#include <Windows.h>
#include <iostream>
#include <Interface/wdk/wdk.hxx>

#define DbgPrint(x) printf(x)//for the aestethic


auto main() -> int {
	DbgPrint("Entry Called\n");
    cWizardComm c;

	if (!c.InitializeFunction()) {
		std::cerr << "Failed to Init Handle, Last Error -> " << GetLastError() << std::endl;
	}
	c.PrintDeviceHandle();

	c.HelloDexterMorgan();

	getchar();
	return 0;
}