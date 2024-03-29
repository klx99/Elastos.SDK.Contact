file( GLOB Elastos.SDK.Contact-SOURCES "*.cpp" )
file( GLOB Elastos.SDK.Contact-HEADERS "*.hpp" "../include/*.hpp" )

if(("${CFG_TARGET_PLATFORM}" STREQUAL "Android") OR ("${CFG_TARGET_PLATFORM}" STREQUAL "iOS"))
list(FILTER Elastos.SDK.Contact-SOURCES EXCLUDE REGEX "Contact.cpp")
endif()

add_library(Elastos.SDK.Contact)
target_sources(Elastos.SDK.Contact PRIVATE
    ${Elastos.SDK.Contact-SOURCES})
target_include_directories(Elastos.SDK.Contact PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "../crosspl")
target_link_libraries(Elastos.SDK.Contact PRIVATE
    common
    json
    httpclient
    platform
    src)

if("${CFG_TARGET_PLATFORM}" STREQUAL "Android")
    target_link_libraries(Elastos.SDK.Contact PRIVATE log)
elseif("${CFG_TARGET_PLATFORM}" STREQUAL "iOS")
    target_link_libraries(Elastos.SDK.Contact PRIVATE "-framework CFNetwork")
endif()

set_target_properties(Elastos.SDK.Contact PROPERTIES PUBLIC_HEADER "${Elastos.SDK.Contact-HEADERS}")
install(TARGETS Elastos.SDK.Contact
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/Elastos.SDK.Contact)
