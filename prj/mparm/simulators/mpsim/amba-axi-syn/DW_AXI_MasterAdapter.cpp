#include "axi_types.h"
using namespace axi_namespace;

#include "axi_master_port.h"
#include "DW_AXI_MasterAdapter.h"

template <int data_width>
inline sc_bv<data_width>
array2sc_bv(const unsigned int* data) {
	sc_bv<data_width> tmp_data;

	for(int i=(data_width/32)-1; i>=0; --i) {
		tmp_data <<= 32;
		tmp_data |= data[i];
	}
	return tmp_data;
}

template <int data_width>
inline void
sc_bv2array(sc_bv<data_width> tmp_data, unsigned int* data) {
	for(int i=0; i<data_width/32; i++) {
		data[i] = tmp_data.to_uint();
		tmp_data = tmp_data>>32;
	}
}

// ----------------------------------------------------------------------------
// Constructor:
// ------------------------------x----------------------------------------------
template<int DataBusWidth>
DW_AXI_MasterAdapter<DataBusWidth>::
DW_AXI_MasterAdapter(sc_module_name name_,
					 int ID)
	: sc_module         (name_)
    , AXI_SlaveIFMoImpl<DataBusWidth> ()
	, AVALID            ("AVALID")
	, ADDR              ("ADDR")
	, AWRITE            ("AWRITE")
	, ALEN              ("ALEN")
	, ASIZE             ("ASIZE")
	, ABURST            ("ABURST")
	, ALOCK             ("ALOCK")
	, ACACHE            ("ACACHE")
	, APROT             ("APROT")
	, AID               ("AID")
	, RREADY            ("RREADY")
	, WVALID            ("WVALID")
	, WLAST             ("WLAST")
	, WDATA             ("WDATA")
	, WSTRB             ("WSTRB")
	, WID               ("WID")
	, BREADY            ("BREADY")
	, MasterPort        (ID)
	, W                 (DataBusWidth, (DataBusWidth+31)/32)
	, m_ID              (ID)
{
	AXI_SlaveIFMoImpl<DataBusWidth>::EnableMonitor(true);

	SC_METHOD(on_address);
	sensitive << AVALID << ADDR << AWRITE << ALEN << ASIZE << ABURST << ALOCK << ACACHE << APROT << AID;

	SC_METHOD(on_rready);
	sensitive << RREADY;

	SC_METHOD(on_write);
	sensitive << WVALID << WLAST << WDATA << WSTRB << WID;

	SC_METHOD(on_bready);
	sensitive << BREADY;
}

// ----------------------------------------------------------------------------
// Register port metod
// ----------------------------------------------------------------------------
template <int DataBusWidth>
void DW_AXI_MasterAdapter<DataBusWidth>::
register_port(sc_port_base& port, const char* if_typename)
{
	AXI_SlaveIFMoImpl<DataBusWidth>::register_port(port, if_typename);
}

template <int DataBusWidth>
void DW_AXI_MasterAdapter<DataBusWidth>::
on_address()
{
	A.Avalid = AVALID.read().to_int();
	A.Addr   = ADDR.read().to_uint();
	A.Awrite = AWRITE.read().to_int();
	A.Alen   = ALEN.read().to_int();
	A.Asize  = static_cast<axi_asize_t>(ASIZE.read().to_int());
	A.Aburst = static_cast<axi_aburst_t>(ABURST.read().to_int());
	A.Alock  = static_cast<axi_alock_t>(ALOCK.read().to_int());
	A.Acache = ACACHE.read().to_int();
	A.Aprot  = APROT.read().to_int();
	A.AID    = AID.read().to_int();

	MasterPort.PutChannel(A);
}

template <int DataBusWidth>
void DW_AXI_MasterAdapter<DataBusWidth>::
on_rready()
{
	Rr.Rready = RREADY.read().to_int();

	MasterPort.PutChannel(Rr);
}

template <int DataBusWidth>
void DW_AXI_MasterAdapter<DataBusWidth>::
on_write()
{
	// HP compiler crash workaround:
	enum { X = DataBusWidth/8 };

	W.Wvalid = WVALID.read().to_int();
	W.Wlast  = WLAST.read().to_int();
	sc_bv2array<DataBusWidth>(WDATA.read(), W.Wdata);
	sc_bv2array<X>(WSTRB.read(), W.Wstrb);
	W.WID    = WID.read().to_int();

	MasterPort.PutChannel(W);
}

template <int DataBusWidth>
void DW_AXI_MasterAdapter<DataBusWidth>::
on_bready()
{
	Br.Bready = BREADY.read().to_int();

	MasterPort.PutChannel(Br);
}

template class DW_AXI_MasterAdapter<   8>;
template class DW_AXI_MasterAdapter<  16>;
template class DW_AXI_MasterAdapter<  32>;
template class DW_AXI_MasterAdapter<  64>;
template class DW_AXI_MasterAdapter< 128>;
template class DW_AXI_MasterAdapter< 256>;
template class DW_AXI_MasterAdapter< 512>;
template class DW_AXI_MasterAdapter<1024>;
