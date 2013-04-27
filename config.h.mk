
eq = $(and $(findstring $(1),$(2)),$(findstring $(2),$(1)))

define def
$(if $(call eq,$2,yes),\
  #define $1 &echo.>>config.h,\
  $(if $(call eq,$2,no),,\
    #define $1 $2 &echo.>>config.h))
endef


config.h: config.mk
	@echo "Generating config.h for $(MCU)"
	@echo // XBoot config header file > config.h
	@echo // MCU: $(MCU) >> config.h
	@echo // F_CPU: $(F_CPU) >> config.h

	@echo $(foreach v, \
	$(sort $(filter-out USE_CONFIG_H,$(filter \
				ATTACH_% \
				ENABLE_% \
				ENTER_% \
				FIFO_% \
				I2C_% \
				LED_% \
				UART_% \
				USE_% \
				WATCHDOG_%, $(.VARIABLES)))), \
	$(call def,$(v),$($(v))))