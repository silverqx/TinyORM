TEMPLATE = subdirs

SUBDIRS += src

# Can be enabled by CONFIG += build_tests when the qmake.exe for the project is called
build_tests {
    SUBDIRS += tests
    tests.depends = src
}
