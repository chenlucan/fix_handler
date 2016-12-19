# fix_handler

Use docker to build quickfix dynamic libs （用docker编译quickfix动态库）
 - build docker image:  
    - docker build --rm -t local/quickfix -f Dockerfile-quickfix .
 - start container
    - docker run -v /[quickfix dir]:/quickfix:z --name build-quickfix -it local/quickfix /bin/bash
    - cd /quickfix
    - ./bootstrap
    - ./configure --prefix=/quickfix/.
    - make
    - make install 
 - move build libs into vendor/quickfix
    - mv quickfix/lib/libquickfix.so.16 vendor/lib/.
    - mv quickfix/lib/libquickfix.so.16.0.1 vendor/quickfix/lib/.
    - sudo mv quickfix/share/* vendor/quickfix/share/.
