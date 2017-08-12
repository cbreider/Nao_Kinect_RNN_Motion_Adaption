M = dlmread('anglesOut.txt');
sp = M(:,1);
sr = M(:,2);
ep = M(:,3);
ey = M(:,4);

M3 = dlmread('objectPos.txt');
x = M3(:,1);
y = M3(:,2);
z = M3(:,3);

M2 = dlmread('test0.txt');
s = size(sp);
time = zeros(s);
rs_sp = M2(:,1);
rs_sr = M2(:,2);
rs_ep = M2(:,3);
rs_ey = M2(:,4);
for i=1:s
 time(i) = i; 
 %rs_sp(i) = sp(i) + (-0.1 + (0.1-(-0.1)).*rand(1,1));
 %rs_sr(i) = sr(i) + (-0.1 + (0.1-(-0.1)).*rand(1,1));
 %rs_ep(i) = ep(i) + (-0.1 + (0.1-(-0.1)).*rand(1,1));
 %rs_ey(i) = ey(i) + (-0.1 + (0.1-(-0.1)).*rand(1,1));
 
end

f1=figure;
plot(time, sp, time, rs_sp)
legend('Demonstration','RNNPB');
title('Schulter Pitch-Winkel');
xlabel('Schritt') % x-axis label
ylabel('Winkel/rad') % y-axis label

f2=figure;
plot(time, sr, time, rs_sr)
legend('Demonstration','RNNPB');
title('Schulter Roll-Winkel');
xlabel('Schritt') % x-axis label
ylabel('Winkel/rad') % y-axis labell

f3=figure;
plot(time, ep, time, rs_ep );
legend('Demonstration','RNNPB');
title('Ellenbogen Pitch-Winkel');
xlabel('Schritt') % x-axis label
ylabel('Winkel/rad') % y-axis label

f4=figure;
plot(time, ey, time, rs_ey)
legend('Demonstration','RNNPB');
title('Ellenbogen Yaw-Winkel');
xlabel('Schritt') % x-axis label
ylabel('Winkel/rad') % y-axis label

f5=figure;
plot(time, x, time, x, time, z)
xlabel('Schritt') % x-axis label
ylabel('Winkel/rad') % y-axis label