#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <memory>
#include <string_view>
#include <cstdint>
#include <vector>
#include "../../cryptor.h"

namespace driver
{
	inline unsigned int process_id;
	inline ULONG64 base_address;
	inline ULONG64 base_address1;


	struct HandleDisposer
	{
		using pointer = HANDLE;
		void operator()(HANDLE handle) const
		{
			if (handle != NULL || handle != INVALID_HANDLE_VALUE)
			{
				CloseHandle(handle);
			}
		}
	};

	using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;

	std::uint32_t get_process_id(std::string_view process_name)
	{

		return NULL;
	}

	static ULONG64 get_module_base_address(const char* module_name)
	{
		ULONG64 base = NULL;
		return base;
	}
	template <class T>
	T Read(UINT_PTR read_address)
	{
		T response{};

		return response;
	}

	bool write_memory(UINT_PTR write_address, UINT_PTR source_address, SIZE_T write_size)
	{
		return 0;
	}

	template<typename S>
	bool write(UINT_PTR write_address, const S& value)
	{
		return write_memory(write_address, (UINT_PTR)&value, sizeof(S));
	}

}