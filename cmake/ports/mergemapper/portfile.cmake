# header-only library
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO alandtse/MergeMapper
    REF 1b122ae01cd13e19f0ec16d4605e813dcc7946c2
    SHA512 ab45ef9087cc79b432ff8b56e3ffd9bfda4c2dfc43d6678add81210b3ac8074c9d4d09cfcf43a41ee6b2a2b33ade1eab62db99ee922c8feac91a063888fe368b
    HEAD_REF main
)

# Install codes
set(MERGEMAPPER_SOURCE ${SOURCE_PATH}/src/MergeMapperPluginAPI.cpp ${SOURCE_PATH}/include/MergeMapperPluginAPI.h)

file(INSTALL ${MERGEMAPPER_SOURCE} DESTINATION ${CURRENT_PACKAGES_DIR}/include)

file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)
