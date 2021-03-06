# Build the test programs...

set (TESTPROGRAMS
  ProjTest TMTest GeodTest ConicTest NaNTester HarmTest EllipticTest)

if (NOT WIN32 AND CXX11FLAG)
  set (TESTPROGRAMS ${TESTPROGRAMS} GeodShort)
endif ()

# Loop over all the tools, specifying the source and library.
add_custom_target (testprograms)
foreach (TESTPROGRAM ${TESTPROGRAMS})

  add_executable (${TESTPROGRAM} EXCLUDE_FROM_ALL ${TESTPROGRAM}.cpp)
  add_dependencies (testprograms ${TESTPROGRAM})
  target_link_libraries (${TESTPROGRAM} ${PROJECT_LIBRARIES}
    ${QUAD_LIBRARIES} ${MPFR_LIBRARIES})

endforeach ()

add_executable (GeodExact EXCLUDE_FROM_ALL GeodExact.cpp
  Geodesic30.cpp GeodesicLine30.cpp
  Geodesic30.hpp GeodesicLine30.hpp)
add_dependencies (testprograms GeodExact)
target_link_libraries (GeodExact ${PROJECT_LIBRARIES}
    ${QUAD_LIBRARIES} ${MPFR_LIBRARIES})
set (TESTPROGRAMS ${TESTPROGRAMS} GeodExact)

# Put all the tools into a folder in the IDE
set_property (TARGET testprograms ${TESTPROGRAMS} PROPERTY FOLDER tests)

# Don't install test programs
