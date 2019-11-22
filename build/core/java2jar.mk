$(APP_JAR): %.jar : %.java $(LIB_CORE_JAR)
	$(JAVA2JAR) $(APP_JAR) $(LIB_CORE_JAR) "$(wildcard *.java)"
