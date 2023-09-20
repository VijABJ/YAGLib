
PROJECT(YAGLib)

# run the ruby script to generate macros and lists we need
# this MUST be placed BEFORE INCLUDEing SourceFiles.txt
EXECUTE_PROCESS(
  COMMAND ruby gen_source_lists.rb ${CMAKE_GENERATOR}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )

# SourceFiles includes a listing of all the source files
INCLUDE(SourceFiles.txt)

# set the destination folders depending on the generator
IF(CMAKE_GENERATOR MATCHES "Visual Studio")
  SET(LIBPREFIX ../../../lib/)
  SET(EXEPREFIX ../../../bin/)
ELSEIF(CMAKE_GENERATOR MATCHES "NMake")
  SET(LIBPREFIX ${LIBDEST}/)
  SET(EXEPREFIX ${EXEDEST}/)
ELSE(CMAKE_GENERATOR MATCHES "NMake")
  SET(LIBPREFIX ${LIBDEST}/lib)
  SET(EXEPREFIX ${LIBDEST}/bin)
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio")

# ---------------------------------------------------------
# further rule refinements
# ---------------------------------------------------------
SET(DEPLIST "")
FOREACH(LIBP ${LIB_PROJECTS})
  SET_TARGET_PROPERTIES(${LIBP}
    PROPERTIES
      PROJECT_LABEL ${LIBP}
      PREFIX ${LIBPREFIX}
      OUTPUT_NAME ${LIBP}
    )
  # only create the dependency instruction if the dependency list
  # is NOT empty.
  #LIST(LENGTH DEPLIST DLEN)
  #IF(DLEN GREATER 0)
  #  ADD_DEPENDENCIES(${LIBP} ${DEPLIST})
  #ENDIF(DLEN GREATER 0)
  # accumulate dependency stuff here
  LIST(APPEND DEPLIST ${LIBP})
ENDFOREACH(LIBP ${LIB_PROJECTS})

FOREACH(APP ${DEPENDENT_APPS})
  SET_TARGET_PROPERTIES(${APP}
    PROPERTIES
      PROJECT_LABEL ${APP}
      PREFIX ${EXEPREFIX}
      OUTPUT_NAME ${APP}
    )
  TARGET_LINK_LIBRARIES(${APP} ${DEPLIST})
  ADD_DEPENDENCIES(${APP} ${DEPLIST})
ENDFOREACH(APP ${DEPENDENT_APPS})

