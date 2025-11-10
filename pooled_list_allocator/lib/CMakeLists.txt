add_library(allocator_list_core INTERFACE)
target_include_directories(allocator_list_core INTERFACE ${CMAKE_SOURCE_DIR})

add_library(allocator_list_dummy EXCLUDE_FROM_ALL
    Find_dummy.cpp
    List_dummy.cpp
    MallocAllocator_dummy.cpp
    PooledAllocator_dummy.cpp
)
target_include_directories(allocator_list_dummy PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_allocator_list_core
    Find_dummy.cpp
    List_dummy.cpp
    MallocAllocator_dummy.cpp
    PooledAllocator_dummy.cpp
)
add_dependencies(lint lint_allocator_list_core)
