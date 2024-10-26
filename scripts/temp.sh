  temperaturess=$(cat /sys/class/hwmon/hwmon5/temp1_input | sed 's/\(..\).*$/\1/')
  echo "  $temperaturess"

  if [ $temperaturess -gt 70 ]; then
    echo "one"
  else
    echo "two"
  fi
  get_capacity="$(cat /sys/class/power_supply/BAT0/capacity)"
  if [ "$get_capacity" -eq 100 ]; then
    printf "ballsack"
  else
    printf "^c$blue^   $get_capacity"
  fi



