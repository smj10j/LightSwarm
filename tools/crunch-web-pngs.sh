echo "----------- Running pngquant on web pngs  --------------"

find web -type d | while read dir
do

echo "In directory: $dir"
tools/pngquant -f -ext .png 256 "$dir"/*.png

done



