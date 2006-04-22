/***************************************************

 file low_power1ICN created by STBusGen

 Author : Carlo Spitale
 CMG Design FMVG STMicroelectronics

****************************************************/

#include "SC_NODE.h"
#include "Node_1.h"

SC_MODULE(low_power1ICN)
{

	/*** clock & reset port definitions ***/
	sc_in_clk ClockGen_1;
	sc_in<bool> ResetGen_1;

	/*** port definitions ***/
	stbus_T2T3_port< uint, uint, uchar> port__Node_1_In_0;
	stbus_T2T3_port< uint, uint, uchar> port__Node_1_In_1;
	stbus_T2T3_port< uint, uint, uchar> port__Node_1_Out_0;
	stbus_T2T3_port< uint, uint, uchar> port__Node_1_Out_1;
	stbus_T2T3_port< uint, uint, uchar> port__Node_1_Out_2;
	stbus_T2T3_port< uint, uint, uchar> port__Node_1_Out_3;

	/*** data link definitions ***/

	/*** sub-block definitions ***/
	Node_1_base_class< uint, uint, uchar, 32, 2, 4, 1, 1 > *Node_1;


	SC_CTOR(low_power1ICN)
	{

		/*** Node_1 instantiation ***/
		Node_1 = new Node_1_base_class< uint, uint, uchar, 32, 2, 4, 1, 1 > ("Node_1");
		Node_1->In[0]( port__Node_1_In_0 );
		Node_1->In[1]( port__Node_1_In_1 );
		Node_1->Out[0]( port__Node_1_Out_0 );
		Node_1->Out[1]( port__Node_1_Out_1 );
		Node_1->Out[2]( port__Node_1_Out_2 );
		Node_1->Out[3]( port__Node_1_Out_3 );
		Node_1->clk(ClockGen_1);
		Node_1->rst(ResetGen_1);
	}
};
