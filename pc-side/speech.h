#ifndef __SPEECH_H__
#define __SPEECH_H__


#include <iostream>
#include "stdafx.h"
#include "sapi.h"
#include <windows.h>

extern DWORD WINAPI ttsThread( LPVOID lpParam );

class Speech
{
public:
  Speech();
  ~Speech(){
    pVoice->Release();
    pVoice = NULL;
    ::CoUninitialize();
  };

  static Speech* instance();
  void speak(const char *);
  void speakStr(const std::string& str);
private:
  ISpVoice * pVoice;
  HRESULT hr;
};


#endif

