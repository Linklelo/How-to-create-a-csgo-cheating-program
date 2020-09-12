#pragma once

#include <Windows.h>
#include <tchar.h>

/* ����� */
class recv_prop;
class recv_table
{
public:
	/* ��ȡ���� */
	const char* get_table_name(HANDLE handle)
	{
		const int size = 128;
		static char data[size]{ 0 };

		SIZE_T bytes = 0;
		DWORD addr = 0;
		ReadProcessMemory(handle, this + 0xC, &addr, sizeof(addr), &bytes);
		if (addr) ReadProcessMemory(handle, (LPCVOID)addr, data, size, &bytes);

#ifdef _DEBUG
		std::cout << "table:" << data << std::endl;
#endif

		return data;
	}

	/* ��ȡ���prop���� */
	int get_max_prop(HANDLE handle)
	{
		SIZE_T bytes = 0;
		int result = 0;
		ReadProcessMemory(handle, this + 0x4, &result, sizeof(result), &bytes);

		return result;
	}

	/* ��ȡprop��ַ */
	recv_prop* get_prop(int index, HANDLE handle)
	{
		SIZE_T bytes = 0;
		DWORD addr = 0;
		ReadProcessMemory(handle, this, &addr, sizeof(addr), &bytes);

		return (recv_prop*)(addr + 0x3C * index);
	}
};

class recv_prop
{
public:
	/* ��ȡֵ������ */
	const char* get_var_name(HANDLE handle)
	{
		const int size = 128;
		static char data[size]{ 0 };

		SIZE_T bytes = 0;
		DWORD addr = 0;
		ReadProcessMemory(handle, this, &addr, sizeof(addr), &bytes);
		if (addr) ReadProcessMemory(handle, (LPCVOID)addr, data, size, &bytes);

#ifdef _DEBUG
		std::cout << "\tvar:" << data << std::endl;
#endif

		return data;
	}

	/* ��ȡƫ�� */
	int get_offset(HANDLE handle)
	{
		int result = 0;

		SIZE_T bytes = 0;
		ReadProcessMemory(handle, this + 0x2C, &result, sizeof(result), &bytes);

		return result;
	}

	/* ��ȡ���ݱ� */
	recv_table* get_data_table(HANDLE handle)
	{
		recv_table* result = nullptr;

		SIZE_T bytes = 0;
		ReadProcessMemory(handle, this + 0x28, &result, sizeof(result), &bytes);

		return result;
	}
};

class client_class
{
public:
	/* ��ȡ�������� */
	const char* get_network_name(HANDLE handle)
	{
		const int size = 128;
		static char data[size]{ 0 };

		SIZE_T bytes = 0;
		DWORD addr = 0;
		ReadProcessMemory(handle, this + 0x8, &addr, sizeof(addr), &bytes);
		if (addr) ReadProcessMemory(handle, (LPCVOID)addr, data, size, &bytes);

#ifdef _DEBUG
		std::cout << "\t\tnetwork:" << data << std::endl;
#endif

		return data;
	}

	/* ��ȡ��һ���� */
	client_class* get_next_class(HANDLE handle)
	{
		client_class* result = nullptr;

		SIZE_T bytes = 0;
		ReadProcessMemory(handle, this + 0x10, &result, sizeof(result), &bytes);

		return result;
	}

	/* ��ȡ�� */
	recv_table* get_table(HANDLE handle)
	{
		recv_table* result = 0;

		SIZE_T bytes = 0;
		ReadProcessMemory(handle, this + 0xC, &result, sizeof(result), &bytes);

		return result;
	}
};

class netvar
{
private:

private:
	HANDLE m_handle;			//���̾��

public:
	netvar(HANDLE h) : m_handle(h) {}
	~netvar() {}

	/* ����ָ�������ָ��ֵ */
	DWORD find_net_var(DWORD base, const char* table, const char* value)
	{
		for (client_class* client = (client_class*)base; client; client = client->get_next_class(m_handle))
		{
			/* ����һ���ж� */
			if (strcmp(client->get_table(m_handle)->get_table_name(m_handle), table)) continue;

			for (int i = 0; i < client->get_table(m_handle)->get_max_prop(m_handle); i++)
			{
				recv_prop* prop = client->get_table(m_handle)->get_prop(i, m_handle);

				if (isdigit(prop->get_var_name(m_handle)[0])) continue;
				if (_tcsstr(prop->get_var_name(m_handle), value) != nullptr) return prop->get_offset(m_handle);
				if (prop->get_data_table(m_handle) == nullptr) continue;

				for (int j = 0; j < prop->get_data_table(m_handle)->get_max_prop(m_handle); j++)
				{
					recv_prop* prop1 = prop->get_data_table(m_handle)->get_prop(j, m_handle);

					if (isdigit(prop1->get_var_name(m_handle)[0])) continue;
					if (_tcsstr(prop1->get_var_name(m_handle), value) != nullptr) return prop1->get_offset(m_handle);
					if (prop1->get_data_table(m_handle) == nullptr) continue;

					for (int k = 0; k < prop1->get_data_table(m_handle)->get_max_prop(m_handle); k++)
					{
						recv_prop* prop2 = prop1->get_data_table(m_handle)->get_prop(k, m_handle);

						if (isdigit(prop2->get_var_name(m_handle)[0])) continue;
						if (_tcsstr(prop2->get_var_name(m_handle), value) != nullptr) return prop2->get_offset(m_handle);
					}
				}
			}
		}

		return 0;
	}
};