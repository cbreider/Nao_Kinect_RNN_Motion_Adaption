M1 = dlmread('anglesIn.txt1');
sp1 = M1(:,1);
sr1 = M1(:,2);
ep1 = M1(:,3);
ey1 = M1(:,4);
s = size(sp1, 1);
s1 = size(sp1, 1)-28;
sp_est1 = zeros(s1, 1);%M_est(:,1);
sr_est1 = zeros(s1,1 );%M_est(:,2);
ep_est1 = zeros(s1,1);%M_est(:,3);
ey_est1 =zeros(s1,1);%&M_est(:,4);
time = zeros(s,1);
time2 = zeros(s1,1) ;

M2 = dlmread('anglesOut.txt1');
sp2 = M2(:,1);
sr2 = M2(:,2);
ep2 = M2(:,3);
ey2 = M2(:,4);
sp_est2 = zeros(s1,1);%M_est(:,1);
sr_est2 = zeros(s1,1);%M_est(:,2);
ep_est2 = zeros(s1,1);%M_est(:,3);
ey_est2 = zeros(s1,1);%&M_est(:,4);

M3 = dlmread('objectPos.txt1');
x = M3(:,1);
y = M3(:,2);
z = M3(:,3);
x_es = zeros(s1,1);%M_est(:,1);
y_es = zeros(s1,1);%M_est(:,2);
z_es = zeros(s1,1);%M_est(:,3);

for i=1:s
time(i) = i;
if(i>=25 && i<= 104)
j = i -24;
time2(j) = j;
sp_est1(j) = sp1(i);
sr_est1(j) = sr1(i) /3;
ep_est1(j) = ep1(i) /3;
ey_est1(j) = ey1(i) /3;

sp_est2(j) = sp2(i) ;
sr_est2(j) = sr2(i) /3;
ep_est2(j) = ep2(i) /3;
ey_est2(j) = ey2(i) /3;

x_es(j) = x(i) /3 ;
y_es(j) = y(i);
z_es(j) = z(i) /3;
end
end

f1=figure;
plot(time2, sp_est1)
legend('Demonstration','Neural Net');
axis([0 80 -1.5 1.5])
f2=figure;
plot(time2, sr_est1)
legend('Demonstration','Neural Net');
axis([0 80 -1.5 1.5])
f3=figure;
plot(time2, ep_est1 )
legend('Demonstration','Neural Net');
axis([0 80 -1.5 1.5])
f4=figure;
plot(time2, ey_est1)
legend('Demonstration','Neural Net');
axis([0 80 -1.5 1.5])

f5=figure;
plot(time2, x_es)
legend('Objektbewegung x-Achse');
axis([0 80 -120 120])
f6=figure;
plot( time2, y_es )
legend('Objektbewegung y-Achse');
axis([0 80 -120 120])
f7=figure;
plot(time2, z_es)
legend('Objektbewegung z-Achse');
axis([0 80 -120 120])

fileID = fopen('anglesIn.txt','w');
fprintf(fileID, '%4.3f %4.3f %4.3f %4.3f\n', [sp_est1,sr_est1,ep_est1,ey_est1].');
fclose(fileID);


fileID = fopen('anglesOut.txt','w');
fprintf(fileID, '%4.3f %4.3f %4.3f %4.3f\n', [sp_est2,sr_est2,ep_est2,ey_est2].');
fclose(fileID);

fileID = fopen('objectPos.txt','w');
fprintf(fileID, '%4.3f %4.3f %4.3f\n', [x_es,y_es,z_es].');
fclose(fileID);

