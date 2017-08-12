M1 = dlmread('anglesIn.txt1.tmp');
sp1 = M1(:,1);
sr1 = M1(:,2);
ep1 = M1(:,3);
ey1 = M1(:,4);
s = size(sp1, 1);
s1 = size(sp1, 1) / 8;
sp_est1 = zeros(s1, 1);%M_est(:,1);
sr_est1 = zeros(s1,1 );%M_est(:,2);
ep_est1 = zeros(s1,1);%M_est(:,3);
ey_est1 =zeros(s1,1);%&M_est(:,4);
time = zeros(s1,1);
sum1 = 0;
sum2 = 0;
sum3 = 0;
sum4 = 0;
for i=1:s
	sum1 = sum1 +sp1(i);
    sum2 = sum2 +sr1(i);
    sum3 = sum3 +ep1(i);
    sum4 = sum4 +ey1(i);
if(mod(i, 8) == 0)
    time(i /8) =i/8;   
    sp_est1(i/ 8) = sum1 /8;
    sr_est1(i / 8) = sum2 /8;
    ep_est1(i/ 8) = sum3 /8;
    ey_est1(i /8) = sum4 /8;
    sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
 end
end

M2 = dlmread('anglesIn.txt2.tmp');
sp2 = M2(:,1);
sr2 = M2(:,2);
ep2 = M2(:,3);
ey2 = M2(:,4);
s2 = size(sp);
sp_est2 = zeros(s1,1);%M_est(:,1);
sr_est2 = zeros(s1,1);%M_est(:,2);
ep_est2 = zeros(s1,1);%M_est(:,3);
ey_est2 = zeros(s1,1);%&M_est(:,4);
for i=1:s

        sum1 = sum1 +sp2(i);
        sum2 = sum2 +sr2(i);
        sum3 = sum3 +ep2(i);
        sum4 = sum4 +ey2(i);
    if(mod(i, 8) == 0)
    sp_est2(i/ 8) = sum1 /8;
    sr_est2(i / 8) = sum2 /8;
    ep_est2(i/ 8) = sum3 /8;
    ey_est2(i /8) = sum4 /8;
        sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    end
end
M3 = dlmread('anglesIn.txt3.tmp');
sp3 = M3(:,1);
sr3 = M3(:,2);
ep3 = M3(:,3);
ey3 = M3(:,4);
s3 = size(sp);
sp_est3 = zeros(s1,1);%M_est(:,1);
sr_est3 = zeros(s1,1);%M_est(:,2);
ep_est3 = zeros(s1,1);%M_est(:,3);
ey_est3 = zeros(s1,1);%&M_est(:,4);
for i=1:s

        sum1 = sum1 +sp3(i);
        sum2 = sum2 +sr3(i);
        sum3 = sum3 +ep3(i);
        sum4 = sum4 +ey3(i);

    if(mod(i, 8) == 0)
    sp_est3(i/ 8) = sum1 /8;
    sr_est3(i / 8) = sum2 /8;
    ep_est3(i/ 8) = sum3 /8;
    ey_est3(i /8) = sum4 /8;
        sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    end
 end


M4 = dlmread('anglesIn.txt4.tmp');
sp4 = M4(:,1);
sr4 = M4(:,2);
ep4 = M4(:,3);
ey4 = M4(:,4);
s4 = size(sp);
sp_est4 = zeros(s1,1);%M_est(:,1);
sr_est4 = zeros(s1,1);%M_est(:,2);
ep_est4 = zeros(s1,1);%M_est(:,3);
ey_est4 =zeros(s1,1);%&M_est(:,4);
for i=1:s
        sum1 = sum1 +sp4(i);
        sum2 = sum2 +sr4(i);
        sum3 = sum3 +ep4(i);
        sum4 = sum4 +ey4(i);
    if(mod(i, 8) == 0)
    sp_est4(i/ 8) = sum1 /8;
    sr_est4(i / 8) = sum2 /8;
    ep_est4(i/ 8) = sum3 /8;
    ey_est4(i /8) = sum4 /8;
        sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    end
 end



fileID = fopen('anglesOut.txt1','w');
fprintf(fileID, '%4.3f %4.3f %4.3f %4.3f\n', [sp_est1,sr_est1,ep_est1,ey_est1].');
fclose(fileID);


fileID = fopen('anglesOut.txt2','w');
fprintf(fileID, '%4.3f %4.3f %4.3f %4.3f\n', [sp_est2,sr_est2,ep_est2,ey_est2].');
fclose(fileID);

fileID = fopen('anglesOut.txt3','w');
fprintf(fileID, '%4.3f %4.3f %4.3f %4.3f\n', [sp_est3,sr_est3,ep_est3,ey_est3].');
fclose(fileID);

fileID = fopen('anglesOut.txt4','w');
fprintf(fileID, '%4.3f %4.3f %4.3f %4.3f\n', [sp_est4,sr_est4,ep_est4,ey_est4].');
fclose(fileID);

f1=figure;
plot(time, sp_est1, time, sp_est2, time, sp_est3, time, sp_est4)
%legend('Demonstration','Neural Net');
f2=figure;
plot(time, sr_est1, time, sr_est2, time, sr_est3, time, sr_est4)
%legend('Demonstration','Neural Net');
f3=figure;
plot(time, ep_est1, time, ep_est2, time, ep_est3, time, ep_est4 )
%legend('Demonstration','Neural Net');
f4=figure;
plot(time, ey_est1, time, ey_est2, time, ey_est3, time, ey_est4)
%legend('Demonstration','Neural Net');


