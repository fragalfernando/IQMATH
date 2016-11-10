function [ b4 b3 b2 b1 ] = float2bytes( fp )
%FLOAT2BYTES Summary of this function goes here
%   Detailed explanation goes here
d=hex2dec(num2hex(fp));
%d=uint32(d)
b4=d/(2^24);
b4=fix(b4);
b4=uint32(b4);

b3=d/(2^16);
b3=fix(b3);
b3=uint32(b3);
b3=mod(b3,256);

b2=d/256;
b2=fix(b2);
b2=uint32(b2);
b2=mod(b2,256);


b1=fix(d);
b1=uint32(b1);
b1=mod(b1,256);
end

