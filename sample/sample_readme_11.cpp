#include <iostream>

#include <libcopp/utils/config/libcopp_build_features.h>

#if (defined(LIBCOTASK_MACRO_ENABLED) && LIBCOTASK_MACRO_ENABLED) && defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && \
    LIBCOPP_MACRO_ENABLE_WIN_FIBER
// include task header file
#  include <libcotask/task.h>

struct my_task_macro_t {
  using stack_allocator_type = copp::coroutine_fiber_context_default::allocator_type;
  using coroutine_type = copp::coroutine_fiber_context_default;
  using value_type = int;
};

typedef cotask::task<my_task_macro_t> my_task_t;

#  ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4091)

#    include <atlconv.h>
#    include <imagehlp.h>

#    pragma comment(lib, "dbghelp.lib")

#    ifdef UNICODE
#      define SAMPLE_VC_TEXT(x) A2W(x)
#    else
#      define SAMPLE_VC_TEXT(x) x
#    endif

LPTOP_LEVEL_EXCEPTION_FILTER g_msvc_debuger_old_handle = nullptr;
std::string g_msvc_debuger_pattern;

inline void CreateMiniDump(EXCEPTION_POINTERS *pep, LPCTSTR strFileName) {
  HANDLE hFile =
      CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

  if ((hFile != nullptr) && (hFile != INVALID_HANDLE_VALUE)) {
    MINIDUMP_EXCEPTION_INFORMATION mdei;
    mdei.ThreadId = GetCurrentThreadId();
    mdei.ExceptionPointers = pep;
    mdei.ClientPointers = FALSE;
    // MINIDUMP_CALLBACK_INFORMATION mci;
    // mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
    // mci.CallbackParam = 0;
    MINIDUMP_TYPE mdt =
        (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory | MiniDumpWithDataSegs | MiniDumpWithHandleData |
                        MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules);
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, (pep != 0) ? &mdei : 0, 0, nullptr);
    CloseHandle(hFile);
  }
}

LONG WINAPI GPTUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
  //得到当前时间
  SYSTEMTIME st;
  ::GetLocalTime(&st);
  //得到程序所在文件夹
  // TCHAR exeFullPath[256]; // MAX_PATH
  // GetModuleFileName(nullptr, exeFullPath, 256);//得到程序模块名称，全路径

  TCHAR szFileName[_MAX_FNAME] = {0};

  USES_CONVERSION;

  wsprintf(szFileName, TEXT("%s-%04d-%02d-%02d.%02d%02d%02d.%03d.dmp"), SAMPLE_VC_TEXT(g_msvc_debuger_pattern.c_str()),
           st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
  CreateMiniDump(pExceptionInfo, szFileName);

  if (nullptr == g_msvc_debuger_old_handle) {
    return EXCEPTION_EXECUTE_HANDLER;  // 下一个Handle, 一般是程序停止运行
  }

  return g_msvc_debuger_old_handle(pExceptionInfo);
}

void __cdecl sample_setup_msvc_mini_dump(const char *prefix) {
  g_msvc_debuger_pattern = prefix;
  g_msvc_debuger_old_handle = SetUnhandledExceptionFilter(GPTUnhandledExceptionFilter);
  if (g_msvc_debuger_old_handle == GPTUnhandledExceptionFilter) {
    g_msvc_debuger_old_handle = nullptr;
  }
}
#  endif

int main() {
#  ifdef _MSC_VER
  sample_setup_msvc_mini_dump("d:/libcopp-test-minidump");
#  endif
  // create a task using factory function [with lambda expression]
  my_task_t::ptr_t task = my_task_t::create([]() {
    std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " started" << std::endl;
    cotask::this_task::get_task()->yield();
    std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " resumed" << std::endl;

    // ! Make crash and it's will generate a mini dump into d:/libcopp-test-minidump-*.dmp
    // copp::this_coroutine::get_coroutine()->yield();
    return 0;
  });

  std::cout << "task " << task->get_id() << " created" << std::endl;
  // start a task
  task->start();

  std::cout << "task " << task->get_id() << " yield" << std::endl;
  task->resume();
  std::cout << "task " << task->get_id() << " stoped, ready to be destroyed." << std::endl;

  return 0;
}

#  ifdef _MSC_VER
#    pragma warning(pop)
#  endif

#else
int main() {
  std::cerr << "lambda not supported, or fiber is not supported, this sample is not available." << std::endl;
  return 0;
}
#endif