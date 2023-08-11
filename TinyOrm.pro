TEMPLATE = subdirs

SUBDIRS = src

tom_example {
    SUBDIRS += examples
    examples.depends = src
}

# Can be enabled by CONFIG += build_tests when the qmake.exe for the project is called
build_tests {
    SUBDIRS += tests
    tests.depends = src

    !build_pass: message( "Build TinyORM unit tests." )
}
