#pragma once

#include <Windows.h>
#include <TlHelp32.h>

#include <iostream>
#include <string>

class memory
{
private:
	DWORD m_pid;				//����ID
	HANDLE m_handle;			//���̾��
	HWND m_hwnd;				//���ھ��

public:
	memory() {}
	~memory() {}

	inline DWORD get_pid() { return m_pid; }
	inline HANDLE get_handle() { return m_handle; }
	inline HWND get_hwnd() { return m_hwnd; }

	/* ��ʼ�� */
	bool attach(const char* process = "csgo.exe")
	{
		HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (Snap == INVALID_HANDLE_VALUE) return false;

		PROCESSENTRY32 ProcessInfo{ 0 };
		ProcessInfo.dwSize = sizeof(ProcessInfo);

		if (Process32First(Snap, &ProcessInfo))
		{
			do
			{
				if (strcmp(process, ProcessInfo.szExeFile) == 0)
				{
					CloseHandle(Snap);

					/* �������ID */
					m_pid = ProcessInfo.th32ProcessID;

					/* ������̾�� !!!!Σ�յķ�ʽ!!!!! */
					m_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);

					/* ���洰�ھ�� */
					m_hwnd = FindWindowA(nullptr, "Counter-Strike: Global Offensive");

					return m_handle && m_hwnd;
				}
			} while (Process32Next(Snap, &ProcessInfo));
		}

		CloseHandle(Snap);
		return false;
	}

	/* ����ָ��ģ�� */
	MODULEENTRY32 find_module(const char* name)
	{
		MODULEENTRY32 Result{ 0 };
		Result.dwSize = sizeof(Result);

		HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_pid);
		if (Snap == INVALID_HANDLE_VALUE) return Result;

		if (Module32First(Snap, &Result))
		{
			do
			{
				if (strcmp(name, Result.szModule) == 0)
				{
					CloseHandle(Snap);
					return Result;
				}
			} while (Module32Next(Snap, &Result));
		}

		CloseHandle(Snap);
		return {};
	}

	/* ��ȡ�ڴ� */
	template<class T>
	T read(DWORD addr)
	{
		T result{};
		size_t size = sizeof(result);

		SIZE_T bytes = 0;
		ReadProcessMemory(m_handle, (LPCVOID)addr, &result, size, &bytes);

		return result;
	}

	/* д���ڴ� */
	template<class T>
	void write(DWORD addr, T buf)
	{
		size_t size = sizeof(buf);

		SIZE_T bytes = 0;
		WriteProcessMemory(m_handle, (LPVOID)addr, &buf, size, &bytes);
	}
};