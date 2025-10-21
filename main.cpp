#include <Tlhelp32.h>
#include <cstdint>
#include <string_view>



// Create a snapshot of all the running process on our computer

class Memory
{
private:
	std::uintptr_t processId = 0;
	void* processHandle = nullptr;

public:

	// Memory Constructor - used for finding the processId and opening the handle
	Memory(const std::string_view processName) noexcept
	{
		::PROCESSENTRY32 entry = { };
		entry.dwSize = sizeof(::PROCESSENTRY32);

		const auto processSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		// Automatically find the right game process
		while (::Process32Next(processSnapshot, &entry))
		{
			if (!processName.compare(entry.szExeFile))
			{
				processId = entry.th32ProcessID;

				// Get full modifying acces to the game
				processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
				break;
			}
		}

		// Close the handle
		if (processSnapshot)
			::CloseHandle(processSnapshot);
	}

	// Memory Deconstructor - cleanup by closing the process handle to the game's memory.
	~Memory()
	{
		if (processHandle)
			::CloseHandle(processHandle);
	}


	// Make GetModelBaseAdress function
	const std::uintptr_t GetModuleAdress(const std::string_view moduleName) const noexcept
	{
		::MODULEENTRY32 entry = { }; 
		entry.dwSize = sizeof(::MODULEENTRY32);

		const auto processSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE, processId);

		std::uintptr_t moduleBaseAdress = 0;

		while (::Module32Next(processSnapshot, &entry))
		{
			if (!moduleName.compare(entry.szModule))
			{

				moduleBaseAdress = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
				break;
			}
		}

		if (processSnapshot)
			::CloseHandle(processSnapshot);

		return moduleBaseAdress;
	}

	// ReadProcessMemory
	template <typename T>
	constexpr const T Read(const std::uintptr_t& adress) const noexcept
	{
		T value = { };
		::ReadProcessMemory(processHandle, reinterpret_cast<const void*>(adress), &value, sizeof(T), NULL);
		return value;
	}

	// WriteProcessMemory
	template <typename T>
	constexpr void Write(const std::uintptr_t& adress, const T& value) const noexcept
	{
		::WriteProcessMemory(processHandle, reinterpret_cast<void*>(adress), &value, sizeof(T), NULL);
	}
};

