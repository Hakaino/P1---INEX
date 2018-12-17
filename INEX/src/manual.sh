sleep 1
pkill -f explore
echo "[Auto navigation Mode: off]"
sleep 5
roslaunch kobuki_keyop safe_keyop.launch --screen
echo "[Manual navigation Mode: on]"
