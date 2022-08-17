# TinyWeb
C/C++的仿写轻型web服务器

Environment:

  Ubuntu 20.xx + mysql 8.0, VSCode
  
  default mysql info: user = root, passwd = root, dbname = mydb
  
  
  
Init:
  cd TinyWeb
  
  rm ./server
  
  make server
  
  sudo ./server 9006
  
Presure BenchMark
  sudo ./server 9006 -c 1
  
  cd ./test_presure/webbench-1.5
  
  ./webbench -c 6000 -t 5 http://127.0.0.1:9006/
  
  ![benchmark](https://user-images.githubusercontent.com/70505976/185051153-0e262abf-bbbd-4c28-bde8-d5d74f2bd317.png)

