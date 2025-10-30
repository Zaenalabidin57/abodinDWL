#!/bin/bash

# XWayland diagnostic script for dwl
# Helps identify the root cause of XWayland hanging issues

echo "=== dwl XWayland Diagnostic Tool ==="
echo ""

# Check if dwl is running
if pgrep -x dwl > /dev/null; then
    echo "✓ dwl is currently running"
    DWL_PID=$(pgrep -x dwl)
    echo "  PID: $DWL_PID"
    
    # Check for XWayland process
    if pgrep -f "Xwayland" > /dev/null; then
        echo "✓ XWayland server is running"
        XWAYLAND_PID=$(pgrep -f "Xwayland")
        echo "  PID: $XWAYLAND_PID"
        
        # Check XWayland process health
        if kill -0 "$XWAYLAND_PID" 2>/dev/null; then
            echo "✓ XWayland process responds to signals"
        else
            echo "✗ XWayland process is NOT responding"
        fi
        
        # Check memory usage
        XWAYLAND_MEM=$(ps -p "$XWAYLAND_PID" -o rss= 2>/dev/null | tr -d ' ')
        if [ -n "$XWAYLAND_MEM" ]; then
            echo "  Memory usage: $((XWAYLAND_MEM / 1024)) MB"
        fi
    else
        echo "✗ XWayland server is NOT running"
    fi
    
    # Check for DISPLAY variable
    if [ -n "$DISPLAY" ]; then
        echo "✓ DISPLAY is set: $DISPLAY"
        
        # Test X11 connection
        if command -v xdpyinfo >/dev/null 2>&1; then
            if timeout 2 xdpyinfo >/dev/null 2>&1; then
                echo "✓ X11 connection is responsive"
            else
                echo "✗ X11 connection is NOT responsive"
            fi
        fi
    else
        echo "✗ DISPLAY is NOT set"
    fi
    
    echo ""
    echo "=== Recent X11 Errors ==="
    
    # Check for X11 errors in logs
    if [ -f "$HOME/.local/share/xorg/Xwayland.log" ]; then
        echo "Recent Xwayland log entries:"
        tail -20 "$HOME/.local/share/xorg/Xwayland.log" 2>/dev/null | grep -i "error\|warning\|badwindow" || echo "No recent errors found"
    fi
    
    if journalctl --version >/dev/null 2>&1; then
        echo ""
        echo "Recent journal entries for X11:"
        journalctl --user -t dwl --since "5 minutes ago" | grep -i "badwindow\|xcb\|x11" | tail -10 || echo "No recent journal errors"
    fi
    
else
    echo "✗ dwl is NOT running"
    echo ""
    echo "Start dwl with debug logging:"
    echo "  ./dwl -d 2>&1 | tee dwl_debug.log"
fi

echo ""
echo "=== Environment Check ==="

# Check Wayland display
if [ -n "$WAYLAND_DISPLAY" ]; then
    echo "✓ WAYLAND_DISPLAY: $WAYLAND_DISPLAY"
else
    echo "✗ WAYLAND_DISPLAY is NOT set"
fi

# Check XDG_RUNTIME_DIR
if [ -n "$XDG_RUNTIME_DIR" ]; then
    echo "✓ XDG_RUNTIME_DIR: $XDG_RUNTIME_DIR"
else
    echo "✗ XDG_RUNTIME_DIR is NOT set"
fi

# Check for common XWayland issues
echo ""
echo "=== Common Issues Check ==="

# Check for zombie processes
ZOMBIE_COUNT=$(ps aux | awk '$8 ~ /^Z/ { count++ } END { print count+0 }')
if [ "$ZOMBIE_COUNT" -gt 0 ]; then
    echo "⚠ Found $ZOMBIE_COUNT zombie processes (may cause hangs)"
else
    echo "✓ No zombie processes found"
fi

# Check for high memory usage
if command -v free >/dev/null; then
    MEM_USAGE=$(free | awk '/^Mem:/ {printf "%.0f", $3/$2 * 100.0}')
    if [ "$MEM_USAGE" -gt 90 ]; then
        echo "⚠ High memory usage: ${MEM_USAGE}% (may cause instability)"
    else
        echo "✓ Memory usage is normal: ${MEM_USAGE}%"
    fi
fi

# Check disk space in XDG_RUNTIME_DIR
if [ -n "$XDG_RUNTIME_DIR" ] && [ -d "$XDG_RUNTIME_DIR" ]; then
    DISK_USAGE=$(df "$XDG_RUNTIME_DIR" | awk 'NR==2 {print $5}' | sed 's/%//')
    if [ "$DISK_USAGE" -gt 95 ]; then
        echo "⚠ Low disk space in XDG_RUNTIME_DIR: ${DISK_USAGE}%"
    else
        echo "✓ Disk space is adequate: ${DISK_USAGE}%"
    fi
fi

echo ""
echo "=== Recommendations ==="
echo "1. If you see BadWindow errors:"
echo "   → Apply the XWayland stability patch: ./apply_xwayland_fix.sh"
echo ""
echo "2. If XWayland keeps dying:"
echo "   → Check system logs: journalctl --user -f -t dwl"
echo "   → Run with debug: ./dwl -d"
echo ""
echo "3. If display is unresponsive:"
echo "   → Try restarting dwl: pkill dwl && ./dwl"
echo "   → Check for stuck X11 apps: pkill -f Xwayland"
echo ""
echo "4. For immediate testing:"
echo "   → Disable XWayland temporarily: comment out XWAYLAND in config.mk"
echo "   → This will confirm if issue is XWayland-specific"