$(APP_S): %.VtableImpl.s : %.jar $(MAPLE_BIN) $(JBC2MPL_BIN) $(MPLCG_BIN) $(LIB_CORE_MPLT)
	$(MAPLE_BIN) -$(OPT) --mplt $(LIB_CORE_MPLT) $<
