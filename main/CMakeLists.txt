idf_component_register(
	 SRCS 
	    "buffer.c"
	 	"cat.c"
		"config.c"
		"http.c"
		"info.c"
		"main.c"
		"network.c"
		"observer.c"
		"rig_commands.c"
		"rig_monitor.c"
		"rig_uart.c"
		"settings.c"
		"wifi.c"
	PRIV_REQUIRES "dns_server" 
	PRIV_REQUIRES "esp_driver_uart"
	PRIV_REQUIRES "esp_http_server"
	PRIV_REQUIRES "esp_timer"
	PRIV_REQUIRES "esp_wifi"
	PRIV_REQUIRES "json"
	PRIV_REQUIRES "nvs_flash"
	INCLUDE_DIRS ".")
littlefs_create_partition_image(html ../html FLASH_IN_PROJECT)
