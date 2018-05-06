execute_process(COMMAND "${QT_DIR}/bin/macdeployqt" "${CMAKE_BINARY_DIR}/procalc.app")
file(COPY "${DATA_DIR}" DESTINATION "${CMAKE_BINARY_DIR}/procalc.app/Contents/Resources/")
