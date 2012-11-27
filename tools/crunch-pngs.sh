echo "----------- Running pngquant on splash screens --------------"
tools/pngquant -f -ext .png 256 Default*.png

tools/crunch-screenshots.sh
tools/crunch-spritesheets.sh
