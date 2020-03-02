$(APP_S): %.VtableImpl.s : %.jar $(MAPLE_BIN) $(JBC2MPL_BIN) $(MPLCG_BIN) $(LIB_CORE_MPLT)
	$(MAPLE_BIN) -O0 --mplt $(LIB_CORE_MPLT) $<
