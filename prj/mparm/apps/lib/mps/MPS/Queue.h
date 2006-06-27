#include <MPS/Config.h>
#include <string.h>

/*

Data layout:

size | data

if size < 0 buffer is already released.

 */

namespace MPS {

  // Ring buffer queue
  class Queue {
  public:
    void write (void* data, int len)
    {
      void* dst = getWriteBuf(len);
      memcpy(dst, data, len);
    }

    int read (void* data, int len)
    {
    }

    void* getWriteBuf(int len)
    {
    }

    void putWriteBuf(int len)
    {
    }
    
    int getReadBuf()
    {
    }

    
    
  private:
  };
  
}
