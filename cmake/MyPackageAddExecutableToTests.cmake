MACRO (MYPACKAGEADDEXECUTABLESTOTESTS)
  SET (_candidates)
  IF ((TARGET ${PROJECT_NAME}_shared) OR (TARGET ${PROJECT_NAME}_iface))
    LIST(APPEND _candidates ${name})
  ENDIF ()
  IF (TARGET ${PROJECT_NAME}_static)
    LIST(APPEND _candidates ${name}_static)
  ENDIF ()

  LIST (APPEND ${PROJECT_NAME}_TEST_EXECUTABLE ${_candidates})
ENDMACRO ()

