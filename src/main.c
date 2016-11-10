#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */


/*********IQ DEFINITIONS**********/
#define IQSIGN(a)  (a&0x80000000)
#define IQMINUS(a) a=a|0x80000000
#define IQPLUS(a)  a=a&0x7FFFFFFF

/*********BIT MANIPULATION DEFINITIONS**********/
#define H32(a)  (a>>16)
#define L32(a)   (a&0x0000FFFF)
typedef struct IQ{
	unsigned long val;
    char Q;
}IQ;

void IQminus(IQ *op)
{
	op->val=~op->val;
	op->val++;

}

void floatToIQ(float *fltn,IQ *fpn,unsigned char Q)
{
	float tmp=*fltn;
	unsigned long pow=0x01;
	pow=pow<<Q;
	if(*fltn<0.000001)
	{
	  tmp=tmp*-1;
	  fpn->Q=Q;
	  fpn->val=(unsigned long)(tmp*pow);
	  IQminus(fpn);
	}
	else
	{
      fpn->Q=Q;
	  fpn->val=(unsigned long)(tmp*pow);
	}
}



//Test functions

void delay_8ms(int ms)
{
	int i=0;
		
	for(i;i<ms;i++){
		SRTISC=0x41;
		while(!SRTISC_RTIF)
		  __RESET_WATCHDOG();
	} 
		SRTISC=0x00;	
}
typedef union
{
	unsigned long lval;
	unsigned char cval[4];
}ul;

void send_char(unsigned char data)
{
	while(SCI1S1_TDRE == 0){__RESET_WATCHDOG();}
	SCI1D=data;
}

unsigned char receive_char()
{
	unsigned char temp;
    while(SCI1S1_RDRF == 0){__RESET_WATCHDOG();}
       temp=SCI1D;
   return temp;
}




//This function tells if the sum of two 32 byte numbers gives 32 integer overflow (a carry is generated). It assumes
// that both d1 and d2 are positive numbers

unsigned long get32msb(unsigned long h32,unsigned long l32,unsigned char Q,unsigned char P)
{
	int I=(int)P-31+(int)Q;
	
	if(I==0)
		return h32>>1;
	if(I<32)   //Integer MSB is in h32 
	{
		if(I<0)
		{
			h32=h32>>((I-1)*-1);
			return h32;
		}
		h32=h32<<(I-1);
		l32=l32>>(33-I);
		h32+=l32;
		return h32;
	}
	else   //Integer MSB is in l32 
	{
	    if(64>(P+Q))
	    {
	       l32=l32>>(64-(P+Q));
	       h32=h32<<((P+Q)-32);
	       return h32+l32;
	    }
	     
	    else
		 l32=l32<<(P+Q)-64;
		return l32;
	}

}
unsigned long carry_32(unsigned long d1,unsigned long d2)
{
	
	 if(d1>(0xFFFFFFFF-d2))
		 return 1;
	 
	 return 0;
}

void get_MS32_mul32(unsigned long op1,unsigned long op2,unsigned long *lr,unsigned long *ur)
{
	unsigned long u,m1,m2,l,r;
	u=H32(op1)*H32(op2); 
	m1=H32(op1)*L32(op2);
	m2=L32(op1)*H32(op2);
	l=L32(op1)*L32(op2);
	*lr=l+(m1<<16)+(m2<<16);
	*ur=u+(m1>>16)+(m2>>16)+carry_32(l,(m1<<16))+carry_32(l+(m1<<16),(m2<<16));
}



void mulIQ(IQ op1,IQ op2,IQ *res,unsigned char Qres)
{
	unsigned long lr,ur,dbg;
	dbg=IQSIGN(op1.val);
	dbg=IQSIGN(op2.val);
	if(IQSIGN(op1.val)==IQSIGN(op2.val))
	{
       if(IQSIGN(op1.val))  //both negatives
       {
    	   IQminus(&op1);
    	   IQminus(&op2);
   		   get_MS32_mul32(op1.val,op2.val,&lr,&ur);
   		   res->val=get32msb(ur,lr,Qres,64-(op1.Q+op2.Q));
       }
       else
       {
   		get_MS32_mul32(op1.val,op2.val,&lr,&ur);
   		res->val=get32msb(ur,lr,Qres,64-(op1.Q+op2.Q));
       }

	}
	else
	{ 
	      if(IQSIGN(op1.val))  //op1 is the negative number
	       {
	    	   IQminus(&op1);
	   		   get_MS32_mul32(op1.val,op2.val,&lr,&ur);
	   		   res->val=get32msb(ur,lr,Qres,64-(op1.Q+op2.Q));
	   		   IQminus(res);
	       }	
	      else
	      {
	    	   IQminus(&op2);
	   		   get_MS32_mul32(op1.val,op2.val,&lr,&ur);
	   		   res->val=get32msb(ur,lr,Qres,64-(op1.Q+op2.Q));
	   		   IQminus(res); 
	      }
	}

	res->Q=Qres;
}

unsigned long lead1(unsigned char pos)
{
       	unsigned long res=0xFFFFFFFF;
       	res=res<<(32-pos);
       	return res;
}
void shiftIQ(IQ *op1,unsigned char sq,char d)
{
	//negative
	if(IQSIGN(op1->val))
	{
		//IQminus(op1);
		
		if(d=='l')      //left
			op1->val=op1->val<<sq;
		else
		{
		  op1->val=op1->val>>sq;
		  op1->val+=lead1(sq);
		}
			

		
	}
	else
	{
		if(d=='l')      //left
			op1->val=op1->val<<sq;
		else
			op1->val=op1->val>>sq;
		//IQPLUS(op1->val);
	}
}


//Assume that both op1 and op2 are positive numbers, add them
void addIQ_intr(IQ op1,IQ op2,IQ *res,unsigned char Qres)
{
	IQ tmp1,tmp2;
    if(Qres>op1.Q)
    	shiftIQ(&op1,Qres-op1.Q,'l');
    else if(Qres<op1.Q)
    	shiftIQ(&op1,op1.Q-Qres,'r');

    
    if(Qres>op2.Q)
    	shiftIQ(&op2,Qres-op2.Q,'l');
    else if(Qres<op2.Q)
    	shiftIQ(&op2,op2.Q-Qres,'r');
    
    
    res->val=op1.val+op2.val;
    res->Q=Qres;
}



void subsIQ_intr(IQ op1,IQ op2,IQ *res,unsigned char Qres)
{
	IQ tmp1,tmp2;
    if(Qres>op1.Q)
    	shiftIQ(&op1,Qres-op1.Q,'l');
    else if(Qres<op1.Q)
    	shiftIQ(&op1,op1.Q-Qres,'r');

    
    if(Qres>op2.Q)
    	shiftIQ(&op2,Qres-op2.Q,'l');
    else if(Qres<op2.Q)
    	shiftIQ(&op2,op2.Q-Qres,'r');
    
    
    res->val=op1.val-op2.val;
    res->Q=Qres;
}


void divIQ_int(IQ op1,IQ op2,IQ *res,unsigned char Qres)
{
	unsigned long I,F,Rem,Q; //Integer part,Fractional part,Remainder,Quotient
    int remaining,i;
    remaining=(int)op1.Q-(int)op2.Q+(int)Qres;
	Q=(op2.val)/(op1.val);
    Rem=(op2.val)%(op1.val);
    for(i=0;i<remaining;i++)
    {
    	Q=Q<<1; 
    	if(Rem==0)
    	  break;   	
    	Rem=Rem<<1;
    	if(Rem>=(op1.val))
    	{
    	  Q+=1;	
    	  Rem=Rem-(op1.val); 	  
    	}
    	   	
    }
    if(remaining-i>=0)
     Q=Q<<(remaining-i);
    else if(remaining<0)
     Q=Q>>(remaining*-1);	
    res->Q=Qres;
    res->val=Q;
}

void divIQ(IQ op1,IQ op2,IQ *res,unsigned char Qres)
{
	if(IQSIGN(op1.val)==IQSIGN(op2.val))
	{
       if(IQSIGN(op1.val))  //both negatives
       {
    	   IQminus(&op1);
    	   IQminus(&op2);
    	   divIQ_int(op1,op2,res,Qres);
       }
       else
       {
    	   divIQ_int(op1,op2,res,Qres);
       }

	}
	else
	{ 
	      if(IQSIGN(op1.val))  //op1 is the negative number
	       {
	    	   IQminus(&op1);
	    	   divIQ_int(op1,op2,res,Qres);
	   		   IQminus(res);
	       }	
	      else
	      {
	    	   IQminus(&op2);
	    	   divIQ_int(op1,op2,res,Qres);
	   		   IQminus(res); 
	      }
	}

	res->Q=Qres;
}


void run_test()
{
	unsigned char oper,q1,q2,qres;
	ul l1,l2,lres;
	IQ op1,op2,r;
	while(1)
	{
		delay_8ms(5);
		send_char('R');
		l1.cval[0]=receive_char();
		l1.cval[1]=receive_char();
		l1.cval[2]=receive_char();
		l1.cval[3]=receive_char();
		q1=receive_char();
		l2.cval[0]=receive_char();
		l2.cval[1]=receive_char();
		l2.cval[2]=receive_char();
		l2.cval[3]=receive_char();
		q2=receive_char();
		qres=receive_char();
		oper=receive_char();
		op1.val=l1.lval;
		op2.val=l2.lval;
		op1.Q=q1;
		op2.Q=q2;
		__RESET_WATCHDOG();
		if(oper==0)
			addIQ_intr(op1,op2,&r,qres);
		else if(oper==1)
			subsIQ_intr(op1,op2,&r,qres);
		else if(oper==2)
			mulIQ(op1,op2,&r,qres);
		else 
			divIQ(op2,op1,&r,qres);
		__RESET_WATCHDOG();
		
		lres.lval=r.val;
		send_char(lres.cval[0]);
		send_char(lres.cval[1]);
		send_char(lres.cval[2]);
		send_char(lres.cval[3]);
		__RESET_WATCHDOG();
	}
}


void IQsine(IQ op,IQ *res)
{
	//For IQsine we use the Taylor series expansion with four terms sin(x)=x(1-(x^2/3!)(1-(x^2/(5*4))(1-x^2/(7*6)))))
	//this expansion is valid if pi>=x>=-pi
	IQ op_2,c1,c2,c3;
	//max value of x^2 is (pi)^2=9.87... which requires 4 integer bits i.e.  27 fractional bits
	mulIQ(op,op,&op_2,27);  //op_2 is the square of op
	c1.Q=31;
	c1.val=51131586;   // 1/(6*7)=0.02381 this is the Q31 representation
	mulIQ(op_2,c1,&c2,31); //9.87*0.02381 has no integer part, so we can use Q31
	c3.Q=30;    //c3 is 1.0000 we need this to compute 1-x^2/(7*6)
	c3.val=1073741824;
	subsIQ_intr(c3,c2,&c1,30);   //This result is at most 0.76 which needs no integer part	                      
	c2.Q=31;                 //1/(5*4)=0.05 this is the Q31 representation
	c2.val=107374182;          
	mulIQ(c1,c2,&c3,31);     //we now multiply (1-x^2/(7*6))* 1/(5*4) the result is in c3
	mulIQ(c3,op_2,&c1,31);     //we now multiply (1-x^2/(7*6))* 1/(5*4) by x^2 the result is in c1
	c3.Q=30;    //c3 is 1.0000 
	c3.val=1073741824;
	subsIQ_intr(c3,c1,&c2,30);  //(1-(x^2/(5*4))(1-x^2/(7*6)))) result is in c2
	c3.Q=31;
	c3.val=357913798;    // 1 /(2*3)
	mulIQ(c2,c3,&c1,31);   // (1/(2*3))(1-(x^2/(5*4))(1-x^2/(7*6)))) result is in c1
	mulIQ(c1,op_2,&c2,30);  //At least 1 bit for integer part are needed here...result is in c2
	c3.Q=30;    //c3 is 1.0000 
	c3.val=1073741824;
	subsIQ_intr(c3,c2,&c1,30);  //(1-(x^2/(5*4))(1-x^2/(7*6)))) result is in c1
	mulIQ(c1,op,&c3,30);  
	res->Q=30;
	res->val=c3.val;
}


void test_sine()
{
	//In this function we test IQsine generating inputs from -pi to pi and casting them to IQ
	//the result is graphed and compared against a floating point implementation in Matlab
	float iter=-3.1416;
	unsigned int counter=0;
	IQ temp,res;
	ul send_res;
	send_char('R');
	for(counter=0;counter<1258;counter++)   //We create 0.005 steps to reach pi, 2pi/0.005=~1257
	{
		floatToIQ(&iter,&temp,28);
		IQsine(temp,&res);
		send_res.lval=res.val;
		//We send the result to the PC to compare it against the Matlab result
		send_char(send_res.cval[0]);
		send_char(send_res.cval[1]);
		send_char(send_res.cval[2]);
		send_char(send_res.cval[3]);
		iter+=0.005;
	}
	
}
void main(void) {

  EnableInterrupts;
  /* include your code here */

   SCI1BD = 27;    //(BAUD RATE 9600)
   SCI1C2 = 12;   //seleccionamos la UART1
   run_test(); 
   //test_sine();
   while(1) __RESET_WATCHDOG();	

  
  for(;;) {
    __RESET_WATCHDOG();	/* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
