#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "./SimpleFileReader.h"


  SimpleFileReader::SimpleFileReader(const string& fname){

    fd_ = open(fname.c_str(), O_RDONLY);
    if(fd_ == -1){
      good_= false;
      perror("open failed");
      exit(EXIT_FAILURE);
    }
    lseek(fd_, 0, SEEK_SET);
    good_=true;
  }


  SimpleFileReader::~SimpleFileReader(){
    close(fd_);
    good_=false;
  }


  void SimpleFileReader::open_file(const string& fname){
    // check if file is opened
    if (fd_ != -1){ 
      close(fd_); // close file if it's opened
    }
      
    fd_ = open(fname.c_str(), O_RDONLY);
      if(fd_ == -1){
      good_=false;
      perror("open failed");
      exit(EXIT_FAILURE);
    }
      lseek(fd_, 0, SEEK_SET);
      good_ = true;
  }
   // fd_!= -1 if file is opened 

  void SimpleFileReader::close_file(){
      if(fd_ != -1){ 
        good_=false;
        close(fd_);  
      } 
  }

  char SimpleFileReader::get_char(){

    char c;

    int num = read(fd_, &c, 1);  

    if(num == 0 || fd_==-1){
      good_ = false;
      return EOF;
    } 
    good_ = true;
    return c;
  }

  string SimpleFileReader::get_chars(size_t n){

      if(fd_ == -1){ // if file is closed
        good_=false;
        exit(1);
      }    

    char *buf = new char[n+1];
    int str;
    int bytes_left = n;

      while(bytes_left>0){
        str = read(fd_, buf + n - bytes_left, bytes_left);
        if(str==0){
            good_=false;
            break;
        }
        if(str == -1){
          if(errno != EINTR){
            good_=false;
            exit(EXIT_FAILURE);
            } continue;
        }  
            good_=true;
            bytes_left -= str;
      } 

    buf[n - bytes_left]='\0';
    string ret = buf;
    delete[] buf;
    return ret;
  }


  int SimpleFileReader::tell(){

    int position = lseek(fd_, 0, SEEK_CUR);
    return position;
  }


  void SimpleFileReader::rewind(){
      if(fd_ == -1){ // if file is closed
        good_=false;
        exit(1);
      }   
    lseek(fd_, 0, SEEK_SET);
    good_=true;
  }


  bool SimpleFileReader::good(){
    return good_;
  }