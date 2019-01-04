# Copyright (c) Binary Kitchen e.V., 2017-2018
#   Author: Ralf Ramsauer <ralf@binary-kitchen.de>
#
# This work is licensed under the terms of CC-BY-SA, version 4. See the COPYING
# file in the top-level directory.

MAN_LANGS=de en fr
PROJECTS= $(foreach MAN_LANG,$(MAN_LANGS), $(wildcard */manual/configuration/$(MAN_LANG)_config.csv))
PROJECTLABELS= $(wildcard */manual/configuration/de_config.csv)

TEMP_FILES=*.aux *.fdb_latexmk *.fls *.log

projectname = $(subst /manual/configuration/, ,$(dir $(PROJECT)))
projectfolder = $(join $(projectname), /manual/)
projectlanguage = $(join _, $(patsubst %_config.csv, %, $(notdir $(PROJECT))))

MANUALS=$(foreach PROJECT,$(PROJECTS), \
		$(join $(projectfolder),$(join $(projectname), $(projectlanguage))))
		#source = "LaTeX_config/solderingTut"
		
LABELS=$(foreach PROJECT,$(PROJECTLABELS), \
       		$(join $(projectfolder),$(join misc/, $(join $(projectname), _label))))
		#source = "LaTeX_config/solderingLabels"

BOXLABELS=$(foreach PROJECT,$(PROJECTLABELS), \
       		$(join $(projectfolder),$(join misc/, $(join $(projectname), _boxlabel))))
		#source = "LaTeX_config/solderingBoxLabels"
		

all: $(MANUALS) $(LABELS) $(BOXLABELS)

manuals: $(MANUALS)

labels: $(LABELS) $(BOXLABELS)

clean:
	rm $(foreach TEMP, $(TEMP_FILES), \
		$(wildcard */*/$(TEMP)))
	rm  $(foreach TEMP, $(TEMP_FILES), \
		$(wildcard */$(TEMP)))

$(MANUALS):
	latexmk -lualatex --jobname=$@ "LaTeX_config/solderingTut"
	
$(LABELS):
	latexmk -lualatex --jobname=$@ "LaTeX_config/solderingLabels"

$(BOXLABELS):
	latexmk -lualatex --jobname=$@ "LaTeX_config/solderingBoxLabels"
	
