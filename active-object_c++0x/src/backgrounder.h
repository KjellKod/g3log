/** ==========================================================================
* 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
*
* Example of a Background worker that uses an Active object (by composition)
* to process jobs in the background.
* Calling the Background worker to do a job is an asynchronous call, returning 
* almost immediately. The Backgrounder will create a job and push it onto a 
* queue that is processed in FIFO order by the Active object. */

#include <vector>
#include <chrono>
#include <memory>

#include "active.h"


/// Silly test background worker that only receives dummy encapsuled data  and stores them in a vector
/// the worker eceives the "jobs" and sends them to a work queue to be asynchronously 
//    executed in FIFO order by the background thread.
template<typename T>
class Backgrounder {
private:    
  std::unique_ptr<kjellkod::Active> active;
  std::vector<T>& receivedQ;
  unsigned int c_processTimeUs; // to fake processing time, in microseconds

  // Container for faking some imporant stuff type instead of a dummy value
  // so that it 'makes sense' storing it in an unique_ptr
  struct Data {
    Data(T v_) :value(v_) {}
    const T value;
  };

  // bg processing, FAKING that each job takes a  few ms
  void bgStoreData(std::shared_ptr<Data> msg_){
    receivedQ.push_back(msg_->value);
    // fake processing time
    std::this_thread::sleep_for(std::chrono::microseconds(c_processTimeUs));
  }

public:
  explicit Backgrounder(std::vector<T>& saveQ_)
    : active(kjellkod::Active::createActive())
    , receivedQ(saveQ_)
    , c_processTimeUs(1){}

  virtual ~Backgrounder(){}

  // Asynchronous msg API, for sending jobs for bg thread processing
  void saveData(const T value_){
    using namespace kjellkod;
    std::shared_ptr<Data> ptrBg(new Data(value_));
    Callback func = std::bind(&Backgrounder::bgStoreData, this, ptrBg);
    active->send(func);
  }
};
