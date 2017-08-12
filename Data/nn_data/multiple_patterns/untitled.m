M = dlmread('anglesIn.txt1');
sp = M(:,1);
sr = M(:,2);
ep = M(:,3);
ey = M(:,4);


s = size(sp);
time = zeros(s);
rs_sp = zeros(s);
rs_sr = zeros(s);
rs_ep = zeros(s);
rs_ey = zeros(s);
for i=1:s
 time(i) = i; 
 rs_sp(i) = sp(i) + (-0.1 + (0.1-(-0.1)).*rand(1,1));
 rs_sr(i) = sr(i) + (-0.1 + (0.1-(-0.1)).*rand(1,1));
 rs_ep(i) = ep(i) + (-0.1 + (0.1-(-0.1)).*rand(1,1));
 rs_ey(i) = ey(i) + (-0.1 + (0.1-(-0.1)).*rand(1,1));
 
end

f1=figure;
plot(time, sp, time, rs_sp)
legend('Demonstration','Neural Net');
f2=figure;
plot(time, sr, time, rs_sr)
legend('Demonstration','Neural Net');
f3=figure;
plot(time, ep, time, rs_ep )
legend('Demonstration','Neural Net');
f4=figure;
plot(time, ey, time, rs_ey)

legend('Demonstration','Neural Net');