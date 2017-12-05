#!/bin/bash

# Copyright (c) Binary Kitchen e.V., 2017
#   Author: Ralf Ramsauer <ralf@binary-kitchen.de>
#
# This work is licensed under the terms of CC-BY-SA, version 4. See the COPYING
# file in the top-level directory.

MAN_LANG=de

function die {
	echo "$@" 1>&2;
	exit -1;
}

# We're looking for all directories containing a manual subdir
for target in */manual ; do
	project=${target%%/*}
	pushd $target
	latexmk --lualatex --jobname=${project}_${MAN_LANG} ../../LaTeX_config/solderingTut || \
	       	die "Latexmk failed for $target"
	latexmk --lualatex --jobname=misc/${project}_label_${MAN_LANG} ../../LaTeX_config/solderingLabels.tex || \
	       	die "Latexmk failed for $target label"
	latexmk --lualatex --jobname=misc/${project}_Boxlabel_${MAN_LANG} ../../LaTeX_config/solderingBoxLabels.tex || \
	       	die "Latexmk failed for $target label"
	popd
done

pushd misc
latexmk --lualatex --jobname="commonFailures_de" ../LaTeX_config/commonFailures
latexmk --lualatex --jobname="preisliste_de" ../LaTeX_config/Preisliste
popd
