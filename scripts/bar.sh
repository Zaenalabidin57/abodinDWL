#!/bin/dash

# ^c$var^ = fg color
# ^b$var^ = bg color

interval=0

# load colors
#. ~/.config/chadwm/scripts/bar_themes/dracula

cpu() {
  cpu_val=$(cat /proc/cpuinfo | grep 'cpu MHz' | awk '{printf("%.1f\n", $4 / 1000)}' | head -n 1)

  printf "CPU"
  printf " $cpu_val"
}

#pkg_updates() {
#  temperaturess=$(cat /sys/class/hwmon/hwmon5/temp1_input)
#
#  if [ temperaturess > 7000 ]; then
#    printf "^c$red^   $temperaturess"
#  else
#    printf "^c$blue^   $temperaturess"
# fi 
#}

#pkg_updates() {
   # #updates=$({ timeout 20 doas xbps-install -un 2>/dev/null || true; } | wc -l) # void
   # updates=$({ timeout 20 checkupdates 2>/dev/null || true; } | wc -l) # arch
   # # updates=$({ timeout 20 aptitude search '~U' 2>/dev/null || true; } | wc -l)  # apt (ubuntu, debian etc)
   #
   # if [ -z "$updates" ]; then
   #   printf "  ^c$green^    Fully Updated"
   # else
   #   printf "  ^c$green^    $updates"" updates"
   # fi
#}

battery() {
  get_capacity="$(cat /sys/class/power_supply/BAT0/capacity)"
  printf "  $get_capacity"
}

brightness() {
  printf "   "
  printf "%.0f\n" $(cat /sys/class/backlight/*/brightness)
}

mem() {
  printf "  "
  printf " $(free -h | awk '/^Mem/ { print $3 }' | sed s/i//g)"
}

wlan() {
	case "$(cat /sys/class/net/wl*/operstate 2>/dev/null)" in
	#up) printf "^c$black^ ^b$blue^ 󰤨 ^d^%s" " ^c$blue^Connected" ;;
	up) printf " 󰤨 ";;
	down) printf " 󰤭 " " Disconnected" ;;
	esac
}

temperature() {
  temperaturess=$(cat /sys/class/hwmon/hwmon5/temp1_input | sed 's/\(..\).*$/\1/')
  printf "   $temperaturess"
}
#  if [ temperaturess > 75 ]; then
#    printf "^c$black^ ^b$red^   $temperaturess"
#  else
#    printf "^c$black^ ^b$blue^   $temperaturess"
#  fi
#}

clock() {
	printf " 󱑆 "
	printf " $(date '+%H:%M')  "
}

while true; do

  sleep 1 && echo "$(temperature) $(battery) $(brightness) $(cpu) $(mem) $(wlan) $(clock)"
done
