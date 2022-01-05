# 549 Final Project
Repository for the 549 Final Project.  
Will feature:  
1. Sequential/Parallel merge sort
2. Sequential/Parallel quick sort
3. Cache-aware multi-way merge sort
4. Sequential funnel sort
5. Parallel cache-oblivious sort

Analysis will try to compare their performance on speed and on cache misses/usage if possible.  
To use the docker image for testing:  
Windows (make sure docker container backend is on WSL):
```
> docker build . -t cilk
> docker run -v %cd%:/test -it cilk
$ clang -fcilkplus TestRunner.c -o test.exe -lm
$ ./test.exe
```
Linux:
```
$ docker build . -t cilk
$ docker run -v $(pwd):/test -it cilk
$ clang -fcilkplus TestRunner.c -o test.exe -lm
$ ./test.exe
```