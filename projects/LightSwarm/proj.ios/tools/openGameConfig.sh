SDK_VERSION='6.0'
APP_NAME="LightSwarm"

GUID=`ls -t -r /Users/$USER/Library/Application\ Support/iPhone\ Simulator/$SDK_VERSION/Applications/ | tail -n 1`
SIMULATOR_ROOT_DIR="/Users/$USER/Library/Application Support/iPhone Simulator/$SDK_VERSION/Applications/$GUID/$APP_NAME.app"
open "$SIMULATOR_ROOT_DIR/GameConfig.json"

