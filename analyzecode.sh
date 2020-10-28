#!/bin/bash

METRIX="python ${HOME}/src/metrixplusplus/metrix++.py"

${METRIX} \
	collect \
	--std.code.lines.code \
	--std.code.complexity.cyclomatic \
	--std.code.complexity.maxindent

${METRIX} \
	limit \
	--max-limit=std.code.complexity:cyclomatic:3