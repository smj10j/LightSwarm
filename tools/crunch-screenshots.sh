echo "----------- Running pngquant on screenshots  --------------"

find screenshots -type d | while read dir
do

echo "In directory: $dir"
tools/pngquant -f -ext .png 256 "$dir"/*.png

done



