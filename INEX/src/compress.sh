tar -czf ~/ros_ws/src/INEX/photos.tar.gz ~/ros_ws/src/INEX/photos
tar -czf ~/ros_ws/src/INEX/anomalies.tar.gz ~/ros_ws/src/INEX/anomalies
chmod 777 ~/ros_ws/src/INEX/photos.tar.gz
chmod 777 ~/ros_ws/src/INEX/anomalies.tar.gz
find /home/dud/ros_ws/src/INEX/photos -type f -iname \*.png -delete
find /home/dud/ros_ws/src/INEX/anomalies -type f -iname \*.png -delete
