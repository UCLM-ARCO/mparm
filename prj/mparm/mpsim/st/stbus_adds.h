#ifndef ____STBUS_ADDS_included____
#define ____STBUS_ADDS_included____


template <class datatype, class TADDRESS, class TBE>
struct stbus_T2T3_link_direct
{
    sc_signal<datatype> data;
    sc_signal<TADDRESS> add;
    sc_signal<bool> req;
    sc_signal<bool> eop;
    sc_signal<TBE> be;
    sc_signal<uchar> opc;
    sc_signal<bool> lck;
    sc_signal<uchar> src;
    sc_signal<uchar> tid;
    sc_signal<uchar> pri;
    sc_signal<bool> gnt;
};


template <class datatype, class TADDRESS, class TBE>
struct stbus_T2T3_link_response
{
    sc_signal<datatype> r_data;
    sc_signal<bool> r_req;
    sc_signal<bool> r_opc;
    sc_signal<bool> r_gnt;
    sc_signal<bool> r_eop;
    sc_signal<uchar> r_src;
    sc_signal<uchar> r_tid;
    sc_signal<uchar> r_pri;
    sc_signal<bool> r_lck;
};


template <class datatype, class TADDRESS, class TBE>
class stbus_T2T3_port_direct
{
public:
    sc_inout<datatype> data;
    sc_inout<TADDRESS> add;
    sc_inout<bool> req;
    sc_inout<bool> eop;
    sc_inout<TBE> be;
    sc_inout<uchar> opc;
    sc_inout<bool> lck;
    sc_inout<uchar> src;
    sc_inout<uchar> tid;
    sc_inout<uchar> pri;
    sc_inout<bool> gnt;

    void operator () ( stbus_T2T3_link_direct< datatype, TADDRESS, TBE > & stb_link )
    {
      data( stb_link.data );
      add( stb_link.add );
      req( stb_link.req );
      eop( stb_link.eop );
      be( stb_link.be );
      opc( stb_link.opc );
      lck( stb_link.lck );
      src( stb_link.src );
      tid( stb_link.tid );
      pri( stb_link.pri );
      gnt( stb_link.gnt );
    }
};

template <class datatype, class TADDRESS, class TBE>
class stbus_T2T3_port_response
{
public:
    sc_inout<datatype> r_data;
    sc_inout<bool> r_req;
    sc_inout<bool> r_opc;
    sc_inout<bool> r_gnt;
    sc_inout<bool> r_eop;
    sc_inout<uchar> r_src;
    sc_inout<uchar> r_tid;
    sc_inout<uchar> r_pri;
    sc_inout<bool> r_lck;

    void operator () ( stbus_T2T3_link_response< datatype, TADDRESS, TBE > & stb_link )
    {
      r_data( stb_link.r_data );
      r_req( stb_link.r_req );
      r_opc( stb_link.r_opc );
      r_gnt( stb_link.r_gnt );
      r_eop( stb_link.r_eop );
      r_src( stb_link.r_src );
      r_tid( stb_link.r_tid );
      r_pri( stb_link.r_pri );
      r_lck( stb_link.r_lck );
    }
};



template <class datatype, class TADDRESS, class TBE>
void sc_trace( sc_trace_file* tf, stbus_T2T3_link_direct< datatype, TADDRESS, TBE >& stb_link, char * name, unsigned int tflag = ALL_SIG )
{
  char n[255];
  if( tflag & TRACE_DATA )
  {
    sprintf(n,"%s_data", name);
    sc_trace( tf, stb_link.data, n );
  }
  if( tflag & TRACE_ADD )
  {
    sprintf(n,"%s_add", name);
    sc_trace( tf, stb_link.add, n );
  }
  if( tflag & TRACE_REQ )
  {
    sprintf(n,"%s_req", name);
    sc_trace( tf, stb_link.req, n );
  }
  if( tflag & TRACE_EOP )
  {
    sprintf(n,"%s_eop", name);
    sc_trace( tf, stb_link.eop, n );
  }
  if( tflag & TRACE_BE )
  {
    sprintf(n,"%s_be", name);
    sc_trace( tf, stb_link.be, n );
  }
  if( tflag & TRACE_OPC )
  {
    sprintf(n,"%s_opc", name);
    sc_trace( tf, stb_link.opc, n );
  }
    if( tflag & TRACE_LCK )
    {
      sprintf(n,"%s_lck", name);
      sc_trace( tf, stb_link.lck, n );
    }
    if( tflag & TRACE_SRC )
    {
      sprintf(n,"%s_src", name);
      sc_trace( tf, stb_link.src, n );
    }
    if( tflag & TRACE_TID )
    {
      sprintf(n,"%s_tid", name);
      sc_trace( tf, stb_link.tid, n );
    }
    if( tflag & TRACE_PRI )
    {
      sprintf(n,"%s_pri", name);
      sc_trace( tf, stb_link.pri, n );
    }
    if( tflag & TRACE_GNT )
    {
      sprintf(n,"%s_gnt", name);
      sc_trace( tf, stb_link.gnt, n );
    }
}


template <class datatype, class TADDRESS, class TBE>
void sc_trace( sc_trace_file* tf, stbus_T2T3_link_response< datatype, TADDRESS, TBE >& stb_link, char * name, unsigned int tflag = ALL_SIG )
{
  char n[255];
  if( tflag & TRACE_R_DATA )
  {
    sprintf(n,"%s_r_data", name);
    sc_trace( tf, stb_link.r_data, n );
  }
  if( tflag & TRACE_R_REQ )
  {
    sprintf(n,"%s_r_req", name);
    sc_trace( tf, stb_link.r_req, n );
  }
  if( tflag & TRACE_R_OPC )
  {
    sprintf(n,"%s_r_opc", name);
    sc_trace( tf, stb_link.r_opc, n );
  }
    if( tflag & TRACE_R_GNT )
    {
      sprintf(n,"%s_r_gnt", name);
      sc_trace( tf, stb_link.r_gnt, n );
    }
    if( tflag & TRACE_R_EOP )
    {
      sprintf(n,"%s_r_eop", name);
      sc_trace( tf, stb_link.r_eop, n );
    }
    if( tflag & TRACE_R_SRC )
    {
      sprintf(n,"%s_r_src", name);
      sc_trace( tf, stb_link.r_src, n );
    }
    if( tflag & TRACE_R_TID )
    {
      sprintf(n,"%s_r_tid", name);
      sc_trace( tf, stb_link.r_tid, n );
    }
    if( tflag & TRACE_R_PRI )
    {
      sprintf(n,"%s_r_pri", name);
      sc_trace( tf, stb_link.r_pri, n );
    }
    if( tflag & TRACE_R_LCK )
    {
      sprintf(n,"%s_r_lck", name);
      sc_trace( tf, stb_link.r_lck, n );
    }
}



#endif     /*  #ifndef ____STBUS_ADDS_included____  */

