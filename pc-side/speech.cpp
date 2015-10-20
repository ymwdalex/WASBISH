#include "socket.h"
#include "speech.h"
#include <assert.h>


Speech::Speech(){
  pVoice = NULL;
  ::CoInitialize(NULL);
  hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
}

Speech* Speech::instance(){
  static Speech speech;
  return &speech;
}

void Speech::speakStr(const std::string& str){
  const char *text = str.c_str();
  speak(text);
}

void Speech::speak(const char *text)
{
    wchar_t* pwszString = NULL;
    int   iRequiredSize = 0;

      // Process input argument.      
      iRequiredSize = MultiByteToWideChar
      (
        CP_ACP,
        0,
        text,
        -1,
        NULL,
        0
      );
    
      pwszString = new wchar_t[iRequiredSize];

      MultiByteToWideChar
      (
        CP_ACP,
        0,
        text,
        -1,
        pwszString,
        iRequiredSize
      );

      //hr = pVoice->Speak(L"This sounds normal <pitch middle = '-10'/> but the pitch drops half way through", SPF_IS_XML, NULL );
      hr = pVoice->Speak(pwszString, SPF_IS_XML, NULL );

      delete [] pwszString;
}


DWORD WINAPI ttsThread( LPVOID lpParam ) 
{
  Speech* speech = Speech::instance();
  SocketServer in("127.0.0.1", 8765, 5);
  char buf[1024];

  sockaddr_in clientAddr ;

  // first time
  int size = sizeof(clientAddr);
  memset(buf, 0, sizeof(buf));
  extern HANDLE ghSemaphore;
  // After speech thread is ready, let main thread continue
  ReleaseSemaphore(ghSemaphore, 1, NULL);
  int len = recvfrom(in.s_, buf, sizeof(buf), 0, (sockaddr *)&clientAddr, &size);
  speech->speak(buf);

  while (1) {
    int size = sizeof(clientAddr);
    memset(buf, 0, sizeof(buf));
    int len = recvfrom(in.s_, buf, sizeof(buf), 0, (sockaddr *)&clientAddr, &size);
    speech->speak(buf);
  }

  return 0;
}

