#!/bin/bash

for tmp in $(find ./ -iname "*.ipynb" | grep -v 'venv' | grep -v 'checkpoint')
do
	echo "- Stripping ${tmp}..."
	jupyter nbconvert --ClearOutputPreprocessor.enabled=True --ClearMetadataPreprocessor.enabled=False --to=notebook "${tmp}" --output $(basename ${tmp})
done
