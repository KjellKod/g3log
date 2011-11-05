
/** ==========================================================================
* 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
* Please See readme or CMakeList.txt for building instructions */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <memory>

#include <cmath>
#include <ctime>
#include <cassert>

#include "backgrounder.h"



namespace {
void printPercentageLeft(const unsigned nbr_, unsigned & progress_, const unsigned max_){
  float percent = 100 * ((float)nbr_/max_);
  unsigned int rounded = ((int)percent/10)*10;
  if(rounded != progress_){
    std::cout << 100 - progress_ << " " << std::flush;
    progress_ = rounded;
    return;
  }
}

template<typename T>
void printProgress(const std::vector<T>& out_,const std::vector<T>& in_, const unsigned max_)
{
  std::cout << "\nLeft to Process [%]: ";
  unsigned progress = 100;
  do{
    unsigned pSize = in_.size();
    unsigned remaining = pSize - out_.size();
    printPercentageLeft(remaining, progress, max_);
  }while((out_.size() < max_) && progress <= 100);
}
}


void runStrWorkers(const int c_nbrItems)
{
  std::vector<std::string> saveToQ;
  std::vector<std::string> compareQ;
  const clock_t start = clock();
  {
    Backgrounder<std::string> worker(saveToQ);
    srand((unsigned)time(0));

    for(int idx=0; idx < c_nbrItems; ++idx)
    {
      unsigned random = rand();
      std::ostringstream oss;
      oss << random;
      compareQ.push_back(oss.str());
      worker.saveData(oss.str());
    }
    double pushTime = ((clock() - start)/(double)CLOCKS_PER_SEC);
    std::cout<<"Finished pushing #"<<c_nbrItems<<" jobs to bg worker";
    std::cout<<" in "<<pushTime<<" [s]"<< std::endl;

    printProgress(saveToQ, compareQ, c_nbrItems);
  } // Trigger Backgrounder to go out of scope
  double workTime = ((clock() - start)/(double)CLOCKS_PER_SEC);
  std::cout << "\nBackgrounder finished with processing jobs in ";
  std::cout <<workTime<<" [s]"<< std::endl;

  // just dummy to make sure that nothing was lost
  assert(std::equal(compareQ.begin(), compareQ.end(), saveToQ.begin()));
  assert(saveToQ.size() == c_nbrItems);
}

void runIntWorkers(const int c_nbrItems)
{
  std::vector<int> saveToQ;
  std::vector<int> compareQ;
  const clock_t start = clock();
  {
    Backgrounder<int> worker(saveToQ);
    srand((unsigned)time(0));

    // all except one is random, save space for "zero" after the
    // loop
    for(int idx=0; idx < c_nbrItems-1; ++idx)
    {
      unsigned random = rand();
      compareQ.push_back(random);
      worker.saveData(random);
    }
    // extra case for empty item
    compareQ.push_back(0);
    worker.saveData(0);


    double pushTime = ((clock() - start)/(double)CLOCKS_PER_SEC);
    std::cout<<"Finished pushing #"<<c_nbrItems<<" jobs to bg worker";
    std::cout<<" in "<<pushTime<<" [s]"<< std::endl;

    printProgress(saveToQ, compareQ, c_nbrItems);
  } // Trigger Backgrounder to go out of scope
  double workTime = ((clock() - start)/(double)CLOCKS_PER_SEC);
  std::cout << "\nBackgrounder finished with processing jobs in ";
  std::cout<<" in "<<workTime<<" [s]"<< std::endl;

  // just dummy to make sure that nothing was lost
  assert(std::equal(compareQ.begin(), compareQ.end(), saveToQ.begin()));
  assert(saveToQ.size() == c_nbrItems);
}



int main(int argc, char** argv)
{
  const int c_nbrItems = 100000;
  std::cout << c_nbrItems << " transactions of std::string/int" << std::endl;
  runStrWorkers(c_nbrItems);

  std::cout << "\n\n" << c_nbrItems << " transactions of int" << std::endl;
  runIntWorkers(c_nbrItems);

  return 0;
}
