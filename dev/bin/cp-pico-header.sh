#!/bin/sh
PICO_INCLUDE_PATH=$1
DEST=$2

echo "-- copy libttspico header files from $PICO_INCLUDE_PATH"
mkdir -p "$DEST"
for input in $(find "$PICO_INCLUDE_PATH" -name "pico*.h")
do
	output="$DEST/$(basename $input)"
	# echo "$input > $output"
	sed -e 's:\bthis\b:that:g' "$input" > "$output"
done
exit 0
