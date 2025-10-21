#include "memory.h"

int main() {
	Memory memory;
	const auto ModuleBaseAdress = memory.GetModuleBaseAddress("urGame.exe");


	while (true)
	{
		memory.Write<int>(ModuleBaseAdress + 0x123, 99999);
	}
}
