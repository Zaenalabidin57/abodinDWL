#!/bin/sh
waypaper --restore &
waybar -c ./waybar/config &

wl-paste --watch cliphist store &

/usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1 &

mako &

while type dwl >/dev/null; do dwl && continue || break; done

