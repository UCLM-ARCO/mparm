#include "axi_types.h"
using namespace axi_namespace;

#include "DW_AXI_SlaveAdapter.h"

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
// ----------------------------------------------------------------------------
template<int DataBusWidth>
DW_AXI_SlaveAdapter<DataBusWidth>::DW_AXI_SlaveAdapter(sc_module_name Name,
													   int ID)
	: sc_module       (Name)
    , AXI_MasterIFMoImpl<DataBusWidth>()
	, AREADY          ("AREADY")
	, RVALID          ("RVALID")
	, RLAST           ("RLAST")
	, RDATA           ("RDATA")
	, RRESP           ("RRESP")
	, RID             ("RID")
	, WREADY          ("WREADY")
	, BVALID          ("BVALID")
	, BRESP           ("BRESP")
	, BID             ("BID")
	, SlavePort       (ID)
	, R               (DataBusWidth/8, (DataBusWidth+31)/32)
	, m_ID            (ID)
{
	AXI_MasterIFMoImpl<DataBusWidth>::EnableMonitor(true);

	SC_METHOD(on_addrready);
	sensitive << AREADY;

	SC_METHOD(on_read);
	sensitive << RVALID << RLAST << RDATA << RRESP << RID;

	SC_METHOD(on_writeready);
	sensitive << WREADY;

	SC_METHOD(on_bresp);
	sensitive << BVALID << BRESP << BID;
}

// ----------------------------------------------------------------------------
// Register port metod
// ----------------------------------------------------------------------------
template<int DataBusWidth>
void DW_AXI_SlaveAdapter<DataBusWidth>::
register_port(sc_port_base& port, const char* if_typename)
{
	AXI_MasterIFMoImpl<DataBusWidth>::register_port(port, if_typename);
}

template<int DataBusWidth>
void DW_AXI_SlaveAdapter<DataBusWidth>::
on_addrready() {
	Ar.Aready = AREADY.read().to_int();

	SlavePort.PutChannel(Ar);
}

template<int DataBusWidth>
void DW_AXI_SlaveAdapter<DataBusWidth>::
on_read() {
	R.Rvalid = RVALID.read().to_int();
	R.Rlast  = RLAST.read().to_int();
	sc_bv2array<DataBusWidth>(RDATA.read(), R.Rdata);
	R.Rresp  = static_cast<axi_resp_t>(RRESP.read().to_int());
	R.RID    = RID.read().to_int();

	SlavePort.PutChannel(R);
}

template<int DataBusWidth>
void DW_AXI_SlaveAdapter<DataBusWidth>::
on_writeready(){
	Wr.Wready = WREADY.read().to_int();

	SlavePort.PutChannel(Wr);
}

template<int DataBusWidth>
void DW_AXI_SlaveAdapter<DataBusWidth>::
on_bresp() {
	B.Bvalid = BVALID.read().to_int();
	B.Bresp  = static_cast<axi_resp_t>(BRESP.read().to_int());
	B.BID    = BID.read().to_int();

	SlavePort.PutChannel(B);
}

template class DW_AXI_SlaveAdapter<   8>;
template class DW_AXI_SlaveAdapter<  16>;
template class DW_AXI_SlaveAdapter<  32>;
template class DW_AXI_SlaveAdapter<  64>;
template class DW_AXI_SlaveAdapter< 128>;
template class DW_AXI_SlaveAdapter< 256>;
template class DW_AXI_SlaveAdapter< 512>;
template class DW_AXI_SlaveAdapter<1024>;
