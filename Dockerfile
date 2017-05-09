FROM clarencep/lap53


RUN yum install -y wget gcc automake make php-devel 

COPY . /usr/local/src/rtrack

RUN cd /usr/local/src/rtrack \
    && phpize \
    && ./configure \
    && make \
    && make test \
    && echo 'extension=/usr/local/src/rtrack/modules/rtrack.so' >> /etc/php.d/rtrack.ini 
    
