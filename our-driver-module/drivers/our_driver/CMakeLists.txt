if(CONFIG_OUR_DRIVER)
  zephyr_library()
  zephyr_library_sources(our_driver.c)
endif()
