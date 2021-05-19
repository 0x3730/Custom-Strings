#include <Windows.h> 

int m_strlen(LPCWSTR lpStr)
{
	if (!lpStr)
		return 0;

	int i = 0;
	while (*lpStr++)
		i++;

	return i;
}

bool m_strcmp(LPCWSTR lpStr1, LPCWSTR lpStr2)
{
	if (!lpStr1 || !lpStr2)
		return false;

	if (m_strlen(lpStr1) != m_strlen(lpStr2))
		return false;

	bool bRes = true;
	while (*lpStr1)
	{
		if (*lpStr1++ != *lpStr2++)
		{
			bRes = false;
			break;
		}
	}

	return bRes;
}

int m_strstr(LPCWSTR lpStr, LPCWSTR lpSubStr)
{
	if (!lpStr || !lpSubStr)
		return false;

	//bool bRes = false;
	int iSubStrLen = m_strlen(lpSubStr);
	int i = 0;
	int x = 0;
	int iFirstIndex = -1;

	while (lpStr[x])
	{
		if (lpStr[x] == lpSubStr[i])
		{
			if (!i)
				iFirstIndex = x;

			i++;
			if (i >= iSubStrLen)
			{
				//bRes = true;
				break;
			}
		}
		else
		{
			i = 0;
			iFirstIndex = -1;
		}

		x++;
	}

	return iFirstIndex;
}

#pragma function(memset)
LPVOID memset(LPVOID _Dst, int _Val, int _Size)
{
	if (!_Dst)
		return NULL;

	__asm
	{
		pushad
		mov		edi, [_Dst]
		mov		ecx, [_Size]
		mov		eax, [_Val]
		rep		stosb
		popad
	}

	return NULL;
}

LPVOID m_memcpy(LPVOID _Dst, const void* _Src, int _Size)
{
	if (!_Dst || !_Src || !_Size)
		return NULL;

	__asm
	{
		pushad
		mov		esi, [_Src]
		mov		edi, [_Dst]
		mov		ecx, [_Size]
		rep		movsb
		popad
	}

	return NULL;
}

struct String
{
protected:
	LPWSTR lpArray = NULL;
	int iSize = 0;
	//
	
	bool DeallocMemory(LPVOID lpMem)
	{
		return HeapFree(GetProcessHeap(), 0, lpMem);
	}

	LPWSTR AllocMemory(int iSize)
	{
		return (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, iSize);
	}

public:
	void free()
	{
		if (this->lpArray && this->iSize > 0)
			if (DeallocMemory(this->lpArray))
				this->iSize = 0;
	}

	void append(LPCWSTR lpString)
	{
		int iNewString = 0;
		int iSumString = 0;
		int iCurrentSize = m_strlen(this->lpArray);
		LPWSTR lpTempArray = NULL;

		do
		{
			if ((iNewString = m_strlen(lpString)) == 0)
				break;

			iSumString = iCurrentSize + iNewString;
			if ((lpTempArray = AllocMemory(iCurrentSize * 2 + 1)) == NULL)
				break;

			m_memcpy(lpTempArray, this->lpArray, iCurrentSize * 2);
			if (!DeallocMemory(this->lpArray))
				break;

			if ((this->lpArray = AllocMemory(iSumString * 2 + 1)) == NULL)
				break;

			m_memcpy(this->lpArray, lpTempArray, iCurrentSize * 2);
			m_memcpy(&this->lpArray[iCurrentSize], lpString, iNewString * 2);
			this->iSize = iSumString;

		} while (false);

		DeallocMemory(lpTempArray);
	}

	void equate(LPCWSTR lpString)
	{
		int iNewString = 0;
		do
		{
			if (!lpString)
				break;

			iNewString = m_strlen(lpString);

			if(this->lpArray)
				if (!DeallocMemory(this->lpArray))
					break;

			if ((this->lpArray = AllocMemory(iNewString * 2 + 1)) == NULL)
				break;

			m_memcpy(this->lpArray, lpString, iNewString * 2);
			this->iSize = iNewString;
		} while (false);
	}

	bool equal(LPCWSTR lpString)
	{
		return m_strcmp(this->lpArray, lpString);
	}

	void resize(int iNewSize, int iChar)
	{
		do
		{
			free();

			if ((this->lpArray = AllocMemory(iNewSize * 2 + 1)) == NULL)
				break;

			memset(this->lpArray, iChar, iNewSize * 2);
			this->iSize = iNewSize;
		} while (false);
	}

	bool find(LPCWSTR lpSubString)
	{
		return m_strstr(this->lpArray, lpSubString);
	}

	void erase(LPCWSTR lpSubString)
	{
		int iSubString = 0;
		int iFirstIndex = 0;
		int iLastIndex = 0;
		int iNewSize = 0;
		LPWSTR lpTempArray = NULL;

		do
		{
			if ((iSubString = m_strlen(lpSubString)) == 0)
				break;

			if ((iFirstIndex = m_strstr(this->lpArray, lpSubString)) == -1)
				break;

			iLastIndex = iFirstIndex + iSubString;
			iNewSize = this->iSize - iSubString;
			if ((lpTempArray = AllocMemory(iNewSize * 2 + 1)) == NULL)
				break;

			m_memcpy(lpTempArray, this->lpArray, iFirstIndex * 2);
			m_memcpy(&lpTempArray[iFirstIndex], &this->lpArray[iLastIndex], (this->iSize - iLastIndex) * 2);

			equate(lpTempArray);

		} while (false);

		DeallocMemory(lpTempArray);
	}

	void int2str(int someInteger, bool bErase)
	{
		int iTemp = someInteger;
		int iTemp2 = someInteger;
		int iChars = 0;
		WCHAR lpTempArr[32] = { 0 };

		while (iTemp /= 10)
			iChars++;

		for (int i = iChars; i >= 0; i--, iTemp2 /= 10)
		{
			lpTempArr[i] = iTemp2 % 10 + '0';
		}

		if (bErase)
			append(lpTempArr);
		else
			equate(lpTempArr);

	}

	int str2int()
	{
		int iRet = 0;
		for (int i = 0; i < this->iSize; i++)
		{
			iRet = (this->lpArray[i] - '0') + (iRet * 10);
		}

		return iRet;
	}

	LPWSTR data()
	{
		return this->lpArray;
	}

	int length()
	{
		return this->iSize;
	}

	//operators

	void operator =(LPCWSTR lpNewString)
	{
		equate(lpNewString);
	}

	void operator +=(LPCWSTR lpNewString)
	{
		append(lpNewString);
	}

	bool operator ==(LPCWSTR lpString)
	{
		return equal(lpString);
	}

	//

	void operator =(String& stringStruct)
	{
		equate(stringStruct.data());
	}

	void operator +=(String& stringStruct)
	{
		append(stringStruct.data());
	}

	bool operator ==(String& stringStruct)
	{
		return equal(stringStruct.data());
	}

	String operator +(String& stringStruct)
	{
		String newString;
		newString += this->lpArray;
		newString += stringStruct.data();

		return newString;
	}

	//

	void operator =(int someInteger)
	{
		int2str(someInteger, true);
	}

	void operator +=(int someInteger)
	{
		int2str(someInteger, false);
	}

	bool operator ==(int someInteger)
	{
		String tempStr = someInteger;
		return equal(tempStr.data());
	}

	String()
	{
		return;
	}

	String(int iSize, int iChar)
	{
		resize(iSize, iChar);
	}

	String(int someInteger)
	{
		int2str(someInteger, true);
	}

	String(LPCWSTR lpNewString)
	{
		equate(lpNewString);
	}

	String(const String &stringStruct)
	{
		equate(stringStruct.lpArray);
	}

	~String()
	{
		free();
	}
};

template <int N>
struct StaticString
{
private:
	WCHAR wData[N] = { 0 };
	static constexpr DWORD dwKey[8] = { __TIME__[2], __TIME__[3], __TIME__[1], __TIME__[5], __TIME__[7], __TIME__[4], __TIME__[6], __TIME__[1] };

public:
	constexpr StaticString(LPCWSTR data)
	{
		for (int i = 0; i < N; i++) {
			wData[i] = data[i] ^ dwKey[i % 2] % 32;
		}
	}

	void deobfoscate(WCHAR* lpRet) const
	{
		int i = 0;
		do
		{
			lpRet[i] = wData[i] ^ dwKey[i % 2] % 32;
			i++;
		} while (lpRet[i - 1]);
	}
};

#define static_string(str) \
    []() -> wchar_t* { \
        constexpr int size = sizeof(str) / sizeof(str[0]); \
        constexpr StaticString<size> obfuscated_str(str); \
        static WCHAR original_string[size]; \
        obfuscated_str.deobfoscate((WCHAR *)original_string); \
        return original_string; \
    }()

struct RC4
{
private:
	int binPassword[257] = { 0 };
	int swap = 0;

	void init(LPCWSTR lpKey, int iSize)
	{
		int b = 0, a = 0;

		for (a = 0; a < 257; a++)
		{
			this->binPassword[a] = a;
		}

		for (a = 0; a < 257; a++)
		{
			b = (b + this->binPassword[a] + lpKey[a % iSize]) % 257;
			this->swap = this->binPassword[a];
			this->binPassword[a] = this->binPassword[b];
			this->binPassword[b] = this->swap;
		}
	}

	void rc4(LPWSTR lpRetArr, int iSize)
	{
		int tempPassword[257];
		int i = 0, j = 0;

		m_memcpy(tempPassword, this->binPassword, 257 * sizeof(int));
		for (int x = 0; x < iSize; x++)
		{
			i = (i + 1) % 257;
			j = (j + tempPassword[i]) % 257;
			this->swap = tempPassword[i];
			tempPassword[i] = tempPassword[j];
			tempPassword[j] = this->swap;
			lpRetArr[x] ^= tempPassword[(tempPassword[i] + tempPassword[j]) % 257];
		}
	}

public:

	void free()
	{
		memset(this->binPassword, 0, 256);
		this->swap = 0;
	}

	String protect(LPCWSTR lpData)
	{
		String tempString = lpData;
		rc4(tempString.data(), tempString.length());
		return tempString;
	}

	RC4(LPCWSTR lpKey)
	{
		init(lpKey, m_strlen(lpKey));
	}

	~RC4()
	{
		free();
	}
};

int main()
{
	String str = static_string(L"Hello, world, Nibbers!!!");

	MessageBoxW(0, str.data(), 0, 0);
}
