#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <string.h>
#include "BufferedFileReader.h"

 BufferedFileReader::BufferedFileReader(const string& fname, const string& delims){
   
   fd_ = open(fname.c_str(), O_RDONLY);
    if(fd_ == -1){
      perror("open failed");
      good_= false;
      return;
    }
    // good_=true;
    delims_ = delims;
    curr_index_=0;
    curr_length_=0;
    fillCount = 0;
    fill_buffer(); 
 }

  BufferedFileReader::~BufferedFileReader(){
    close(fd_);
    good_=false;
  }

  void BufferedFileReader::open_file(const string& fname){
    // check if file is opened
    if (fd_ != -1){ 
      close_file(); // close file if it's opened
    }
      
    fd_ = open(fname.c_str(), O_RDONLY);
      if(fd_ == -1){
        good_=false;
        perror("open failed");
        return;
    }
      good_=true;
      fill_buffer();
  }

  void BufferedFileReader::close_file(){
      if(fd_ != -1){ 
        good_=false;
        fillCount=0;
        curr_index_=-1;
        curr_length_=-1;
        close(fd_);  
      } 
  }


  char BufferedFileReader::get_char(){

    char ret = EOF;

   if(curr_index_ == BUF_SIZE){
      good_=false;
      fill_buffer();
    }
    
    if(good_){    
      ret = buffer_[curr_index_];   // return curr_index in buffer
      if(ret == -1){
        good_=false;
        return EOF;
      }
      curr_index_++;              // increment curr_index_ for each get_char() call
    } 
    return ret;
  }

  
  string BufferedFileReader::get_token() {

    string ret="";
    char c = get_char();

    while(!is_delim(c)){
      if (c == -1){
        break;
      }
      ret += c; 
      c = get_char();
    }
    return ret;
  }

  string* BufferedFileReader::get_line(int* len){

    std::vector<string>str; 
    *len = 1; // initialize length
    string token;

    if(!good_){
      return nullptr;
    }

    char c = get_char();

    if(c=='\n' || buffer_[curr_index_]==EOF){ //considering case where we are at eof
      string* strArr = new string[1];
      strArr[0]="";
      return strArr;
    }

    while(!is_delim('\n')){      
        delims_+="\n"; // add line breaker to delims if it is not there
    }
   
    while(c != EOF){
      curr_index_--;  // make sure we are not missing next token - go back an index
      token=get_token();
      str.push_back(token);  // push token to output at the end 

      if(buffer_[curr_index_-1] =='\n'){ //if we find a line breaker then break
        break;
      }
      (*len)++;                     //increment len for each str 
      c = get_char();              //re-loop
    }

    string* strArr = new string[str.size()]; // resulting array is all string output 
    std::copy(str.begin(), str.end(), strArr); // push all strings into array
    return strArr;
  }

  int BufferedFileReader::tell(){
    return (fillCount-1) * BUF_SIZE + curr_index_; // exclude first fill count from constructor
  }

  void BufferedFileReader::rewind(){
      if(fd_ == -1){ // if file is closed
        good_=false;
        return;
      }   
    lseek(fd_, 0, SEEK_SET);   //reset to index 0 of file 
    fillCount=0;
    fill_buffer();
    curr_length_ = 0;
    curr_index_ = 0;
  }


  bool BufferedFileReader::good(){
    return good_;
  }

// pass in all character/string (find) delim_find
  bool BufferedFileReader::is_delim(char c){

    std::size_t found = delims_.find(c);

    if(found!=std::string::npos){
      return true;
    } return false;
  }

  void BufferedFileReader::fill_buffer(){

    int result; 
    int bytes_left = BUF_SIZE;

       while(bytes_left>0){
        result = read(fd_, buffer_ + BUF_SIZE - bytes_left, bytes_left);
        if(result==0){                                        
            good_=false;
            break;
        }
        if(result== -1){
          if(errno != EINTR){
              good_=false;
              return;
            } continue;
        }  
            good_=true;
            bytes_left -= result;
      } 
       curr_length_=BUF_SIZE-bytes_left;     // keep track of length read

       if(curr_length_ != BUF_SIZE){
         buffer_[curr_length_]= -1;           // marker for end of buffer read
       }
       curr_index_=0;                       // resets current index
       if(curr_length_ == 0){               // nothing read
        good_=false;
        return;
       }
       good_=true;
       fillCount++;                       // increment our fillcount for tell method
  }

