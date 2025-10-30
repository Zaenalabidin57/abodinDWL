#!/bin/bash

# XWayland stability fix for dwl
# This script applies patches to fix BadWindow errors and XWayland hanging issues

set -e

echo "=== dwl XWayland Stability Fix ==="
echo "This will patch and rebuild dwl to fix XWayland hanging issues"
echo ""

# Check if we're in the dwl directory
if [ ! -f "dwl.c" ]; then
    echo "Error: Please run this script from the dwl source directory"
    exit 1
fi

# Backup original files
echo "Creating backups..."
cp dwl.c dwl.c.backup
cp client.h client.h.backup

echo "Applying XWayland stability patches..."

# Apply the main patch
if [ -f "xwayland_stability.patch" ]; then
    patch -p1 < xwayland_stability.patch
    echo "✓ Applied stability patch"
else
    echo "Error: xwayland_stability.patch not found"
    exit 1
fi

# Clean build
echo "Cleaning previous build..."
make clean 2>/dev/null || true

# Build with debug enabled for better logging
echo "Building dwl with debug logging..."
make CFLAGS="-DXWAYLAND -DWLR_DEBUG -g -O0"

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ Build successful!"
    echo ""
    echo "=== Installation Instructions ==="
    echo "1. Test the new dwl binary:"
    echo "   ./dwl -d  # Run with debug logging"
    echo ""
    echo "2. If it works, install system-wide:"
    echo "   sudo make install"
    echo ""
    echo "=== What this fixes ==="
    echo "• BadWindow (invalid Window parameter) errors"
    echo "• XWayland server hanging and crashes"
    echo "• Race conditions in XCB resource management"
    echo "• Automatic XWayland recovery when it dies"
    echo "• Better error handling and logging"
    echo ""
    echo "=== Monitoring ==="
    echo "Run with: ./dwl -d 2>&1 | tee dwl.log"
    echo "Check the log for XWayland events and any remaining errors"
else
    echo ""
    echo "✗ Build failed! Restoring backup..."
    mv dwl.c.backup dwl.c
    mv client.h.backup client.h
    echo "Restored original files"
    exit 1
fi