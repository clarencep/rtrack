#!/bin/sh


docker build -t clarencep/lap53:rtrack .

docker run --rm -it --name lap53-rtrack clarencep/lap53:rtrack sh -c 'php -m | grep rtrack && echo OK. || echo Error: rtrack not loaded.'
# docker run --rm -it --name lap53-rtrack clarencep/lap53 php -r 

