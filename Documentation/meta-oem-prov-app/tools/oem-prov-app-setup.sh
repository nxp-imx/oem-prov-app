#!/bin/bash
# Copyright 2025 NXP

show_help() {
	echo "Usage: $0"
}

if [ "$1" == "--help" ]; then
	show_help
	exit 0
fi

BUILD_DIR=.
echo "Adding the OEM Provisioning Application layer"
NO_CONFIG=$1

BBLAYERS_CONF=$BUILD_DIR/conf/bblayers.conf
LAYER_NAME="meta-oem-prov-app"
LAYER_PATH="/sources/meta-oem-prov-app"

#check if the layer is already in bblayers.conf
if grep -q "$LAYER_NAME" "$BBLAYERS_CONF"
then
	echo "Layer $LAYER_PATH is already in $BBLAYERS_CONF"
else
	#Add the custome layer to bblayers.conf
	echo "" >> $BBLAYERS_CONF
	echo "# OEM Provisioning Application" >> $BBLAYERS_CONF
	echo "BBLAYERS += \"\${BSPDIR}$LAYER_PATH\"" >> $BBLAYERS_CONF
	echo "Adding $LAYER_NAME to $BBLAYERS_CONF"
fi

LOCAL_CONF=$BUILD_DIR/conf/local.conf

add_recipe_to_local_conf() {
	RECIPE_NAME=$1
	#check if the recipe is already in local.conf file
	if grep -q "$RECIPE_NAME" "$LOCAL_CONF"; then
		echo "Recipe $RECIPE_NAME is already in $LOCAL_CONF"
	else
		echo "" >> $LOCAL_CONF
		echo "CORE_IMAGE_EXTRA_INSTALL:append=\" $RECIPE_NAME\"" >> $LOCAL_CONF
		echo "Adding $RECIPE_NAME to $LOCAL_CONF"
	fi
}
add_recipe_to_local_conf "oem-prov-config"

