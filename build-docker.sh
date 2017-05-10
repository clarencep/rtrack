#!/bin/sh 

echo "=============== Building the docker image. ==============="
docker build --no-cache -t clarencep/lap56:rtrack .
if [ "$?" != "0" ]; then
    exit 1;
fi;

echo "=============== Run and do simple test. ==============="
cat rtrack.php | docker run -i --rm --name lap56-rtrack clarencep/lap56:rtrack php
docker run --rm --name lap56-rtrack clarencep/lap56:rtrack sh -c 'php -m | grep rtrack && echo OK. || echo Error: rtrack not loaded.'
# docker run --rm -it --name lap56-rtrack clarencep/lap56 php -r 

