#!/bin/bash

set -xe

binary=chase

gcc -O1 -pthread -o $binary main.c
