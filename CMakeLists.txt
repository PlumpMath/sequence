ADD_BII_TARGETS()


IF(APPLE)
   TARGET_COMPILE_OPTIONS(${BII_BLOCK_TARGET} INTERFACE "-std=c++1y -stdlib=libc++")
ELSEIF (WIN32 OR UNIX)
   TARGET_COMPILE_OPTIONS(${BII_BLOCK_TARGET} INTERFACE "-std=c++1y")
ENDIF(APPLE)

include(biicode/boost/setup)
SET(Boost_USE_STATIC_LIBS ON)
bii_find_boost(COMPONENTS system coroutine context thread REQUIRED)
target_include_directories(${BII_BLOCK_TARGET} INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(${BII_BLOCK_TARGET} INTERFACE ${Boost_LIBRARIES})
