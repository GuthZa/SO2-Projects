#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <fcntl.h>

#define TAM 200


int _tmain(int argc, TCHAR** argv) {
	
#ifdef _UNICODE
	(void)_setmode(_fileno(stdin), _O_WTEXT);
	(void)_setmode(_fileno(stdout), _O_WTEXT);
	(void)_setmode(_fileno(stderr), _O_WTEXT);
#endif

	HKEY key = NULL;
	TCHAR str[TAM], key_name[TAM], pair_name[TAM], pair_value[TAM];
	DWORD status, type, tam_value, op, i = 0;
	LSTATUS res;

	do {
		_tprintf_s(_T("Opção ? (1 - Criar Key, 2 - Adicionar PAR, 3 - Consultar PAR, 4 - Remove PAR , 5 - Mostrar valores , 6 - Remove Key , 7 - Sair\n"));
		_tscanf_s(_T("%d"), &op);
		switch (op) {
		case 1:
			if (key != NULL)
				RegCloseKey(key);

			_tprintf(_T("NomeKey?"));
			_tscanf_s(_T("%s"), key_name, TAM);
			_stprintf_s(str, TAM, _T("SOFTWARE\\AULA\\%s"), key_name);

			res = RegCreateKeyEx(HKEY_CURRENT_USER,
				str,
				0, NULL,
				REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS,
				NULL,
				&key, &status);

			if (res == ERROR_SUCCESS) {
				if (status == REG_CREATED_NEW_KEY)
					_tprintf_s(_T("Chave criada...\n"));
				else if (status == REG_OPENED_EXISTING_KEY)
					_tprintf_s(_T("Chave aberta...\n"));
			}
			break;
		case 2:
			if (key == NULL) {
				_tprintf_s(_T("Nenhuma chave aberta...\n"));
				continue;
			}

			_tprintf_s(_T("Nome par?"));
			_tscanf_s(_T("%s"), pair_name, TAM);
			_tprintf_s(_T("Valor par?"));
			_tscanf_s(_T("%s"), pair_value, TAM);

			res = RegSetValueEx(key,
				pair_name, 0,
				REG_SZ,
				(LPBYTE)pair_value,
				(DWORD)(_tcslen(pair_value) + 1) * sizeof(TCHAR));

			if (res == ERROR_SUCCESS) {
				_tprintf_s(_T("Chave adicionada..\n"));
			}
			else {
				_tprintf_s(_T("Erro a adicionar o par chave\n"));
			}
			break;

		case 3:
			if (key == NULL) {
				_tprintf_s(_T("Nenhuma chave aberta...\n"));
				continue;
			}

			_tprintf_s(_T("Nome par?"));
			_tscanf_s(_T("%s"), pair_name, TAM);

			tam_value = sizeof(pair_value);

			res = RegQueryValueEx(key,
				(LPCSTR)pair_name,
				NULL, //Reservado...
				(LPDWORD)&type,
				(LPBYTE)pair_value,
				(LPDWORD)&tam_value);

			if (res == ERROR_SUCCESS) {
				_tprintf_s(_T("Valor: %s\n"), pair_value);
			}
			else {
				_tprintf_s(_T("Erro ao obter o valor para a chave: %s\n"), pair_name);
			}
			break;
		case 4:
			if (key == NULL) {
				_tprintf_s(_T("Nenhuma chave aberta...\n"));
				continue;
			}

			_tprintf_s(_T("Nome par?"));
			_tscanf_s(_T("%s"), pair_name, TAM);

			res = RegDeleteValue(key, (LPCWSTR)pair_name);

			if (res == ERROR_SUCCESS) {
				_tprintf_s(_T("Valor removido\n"));
			}
			else {
				_tprintf_s(_T("Erro ao remover o valor para a chave: %s\n"), pair_name);
			}

			break;
		case 5:
			if (key == NULL) {
				_tprintf_s(_T("Nenhuma chave aberta...\n"));
				continue;
			}

			i = 0;
			do
			{
				tam_value = TAM;
				res = RegEnumValue(key, i,
					pair_name,
					&tam_value,
					NULL, &type,
					NULL, NULL);
				if (res == ERROR_SUCCESS)
					_tprintf_s(_T("P: %s (%d)\n"), pair_name, type);

				i++;
			} while (res == ERROR_SUCCESS);
			break;
		case 6:
			if (key == NULL) {
				_tprintf_s(_T("Nenhuma chave aberta...\n"));
				continue;
			}

			res = RegDeleteKeyEx(HKEY_CURRENT_USER,
				str,
				KEY_ALL_ACCESS, 0);

			if (res == ERROR_SUCCESS) {
				_tprintf_s(_T("Key removida\n"));
			}
			else {
				_tprintf_s(_T("Erro ao remover a chave\n"));
			}
			break;
		case 7:
			_tprintf(_T("Adeus...\n"));
			break;
		default:
			_tprintf(_T("Escolhe uma opção válida!"));
			;			break;
		}
	} while (op != 7);
	if (key != NULL)
		RegCloseKey(key);

	return 0;
}