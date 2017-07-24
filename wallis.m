clear,clc;close all;
[filename,filepath] = uigetfile('*.*','Select the image');  
if isequal(filename,0)||isequal(filepath,0)
    return;
else
   filefullpath=[filepath,filename];
end
%得到文件名，以便建立一个文件夹保存分块图像
[pathstr,name,ext] = fileparts(filename);
Im=imread(filefullpath);
figure;
imshow(Im);
L = size(Im);
%分块大小
height=500;
width=500;
h_val=height;
w_val=width;
max_row = (L(1)-height)/h_val+1;
max_col = (L(2)-width)/w_val+1;
%判断能否完整分块
if max_row==fix(max_row)%判断是否能够整分
    max_row=max_row;
else
    max_row=fix(max_row+1);
end
if max_col==fix(max_col)%判断是否能够整分
    max_col=max_col;
else
    max_col=fix(max_col+1);
end
seg = cell(max_row,max_col);
for row = 1:max_row      
    for col = 1:max_col        
        if ((width+(col-1)*w_val)>L(2)&&((row-1)*h_val+height)<=L(1))%判断最右边不完整的部分
    seg(row,col)= {Im((row-1)*h_val+1:height+(row-1)*h_val,(col-1)*w_val+1:L(2),:)};
        elseif((height+(row-1)*h_val)>L(1)&&((col-1)*w_val+width)<=L(2))%判断最下边不完整的部分
    seg(row,col)= {Im((row-1)*h_val+1:L(1),(col-1)*w_val+1:width+(col-1)*w_val,:)}; 
        elseif((width+(col-1)*w_val)>L(2)&&((row-1)*h_val+height)>L(1))%判断最后一张
    seg(row,col)={Im((row-1)*h_val+1:L(1),(col-1)*w_val+1:L(2),:)};       
        else
     seg(row,col)= {Im((row-1)*h_val+1:height+(row-1)*h_val,(col-1)*w_val+1:width+(col-1)*w_val,:)}; %其余完整部分  
        end
    end
end 
 imshow(Im);
 hold on
% 画框显示
 for row = 1:max_row      
    for col = 1:max_col  
    c=rand(1,3);%随机颜色
     rectangle('Position',[w_val*(col-1),h_val*(row-1),width,height],...
         'LineWidth',2,'LineStyle','-','EdgeColor',c);
    end
 end 
 k=0;
for i=1:max_row
    for j=1:max_col
        
a=seg(i,j);
mat=a{1}; 
k=k+1;
aver_gray(k)= mean(mean(mat));
std_gray(k)=std2(mat);
var_gray(k)=std_gray(k)*std_gray(k);

format long
mf=127;sf=60;
c=0.8;b=0.5;%设定为40-70之间的数值
mg=aver_gray;
sg=var_gray;
%r1(k)=(c*sf)/(c*sg(k)+sf/c);
r1(k)=(c*sf)/(c*std_gray(k)+(1-c)*sf);
r0(k)=b*mf+(1-b-r1(k))*mg(k);
    end
end
r1=(reshape(r1,5,5))';                                                                                                                            
r0=(reshape(r0,5,5))';

Im=im2uint8(Im);

for i=1:L(1)
    for j=1:L(2)
        m=fix(i/500)+1;
        n=fix(j/500)+1;
  
        R1=r1(m,n);                                                                                 
       R0=r0(m,n);
        wallis_gray(i,j)=Im(i,j)*R1+R0;
    end
end

%  wallis_gray=im2uint8(wallis_gray);
 P=max(max(wallis_gray));
  Q=min(min(wallis_gray));
 wallis_gray=wallis_gray*(255/(P - Q))-(Q*255/(P - Q));

figure,imshow(wallis_gray);
