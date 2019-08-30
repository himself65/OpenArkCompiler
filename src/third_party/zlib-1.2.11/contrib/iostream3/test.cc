/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reverved.
 *
 * OpenArkCompiler is licensed under the Mulan PSL v1. 
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 * 	http://license.coscl.org.cn/MulanPSL 
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v1 for more details.  
 */

#include "zfstream.h"
#include <iostream>      // for cout

int main() {

  gzofstream outf;
  gzifstream inf;
  char buf[80];

  outf.open("test1.txt.gz");
  outf << "The quick brown fox sidestepped the lazy canine\n"
       << 1.3 << "\nPlan " << 9 << std::endl;
  outf.close();
  std::cout << "Wrote the following message to 'test1.txt.gz' (check with zcat or zless):\n"
            << "The quick brown fox sidestepped the lazy canine\n"
            << 1.3 << "\nPlan " << 9 << std::endl;

  std::cout << "\nReading 'test1.txt.gz' (buffered) produces:\n";
  inf.open("test1.txt.gz");
  while (inf.getline(buf,80,'\n')) {
    std::cout << buf << "\t(" << inf.rdbuf()->in_avail() << " chars left in buffer)\n";
  }
  inf.close();

  outf.rdbuf()->pubsetbuf(0,0);
  outf.open("test2.txt.gz");
  outf << setcompression(Z_NO_COMPRESSION)
       << "The quick brown fox sidestepped the lazy canine\n"
       << 1.3 << "\nPlan " << 9 << std::endl;
  outf.close();
  std::cout << "\nWrote the same message to 'test2.txt.gz' in uncompressed form";

  std::cout << "\nReading 'test2.txt.gz' (unbuffered) produces:\n";
  inf.rdbuf()->pubsetbuf(0,0);
  inf.open("test2.txt.gz");
  while (inf.getline(buf,80,'\n')) {
    std::cout << buf << "\t(" << inf.rdbuf()->in_avail() << " chars left in buffer)\n";
  }
  inf.close();

  return 0;

}
