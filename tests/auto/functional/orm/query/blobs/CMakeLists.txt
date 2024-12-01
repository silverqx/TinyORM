add_executable(blobs
    tst_blobs.cpp
)

add_test(NAME blobs COMMAND blobs)

tiny_configure_test(blobs DEPENDS_ON_UNITTESTS)
