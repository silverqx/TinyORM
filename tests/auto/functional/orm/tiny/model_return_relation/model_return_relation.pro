include($$TINYORM_SOURCE_TREE/tests/qmake/common.pri)
include($$TINYORM_SOURCE_TREE/tests/qmake/TinyUtils.pri)
include($$TINYORM_SOURCE_TREE/tests/models/models.pri)

# To avoid: error: redefinition of 'bool __tls_guard'
# Even the latest GCC v14.2.1 has still TLS wrapper related bugs if the class that
# defines the thread_local static is templated and polymorphic. 😞
# This is the only test case where this is happening and I didn't find any workaround
# after 8 hours of investigation.
# Only workaround I found is to #include the torrent_includeslist.hpp then it compiles,
# so I'm disabling the thread_local for this test case.
# See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66944
# thread_local is already disabled on MSYS2 g++ so no need to target unix platform.
gcc:!clang: DEFINES *= TINYORM_DISABLE_THREAD_LOCAL

SOURCES += tst_model_return_relation.cpp
