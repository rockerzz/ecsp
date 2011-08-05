subdirs(dict 
        engine
        events
        logic 
        api 
        plugins 
        acl 
        db 
        platform 
        toolkit
        3rdparty
        python)

add_executable(${PROJECT_NAME} main.cc)
target_link_libraries(${PROJECT_NAME} engine solution_logic)
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_BIN_DIR})





