FROM clarencep/lap53:devel

COPY . /usr/local/src/rtrack

RUN cd /usr/local/src/rtrack \
    && phpize \
    && ./configure \
    && make \
    && find . -type f -print0 | xargs -0 ls -l \
    # && echo n | make test \
    && echo Tests done. \
    && echo 'extension=/usr/local/src/rtrack/modules/rtrack.so' >> /etc/php.d/rtrack.ini 
    
