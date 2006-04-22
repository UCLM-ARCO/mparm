#ifndef __DW_AHB_TL_Master_h
#define __DW_AHB_TL_Master_h


#include <systemc.h>
#include <ccss_systemc.h>

#include "ahb_types.h"
#include "ahb_master_port.h"
#include "ahb_direct_if.h"

// the random number generator
class MersenneTwister;

#include "ahb_dw_misc.h"

typedef enum {
	STIM_IDLE,
	STIM_BUSY,
	STIM_REQUEST,
	STIM_TRANSACTION,
	STIM_SUSPEND
} stim_state;

struct burst_buffer;
struct burst_control;

#define MAX_QUEUED_TRANSFERS 6
#ifdef CCSS_USE_SC_CTOR
#undef CCSS_USE_SC_CTOR
#endif

#define CCSS_INIT_MEMBERS 

// The DW_AHB_TL_Master module is a traffic generator for the AHB
// bus. It implements a generic programmable master FSM that
// can be used by AHB_Master modules. The module process is
// sensitive to the positive edge of an external clock connected
// through the clock signal reference constructor argument.
// 
// The constructor requires the following arguments:
// 
// - sc_module_name, instance name of the module
// 
// - sc_in<bool>&, clock signal reference
// 
// - sc_in<bool>&, reset signal reference
// 
// - bool = true, memory encoding (little-endian default)
// 
// 
// Note: The programmable master FSM can not be instantiated directly
// in top level models.
// 
// 
// AHB Master Command Reference
// 
// This section contains a listing of all AHB Master commands. For
// more details see the DesignWare AHB Verification IP Databook.
// 
// The master commands must be called from a SystemC thread process,
// since some of the calls are blocking.
// 
// idle
// ----
// 
// Hold the bus idle for a specified number of clock cycles.
// 
//   Syntax:
// 
//     idle(int streamID,
//          int xfer_attr,
//          int num_cycles)
// 
//   Arguments:
// 
//     streamID:
// 	(unused)
// 
//     xfer_attr:
// 	An integer containing the transfer attributes for this command.
// 
//     num_cycles:
//     An integer number of clock cycles.
// 
//   Description:
// 
//     The idle command holds the bus i the idle state for the specified
//     number of clock cycles.
// 
// read
// ----
// 
// Initiates a non-sequential read cycle.
// 
//   Syntax:
// 
//     read(int streamID,
//          ahb_addr_t addr,
//          int xfer_attr,
//          int& handle);
// 
//   Arguments:
//     streamID:
// 	(unused)
// 
//     addr:
//     A slave device address.
// 
//     xfer_attr:
//     An integer containing the transfer attributes for this command.
// 
//     handle:
// 	An integer slave response handle where data is to be saved.
// 
//   Description:
// 
//     The read command performs a non-sequential read cycle. The
//     handle argument identifies the storage location. If handle is
//     VMT_IGNORE, the result is not saved for later retrieval.
// 
// read_expect
// -----------
// 
// Initiates a non-sequential read cycle, then compares the result
// against
// 
//   Syntax:
// 
//     read_expect(int streamID,
//                 ahb_addr_t addr,
//                 ahb_data_t exp_data,
//                 int xfer_attr);
// 
//   Arguments:
// 
//     streamID:
// 	(unused)
// 
//     addr:
//     A slave device address.
// 
//     exp_data:
//     An expected data value for comparison.
// 
//     xfer_attr:
//     An integer containing the transfer attributes for this command.
// 
//   Description:
// 
//     The read_expect command reads one word of result data at the
//     specified address and places the data in a read buffer. The
//     command compares the result data with the specified expected
//     data value. If the values do not match, read_expect issues an
//     error message.
// 
// read_burst
// ----------
// 
// Initiates a burst read cycle.
// 
//   Syntax:
// 
//     read_burst(int streamID,
//                ahb_addr_t addr,
//                int num_beats,
//                int burst_buffer,
//                int& handle);
// 
//   Arguments:
//     streamID
// 	(unused)
// 
//     addr:
//     A slave device start address.
// 
//     num_beats:
// 	An integer burst length.
// 
//     burst_buffer:
//     An integer identifier of the burst buffer to read.
// 
//     handle:
//     A returned integer identifier of the burst storage location.
// 
//   Description:
// 
//     The read_burst command performs a burst read transfer using the busy
//     and byte strobe information stored in the specified burst_buffer.
// 
// 	The num_beats specified must be less than or equal to the number of
//     words stored in the specified burst_buffer.
// 
// 	To get the slave response information for the specified burst_buffer
//     and handle, use the get_burst_result command. If handle is set to
//     VMT_IGNORE, read results are not saved for later retrieval.
// 
// read_burst_expect
// -----------------
// 
// Initiates a burst read cycle, then compares the result against an
// expected value.
// 
//   Syntax:
// 
//     read_burst_expect(int streamID,
//                       ahb_addr_t addr,
//                       int num_beats,
//                       int burst_buffer);
// 
//   Arguments:
// 
//     streamID:
// 	(unused)
// 
//     addr:
// 	A slave device starting address.
// 
//     num_beats:
// 	An integer bust length.
// 
//     burst_buffer:
// 	An integer identifier for the burst buffer to read.
// 
//   Description:
// 
//     The read_burst_expect command performs a read burst transfer,
// 	then checks the incoming data against information from a specified
// 	comparison burst buffer. The command uses busy and byte strobe
// 	information stored in burst_buffer. If results do not match, the
// 	command issues an error.
// 
// write
// -----
// 
// Initiates a non-sequential write cycle.
// 
//   Syntax:
// 
//     write(int streamID,
//           ahb_addr_t addr,
//           ahb_data_t hdata,
//           int xfer_attr,
//           int& handle);
// 
//   Arguments:
// 
//     streamID:
// 	(unused)
// 
//     addr:
// 	A slave device address.
// 
//     hdata:
// 	A data value.
// 
//     xfer_attr:
// 	An integer containing the transfer attributes for this command.
// 
//     handle:
// 	A returned integer identifier of the response storage location.
// 
//   Description:
// 
//     The write command performs a data write on the specified slave device.
// 
// write_burst
// -----------
// 
// Initiates a burst write cycle.
// 
//   Syntax:
// 
//     write_burst(int streamID,
//                 ahb_addr_t addr,
//                 int num_beats,
//                 int burst_buffer,
//                 int& handle);
// 
//   Arguments:
// 
//     streamID:
// 	(unused)
// 
//     addr:
// 	A slave device starting address.
// 
//     num_beats:
//     An integer burst length.
// 
//     burst_buffer:
//     An integer identifier of the burst buffer to write.
// 
//     handle:
//     A returned integer identifier of the burst storage location.
// 
//   Description:
// 
//     The write_burst command performs a burst write transfer using the
// 	busy and byte strobe information stored in the specified burst_buffer.
// 
// 	The num_beats specified must be less than or equal to the number of
// 	words currently stored in the specified burst_buffer.
// 
// 	To get slave response information for the specified burst_buffer
// 	and handle, use the get_burst_result command. If handle is set to
// 	VMT_IGNORE, slave response information is not saved for later retrieval.
// 
// new_burst_buffer
// ----------------
// 
// Creates a new burst buffer.
// 
//   Syntax:
// 
//     new_burst_buffer(int width,
//                      int pattern,
//                      unsigned int initial_value,
//                      int num_words,
//                      int& buffer_handle)
// 
//   Arguments:
// 
//     width:
//     An integer containing the burst buffer data width. The width
//     must be a power-of-2 number and less than or equal to the bus
//     data width.
// 
//     pattern:
//     An integer memory pattern. Following options are available:
//       0: Sets all bits in the region to 0
//       1: Sets all bits in the region to 1
//       2: Sets all bytes in the region to "a5"
//       3: Sets all bytes in the region to "5a"
//       4: Sets all words in the region to walking 0 pattern
//       5: Sets all words in the region to walking 1 pattern
//       6: Sets all words in the region to an incrementing pattern
//       7: Sets all words in the region to a decrementing pattern
//       7: Sets all words in the region to the initial_value constant argument
//       default: Sets all words in the region to random values. The random
//                source is initiated with the initial_value argument.
// 
//     initial_value:
//     A unsigned int value used for the incrementing, decrementing,
//     constant, and random value seed. 
// 
//     num_words:
//     Number of data words. This should be the same as the number of beats in
//     the desired burst.
// 
//     buffer_handle:
//     A returned integer buffer handle.
// 
//   Description:
// 
//     The new_burst_buffer command creates a burst buffer to hold the data
//     or expected data from a burst transfer. The returned buffer_handle
//     identifies the burst buffer. The burst buffer also contains the number
//     of busy cycles for each burst transfer with a default value of 0.
// 
// copy_burst_buffer
// -----------------
// 
// Creates a copy of an existing burst buffer.
// 
//   Syntax:
// 
//     copy_burst_buffer(int orig_burst_buffer,
//                       int& new_burst_buffer)
// 
//   Arguments:
//     orig_burst_buffer:
//     An integer identifier of the burst buffer to copy.
//     
//     new_burst_buffer:
//     A returned integer identifier of the burst buffer created by this
//     command.
// 
//   Description:
// 
//     The copy_burst_buffer command copies an existing burst buffer, creates
//     a new burst buffer, and returns its handle.
// 
// delete_buffer
// -------------
// 
// Deletes an existing response or burst buffer.
// 
//   Syntax:
// 
//     delete_buffer(int buffer_handle)
// 
//   Arguments:
//     buffer_handle:
//     An integer buffer identifier.
// 
//   Description:
// 
//     The delete_buffer command deletes a response buffer or burst
//     buffer. Use this command to free up memory when a buffer is
//     no longer needed by a testbench. If the buffer_andle is set
//     to DW_VIP_AMBA_FLUSH_BUFFER, then all response buffers are deleted.
// 
// set_burst_buffer_xfer_attr
// --------------------------
// 
// Sets transfer attributes of a burst buffer.
// 
//   Syntax:
// 
//     set_burst_buffer_xfer_attr(int burst_buffer,
//                                int beat,
//                                int xfer_attr)
// 
//   Arguments:
// 
//     burst_buffer:
//     An integer identifier of the burst buffer to modify.
// 
//     beat:
//     (ignored)
// 
//     xfer_attr:
//     An integer containing the transfer attributes for this command.
//     For information on xfer_attr values, see the dw_ahb_misc.h file.
// 
//   Description:
// 
//     The set_burst_buffer_xfer_attr command sets the transfer attributes
//     for all beats of a burst. 
// 
// set_burst_buffer_busy
// ---------------------
// 
// Sets the number of busy cycles inserted into a burst buffer.
// 
//   Syntax:
// 
//     set_burst_buffer_busy(int buffer_handle,
//                           int beat,
//                           int num_busy_cycles)
// 
//   Arguments:
// 
//     buffer_handle:
//     An integer identifier of a burst buffer.
// 
//     beat:
//     An integer beat number where busy is appended; must be >= 1.
// 
//     num_busy_cycles:
//     An integer number of busy transfer (>=0) to insert after the
//     specified beat.
// 
//   Description:
// 
//     The set_burst_buffer_busy command sets the number of busy transfers
//     to be inserted after all beats or a specific beat of a burst. Set
//     beat to DW_VIP_AMBA_ALL_BEATS to set all beats to this busy value,
//     which overwrites all busy values previously set for specific beats.
// 
// set_burst_buffer_data
// ---------------------
// 
// Writes a specific location in a burst buffer.
// 
//   Syntax:
// 
//     set_burst_buffer_data(int bust_buffer,
//                           int beat,
//                           ahb_data_t data)
// 
//   Arguments:
// 
//     bust_buffer:
//     An integer identifier of the burst buffer to write.
// 
//     beat:
//     An integer beat number where data is set; must be >=1.
// 
//     data:
//     The data value to set.
// 
//   Description:
// 
//     The set_burst_buffer_data command sets data of a specified beat of
//     a specified burst buffer.
// 
// get_buffer_data
// ---------------
// 
// Gets read cycle data from a response buffer.
// 
//   Syntax:
// 
//     get_buffer_data(int buffer_handle,
//                     ahb_data_t data)
// 
//   Arguments:
// 
//     buffer_handle:
//     An integer buffer identifier.
// 
//     data:
//     A returned data value from a read transfer.
// 
//   Description:
// 
//     The get_buffer_data command gets data from a response buffer. The
//     response buffer data must have come from a read or read_expect command.
// 
// get_buffer_response
// -------------------
// 
//   Syntax:
// 
//     get_buffer_response(int buffer_handle,
//                         int response_type,
//                         int &response)
// 
//   Arguments:
// 
//     buffer_handle:
//     An integer buffer identifier.
// 
//     response_type:
//     An integer response type to get, see the dw_ahb_misc.h file.
// 
//     response:
//     A returned integer signal value.
// 
//   Description:
// 
//     The get_buffer_response command returns the slave's response to a read,
//     read_expect or write transfer command. The buffer_handle identifies the
//     buffer from which to get the response. Use the buffer_handle returned by
//     the get_result command.
// 
// get_burst_buffer_data
// ---------------------
// 
// Read a specific location in a burst buffer.
// 
//   Syntax:
// 
//     get_burst_buffer_data(int burst_buffer,
//                           int beat,
//                           ahb_data_t value)
// 
//   Arguments:
//     burst_buffer:
//     An integer identifier of the burst buffer to read.
// 
//     beat:
//     An integer beat number; must be >= 1.
// 
//     value:
//     A returned data value.
// 
//   Description:
// 
//     The bet_burst_buffer_data command gets the specified beat of data
//     from a burst buffer.
// 
// get_burst_buffer_num_words
// --------------------------
// 
// Returns the number of data words in the memory of a burst buffer.
// 
//   Syntax:
// 
//     get_burst_buffer_num_words(int burst_buffer,
//                                int& num_words)
// 
//   Arguments:
//     burst_buffer:
//     An integer identifier of the burst buffer to examine.
// 
//     num_words:
//     A returned integer containing the number of words in a buffer memory.
// 
//   Description:
// 
//     The get_burst_buffer_num_words command gets the specified number of
//     data words (num_words) in the memory of a burst buffer.
// 
// get_burst_buffer_response
// -------------------------
// 
// Returns the response of a specified slave to a burst data transfer.
// 
//   Syntax:
// 
//     get_burst_buffer_response(int burst_buffer,
//                               int beat,
//                               int response_type,
//                               int& response)
// 
//   Arguments:
//     burst_buffer:
//     An integer identifier of the burst buffer to read.
// 
//     beat:
//     An integer beat number; must be >= 1 and <= num_words.
// 
//     response_type:
//     An integer response type to get, see the dw_ahb_misc.h file.
// 
//     response:
//     A returned integer buffer response.
// 
//   Description:
// 
//     The get_burst_buffer_response command gets the slave response for
//     for the specified beat of a bursts buffer that was filled by a
//     read_burst command.
// 
// get_burst_buffer_width
// ----------------------
// 
//   Syntax:
// 
//     get_burst_buffer_width(int burst_buffer,
//                            int& width)
// 
//   Arguments:
// 
//     burst_buffer:
//     An integer identifier of the burst buffer to read.
// 
//     width:
//     A returned integer containing the burst buffer data width.
// 
//   Description:
// 
//     The get_burst_buffer_width command gets the data width of the
//     memory of the specified burst buffer.
// 
// get_burst_result
// ----------------
// 
// Retrieves burst read results and slave responses in burst buffer format.
// 
//   Syntax:
// 
//     get_burst_result(int streamID,
//                      int handle,
//                      int& burst_buffer)
// 
//   Arguments:
//     streamID:
//     (unused)
// 
//     handle:
//     An integer slave response handle used in a read_burst or
//     write_burst command.
// 
//     burst_buffer:
//     A returned integer identifier of the burst buffer.
// 
//   Description:
// 
//     The get_burst_result command gets read data and slave response
//     in burst buffer format.
// 
// get_result
// ----------
// 
// Retrieves slave response results from a specific read or write cycle.
// 
//   Syntax:
// 
//     get_result(int streamID,
//                int handle,
//                int& buffer_handle)
// 
//   Arguments:
//     streamID:
//     (unused)
// 
//     handle:
//     An integer slave response handle used in a read, read_expect,
//     or write command.
// 
//     buffer_handle:
//     A returned integer response buffer identifier.
// 
//   Description:
// 
//     The get_result command returns the handle of a response buffer
//     of a corresponding transfer command.
// 
// set_burst_buffer_strobe
// -----------------------
// 
// Writes a specific byte strobe in a burst buffer.
// 
//   Syntax:
// 
//     set_burst_buffer_strobe(int buffer_handle,
//                             int beat,
//                             const unsigned int* strobe)
// 
//   Arguments:
//     buffer_handle
//     Am integer buffer handle
// 
//     beat
//     An integer beat number where the byte strobe should be set for; must be >= 1.
// 
//     strobe
// 	Byte strobe information
// 
//   Description:
// 
//     The set_burst_buffer_strobe command sets byte strobe information
// 	to be inserted for all beats or for a specific beat of a burst. Set
//     beat to DW_VIP_AMBA_ALL_BEATS to set all beats to this byte strobe value,
//     which overwrites all values previously set for specific beats.
// 
template<class T_PORT = ahb_namespace::ahb_master_port  >
class DW_AHB_TL_Master// Generic Programmable AMBA TL Master
: public sc_module
{
public:

	SC_HAS_PROCESS(DW_AHB_TL_Master);

	// default constructor
	DW_AHB_TL_Master(sc_module_name name_,
	                 sc_in<bool>& hclk_,
	                 sc_in<bool>& hresetn_,
	                 bool big_endian_ = false);

	~DW_AHB_TL_Master();

	void
	register_master_port(T_PORT*);

	void
	idle(int streamID,
	     int xfer_attr,
	     int num_cycles);

	void
	read(int streamID,
	     ahb_namespace::ahb_addr_t addr,
	     int xfer_attr,
	     int& handle);

	void
	read_expect(int streamID,
	            ahb_namespace::ahb_addr_t addr,
	            ahb_namespace::ahb_data_t exp_data,
	            int xfer_attr);

	void
	read_burst(int streamID,
	           ahb_namespace::ahb_addr_t addr,
	           int num_beats,
	           int burst_buffer,
	           int& handle);

	void
	read_burst_expect(int streamID,
	                  ahb_namespace::ahb_addr_t addr,
	                  int num_beats,
	                  int burst_buffer);

	void
	write(int streamID,
	      ahb_namespace::ahb_addr_t addr,
	      ahb_namespace::ahb_data_t hdata,
	      int xfer_attr,
	      int& handle);

	void
	write_burst(int streamID,
	            ahb_namespace::ahb_addr_t addr,
	            int num_beats,
	            int burst_buffer,
	            int& handle);


	void
	new_burst_buffer(int width,
	                 int pattern,
	                 unsigned int initial_value,
	                 int num_words,
	                 int& buffer_handle);

	void
	copy_burst_buffer(int orig_burst_buffer,
	                  int& new_burst_buffer);

	void
	delete_buffer(int buffer_handle);


	void
	set_burst_buffer_xfer_attr(int burst_buffer,
	                           int beat,
	                           int xfer_attr);

	void
	set_burst_buffer_busy(int buffer_handle,
	                      int beat,
	                      int num_busy_cycles);

	void
	set_burst_buffer_data(int bust_buffer,
	                      int beat,
	                      ahb_namespace::ahb_data_t data);

	void
	get_buffer_data(int buffer_handle,
	                ahb_namespace::ahb_data_t data);

	void
	get_buffer_response(int buffer_handle,
	                    int response_type,
						int &response);

	void
	get_burst_buffer_data(int burst_buffer,
	                      int beat,
						  ahb_namespace::ahb_data_t value);

	void
	get_burst_buffer_num_words(int burst_buffer,
	                           int& num_words);

	void
	get_burst_buffer_response(int burst_buffer,
	                          int beat,
	                          int response_type,
							  int& response);

	void
	get_burst_buffer_width(int burst_buffer,
	                       int& width);

	void
	get_burst_result(int streamID,
	                 int handle,
	                 int& burst_buffer);

	void
	get_result(int streamID,
	           int handle,
	           int& burst_buffer);

	// ARM 11 Extension:
	void
	set_burst_buffer_strobe(int buffer_handle,
	                        int beat,
	                        const unsigned int* strobe);

	void
	add_queue(burst_control*);

	bool locking_enabled;

	// using blocking method at higher abstraction level

	bool
	blocking_read_expect(ahb_namespace::ahb_addr_t, // start address
	                     int,                       // burst length
	                     ahb_namespace::ahb_hburst, // burst mode
	                     ahb_namespace::ahb_hsize,  // hsize
			             int,                       // pattern
			             unsigned int);             // initial_value

	bool
	blocking_read(ahb_namespace::ahb_addr_t,        // start address
	              int,                              // burst length
	              ahb_namespace::ahb_hburst,        // burst mode
	              ahb_namespace::ahb_hsize);        // hsize

	bool
	blocking_write(ahb_namespace::ahb_addr_t,       // start address
	               int,                             // burst length
	               ahb_namespace::ahb_hburst,       // burst mode
	               ahb_namespace::ahb_hsize,        // hsize
			       int,                             // pattern
			       unsigned int);                   // initial_value
public:
    // parameters

    // initialize parameters
    virtual void InitParameters() {
    }

private:

	sc_in<bool>& hclk;
	sc_in<bool>& hresetn;

	T_PORT*      p;

	MersenneTwister* mt;

	burst_control* _control_queue[4];

	burst_control* _control_data;
	burst_control* _control_flush;

	sc_event       get_next;
	sc_event       activate;
	bool           sleep;

	// processes
	void
	main_action();
	int
	create_burst_buffer(int, bool);
	void
	delete_burst_buffer(int);

	// private methods

	bool           compare_burst(ahb_namespace::ahb_addr_t, unsigned int*, unsigned int*, int, ahb_namespace::ahb_hsize);
	bool           compare_beat(unsigned int, unsigned int*, unsigned int*, unsigned int*, int, int);
	void           set_next_transfer();
	void           set_request();
	void           set_lock();
	void           set_strobe(burst_control*);
	void           realloc_buffer(int);

	// control and buffer for queued transfers
	burst_control* _transfer[MAX_QUEUED_TRANSFERS];
	unsigned int*  tmp_buffer[MAX_QUEUED_TRANSFERS];
	unsigned int*  tmp_buffer_orig[MAX_QUEUED_TRANSFERS];
	int*           tmp_buffer_busy_cycles[MAX_QUEUED_TRANSFERS];
	unsigned int** tmp_buffer_byte_strobe[MAX_QUEUED_TRANSFERS];

	// and the lenght of the buffer
	int            tmp_buffer_length[MAX_QUEUED_TRANSFERS];

	int            _index;

	burst_buffer** _burst_buffer;
	int            _burst_index;
	int            _num_bbuffer;

	// private data members
	bool           _reset;
	bool           _busy;

	int            _data_width_byte;
	int            _data_width_word;

	bool           _big_endian;
	bool           _data_fail;

	unsigned int   _strobe_width_word;
	unsigned int   byte_strobe[4];

}; // end module DW_AHB_TL_Master
#undef CCSS_INIT_MEMBERS



#endif
