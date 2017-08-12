M = dlmread('anglesOut.txt4');
sp = M(:,1);
sr = M(:,2);
ep = M(:,3);
ey = M(:,4);
s = size(sp);
M_est = dlmread('test0.txt');
sp_est = zeros(size(sp));%M_est(:,1);
sr_est = zeros(size(sr));%M_est(:,2);
ep_est = zeros(size(ep));%M_est(:,3);
ey_est =zeros(size(ey));%&M_est(:,4);


time = zeros(s);
for i=1:s
 time(i) =i;
 if(i <= 10)
     sp_est(i) = sp(i);
     sr_est(i) = sr(i);
     ep_est(i) = ep(i);
     ey_est(i) = ey(i);
 elseif(i>= s-10)
     sp_est(i) = sp(i);
     sr_est(i) = sr(i);
     ep_est(i) = ep(i);
     ey_est(i) = ey(i);
 else 
     sum1 = 0.00000;
     sum2 = 0.00000;
     sum3 = 0.00000;
     sum4 = 0.00000;
    for ii =-9 : 10
        sum1 = sum1 +sp(i+ii);
        sum2 = sum2 +sr(i+ii);
        sum3 = sum3 +ep(i+ii);
        sum4 = sum4 +ey(i+ii);
    end
    sp_est(i) = sum1 /20;
    sr_est(i) = sum2 /20;
    ep_est(i) = sum3 /20;
    ey_est(i) = sum4 /20;
 end
end

f1=figure;
plot(time, sp, time, sp_est)
legend('Demonstration','Neural Net');
f2=figure;
plot(time, sr, time, sr_est)
legend('Demonstration','Neural Net');
f3=figure;
plot(time, ep, time, ep_est )
legend('Demonstration','Neural Net');
f4=figure;
plot(time, ey, time, ey_est)
legend('Demonstration','Neural Net');

fileID = fopen('anglesOut4.txt','w');
fprintf(fileID, '%4.3f %4.3f %4.3f %4.3f\n', [sp,sr,ep,ey].');
fclose(fileID);

