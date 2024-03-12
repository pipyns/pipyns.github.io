%% Test 3 (TESTING)
clear
time_max = 12*60*60;
b1 = zeros(time_max,1);
b1(40156:end) = 1;

% BATHROOM 1:
% - TOILETS: 
%   - 8s on x6
% - SINKS:
%   - 30-40s on ([31, 38, 33, 39, 30])
%   - 100% COLD
% - SHOWER: 20-30m on
%   - 20-30m on ([20])
%   - 40% HOT, 60% COLD

t1_f = zeros(time_max,1);
t1_f([9668:9668+8, 11020:11020+8, 21857:21857+8, 25284:25284+8, 30200:30200+8, 32455:32455+8]) = 1;

sk1_h = zeros(time_max,1);
sk1_c = zeros(time_max,1);
sk1_c([9688:9688+51, 11040:11040+48, 21877:21877+52, 25304:25304+46, 30220:30220+60, 32475:32475+50]) = 1.0;

sh1_h = zeros(time_max,1);
sh1_c = zeros(time_max,1);
sh1_h([16435:16435+1200]) = 0.4;
sh1_c([16435:16435+1200]) = 0.6;

% BATHROOM 2:
% - TOILETS: 
%   - 10s on x8
% - SINKS:
%   - 45-80 on ([57, 64, 67, 63, 75, 50, 46, 60])
%   - 50% COLD, 50% HOT
% - SHOWER: 20-30m on
%   - 30-45 on ([40, 36])
%   - 100% HOT

t2_f = zeros(time_max,1);
t2_f([3621:3621+10, 8256:8256+10, 9892:9892+10, 11360:11360+10, 19463:19463+10, 29774:29774+10, 32320:32320+10, 39457:39457+10]) = 1;

sk2_h = zeros(time_max,1);
sk2_c = zeros(time_max,1);
sk2_h([3646:3646+57, 8281:8281+64, 9917:9917+67, 11385:11385+63, 19488:19488+75, 29799:29799+50, 32345:32345+46, 39482:39482+60]) = 0.5;
sk2_c([3646:3646+57, 8281:8281+64, 9917:9917+67, 11385:11385+63, 19488:19488+75, 29799:29799+50, 32345:32345+46, 39482:39482+60]) = 0.5;

sh2_h = zeros(time_max,1);
sh2_c = zeros(time_max,1);
sh2_c([17124:17124+2400, 24897:24897+2160]) = 1.0;

b1 = timeseries(b1);
t1_f = timeseries(t1_f);
sk1_h = timeseries(sk1_h);
sk1_c = timeseries(sk1_c);
sh1_h = timeseries(sh1_h);
sh1_c = timeseries(sh1_c);
t2_f = timeseries(t2_f);
sk2_h = timeseries(sk2_h);
sk2_c = timeseries(sk2_c);
sh2_h = timeseries(sh2_h);
sh2_c = timeseries(sh2_c);