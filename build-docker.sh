#!/bin/sh 

echo "=============== Building the docker image. ==============="
docker build -t clarencep/lap53:rtrack .
if [ "$?" != "0" ]; then
    exit 1;
fi;

echo "=============== Run and do simple test. ==============="
cat rtrack.php | docker run -i --rm --name lap53-rtrack clarencep/lap53:rtrack php
docker run --rm --name lap53-rtrack clarencep/lap53:rtrack sh -c 'php -m | grep rtrack && echo OK. || echo Error: rtrack not loaded.'
# docker run --rm -it --name lap53-rtrack clarencep/lap53 php -r 

