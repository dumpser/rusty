#include <Windows.h>
#include <string>


#define READ_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x11, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define WRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x12, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define GET_MODULE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x13, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define READ_REQUEST32 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x14, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define WRITE_REQUEST_FLOAT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x15, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define READ_ALL_TYPE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x20, METHOD_OUT_DIRECT, FILE_SPECIAL_ACCESS)
#define GET_MODULE_REQUEST_GAME CTL_CODE(FILE_DEVICE_UNKNOWN, 0x21, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
typedef struct _KERNEL_READ_NEW
{

	ULONGLONG Address;
	ULONG Size;

} KERNEL_READ_NEW, * PKERNEL_READ_NEW;

typedef struct _KERNEL_READ_REQUEST_32
{

	ULONGLONG Address;
	float Response;
	ULONG Size;

} KERNEL_READ_REQUEST_32, * PKERNEL_READ_REQUEST_32;

typedef struct _KERNEL_READ_REQUEST
{
	ULONG ProcessId;
	DWORD64 Address;
	DWORD64 Response;
	ULONG Size;
} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
	DWORD64 Address;
	ULONG Value;
	ULONG Size;
} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST_FLOAT
{
	DWORD64 Address;
	float Value;
	ULONG Size;
} KERNEL_WRITE_REQUEST_FLOAT, * PKERNEL_WRITE_REQUEST_FLOAT;

class KeInterface
{
public:
	HANDLE hDriver; 

	void Error(int loc,int error)
	{
		Close();//WritePrivateProfileString("cfg", "err", to_string(loc).c_str(), ".\\cfg.ini");
		return;
	}

	void Create()
	{
		hDriver = CreateFileW(L"\\\\.\\QITA", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		if (hDriver == INVALID_HANDLE_VALUE)
		{
			MessageBox(NULL, "1", "1", MB_OK);
		}
		//WritePrivateProfileString(TEXT("cfg"),TEXT("err"),std::to_string(GetLastError()).c_str() ,TEXT("cfg.ini"));
	}

	void Close()
	{
		CloseHandle(hDriver);
	}


	template <typename type>
	type ReadVirtualMemory(ULONGLONG ReadAddress,SIZE_T Size)
	{
		if(ReadAddress <= 0x10000)
			return (type)false;
		DWORD Return, Bytes;
		KERNEL_READ_REQUEST ReadRequest;

		ReadRequest.Address = ReadAddress;
		ReadRequest.Size = Size;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, READ_REQUEST, &ReadRequest,
			sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), 0, 0))
			return (type)ReadRequest.Response;
		else
			return (type)false;
	}

	bool WriteVirtualMemory(ULONGLONG WriteAddress,ULONG WriteValue, SIZE_T WriteSize)
	{
		DWORD Bytes;

		KERNEL_WRITE_REQUEST  WriteRequest;
		WriteRequest.Address = WriteAddress;
		WriteRequest.Value = WriteValue;
		WriteRequest.Size = WriteSize;

		if (DeviceIoControl(hDriver, WRITE_REQUEST, &WriteRequest, sizeof(WriteRequest),
			0, 0, &Bytes, NULL))
			return true;
		else
			return false;
	}

	bool WriteVirtualMemoryFloat(ULONGLONG WriteAddress,float WriteValue, SIZE_T WriteSize)
	{
		DWORD Bytes;

		KERNEL_WRITE_REQUEST_FLOAT  WriteRequest;
		WriteRequest.Address = WriteAddress;
		WriteRequest.Value = WriteValue;
		WriteRequest.Size = WriteSize;

		if (DeviceIoControl(hDriver, WRITE_REQUEST_FLOAT, &WriteRequest, sizeof(WriteRequest),
			0, 0, &Bytes, NULL))
			return true;
		else
			return false;
	}

	DWORD64 GetClientModule()
	{
		DWORD64 Address;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, GET_MODULE_REQUEST, &Address, sizeof(Address),
			&Address, sizeof(Address), &Bytes, NULL))
			return Address;
		else
			return false;
	}


	DWORD64 GetClientModuleGA()
	{
		DWORD64 Address;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, GET_MODULE_REQUEST_GAME, &Address, sizeof(Address),
			&Address, sizeof(Address), &Bytes, NULL))
			return Address;
		else
			return false;
	}

	std::wstring ReadName(ULONGLONG ReadAddress, SIZE_T Size)
	{
		wchar_t name[32];
		if (ReadAddress <= 0x10000)
			return name;


		DWORD Return, Bytes;
		_KERNEL_READ_NEW ReadRequest;

		//ReadRequest.Address = ReadAddress + i * 2;
		//ReadRequest.Size = sizeof(wchar_t);

		ReadRequest.Address = ReadAddress;
		ReadRequest.Size = sizeof(name);

		DeviceIoControl(hDriver, READ_ALL_TYPE, &ReadRequest, sizeof(ReadRequest), &name, sizeof(name), 0, 0);


		std::wstring namew = name;
		//for (int i = 0; i < Size; i++)
		//{

		//	DWORD Return, Bytes;
		//	_KERNEL_READ_NEW ReadRequest;

		//	wchar_t string[1];

		//	ReadRequest.Address = ReadAddress + i * 2;
		//	ReadRequest.Size = sizeof(wchar_t);

		//	DeviceIoControl(hDriver, READ_ALL_TYPE, &ReadRequest, sizeof(ReadRequest), &string, sizeof(string), 0, 0);

		//	name.append(string);
		//}

		return namew;
	}

	std::string ReadNameS(ULONGLONG ReadAddress, SIZE_T Size)
	{
		char namebuba[150];

		if (ReadAddress <= 0x10000)
			return namebuba;

		DWORD Return, Bytes;
		_KERNEL_READ_NEW ReadRequest;
		ReadRequest.Address = ReadAddress;
		ReadRequest.Size = sizeof(namebuba);
		DeviceIoControl(hDriver, READ_ALL_TYPE, &ReadRequest, sizeof(ReadRequest), &namebuba, sizeof(namebuba), 0, 0);
		std::string name = namebuba;
		return name;

	}

	template <typename type>
	type ReadNewShit(ULONGLONG ReadAddress,SIZE_T Size)
	{
		if (ReadAddress <= 0x10000)
			return type();

		type ReturnValue;
		DWORD Return, Bytes;
		_KERNEL_READ_NEW ReadRequest;

		ReadRequest.Address = ReadAddress;
		ReadRequest.Size = Size;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, READ_ALL_TYPE, &ReadRequest,sizeof(ReadRequest), &ReturnValue, sizeof(ReturnValue), 0, 0))
			return (type)ReturnValue;

		return type();

	}

	void* PatternScan(char* base, size_t size, char* pattern, char* mask)
	{
		size_t patternLength = strlen(mask);

		for (unsigned int i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (unsigned int j = 0; j < patternLength; j++)
			{
				if (mask[j] != '?' && pattern[j] != *(base + i + j))
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				return (void*)(base + i);
			}
		}
		return nullptr;
	}

	DWORD64 PatternScanEx(DWORD64 begin, DWORD64 end, char* pattern, char* mask)
	{
		_KERNEL_READ_NEW ReadRequest;

		ReadRequest.Address = begin;
		
		DWORD64 currentChunk = begin;
		ULONG bytesRead;

		while (ReadRequest.Address < end)
		{
			void* internalAddress;
			char buffer[4096];
			memset(buffer, '-', sizeof(buffer));
			ReadRequest.Size = sizeof(buffer);
			//VirtualProtectEx(hProcess, (void*)currentChunk, sizeof(buffer), PAGE_EXECUTE_READWRITE, &oldprotect);
			DeviceIoControl(hDriver, READ_ALL_TYPE, &ReadRequest, sizeof(ReadRequest), &buffer, sizeof(buffer), &bytesRead , 0);
			//ReadProcessMemory(hProcess, (void*)currentChunk, &buffer, sizeof(buffer), &bytesRead);
			//VirtualProtectEx(hProcess, (void*)currentChunk, sizeof(buffer), oldprotect, &oldprotect);

			if (bytesRead == 0)
			{
				return 0;
			}

			if (buffer[0] != '-')
			{
				void* internalAddress = PatternScan((char*)& buffer, bytesRead, pattern, mask);
				if (internalAddress != nullptr)
				{
					//calculate from internal to external
					DWORD64 offsetFromBuffer = (DWORD64)internalAddress - (DWORD64)& buffer;
					return (ReadRequest.Address + offsetFromBuffer);
				}
				else
				{
					//advance to next chunk
					ReadRequest.Address = ReadRequest.Address + bytesRead;
					continue;
				}
			}
				
			ReadRequest.Address = ReadRequest.Address + bytesRead;

		}
		return 0;
		//std::cout << __FUNCTION__ << GetLastError() << std::endl;
	}

};