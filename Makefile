# Copyright (c) Binary Kitchen e.V., 2017
#   Author: Ralf Ramsauer <ralf@binary-kitchen.de>
#
# This work is licensed under the terms of CC-BY-SA, version 4. See the COPYING
# file in the top-level directory.

MAN_LANGS=de en
LANG=de
PROJECTS= $(foreach MAN_LANG,$(MAN_LANGS), $(wildcard */manual/configuration/$(MAN_LANG)_config.csv))

projectname = $(subst /manual/configuration/, ,$(dir $(PROJECT)))
projectfolder = $(join $(projectname), /manual/)
projectlanguage = $(join _, $(patsubst %_config.csv, %, $(notdir $(PROJECT))))

MANUALS=$(foreach PROJECT,$(PROJECTS), \
		$(join $(projectfolder),$(join $(projectname), $(projectlanguage))))
		source = "LaTeX_config/solderingTut"
		
LABELS=$(foreach PROJECT,$(PROJECTS), \
       	$(join $(projectfolder),$(join $(projectname), _label)))
		source = "LaTeX_config/solderingLabels"
BOXLABELS=$(foreach PROJECT,$(PROJECTS), \
       	$(join $(projectfolder),$(join $(projectname), _boxlabel)))
		source = "LaTeX_config/solderingBoxLabels"
		
JOBS=$(MANUALS) $(LABELS) $(BOXLABELS)

CLEANUP=$(MANUALS) $(LABELS) $(BOXLABELS)

all: $(JOBS)

clean: $(CLEANUP)


$(JOBS):
	latexmk -cd -lualatex -recorder -outdir="../" --jobname=$@ $(source)
	
$(CLEANUP):
	latexmk  -c -cd -lualatex -recorder -outdir="../" --jobname=$@ $(source)

