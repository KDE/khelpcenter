########### install files ###############
kdoctools_create_handbook(index.docbook INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en SUBDIR khelpcenter/glossary)

add_test(NAME glossary-checkxrefs
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/checkxrefs.pl
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
