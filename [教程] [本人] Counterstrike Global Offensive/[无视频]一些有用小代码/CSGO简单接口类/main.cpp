/*
�Ƚ����ƵĽӿ���,�ڴ˻�����ʵ�ֹ��ܷǳ�����
���ǽ�����Ե�ʱ��ʹ��,д�����汾��ʱ��Ҫ�����Ӷ�ȡ/д���ڴ�
*/

#include "address.hpp"
#include "memory.hpp"
#include "netvar.hpp"
#include "convar.hpp"

void test()
{
	std::cout << std::hex;

	memory* m = new memory();
	if (m->attach() == false) return;
	DWORD client = (DWORD)m->find_module("client.dll").modBaseAddr;
	DWORD vstdlib = (DWORD)m->find_module("vstdlib.dll").modBaseAddr;
	if (client == 0 || vstdlib == 0) return;
	std::cout << "client : 0x" << client << std::endl;
	std::cout << "vstdlib : 0x" << vstdlib << std::endl;

	address* a = new address();
	if (a->Initialize() == false) return;
	DWORD dwGetAllClasses = a->get_address("dwGetAllClasses");
	if (dwGetAllClasses == 0) return;
	dwGetAllClasses = client + dwGetAllClasses;
	std::cout << "dwGetAllClasses : 0x" << dwGetAllClasses << std::endl;

	netvar* n = new netvar(m->get_handle());
	DWORD iHealth = n->find_net_var(dwGetAllClasses, "DT_BasePlayer", "iHealth");
	std::cout << "iHealth : 0x" << iHealth << std::endl;

	convar* c = new convar();
	c->m_vstdlib = vstdlib;
	c->m_handle = m->get_handle();
	c->m_convar_name_hash_table = a->get_address("convar_name_hash_table");
	c->m_interface_engine_cvar = a->get_address("interface_engine_cvar");
	c->set_convar_value("r_3dsky", 0);
}

int main(int argc, char* argv[])
{
	test();

	system("pause");
	return 0;
}