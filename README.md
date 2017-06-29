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
