#!/bin/bash

set -ev
set -x

apt-get update -y
apt-get install -y apt-utils | true
apt-get install -y software-properties-common | true
apt-get install -y python-software-properties
apt-get update -y
add-apt-repository -y ppa:jonathonf/gcc
apt-get update -y
apt-get install -y cmake software-properties-common git make
apt-get install -y gcc-7 g++-7
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 90
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 90
apt-get install -y unzip zlib1g-dev
apt-get install -y libboost-all-dev