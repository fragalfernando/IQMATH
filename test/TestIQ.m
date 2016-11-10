function [ops,q1,q2,qr,random32_1st,random32_2nd,results,compare,dif] = TestIQ(n,operation)
%TESTFP Summary of this function goes here


s = serial('COM4');
set(s,'BaudRate',9600,'Timeout',100);
fopen(s);


%   Detailed explanation goes here
%random32_1st=randi([0,2^32],1,n);
%random32_2nd=randi([0,2^32],1,n);
%random32_1st=fix(random32_1st);
%random32_2nd=fix(random32_2nd);
random32_1st=zeros(n,1);
random32_2nd=zeros(n,1);
q1=zeros(n,1);
q2=zeros(n,1);
qr=zeros(n,1);
results=zeros(n,1);
ops=zeros(n,1);
switch operation     
    case 'substraction'
        ops=ops+1;
    case 'multiplication'
        ops=ops+2;
    case 'division'
        ops=ops+3;
end

%ops=randi([0,3],1,n);
%q1=randi([0,31],1,n);
%q2=randi([0,31],1,n);
%qr=randi([0,31],1,n);

compare=zeros(n,1);
tmpbyte=zeros(n,4);

for i=1:n
    qr(i)=randi([0,31],1,1);
    random32_1st(i)=randi([0,2^32],1,1);
    random32_1st(i)=fix(random32_1st(i));
    q1(i)=randi([0,31],1,1);
    op1=sfi([],32,q1(i));
    str_random32_1st=num2str(random32_1st(i));
    op1.dec=str_random32_1st;
    
    random32_2nd(i)=randi([0,2^32],1,1);
    random32_2nd(i)=fix(random32_2nd(i));
    q2(i)=randi([0,31],1,1);
    op2=sfi([],32,q2(i));
    str_random32_2nd=num2str(random32_2nd(i));
    op2.dec=str_random32_2nd;

    switch ops(i)
        case 0
            F = fimath('OverflowMode','Wrap', 'RoundMode', 'Floor', ...
                        'SumMode', 'SpecifyPrecision', ...
                        'SumWordLength', 32, 'SumFractionLength', qr(i));
           op1.fimath=F;
           op2.fimath=F;
           new_op1=sfi(op1,32,qr(i));
           new_op2=sfi(op2,32,qr(i));
           num1=str2num(new_op1.dec);
           num2=str2num(new_op2.dec);
           resn=num1+num2;
           res_str=num2str(resn);
           res=op1+op2;
           
           while length(res_str)>32
                 qr(i)=randi([0,31],1,1);
                 random32_1st(i)=randi([0,2^32],1,1);
                 random32_1st(i)=fix(random32_1st(i));
                 q1(i)=randi([0,31],1,1);
                 op1=sfi([],32,q1(i));
                 str_random32_1st=num2str(random32_1st(i));
                 op1.dec=str_random32_1st;

                 random32_2nd(i)=randi([0,2^32],1,1);
                 random32_2nd(i)=fix(random32_2nd(i));
                 q2(i)=randi([0,31],1,1);
                 op2=sfi([],32,q2(i));
                 str_random32_2nd=num2str(random32_2nd(i));
                 op2.dec=str_random32_2nd;
                 F = fimath('OverflowMode','Wrap', 'RoundMode', 'Floor', ...
                  'SumMode', 'SpecifyPrecision', ...
                  'SumWordLength', 32, 'SumFractionLength', qr(i));
                op1.fimath=F;
                op2.fimath=F;
        
                res=op1+op2;
              new_op1=sfi(op1,32,qr(i));
            new_op2=sfi(op2,32,qr(i));
             num1=str2num(new_op1.dec);
             num2=str2num(new_op2.dec);
             resn=num1+num2;
              res_str=num2str(resn);
           end
           
           results(i)=str2num(res.dec);
        case 1
           F = fimath('OverflowMode','Wrap', 'RoundMode', 'Floor', ...
                        'SumMode', 'SpecifyPrecision', ...
                        'SumWordLength', 32, 'SumFractionLength', qr(i));
           op1.fimath=F;
           op2.fimath=F;
           new_op1=sfi(op1,32,qr(i));
           new_op2=sfi(op2,32,qr(i));
           num1=str2num(new_op1.dec);
           num2=str2num(new_op2.dec);
           resn=num1-num2;
           res_str=num2str(resn);
           res=op1-op2;
           
           while length(res_str)>32
                 qr(i)=randi([0,31],1,1);
                 random32_1st(i)=randi([0,2^32],1,1);
                 random32_1st(i)=fix(random32_1st(i));
                 q1(i)=randi([0,31],1,1);
                 op1=sfi([],32,q1(i));
                 str_random32_1st=num2str(random32_1st(i));
                 op1.dec=str_random32_1st;

                 random32_2nd(i)=randi([0,2^32],1,1);
                 random32_2nd(i)=fix(random32_2nd(i));
                 q2(i)=randi([0,31],1,1);
                 op2=sfi([],32,q2(i));
                 str_random32_2nd=num2str(random32_2nd(i));
                 op2.dec=str_random32_2nd;
                 F = fimath('OverflowMode','Wrap', 'RoundMode', 'Floor', ...
                  'SumMode', 'SpecifyPrecision', ...
                  'SumWordLength', 32, 'SumFractionLength', qr(i));
                op1.fimath=F;
                op2.fimath=F;
        
                res=op1-op2;
              new_op1=sfi(op1,32,qr(i));
            new_op2=sfi(op2,32,qr(i));
             num1=str2num(new_op1.dec);
             num2=str2num(new_op2.dec);
             resn=num1-num2;
              res_str=num2str(resn);
           end
           
           results(i)=str2num(res.dec);
        case 2
           F = fimath('OverflowMode','Wrap', 'RoundMode', 'Floor', ...
                       'ProductMode', 'SpecifyPrecision', ...
                        'ProductWordLength', 32, 'ProductFractionLength', qr(i));
           op1.fimath=F;
           op2.fimath=F;
           res=op1*op2;
           results(i)=str2num(res.dec);   
             while strcmp(res.hex,'7fffffff') || strcmp(res.hex,'ffffffff') ||  strcmp(res.hex,'80000000')
                qr(i)=randi([0,31],1,1);
                random32_1st(i)=randi([0,2^32],1,1);
                random32_1st(i)=fix(random32_1st(i));
                q1(i)=randi([0,31],1,1);
                op1=sfi([],32,q1(i));
                str_random32_1st=num2str(random32_1st(i));
                op1.dec=str_random32_1st;

                random32_2nd(i)=randi([0,2^32],1,1);
                random32_2nd(i)=fix(random32_2nd(i));
                q2(i)=randi([0,31],1,1);
                op2=sfi([],32,q2(i));
                str_random32_2nd=num2str(random32_2nd(i));
                op2.dec=str_random32_2nd;
                F = fimath('OverflowMode','Wrap', 'RoundMode', 'Floor', ...
                       'ProductMode', 'SpecifyPrecision', ...
                        'ProductWordLength', 32, 'ProductFractionLength', qr(i));
                op1.fimath=F;
                op2.fimath=F;
                res=op1*op2;
                results(i)=str2num(res.dec);                     
             end
        otherwise
            T = numerictype('Signed',true,'WordLength',32,...
                            'FractionLength',qr(i));
            res=divide(T,op1,op2);      
            results(i)=str2num(res.dec); 
            
         while strcmp(res.hex,'7fffffff') || strcmp(res.hex,'ffffffff') ||  strcmp(res.hex,'80000000')
            qr(i)=randi([0,31],1,1);
            random32_1st(i)=randi([0,2^32],1,1);
            random32_1st(i)=fix(random32_1st(i));
            q1(i)=randi([0,31],1,1);
            op1=sfi([],32,q1(i));
            str_random32_1st=num2str(random32_1st(i));
            op1.dec=str_random32_1st;

            random32_2nd(i)=randi([0,2^32],1,1);
            random32_2nd(i)=fix(random32_2nd(i));
            q2(i)=randi([0,31],1,1);
            op2=sfi([],32,q2(i));
            str_random32_2nd=num2str(random32_2nd(i));
            op2.dec=str_random32_2nd;
            T = numerictype('Signed',true,'WordLength',32,...
                            'FractionLength',qr(i));
            res=divide(T,op1,op2);      
            results(i)=str2num(res.dec); 
                        
    end
  end  
    %Loop until a ready command is send
    while(1)
      data=fread(s,1);
      if(data==82)
       break
      end
    end     
    
     %Send the operand,and operators,receive result
    [ a4 a3 a2 a1 ] = d_to_byte(random32_1st(i));
    [ b4 b3 b2 b1 ] = d_to_byte(random32_2nd(i));
    fwrite(s,[a4,a3,a2,a1]);
    fwrite(s,q1(i));
    fwrite(s,[b4,b3,b2,b1]);
    fwrite(s,q2(i));
    fwrite(s,qr(i));
    fwrite(s,ops(i));
    tmpbyte(i,:)=fread(s,4);
    compare(i)=uint32(uint32(uint32(tmpbyte(i,1))*uint32(16777216))+uint32(uint32(tmpbyte(i,2))*uint32(65536))+uint32(uint32(tmpbyte(i,3))*uint32(256))+uint32(tmpbyte(i,4)));
    fprintf('Test %d  completed \n',i);            
        
end
fclose(s);
results=uint32(results);
compare=uint32(compare);
random32_1st=uint32(random32_1st);
random32_2nd=uint32(random32_2nd);
dif=uint32(abs(double(results)-double(compare)));
fprintf('Test finished with  %d errors  \n',size(dif(dif>1),1));

end