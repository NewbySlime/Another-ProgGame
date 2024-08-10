#include "../../src/communication_header.h"
#include "stdio.h"

using namespace GameUtils::InterProcessCommunication;

int main(){
  header_struct _header;
  printf("check uninitialized header: %s\n", check_header(&_header)? "true": "false");

  initialize_header(&_header);
  printf("check initialized header: %s\n", check_header(&_header)? "true": "false");

  _header.commcode = 0x2010;
  _header.datalen = 0x3122;
  printf("header data: \n");

  uint8_t *_rawdata = object_as_rawdata(&_header);
  for(int i = 0; i < sizeof(header_struct); i++){
    printf("0x%X ", _rawdata[i]);
  }

  printf("\ncheck initialized header: %s\n", check_header(&_header)? "true": "false");
}