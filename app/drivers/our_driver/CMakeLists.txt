target_include_directories(app PRIVATE include)

if(CONFIG_OUR_DRIVER)
    target_sources(app PRIVATE our_driver.c)
endif()
