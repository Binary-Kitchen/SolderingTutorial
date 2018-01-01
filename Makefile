# Copyright (c) Binary Kitchen e.V., 2017
#   Author: Ralf Ramsauer <ralf@binary-kitchen.de>
#
# This work is licensed under the terms of CC-BY-SA, version 4. See the COPYING
# file in the top-level directory.

MAN_LANG=_de _en
PROJECTS=$(shell find . -name manual -type d | \
	 xargs -n 1 dirname | xargs -n 1 basename)

MANUALS=$(foreach PROJECT,$(PROJECTS), \
       	$(addprefix $(PROJECT)/manual/, $(addprefix $(PROJECT), $(MAN_LANG))))
LABELS=$(foreach PROJECT,$(PROJECTS), \
       	$(addprefix $(PROJECT)/manual/misc/, $(addprefix $(PROJECT), _label)))
BOXLABELS=$(foreach PROJECT,$(PROJECTS), \
       	$(addprefix $(PROJECT)/manual/misc/, $(addprefix $(PROJECT), _boxlabel)))

JOBS=$(MANUALS) $(LABELS) $(BOXLABELS)

all: $(JOBS)

$(JOBS):
	latexmk --lualatex --jobname=$@ ./LaTeX_config/solderingTut
