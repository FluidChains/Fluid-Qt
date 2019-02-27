#!/bin/bash

set -e
date

#################################################################
#                  Build Fluid from source                       #
#################################################################

build(){
        NPROC=$(nproc)
        echo "nproc: $NPROC"

	cd src
        sudo make -j$NPROC -f makefile.unix
        sudo cp fluidd /usr/local/bin/fluidd

        echo ""
        echo "Build completed ... copied src/fluidd to /usr/local/bin/fluidd"
        echo "Run the application by typing 'fluidd' at the command prompt"
        exit 0
}

init(){
        config=~/.fluid/fluid.conf
        if [ ! -e "$config" ]
        then
           mkdir -p ~/.fluid && touch $_/fluid.conf
           user=$(gpw 1 7)
           password=$(pwgen 13 1)
           echo "daemon=1" >> $config
           echo "server=1" >> $config
           echo "rpcuser=$user" >> $config
           echo "rpcpassword=$password" >> $config
           echo "" >> $config
           echo "logtimestamps=1" >> $config
        fi

        file=/etc/init.d/fluid
        if [ ! -e "$file" ]
	then
	   printf '%s\n%s\n' '#!/bin/sh' 'sudo fluidd' | sudo tee /etc/init.d/fluid
           sudo chmod +x /etc/init.d/fluid
           sudo update-rc.d fluid defaults
	fi
}


#################################################################
#   Update Ubuntu and install prerequisites for running Fluid   #
#################################################################
sudo apt-get update


#################################################################
#    Install all necessary packages for building Fluid          #
#################################################################
sudo apt-get install -y qt5-default qt5-qmake qtbase5-dev-tools qttools5-dev-tools \
     build-essential libboost-all-dev libssl1.0-dev libdb++-dev libminiupnpc-dev \
     software-properties-common libqrencode-dev gpw pwgen
sudo add-apt-repository -y ppa:bitcoin/bitcoin
sudo apt-get update
sudo apt-get install -y libdb4.8-dev libdb4.8++-dev


init
build


