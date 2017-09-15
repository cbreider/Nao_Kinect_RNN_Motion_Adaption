M1 = dlmread('anglesIn.txt1');
sp1 = M1(:,1);
sr1 = M1(:,2);
ep1 = M1(:,3);
ey1 = M1(:,4);
s = size(sp1, 1);
s1 = size(sp1, 1) / 10;
sp_est1 = zeros(s1, 1);%M_est(:,1);
sr_est1 = zeros(s1,1 );%M_est(:,2);
ep_est1 = zeros(s1,1);%M_est(:,3);
ey_est1 =zeros(s1,1);%&M_est(:,4);
time = zeros(s1,1);
time2 = zeros(s,1);

sum1 = 0;
sum2 = 0;
sum3 = 0;
sum4 = 0;
for i=1:s
    time2(s) = s;
	sum1 = sum1 +sp1(i);
    sum2 = sum2 +sr1(i);
    sum3 = sum3 +ep1(i);
    sum4 = sum4 +ey1(i);
    sp_est1(i) = sum1 /10;
    sr_est1(i ) = sum2 /10;
    ep_est1(i) = sum3 /10;
    ey_est1(i) = sum4 /10;
if(mod(i, 10) == 0)
    time(i /10) =i/10;   
    sp_est1(i/ 10) = sum1 /10;
    sr_est1(i / 10) = sum2 /10;
    ep_est1(i/ 10) = sum3 /10;
    ey_est1(i /10) = sum4 /10;
    sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
 end
end

M2 = dlmread('anglesOut.txt1');
sp2 = M2(:,1);
sr2 = M2(:,2);
ep2 = M2(:,3);
ey2 = M2(:,4);
sp_est2 = zeros(s1,1);%M_est(:,1);
sr_est2 = zeros(s1,1);%M_est(:,2);
ep_est2 = zeros(s1,1);%M_est(:,3);
ey_est2 = zeros(s1,1);%&M_est(:,4);
for i=1:s

        sum1 = sum1 +sp2(i);
        sum2 = sum2 +sr2(i);
        sum3 = sum3 +ep2(i);
        sum4 = sum4 +ey2(i);
    if(mod(i, 10) == 0)
    sp_est2(i/ 10) = sum1 /10;
    sr_est2(i / 10) = sum2 /10;
    ep_est2(i/ 10) = sum3 /10;
    ey_est2(i /10) = sum4 /10;
        sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    end
end
M3 = dlmread('objectPos.txt1');
x = M3(:,1);
y = M3(:,2);
z = M3(:,3);
x_es = zeros(s1,1);%M_est(:,1);
y_es = zeros(s1,1);%M_est(:,2);
z_es = zeros(s1,1);%M_est(:,3);
for i=1:s

        sum1 = sum1 +x(i);
        sum2 = sum2 +y(i);
        sum3 = sum3 +z(i);

    if(mod(i, 10) == 0)
    x_es(i/ 10) = sum1;
    y_es(i / 10) = sum2;
    z_es(i/ 10) = sum3;
    sum1 = 0;
    sum2 = 0;
    sum3 = 0;
    sum4 = 0;
    end
 end

f1=figure;
plot(time, sp_est1, time, sr_est1, time, ep_est1, time, ey_est1)
legend('Demonstration','Neural Net');
f2=figure;
plot(time, sp_est2, time, sr_est2, time, ep_est2, time, ey_est2)
legend('Demonstration','Neural Net');
f3=figure;
plot(time, x_es, time, y_es, time, z_es )
legend('Demonstration','Neural Net');

fileID = fopen('anglesIn.txt1','w');
fprintf(fileID, '%4.3f %4.3f %4.3f %4.3f\n', [sp_est1,sr_est1,ep_est1,ey_est1].');
fclose(fileID);


fileID = fopen('anglesOut.txt1','w');
fprintf(fileID, '%4.3f %4.3f %4.3f %4.3f\n', [sp_est2,sr_est2,ep_est2,ey_est2].');
fclose(fileID);

fileID = fopen('objectPos.txt','w');
fprintf(fileID, '%4.3f %4.3f %4.3f\n', [x_es,y_es,z_es].');
fclose(fileID);