export JAVA_HOME=/opt/wsarch-env-client/jdk1.7.0
./bomberboys/experiments/client/jclient-100-trash/bin/jclient 10 tcp desktop-15 10011 &
pId=$!
sleep 90
kill -2 $pId
