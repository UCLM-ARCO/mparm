/***************************************************

 file ICN created by STBusGen

 Author : Carlo Spitale
 CMG Design FMVG STMicroelectronics

****************************************************/

#include "SC_NODE.h"
#include "Node_1-32x32full.h"

SC_MODULE(full_32x32ICN)
{

	/*** clock & reset port definitions ***/
	sc_in_clk ClockGen_1;
	sc_in<bool> ResetGen_1;

	/*** port definitions ***/
	stbus_T2T3_port< uint, uint, uchar> In_1;
	stbus_T2T3_port< uint, uint, uchar> In_2;
	stbus_T2T3_port< uint, uint, uchar> In_3;
	stbus_T2T3_port< uint, uint, uchar> In_4;
	stbus_T2T3_port< uint, uint, uchar> In_5;
	stbus_T2T3_port< uint, uint, uchar> In_6;
	stbus_T2T3_port< uint, uint, uchar> In_7;
	stbus_T2T3_port< uint, uint, uchar> In_8;
	stbus_T2T3_port< uint, uint, uchar> In_9;
	stbus_T2T3_port< uint, uint, uchar> In_10;
	stbus_T2T3_port< uint, uint, uchar> In_11;
	stbus_T2T3_port< uint, uint, uchar> In_12;
	stbus_T2T3_port< uint, uint, uchar> In_13;
	stbus_T2T3_port< uint, uint, uchar> In_14;
	stbus_T2T3_port< uint, uint, uchar> In_15;
	stbus_T2T3_port< uint, uint, uchar> In_16;
	stbus_T2T3_port< uint, uint, uchar> In_17;
	stbus_T2T3_port< uint, uint, uchar> In_18;
	stbus_T2T3_port< uint, uint, uchar> In_19;
	stbus_T2T3_port< uint, uint, uchar> In_20;
	stbus_T2T3_port< uint, uint, uchar> In_21;
	stbus_T2T3_port< uint, uint, uchar> In_22;
	stbus_T2T3_port< uint, uint, uchar> In_23;
	stbus_T2T3_port< uint, uint, uchar> In_24;
	stbus_T2T3_port< uint, uint, uchar> In_25;
	stbus_T2T3_port< uint, uint, uchar> In_26;
	stbus_T2T3_port< uint, uint, uchar> In_27;
	stbus_T2T3_port< uint, uint, uchar> In_28;
	stbus_T2T3_port< uint, uint, uchar> In_29;
	stbus_T2T3_port< uint, uint, uchar> In_30;
	stbus_T2T3_port< uint, uint, uchar> In_31;
	stbus_T2T3_port< uint, uint, uchar> In_32;
	stbus_T2T3_port< uint, uint, uchar> Out_1;
	stbus_T2T3_port< uint, uint, uchar> Out_2;
	stbus_T2T3_port< uint, uint, uchar> Out_3;
	stbus_T2T3_port< uint, uint, uchar> Out_4;
	stbus_T2T3_port< uint, uint, uchar> Out_5;
	stbus_T2T3_port< uint, uint, uchar> Out_6;
	stbus_T2T3_port< uint, uint, uchar> Out_7;
	stbus_T2T3_port< uint, uint, uchar> Out_8;
	stbus_T2T3_port< uint, uint, uchar> Out_9;
	stbus_T2T3_port< uint, uint, uchar> Out_10;
	stbus_T2T3_port< uint, uint, uchar> Out_11;
	stbus_T2T3_port< uint, uint, uchar> Out_12;
	stbus_T2T3_port< uint, uint, uchar> Out_13;
	stbus_T2T3_port< uint, uint, uchar> Out_14;
	stbus_T2T3_port< uint, uint, uchar> Out_15;
	stbus_T2T3_port< uint, uint, uchar> Out_16;
	stbus_T2T3_port< uint, uint, uchar> Out_17;
	stbus_T2T3_port< uint, uint, uchar> Out_18;
	stbus_T2T3_port< uint, uint, uchar> Out_19;
	stbus_T2T3_port< uint, uint, uchar> Out_20;
	stbus_T2T3_port< uint, uint, uchar> Out_21;
	stbus_T2T3_port< uint, uint, uchar> Out_22;
	stbus_T2T3_port< uint, uint, uchar> Out_23;
	stbus_T2T3_port< uint, uint, uchar> Out_24;
	stbus_T2T3_port< uint, uint, uchar> Out_25;
	stbus_T2T3_port< uint, uint, uchar> Out_26;
	stbus_T2T3_port< uint, uint, uchar> Out_27;
	stbus_T2T3_port< uint, uint, uchar> Out_28;
	stbus_T2T3_port< uint, uint, uchar> Out_29;
	stbus_T2T3_port< uint, uint, uchar> Out_30;
	stbus_T2T3_port< uint, uint, uchar> Out_31;
	stbus_T2T3_port< uint, uint, uchar> Out_32;

	/*** data link definitions ***/

	/*** sub-block definitions ***/
	Node_1_base_class_32x32full< uint, uint, uchar, 32, 32, 32, 32, 32 > *Node_1;

	SC_CTOR(full_32x32ICN)
	{

		/*** Node_1 instantiation ***/
		Node_1 = new Node_1_base_class_32x32full< uint, uint, uchar, 32, 32, 32, 32, 32 > 
		("Node_1", 1, SC_NS);
		Node_1->In[0]( In_1 );
		Node_1->In[1]( In_2 );
		Node_1->In[2]( In_3 );
		Node_1->In[3]( In_4 );
		Node_1->In[4]( In_5 );
		Node_1->In[5]( In_6 );
		Node_1->In[6]( In_7 );
		Node_1->In[7]( In_8 );
		Node_1->In[8]( In_9 );
		Node_1->In[9]( In_10 );
		Node_1->In[10]( In_11 );
		Node_1->In[11]( In_12 );
		Node_1->In[12]( In_13 );
		Node_1->In[13]( In_14 );
		Node_1->In[14]( In_15 );
		Node_1->In[15]( In_16 );
		Node_1->In[16]( In_17 );
		Node_1->In[17]( In_18 );
		Node_1->In[18]( In_19 );
		Node_1->In[19]( In_20 );
		Node_1->In[20]( In_21 );
		Node_1->In[21]( In_22 );
		Node_1->In[22]( In_23 );
		Node_1->In[23]( In_24 );
		Node_1->In[24]( In_25 );
		Node_1->In[25]( In_26 );
		Node_1->In[26]( In_27 );
		Node_1->In[27]( In_28 );
		Node_1->In[28]( In_29 );
		Node_1->In[29]( In_30 );
		Node_1->In[30]( In_31 );
		Node_1->In[31]( In_32 );
		Node_1->Out[0]( Out_1 );
		Node_1->Out[1]( Out_2 );
		Node_1->Out[2]( Out_3 );
		Node_1->Out[3]( Out_4 );
		Node_1->Out[4]( Out_5 );
		Node_1->Out[5]( Out_6 );
		Node_1->Out[6]( Out_7 );
		Node_1->Out[7]( Out_8 );
		Node_1->Out[8]( Out_9 );
		Node_1->Out[9]( Out_10 );
		Node_1->Out[10]( Out_11 );
		Node_1->Out[11]( Out_12 );
		Node_1->Out[12]( Out_13 );
		Node_1->Out[13]( Out_14 );
		Node_1->Out[14]( Out_15 );
		Node_1->Out[15]( Out_16 );
		Node_1->Out[16]( Out_17 );
		Node_1->Out[17]( Out_18 );
		Node_1->Out[18]( Out_19 );
		Node_1->Out[19]( Out_20 );
		Node_1->Out[20]( Out_21 );
		Node_1->Out[21]( Out_22 );
		Node_1->Out[22]( Out_23 );
		Node_1->Out[23]( Out_24 );
		Node_1->Out[24]( Out_25 );
		Node_1->Out[25]( Out_26 );
		Node_1->Out[26]( Out_27 );
		Node_1->Out[27]( Out_28 );
		Node_1->Out[28]( Out_29 );
		Node_1->Out[29]( Out_30 );
		Node_1->Out[30]( Out_31 );
		Node_1->Out[31]( Out_32 );
		Node_1->clk(ClockGen_1);
		Node_1->rst(ResetGen_1);
	}
};
