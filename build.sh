#!/bin/sh

cd ./libcomm_utils/build && cmake ../ && make clean all
cd -
cd ./libcomm_net/build && cmake ../ && make clean all
cd -
cd ./libsmart_net/build && cmake ../ && make clean all
cd -
cd ./smart_net_test/build && cmake ../ && make clean all
cd -
