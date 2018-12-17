tar -czf ~/ros_ws/src/INEX/photos.tar.gz ~/ros_ws/src/INEX/photos
tar -czf ~/ros_ws/src/INEX/anomalies.tar.gz ~/ros_ws/src/INEX/anomalies
chmod 777 ~/ros_ws/src/INEX/photos.tar.gz
chmod 777 ~/ros_ws/src/INEX/anomalies.tar.gz
#wizard@192.168.0.3:home/wizard/Documents/Livros/Escola/AAU/P1/Development
#ros@192.168.0.4:home/Desktop/INEX
#

rsync home/dud/ros_ws/src/INEX/photos.tar.gz wizard@192.168.0.3:home/wizard/Documents/Livros/Escola/AAU/P1/Development
rsync home/dud/ros_ws/src/INEX/anomalies.tar.gz wizard@192.168.0.3:home/wizard/Documents/Livros/Escola/AAU/P1/Development
rsync home/dud/ros_ws/src/INEX/map/mars_map.pgm wizard@192.168.0.3:home/wizard/Documents/Livros/Escola/AAU/P1/Development
