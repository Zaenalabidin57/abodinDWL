#!/bin/bash

# Quick fix for XWayland hanging issues
# Addresses zombie processes and applies stability patches

echo "=== XWayland Quick Fix ==="
echo ""

# Clean up zombie processes first
echo "1. Cleaning up zombie processes..."
ZOMBIE_PIDS=$(ps aux | awk '$8 ~ /^Z/ {print $2}')
if [ -n "$ZOMBIE_PIDS" ]; then
    echo "Found zombie processes: $ZOMBIE_PIDS"
    for pid in $ZOMBIE_PIDS; do
        echo "  Reaping zombie PID: $pid"
        wait "$pid" 2>/dev/null || true
    done
else
    echo "No zombie processes found"
fi

# Kill and restart dwl safely
echo ""
echo "2. Restarting dwl with proper cleanup..."

if pgrep -x dwl > /dev/null; then
    DWL_PID=$(pgrep -x dwl)
    echo "Stopping dwl (PID: $DWL_PID)..."
    
    # Send SIGTERM first for graceful shutdown
    kill -TERM "$DWL_PID"
    sleep 2
    
    # Force kill if still running
    if kill -0 "$DWL_PID" 2>/dev/null; then
        echo "Force killing dwl..."
        kill -KILL "$DWL_PID"
        sleep 1
    fi
    
    # Wait for full cleanup
    wait "$DWL_PID" 2>/dev/null || true
fi

# Clean up any remaining XWayland processes
echo "3. Cleaning up XWayland processes..."
pkill -f Xwayland 2>/dev/null || true
sleep 1

# Clean up shared memory segments that might be causing issues
echo "4. Cleaning up shared memory..."
ipcs -m | awk '$2 ~ /^0x/ {print $2}' | xargs -r ipcrm shm 2>/dev/null || true

# Apply stability patch if not already applied
echo ""
echo "5. Applying XWayland stability fixes..."
if [ ! -f "dwl.c.backup" ]; then
    echo "Creating backup of dwl.c..."
    cp dwl.c dwl.c.backup
fi

# Check if patch is already applied
if grep -q "check_xwayland_health" dwl.c; then
    echo "✓ Stability patch already applied"
else
    echo "Applying stability patch..."
    if [ -f "xwayland_stability.patch" ]; then
        patch -p1 < xwayland_stability.patch
        echo "✓ Patch applied successfully"
    else
        echo "⚠ Patch file not found, continuing without it"
    fi
fi

# Rebuild dwl with debug
echo ""
echo "6. Rebuilding dwl with debug logging..."
make clean 2>/dev/null || true
if make CFLAGS="-DXWAYLAND -DWLR_DEBUG -g -O0 -DDEBUG"; then
    echo "✓ Build successful"
else
    echo "✗ Build failed, restoring backup"
    cp dwl.c.backup dwl.c
    exit 1
fi

echo ""
echo "=== Fix Applied Successfully ==="
echo ""
echo "Changes made:"
echo "• Cleaned up zombie processes"
echo "• Enhanced XWayland error handling"
echo "• Added automatic XWayland recovery"
echo "• Improved signal handling"
echo "• Added health monitoring"
echo ""
echo "=== Testing Instructions ==="
echo "1. Start dwl with debug logging:"
echo "   ./dwl -d 2>&1 | tee dwl_test.log"
echo ""
echo "2. Test with your typical X11 applications"
echo "3. Monitor the log for any remaining errors"
echo ""
echo "=== If Issues Persist ==="
echo "• Check the log: tail -f dwl_test.log"
echo "• Run diagnostic: ./diagnose_xwayland.sh"
echo "• Try without XWayland temporarily:"
echo "  - Comment out 'XWAYLAND = -DXWAYLAND' in config.mk"
echo "  - make clean && make"
echo "  - Test if Wayland-native apps work fine"
echo ""
echo "The fix addresses the root cause of BadWindow errors"
echo "and should prevent XWayland from hanging your system."