

REPORT = report.pl --include=cwmp

TR_181 = cwmp/tr-181-2-3-0.xml
TR_196 = cwmp/tr-196-1-1-0.xml

OPEN_HENB_MDL = OpenHeNB-1-0-0.mdl.xml
OPEN_HENB_DOC = $(patsubst %.xml,%.html,$(OPEN_HENB_MDL))

mdl: $(OPEN_HENB_MDL)

doc: $(OPEN_HENB_DOC)

%.mdl.xml: %.xml
	@echo 'Generate MODEL $<'
	$(REPORT) --report=xml --outfile=$@ $<

%.html: %.xml
	@echo 'Generate DOC $<'
	$(REPORT) --report=html --outfile=$@ $<

cwmp:
	@wget http://broadband-forum.org/cwmp.zip
	@unzip cwmp.zip

clean:
	@rm $(OPEN_HENB_MDL) $(OPEN_HENB_DOC)
	@rm cwmp/*
	@rm cwmp.zip


