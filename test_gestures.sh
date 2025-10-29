#!/bin/bash

# Kill any existing dwl instances
pkill dwl

# Set debug log level and run dwl
export WLR_LOG=DEBUG
echo "Starting dwl with gesture debugging..."
echo "Try 3-finger and 4-finger swipes with MODKEY (Super/Windows key)"
echo "Check the debug output with: journalctl -f | grep dwl"
echo "Press Ctrl+C to stop dwl"

./dwl -s "ghostty" 2>&1 | tee dwl_debug.log
