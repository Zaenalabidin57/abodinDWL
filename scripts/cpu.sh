#!/bin/bash

# Function to extract the CPU frequency from /proc/cpuinfo
get_cpu_freq() {
  cpu_freq=grep "^[c]pu MHz" /proc/cpuinfo | head -n 1 | awk '{print $3}'
  printf "$cpu_freq"
}

# Main loop to continuously print the CPU frequency
freq=$(get_cpu_freq)
echo "$freq"
