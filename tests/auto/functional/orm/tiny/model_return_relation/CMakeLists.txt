project(model_return_relation
    LANGUAGES CXX
)

add_executable(model_return_relation
    tst_model_return_relation.cpp
)

add_test(NAME model_return_relation COMMAND model_return_relation)

include(TinyTestCommon)
tiny_configure_test(model_return_relation DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)

# To avoid: error: redefinition of 'bool __tls_guard'
# Even the latest GCC v14.2.1 has still TLS wrapper related bugs if the class that
# defines the thread_local static is templated and polymorphic. 😞
# This is the only test case where this is happening and I didn't find any workaround
# after 8 hours of investigation.
# Only workaround I found is to #include the torrent_includeslist.hpp then it compiles,
# so I'm disabling the thread_local for this test case.
# See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66944
# thread_local is already disabled on MSYS2 g++ so no need to target unix platform.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_definitions(model_return_relation
        PRIVATE TINYORM_DISABLE_THREAD_LOCAL
    )
endif()
