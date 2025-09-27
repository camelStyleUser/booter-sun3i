#!/bin/sh
sh -c "truncate -s $((($(stat -c%s $1) + 511) / 512 * 512)) $1"
