/***************************************************

 file partial_8x8ICN created by STBusGen

 Author : Carlo Spitale
 CMG Design FMVG STMicroelectronics

****************************************************/

#include "SC_NODE.h"
#include "Node_1-8x8partial.h" 

SC_MODULE(partial_8x8ICN)
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
	stbus_T2T3_port< uint, uint, uchar> Out_1;
	stbus_T2T3_port< uint, uint, uchar> Out_2;
	stbus_T2T3_port< uint, uint, uchar> Out_3;
	stbus_T2T3_port< uint, uint, uchar> Out_4;
	stbus_T2T3_port< uint, uint, uchar> Out_5;
	stbus_T2T3_port< uint, uint, uchar> Out_6;
	stbus_T2T3_port< uint, uint, uchar> Out_7;
	stbus_T2T3_port< uint, uint, uchar> Out_8;

	/*** data link definitions ***/

	/*** sub-block definitions ***/
	Node_1_base_class_8x8partial< uint, uint, uchar, 32, 8, 8, 3, 2 > *Node_1;


	SC_CTOR(partial_8x8ICN)
	{
		
		Node_1 = new Node_1_base_class_8x8partial< uint, uint, uchar, 32, 8, 8, 3, 2 > 
		("Node_1", 1, SC_NS);

		/*** Node_1 instantiation ***/
		Node_1->In[0]( In_1 );
		Node_1->In[1]( In_2 );
		Node_1->In[2]( In_3 );
		Node_1->In[3]( In_4 );
		Node_1->In[4]( In_5 );
		Node_1->In[5]( In_6 );
		Node_1->In[6]( In_7 );
		Node_1->In[7]( In_8 );
		Node_1->Out[0]( Out_1 );
		Node_1->Out[1]( Out_2 );
		Node_1->Out[2]( Out_3 );
		Node_1->Out[3]( Out_4 );
		Node_1->Out[4]( Out_5 );
		Node_1->Out[5]( Out_6 );
		Node_1->Out[6]( Out_7 );
		Node_1->Out[7]( Out_8 );
		Node_1->clk(ClockGen_1);
		Node_1->rst(ResetGen_1);
	}
};
