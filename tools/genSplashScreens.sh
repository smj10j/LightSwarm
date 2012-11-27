srcDir="Penguin Rescue/Resources/src-assets/images/SplashScreen";
dstDir=".";
srcFile="Default-Landscape@2x~ipad.png"

cp "$srcDir/$srcFile" "$dstDir/Default-Landscape@2x~ipad.png"

cp "$srcDir/Default-Landscape@2x~ipad.png" "$dstDir/Default-Landscape~ipad.png"
sips -z 768 1024 "$dstDir/Default-Landscape~ipad.png"

cp "$dstDir/Default-Landscape~ipad.png" "$dstDir/Default-568h@2x.png"
sips -Z 1136 "$dstDir/Default-568h@2x.png"
sips -c 640 1136 "$dstDir/Default-568h@2x.png"

cp "$dstDir/Default-Landscape~ipad.png" "$dstDir/Default@2x.png"
sips -Z 960 "$dstDir/Default@2x.png"
sips -c 640 960 "$dstDir/Default@2x.png"

cp "$dstDir/Default@2x.png" "$dstDir/Default.png"
sips -z 320 480 "$dstDir/Default.png"


sips -r 270 "$dstDir/Default-568h@2x.png"
sips -r 270 "$dstDir/Default.png"
sips -r 270 "$dstDir/Default@2x.png"


echo "----------- Running pngquant on splash screens --------------"
tools/pngquant -f -ext .png 256 $dstDir/Default*.png

