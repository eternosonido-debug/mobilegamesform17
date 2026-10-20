# Supports both runtime sets present on this SD; no firmware files are replaced.
case "${LD_LIBRARY_PATH:-}" in
  */simulator/3168/lib_3168*) FMG_ABI=gnueabi ;;
  *)
    if [ -f /lib/ld-linux-armhf.so.3 ]; then
      FMG_ABI=armhf
    elif [ -f /lib/ld-linux.so.3 ]; then
      FMG_ABI=gnueabi
    else
      printf '%s\n' 'No supported ARM dynamic loader found' >> "$BASE/run.log"
      return 1
    fi
    ;;
esac
if [ "$FMG_ABI" = gnueabi ]; then
  export LD_LIBRARY_PATH="/sdcard/simulator/3168/lib_3168:/usr/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
  export FMG_BUTTON_A=6 FMG_BUTTON_B=11 FMG_BUTTON_X=9 FMG_BUTTON_Y=4
  export FMG_BUTTON_START=5 FMG_BUTTON_SELECT=12
else
  export LD_LIBRARY_PATH="/sdcard/lib:/usr/lib:/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
  export FMG_BUTTON_A=0 FMG_BUTTON_B=1 FMG_BUTTON_X=2 FMG_BUTTON_Y=3
  export FMG_BUTTON_START=9 FMG_BUTTON_SELECT=8
fi
export FMG_EXECUTABLE="$BASE/bin/fake-mobile-games-$FMG_ABI"
printf 'FMG launch: ABI=%s executable=%s\n' "$FMG_ABI" "$FMG_EXECUTABLE" >> "$BASE/run.log"
if [ -r /etc/version ]; then cat /etc/version >> "$BASE/run.log"; fi
export SDL_GAMECONTROLLERCONFIG='19001e5a010000001200000000010000,i2c-keys,a:b6,b:b11,x:b9,y:b4,back:b12,start:b5,dpup:b13,dpdown:b16,dpleft:b14,dpright:b15,platform:Linux,
1900d410010000000100000000010000,rk29-keypad,a:b4,b:b7,x:b6,y:b2,back:b8,start:b3,dpup:b9,dpdown:b12,dpleft:b10,dpright:b11,platform:Linux,
03000000100800000100000010010000,USB Gamepad,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftx:a0,lefty:a1,platform:Linux,'
export FMG_KEY_A='A' FMG_KEY_B='B' FMG_KEY_X='X' FMG_KEY_Y='Y'
export FMG_KEY_START='Return' FMG_KEY_SELECT='C'
# Escape and Right Shift also exit. All physical mappings still need console testing.
