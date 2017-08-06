M = dlmread('anglesIn.txt3');
sp = M(:,1);
sr = M(:,2);
ep = M(:,3);
ey = M(:,4);
M_est = dlmread('est.txt');
sp_est = M_est(:,1);
sr_est = M_est(:,2);
ep_est = M_est(:,3);
ey_est = M_est(:,4);

s = size(sp);
time = zeros(s);
for i=1:s
 time(i) = i;   
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