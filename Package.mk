$(BUILD_DIR_ZIP)/chuni.zip:
	$(V)echo ... $@
	$(V)mkdir -p $(BUILD_DIR_ZIP)/chuni
	$(V)mkdir -p $(BUILD_DIR_ZIP)/chuni/DEVICE
	$(V)cp $(BUILD_DIR_32)/subprojects/capnhook/inject/inject.exe \
		$(BUILD_DIR_32)/chunihook/chunihook.dll \
		$(DIST_DIR)/chuni/segatools.ini \
		$(DIST_DIR)/chuni/start.bat \
		$(BUILD_DIR_ZIP)/chuni
	$(V)cp pki/billing.pub \
		pki/ca.crt \
    	$(BUILD_DIR_ZIP)/chuni/DEVICE
	$(V)strip $(BUILD_DIR_ZIP)/chuni/*.{exe,dll}
	$(V)cd $(BUILD_DIR_ZIP)/chuni ; zip -r ../chuni.zip *
	
$(BUILD_DIR_ZIP)/chusan.zip:
	$(V)echo ... $@
	$(V)mkdir -p $(BUILD_DIR_ZIP)/chusan
	$(V)mkdir -p $(BUILD_DIR_ZIP)/chusan/DEVICE
	$(V)cp $(BUILD_DIR_32)/subprojects/capnhook/inject/inject_x86.exe \
		$(BUILD_DIR_32)/chusanhook/chusanhook_x86.dll \
		$(BUILD_DIR_64)/subprojects/capnhook/inject/inject_x64.exe \
		$(BUILD_DIR_64)/chusanhook/chusanhook_x64.dll \
		$(DIST_DIR)/chusan/segatools.ini \
		$(DIST_DIR)/chusan/start.bat \
		$(BUILD_DIR_ZIP)/chusan
	$(V)cp pki/billing.pub \
		pki/ca.crt \
    	$(BUILD_DIR_ZIP)/chusan/DEVICE
	$(V)strip $(BUILD_DIR_ZIP)/chusan/*.{exe,dll}
	$(V)cd $(BUILD_DIR_ZIP)/chusan ; zip -r ../chusan.zip *

$(BUILD_DIR_ZIP)/idz.zip:
	$(V)echo ... $@
	$(V)mkdir -p $(BUILD_DIR_ZIP)/idz
	$(V)mkdir -p $(BUILD_DIR_ZIP)/idz/DEVICE
	$(V)cp $(BUILD_DIR_64)/subprojects/capnhook/inject/inject.exe \
		$(BUILD_DIR_64)/idzhook/idzhook.dll \
		$(DIST_DIR)/idz/segatools.ini \
		$(DIST_DIR)/idz/start.bat \
    	$(BUILD_DIR_ZIP)/idz
	$(V)cp pki/billing.pub \
		pki/ca.crt \
    	$(BUILD_DIR_ZIP)/idz/DEVICE
	$(V)strip $(BUILD_DIR_ZIP)/idz/*.{exe,dll}
	$(V)cd $(BUILD_DIR_ZIP)/idz ; zip -r ../idz.zip *

$(BUILD_DIR_ZIP)/mai2.zip:
	$(V)echo ... $@
	$(V)mkdir -p $(BUILD_DIR_ZIP)/mai2
	$(V)mkdir -p $(BUILD_DIR_ZIP)/mai2/DEVICE
	$(V)cp $(BUILD_DIR_64)/subprojects/capnhook/inject/inject.exe \
		$(BUILD_DIR_64)/mai2hook/mai2hook.dll \
		$(DIST_DIR)/mai2/segatools.ini \
		$(DIST_DIR)/mai2/start.bat \
		$(BUILD_DIR_ZIP)/mai2
	$(V)cp pki/billing.pub \
		pki/ca.crt \
    	$(BUILD_DIR_ZIP)/mai2/DEVICE
	$(V)strip $(BUILD_DIR_ZIP)/mai2/*.{exe,dll}
	$(V)cd $(BUILD_DIR_ZIP)/mai2 ; zip -r ../mai2.zip *

$(BUILD_DIR_ZIP)/doc.zip: \
		$(DOC_DIR)/config \
		$(DOC_DIR)/chunihook.md \
		$(DOC_DIR)/idzhook.md \
		| $(zipdir)/
	$(V)echo ... $@
	$(V)zip -r $@ $^

$(BUILD_DIR_ZIP)/segatools.zip: \
		$(BUILD_DIR_ZIP)/chuni.zip \
		$(BUILD_DIR_ZIP)/chusan.zip \
		$(BUILD_DIR_ZIP)/doc.zip \
		$(BUILD_DIR_ZIP)/idz.zip \
        $(BUILD_DIR_ZIP)/mai2.zip \
		CHANGELOG.md \
		README.md \

	$(V)echo ... $@
	$(V)zip -j $@ $^
