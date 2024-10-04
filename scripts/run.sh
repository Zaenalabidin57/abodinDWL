waypaper --restore &
waybar -c ./waybar/config -s ./waybar/style.css &

wl-paste --watch cliphist store &

exec-once = /usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1 &

mako &

while type dwl >/dev/null; do dwl && continue || break; done

