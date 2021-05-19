#include "String.h"
#include <Windows.h>

#pragma comment(linker,"/MERGE:.rdata=.text /MERGE:.data=.text")
#pragma comment(linker, "/SECTION:.text,EWR")
#pragma comment(linker,"/FILEALIGN:512")

void Entry()
{
    String lolol = static_string(L"Hahaha, classic...");

    MessageBoxW(0, lolol.data(), 0, 0);
}