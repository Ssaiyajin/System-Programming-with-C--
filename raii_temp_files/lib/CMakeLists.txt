include(CheckCXXSymbolExists)

function(require_symbol SYMBOL)
   check_cxx_symbol_exists("${SYMBOL}" "${ARGN}" "HAVE_${SYMBOL}")

   if (NOT HAVE_${SYMBOL})
      message(FATAL_ERROR "${SYMBOL} is required (${HAVE_${SYMBOL}})")
   endif ()
endfunction()

require_symbol(mkdir sys/stat.h sys/types.h)
require_symbol(mkdirat sys/stat.h sys/types.h fcntl.h)
require_symbol(open sys/stat.h sys/types.h fcntl.h)
require_symbol(openat sys/stat.h sys/types.h fcntl.h)
require_symbol(unlinkat unistd.h)
require_symbol(rmdir unistd.h)
require_symbol(close unistd.h)

set(LIB_SOURCES CommandLine.cpp OSApi.cpp TempDirectory.cpp TempFile.cpp)

add_library(raii_core ${LIB_SOURCES})
target_include_directories(raii_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_raii_core ${LIB_SOURCES})
add_dependencies(lint lint_raii_core)
