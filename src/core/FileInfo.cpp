//---------------------------------------------------------------------------
#include "stdafx.h"
#include <Windows.h>

#include "boostdefines.hpp"
#include <boost/scope_exit.hpp>

#include <Common.h>
#include <Exceptions.h>
#include "FileInfo.h"
#include "FileBuffer.h"
//---------------------------------------------------------------------------
#ifndef _MSC_VER
#pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
#define DWORD_ALIGN( base, ptr ) \
    ( (LPBYTE)(base) + ((((LPBYTE)(ptr) - (LPBYTE)(base)) + 3) & ~3) )
struct VS_VERSION_INFO_STRUCT32
{
  WORD  wLength;
  WORD  wValueLength;
  WORD  wType;
  WCHAR szKey[1];
};
//---------------------------------------------------------------------------
unsigned int VERSION_GetFileVersionInfo_PE(const wchar_t * FileName, unsigned int DataSize, void * Data)
{
  size_t Len = 0;

  bool NeedFree = false;
  HMODULE Module = GetModuleHandle(FileName);
  // DEBUG_PRINTF(L"Module = %d", Module);
  if (Module == NULL)
  {
    Module = LoadLibraryEx(FileName, 0, LOAD_LIBRARY_AS_DATAFILE);
    NeedFree = true;
  }
  if (Module == NULL)
  {
  }
  else
  {
    {
      BOOST_SCOPE_EXIT ( (&NeedFree) (&Module) )
      {
        if (NeedFree)
        {
          FreeLibrary(Module);
        }
      } BOOST_SCOPE_EXIT_END
      HANDLE Rsrc = FindResource(Module, MAKEINTRESOURCE(VS_VERSION_INFO),
                                 MAKEINTRESOURCE(VS_FILE_INFO));
      if (Rsrc == NULL)
      {
      }
      else
      {
        Len = SizeofResource(Module, static_cast<HRSRC>(Rsrc));
        HANDLE Mem = LoadResource(Module, static_cast<HRSRC>(Rsrc));
        if (Mem == NULL)
        {
        }
        else
        {
          {
            BOOST_SCOPE_EXIT ( (&Mem) )
            {
              FreeResource(Mem);
            } BOOST_SCOPE_EXIT_END
            VS_VERSION_INFO_STRUCT32 * VersionInfo = static_cast<VS_VERSION_INFO_STRUCT32 *>(LockResource(Mem));
            const VS_FIXEDFILEINFO * FixedInfo =
              (VS_FIXEDFILEINFO *)DWORD_ALIGN(VersionInfo, VersionInfo->szKey + wcslen(VersionInfo->szKey) + 1);

            if (FixedInfo->dwSignature != VS_FFI_SIGNATURE)
            {
              Len = 0;
            }
            else
            {
              if (Data != NULL)
              {
                if (DataSize < Len)
                {
                  Len = DataSize;
                }
                if (Len > 0)
                {
                  memmove(Data, VersionInfo, Len);
                }
              }
            }
          }
        }
      }
    }
  }

  return Len;
}
//---------------------------------------------------------------------------
unsigned int __fastcall GetFileVersionInfoSizeFix(const wchar_t * FileName, unsigned long * Handle)
{
  unsigned int Len;
  if (IsWin7())
  {
    *Handle = 0;
    Len = VERSION_GetFileVersionInfo_PE(FileName, 0, NULL);
    // DEBUG_PRINTF(L"Len = %d", Len);

    if (Len != 0)
    {
      Len = (Len * 2) + 4;
    }
  }
  else
  {
    Len = GetFileVersionInfoSize(const_cast<wchar_t *>(FileName), Handle);
  }

  return Len;
}
//---------------------------------------------------------------------------
bool __fastcall GetFileVersionInfoFix(const wchar_t * FileName, unsigned long Handle,
                                      unsigned int DataSize, void * Data)
{
  bool Result = false;
  // DEBUG_PRINTF(L"IsWin7 = %d, Handle = %d, DataSize = %d", IsWin7(), Handle, DataSize);
  if (IsWin7())
  {
    VS_VERSION_INFO_STRUCT32 * VersionInfo = static_cast<VS_VERSION_INFO_STRUCT32 *>(Data);

    unsigned int Len = VERSION_GetFileVersionInfo_PE(FileName, DataSize, Data);
    // DEBUG_PRINTF(L"Len = %d, VersionInfo->wLength = %d", Len, VersionInfo->wLength);

    Result = (Len != 0);
    if (Result)
    {
      static const char Signature[] = "FE2X";
      size_t BufSize = VersionInfo->wLength + strlen(Signature);
      unsigned int ConvBuf;
      // DEBUG_PRINTF(L"DataSize = %d, BufSize = %d", DataSize, BufSize);

      if (DataSize >= BufSize)
      {
        ConvBuf = DataSize - VersionInfo->wLength;
        // DEBUG_PRINTF(L"ConvBuf = %d", ConvBuf);
        memmove((static_cast<char *>(Data)) + VersionInfo->wLength, Signature, ConvBuf > 4 * sizeof(char) ? 4 * sizeof(char) : ConvBuf );
        // DEBUG_PRINTF(L"Data = %s", Data);
      }
    }
  }
  else
  {
    Result = GetFileVersionInfo(const_cast<wchar_t *>(FileName), Handle, DataSize, Data);
  }

  return Result;
}
//---------------------------------------------------------------------------
// Return pointer to file version info block
void * __fastcall CreateFileInfo(const UnicodeString FileName)
{
  unsigned long Handle;
  unsigned int Size;
  void * Result = NULL;


  // Get file version info block size
  Size = GetFileVersionInfoSizeFix(FileName.c_str(), &Handle);
  // If size is valid
  // DEBUG_PRINTF(L"FileName = %s, Size = %d, Handle = %u", FileName.c_str(), Size, Handle);
  if (Size > 0)
  {
    Result = new char[Size];
    // Get file version info block
    if (!GetFileVersionInfoFix(FileName.c_str(), Handle, Size, Result))
    {
      delete[] Result;
      Result = NULL;
    }
  }
  else
  {
  }
  return Result;
}
//---------------------------------------------------------------------------
// Free file version info block memory
void __fastcall FreeFileInfo(void * FileInfo)
{
  delete[] FileInfo;
}
//---------------------------------------------------------------------------
typedef TTranslation TTranslations[65536];
typedef TTranslation * PTranslations;
//---------------------------------------------------------------------------
// Return pointer to fixed file version info
VS_FIXEDFILEINFO __fastcall GetFixedFileInfo(void * FileInfo)
{
  UINT Len;
  VS_FIXEDFILEINFO * pResult = NULL;
  if (!VerQueryValue(FileInfo, L"\\", reinterpret_cast<void **>(&pResult), &Len))
  {
    throw std::exception("Fixed file info not available");
  }
  return *pResult;
};

//---------------------------------------------------------------------------
// Return number of available file version info translations
unsigned __fastcall GetTranslationCount(void * FileInfo)
{
  PTranslations P;
  UINT Len;
  if (!VerQueryValue(FileInfo, L"\\VarFileInfo\\Translation", reinterpret_cast<void **>(&P), &Len))
  {
    throw std::exception("File info translations not available");
  }
  return Len / 4;
}
//---------------------------------------------------------------------------
// Return i-th translation in the file version info translation list
TTranslation __fastcall GetTranslation(void * FileInfo, unsigned i)
{
  PTranslations P;
  UINT Len;

  if (!VerQueryValue(FileInfo, L"\\VarFileInfo\\Translation", reinterpret_cast<void **>(&P), &Len))
  {
    throw std::exception("File info translations not available");
  }
  // DEBUG_PRINTF(L"Len = %d, Language = %x", Len, P[i].Language);
  if (i * sizeof(TTranslation) >= Len)
  {
    throw std::exception("Specified translation not available");
  }
  return P[i];
};
//---------------------------------------------------------------------------
// Return the name of the specified language
UnicodeString __fastcall GetLanguage(unsigned int Language)
{
  UINT Len;
  wchar_t P[512];

  Len = VerLanguageName(Language, P, sizeof(P));
  if (Len > sizeof(P))
  {
    throw std::exception("Language not available");
  }
  return UnicodeString(P, Len);
};
//---------------------------------------------------------------------------
// Return the value of the specified file version info string using the
// specified translation
UnicodeString __fastcall GetFileInfoString(void * FileInfo,
    TTranslation Translation, UnicodeString StringName)
{
  UnicodeString Result;
  wchar_t * P = NULL;
  UINT Len;
  UnicodeString subBlock = UnicodeString((L"\\StringFileInfo\\000004E4") +
                                         // IntToHex(Translation.Language, 4) +
                                         // IntToHex(Translation.CharSet, 4) +
                                         UnicodeString(L"\\") + StringName);
  // 4e40409 58324546\Comments
  if (!VerQueryValue(FileInfo, subBlock.c_str(), reinterpret_cast<void **>(&P), &Len))
  {
    throw std::exception("Specified file info string not available");
  }
  // c_str() makes sure that returned string has only necessary bytes allocated
  if (P)
  {
    Result = UnicodeString(P, Len).c_str();
  }
  // DEBUG_PRINTF(L"Result = %s", Result.c_str());
  return Result;
};
//---------------------------------------------------------------------------
int __fastcall CalculateCompoundVersion(int MajorVer,
                                        int MinorVer, int Release, int Build)
{
  int CompoundVer = Build + 10000 * (Release + 100 * (MinorVer +
                                     100 * MajorVer));
  return CompoundVer;
}
