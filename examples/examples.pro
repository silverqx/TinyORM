TEMPLATE = subdirs

tom_example:!disable_tom {
    SUBDIRS += tom

    !build_pass: message("Build the tom example.")
}
