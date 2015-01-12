#pragma once
//Public domain dedication by Robert Engeln
//  Originally published at: http://code-freeze.blogspot.com/2012/01/generating-stack-traces-from-c.html
#include <windows.h>
#include <DbgHelp.h>
#include <stdio.h>
#include <conio.h>
#include <csignal>
#include <iostream>
#include <string>
#include <sstream>

// temporary
#include <cassert>

class sym_handler  
{  
public:  
  static sym_handler& get_instance()  
  {  
    static sym_handler instance;  
    return instance;  
  }  

  std::string get_symbol_info(DWORD64 addr)  
  {  

    std::stringstream ss;  
    DWORD64 displacement64;  
    DWORD displacement;  
    char symbol_buffer[sizeof(SYMBOL_INFO) + 256];  
    SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbol_buffer);  
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);  
    symbol->MaxNameLen = 255;  

    IMAGEHLP_LINE64 line;  
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);  

   // ss << addr;
    if (m_initialized)  
    {  
      if (SymFromAddr(GetCurrentProcess(),  
        addr,  
        &displacement64,  
        symbol))  
      {  
        ss  << " " << std::string(symbol->Name, symbol->NameLen)  << std::endl;
        if (SymGetLineFromAddr64(GetCurrentProcess(),  
          addr,  
          &displacement,  
          &line))  
        {  
          ss << "File Name:" << line.FileName << " line: " << line.LineNumber;
        }  
      }
      else{

        DWORD err = GetLastError();
      }
    }  
    return ss.str();  
  }  

  void capture_stack_trace(CONTEXT* context, DWORD64* frame_ptrs, size_t count)  
  {  
    if (m_initialized)  
    {  

      assert(context);  
      //CONTEXT current_context;  
      //// In the case of a windows exception then the context
      //// wil be pre-set (ref: windows_exception_handler)
      //// ACTUALLY ... Why can't the signal handler call 
      //// RtlCaptureContext. 
      //if (!context)  
      //{  
      //  RtlCaptureContext(&current_context);  
      //  context = &current_context;  
      //}  

      DWORD machine_type;  
      STACKFRAME64 frame;  
      ZeroMemory(&frame, sizeof(frame));  
      frame.AddrPC.Mode = AddrModeFlat;  
      frame.AddrFrame.Mode = AddrModeFlat;  
      frame.AddrStack.Mode = AddrModeFlat;  
#ifdef _M_X64  
      frame.AddrPC.Offset = context->Rip;  
      frame.AddrFrame.Offset = context->Rbp;  
      frame.AddrStack.Offset = context->Rsp;  
      machine_type = IMAGE_FILE_MACHINE_AMD64;  
#else  
      frame.AddrPC.Offset = context->Eip;  
      frame.AddrPC.Offset = context->Ebp;  
      frame.AddrPC.Offset = context->Esp;  
      machine_type = IMAGE_FILE_MACHINE_I386;  
#endif  
      for (size_t i = 0; i < count; i++)  
      {  
        if (StackWalk64(machine_type,  
          GetCurrentProcess(),  
          GetCurrentThread(),  
          &frame,  
          context,  
          NULL,  
          SymFunctionTableAccess64,  
          SymGetModuleBase64,  
          NULL))  
        {  
            frame_ptrs[i] = frame.AddrPC.Offset;  
        }  
        else  
        {  
          break;  
        }  
      }  
    }  
  }  

private:  
  sym_handler()  
  {  
    m_initialized = SymInitialize(GetCurrentProcess(), NULL, TRUE) == TRUE;  
  }  

  ~sym_handler()  
  {  
    if (m_initialized)  
    {  
      SymCleanup(GetCurrentProcess());  
      m_initialized = false;  
    }  
  }  

  bool m_initialized;  
};  







class stack_trace  
{  
public:  
  stack_trace(CONTEXT* context)  
  {  
    ZeroMemory(m_frame_ptrs, sizeof(m_frame_ptrs));  
    sym_handler::get_instance().capture_stack_trace(context,  
      m_frame_ptrs,  
      max_frame_ptrs);  

  }  

  std::string to_string() const  
  {  
    std::stringstream ss;  
    for (size_t i = 0;  
      i < max_frame_ptrs && m_frame_ptrs[i];  
      ++i)  
    {  
      ss << sym_handler::get_instance().get_symbol_info(m_frame_ptrs[i]) << "\n";  
    }  
    return ss.str();  
  }  

private:  
  static const size_t max_frame_ptrs = 16;  
  DWORD64 m_frame_ptrs[max_frame_ptrs];  
};  


inline std::string to_string(const stack_trace& trace)  
{  
  return trace.to_string();  
}  

