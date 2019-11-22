$(APP_MPL): %.mpl : %.jar $(JBC2MPL_BIN) $(LIB_CORE_MPLT)
	$(JBC2MPL_BIN) --mplt $(LIB_CORE_MPLT) -injar $(APP_JAR) -out $(APP)
