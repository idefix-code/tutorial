#!/bin/bash

for tmp in $(git ls-files | grep ".ipynb")
do
	echo "- Stripping ${tmp}..."
	jupyter nbconvert --ClearOutputPreprocessor.enabled=True --ClearMetadataPreprocessor.enabled=False --to=notebook "${tmp}" --output $(basename ${tmp})
done
